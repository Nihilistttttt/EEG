package com.nihilisttt.eegpatient;

import android.os.Handler;
import android.os.Looper;
import android.util.Log;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicLong;

/**
 * 患者端与医生端的连接管理器。
 *
 * 热点部署方式：医生端开启热点并作为 TCP 服务端，患者端连接医生热点后作为 TCP 客户端。
 *
 * 通道职责：
 * 41004：医生端 -> 患者端，原始脑电数据；
 * 41005：患者端 -> 医生端，患者端命令；
 * 41006：医生端 -> 患者端，页面及 SSVEP 控制指令；
 * 41007：UDP 自动发现。
 *
 * v0.6.0：
 * 1. 41006 使用 CONTROL_READY 双向握手；
 * 2. 医生端每 2 秒发送 PING，患者端返回 PONG；
 * 3. 控制线程为永久监督线程，数据连接恢复后会自动重新建立 41006；
 * 4. 使用连接代次和 Socket 身份校验，旧线程不能覆盖新连接状态。
 */
public class DoctorConnector {
    private static final String TAG = "PATIENT";

    private static final int DATA_PORT = 41004;
    private static final int CMD_PORT = 41005;
    private static final int D2P_CMD_PORT = 41006;
    private static final int DISCOVERY_PORT = 41007;

    private static final int DISCOVERY_TIMEOUT_MS = 5000;
    private static final int CONNECT_TIMEOUT_MS = 4000;
    private static final int CONTROL_RETRY_MS = 1000;
    private static final int CONTROL_READ_TIMEOUT_MS = 8000;
    private static final int AUTO_RETRY_MS = 3000;

    private static final DoctorConnector INSTANCE = new DoctorConnector();

    private final CopyOnWriteArrayList<DataListener> listeners = new CopyOnWriteArrayList<>();
    private final Handler mainHandler = new Handler(Looper.getMainLooper());
    private final AtomicBoolean controlSupervisorRunning = new AtomicBoolean(false);
    private final AtomicLong controlGenerationCounter = new AtomicLong(0);
    private final Object socketLock = new Object();
    private final Object d2pWriteLock = new Object();
    private final ExecutorService controlReplyWriter = Executors.newSingleThreadExecutor(r -> {
        Thread t = new Thread(r, "patient-control-reply-writer");
        t.setDaemon(true);
        return t;
    });

    private volatile String doctorIp;

    private Socket dataSocket;
    private Socket cmdSocket;
    private Socket d2pSocket;
    private OutputStream cmdOutput;

    private volatile boolean sessionActive;
    private volatile boolean autoConnecting;

    private volatile boolean dataConnected;
    private volatile boolean commandConnected;
    private volatile boolean controlConnected;
    private volatile long currentControlGeneration;
    private volatile long lastControlRxAt;

    private volatile boolean lastReportedConnected;
    private volatile boolean lastReportedDataConnected;
    private volatile boolean lastReportedControlConnected;

    /** 缓存医生端当前页面及 SSVEP 状态，解决 Fragment 晚注册导致的事件丢失。 */
    private volatile int currentPatientPage = 0;
    private volatile boolean ssvepStateKnown;
    private volatile boolean ssvepActive;
    private volatile int ssvepFreqIndex = -1;

    public interface DataListener {
        void onWaveData(int cmd, float ch0, float ch1);
        void onEegFrame(EegFrame frame);
        void onInferenceResult(InferenceResult result);
        void onIpcDiag(IpcDiagInfo diag);
        void onConnectionChanged(boolean connected);
        void onTaskStart(String side);
        void onTaskDone();
        void onReadyTrain();
        void onReadyTest();
        void onModeSetOk(int mode);
        void onPageSwitch(int page);
        void onFocusData(float attn0, float attn1, float ema0, float ema1, int trend, int instant);
        default void onSsvepStart(int freqIndex) {}
        default void onSsvepStop() {}
        default void onSsvepResult(PatientSsvepResult result) {}
    }

    private DoctorConnector() {}

    public static DoctorConnector getInstance() {
        return INSTANCE;
    }

    public void setDoctorIp(String ip) {
        this.doctorIp = ip;
    }

    public void addListener(DataListener listener) {
        if (listener == null) return;
        listeners.addIfAbsent(listener);
        replayState(listener);
    }

    public void removeListener(DataListener listener) {
        if (listener != null) listeners.remove(listener);
    }

    /** 完整连接：数据通道已连接，并且 41006 已完成双向握手。 */
    public boolean isConnected() {
        return dataConnected && controlConnected;
    }

    public boolean isDataConnected() {
        return dataConnected;
    }

    public boolean isControlConnected() {
        return controlConnected;
    }

    public boolean isCommandConnected() {
        return commandConnected;
    }

    public boolean isSsvepActive() {
        return ssvepActive;
    }

    public int getSsvepFreqIndex() {
        return ssvepFreqIndex;
    }

    public int getCurrentPatientPage() {
        return currentPatientPage;
    }

    public void startAutoConnect() {
        if (autoConnecting) return;
        autoConnecting = true;
        Log.i(TAG, "startAutoConnect: hotspot client mode enabled");

        startControlSupervisor();

        new Thread(() -> {
            while (autoConnecting) {
                if (!dataConnected) {
                    String ip = discoverDoctorIp();
                    if (ip != null && !ip.trim().isEmpty()) {
                        doctorIp = ip.trim();
                        Log.i(TAG, "autoConnect: found doctor hotspot host at " + doctorIp);
                        doConnectDataSession();
                        Log.w(TAG, "autoConnect: data session ended, retrying");
                    } else {
                        Log.w(TAG, "autoConnect: discovery failed, retrying");
                    }
                }
                sleepQuietly(AUTO_RETRY_MS);
            }
            Log.i(TAG, "patient-auto-connect exited");
        }, "patient-auto-connect").start();
    }

    public void stopAutoConnect() {
        autoConnecting = false;
        doDisconnect();
    }

    private String discoverDoctorIp() {
        DatagramSocket socket = null;
        try {
            socket = new DatagramSocket();
            socket.setBroadcast(true);
            socket.setSoTimeout(DISCOVERY_TIMEOUT_MS);

            byte[] requestData = "EEG_DOCTOR_DISCOVERY".getBytes("UTF-8");
            String broadcastAddr = getBroadcastAddress();
            if (broadcastAddr == null) broadcastAddr = "255.255.255.255";

            DatagramPacket packet = new DatagramPacket(
                    requestData,
                    requestData.length,
                    InetAddress.getByName(broadcastAddr),
                    DISCOVERY_PORT);
            socket.send(packet);

            byte[] buf = new byte[256];
            DatagramPacket response = new DatagramPacket(buf, buf.length);
            socket.receive(response);

            String resp = new String(response.getData(), 0, response.getLength(), "UTF-8").trim();
            if (resp.startsWith("EEG_DOCTOR:")) {
                String announcedIp = resp.substring("EEG_DOCTOR:".length()).trim();
                // 热点环境下优先使用 UDP 响应包的源地址，避免医生端枚举到错误网卡地址。
                String sourceIp = response.getAddress() == null ? null : response.getAddress().getHostAddress();
                if (sourceIp != null && !sourceIp.isEmpty()) return sourceIp;
                return announcedIp;
            }
        } catch (Exception e) {
            Log.d(TAG, "discovery: " + e.getClass().getSimpleName() + ": " + e.getMessage());
        } finally {
            if (socket != null) socket.close();
        }

        // 兼容部分 Android 热点不转发 UDP 广播：按患者端当前 IPv4 网段推测热点网关为 x.x.x.1。
        try {
            java.util.List<java.net.NetworkInterface> interfaces =
                    java.util.Collections.list(java.net.NetworkInterface.getNetworkInterfaces());
            for (java.net.NetworkInterface intf : interfaces) {
                for (java.net.InetAddress addr : java.util.Collections.list(intf.getInetAddresses())) {
                    if (!addr.isLoopbackAddress() && addr.getAddress().length == 4) {
                        String ip = addr.getHostAddress();
                        if (isPrivateIpv4(ip)) {
                            String[] parts = ip.split("\\.");
                            return parts[0] + "." + parts[1] + "." + parts[2] + ".1";
                        }
                    }
                }
            }
        } catch (Exception ignored) {}
        return null;
    }

    private String getBroadcastAddress() {
        try {
            for (java.net.NetworkInterface intf :
                    java.util.Collections.list(java.net.NetworkInterface.getNetworkInterfaces())) {
                for (java.net.InterfaceAddress interfaceAddress : intf.getInterfaceAddresses()) {
                    InetAddress broadcast = interfaceAddress.getBroadcast();
                    InetAddress address = interfaceAddress.getAddress();
                    if (broadcast != null && address != null
                            && !address.isLoopbackAddress()
                            && address.getAddress().length == 4
                            && isPrivateIpv4(address.getHostAddress())) {
                        return broadcast.getHostAddress();
                    }
                }
            }
        } catch (Exception ignored) {}
        return null;
    }

    private boolean isPrivateIpv4(String ip) {
        if (ip == null) return false;
        return ip.startsWith("10.")
                || ip.startsWith("192.168.")
                || ip.matches("172\\.(1[6-9]|2[0-9]|3[0-1])\\..*");
    }

    private Socket connectSocket(String ip, int port) throws Exception {
        Socket socket = new Socket();
        socket.connect(new InetSocketAddress(ip, port), CONNECT_TIMEOUT_MS);
        socket.setKeepAlive(true);
        socket.setTcpNoDelay(true);
        return socket;
    }

    /** 建立 41005、41004；41006 由永久控制监督线程独立维护。 */
    private void doConnectDataSession() {
        if (doctorIp == null || doctorIp.isEmpty()) return;

        Socket newCmdSocket = null;
        Socket newDataSocket = null;
        try {
            newCmdSocket = connectSocket(doctorIp, CMD_PORT);
            newDataSocket = connectSocket(doctorIp, DATA_PORT);

            synchronized (socketLock) {
                cmdSocket = newCmdSocket;
                dataSocket = newDataSocket;
                cmdOutput = newCmdSocket.getOutputStream();
            }

            commandConnected = true;
            dataConnected = true;
            sessionActive = true;
            Log.i(TAG, "CMD socket OK: " + doctorIp + ":" + CMD_PORT);
            Log.i(TAG, "DATA socket OK: " + doctorIp + ":" + DATA_PORT);
            updateReportedConnectionState();

            InputStream input = newDataSocket.getInputStream();
            FrameParser parser = new FrameParser();
            byte[] buf = new byte[1024];
            int len;
            while (autoConnecting && sessionActive && (len = input.read(buf)) != -1) {
                for (int i = 0; i < len; i++) parser.parse(buf[i] & 0xFF);
            }
            Log.w(TAG, "DATA connection closed by doctor");
        } catch (Exception e) {
            Log.e(TAG, "data session error: " + e.getClass().getSimpleName() + ": " + e.getMessage());
        } finally {
            endDataSession(newDataSocket, newCmdSocket);
        }
    }

    /**
     * 永久 41006 监督线程。它不会因一次控制连接失败而永久退出；
     * 只要患者仍在自动连接模式且 41004 已恢复，就会持续重连 41006。
     */
    private void startControlSupervisor() {
        if (!controlSupervisorRunning.compareAndSet(false, true)) return;

        new Thread(() -> {
            try {
                while (autoConnecting) {
                    if (!dataConnected || doctorIp == null || doctorIp.isEmpty()) {
                        sleepQuietly(300);
                        continue;
                    }
                    runSingleControlSession(doctorIp);
                    if (autoConnecting) sleepQuietly(CONTROL_RETRY_MS);
                }
            } finally {
                controlSupervisorRunning.set(false);
                Log.i(TAG, "control supervisor exited");
            }
        }, "patient-control-supervisor").start();
    }

    private void runSingleControlSession(String ip) {
        Socket socket = null;
        final long generation = controlGenerationCounter.incrementAndGet();
        try {
            socket = connectSocket(ip, D2P_CMD_PORT);
            socket.setSoTimeout(CONTROL_READ_TIMEOUT_MS);

            synchronized (socketLock) {
                if (!autoConnecting || !dataConnected) {
                    closeQuietly(socket);
                    return;
                }
                closeQuietly(d2pSocket);
                d2pSocket = socket;
                currentControlGeneration = generation;
            }

            // 此时只代表 TCP 已建立；收到医生端 CONTROL_READY 后才报告控制已连接。
            setControlConnectedIfCurrent(socket, generation, false);
            sendControlLine(socket, generation, "PATIENT_READY");
            Log.i(TAG, "D2P TCP connected, waiting CONTROL_READY: " + ip + ":" + D2P_CMD_PORT
                    + ", generation=" + generation);

            BufferedReader reader = new BufferedReader(
                    new InputStreamReader(socket.getInputStream(), "UTF-8"));
            while (autoConnecting && dataConnected && isCurrentControlSession(socket, generation)) {
                String line;
                try {
                    line = reader.readLine();
                } catch (SocketTimeoutException timeout) {
                    Log.w(TAG, "D2P heartbeat timeout, reconnecting generation=" + generation);
                    break;
                }
                if (line == null) {
                    Log.w(TAG, "D2P connection closed by doctor generation=" + generation);
                    break;
                }
                line = line.trim();
                if (line.isEmpty()) continue;
                lastControlRxAt = System.currentTimeMillis();
                Log.i(TAG, "D2P RECV: [" + line + "] generation=" + generation);
                parseD2pCommand(line, socket, generation);
            }
        } catch (Exception e) {
            if (autoConnecting && dataConnected) {
                Log.w(TAG, "D2P session error generation=" + generation + ": "
                        + e.getClass().getSimpleName() + ": " + e.getMessage());
            }
        } finally {
            clearControlSessionIfCurrent(socket, generation, "control session ended");
            closeQuietly(socket);
        }
    }

    private boolean isCurrentControlSession(Socket socket, long generation) {
        synchronized (socketLock) {
            return d2pSocket == socket && currentControlGeneration == generation;
        }
    }

    private void setControlConnectedIfCurrent(Socket socket, long generation, boolean connected) {
        boolean changed = false;
        synchronized (socketLock) {
            if (d2pSocket == socket && currentControlGeneration == generation) {
                if (controlConnected != connected) {
                    controlConnected = connected;
                    changed = true;
                }
            }
        }
        if (changed) updateReportedConnectionState();
    }

    private void clearControlSessionIfCurrent(Socket socket, long generation, String reason) {
        boolean wasCurrent = false;
        synchronized (socketLock) {
            if (d2pSocket == socket && currentControlGeneration == generation) {
                d2pSocket = null;
                controlConnected = false;
                wasCurrent = true;
            }
        }
        if (wasCurrent) {
            Log.w(TAG, "control channel cleared: " + reason + ", generation=" + generation);
            updateReportedConnectionState();
        }
    }

    private void invalidateCurrentControlSession(String reason) {
        Socket socket;
        boolean hadState;
        synchronized (socketLock) {
            socket = d2pSocket;
            d2pSocket = null;
            currentControlGeneration = controlGenerationCounter.incrementAndGet();
            hadState = controlConnected || socket != null;
            controlConnected = false;
        }
        closeQuietly(socket);
        if (hadState) {
            Log.w(TAG, "control channel invalidated: " + reason);
            updateReportedConnectionState();
        }
    }

    public void disconnect() {
        autoConnecting = false;
        doDisconnect();
    }

    private void doDisconnect() {
        sessionActive = false;
        invalidateCurrentControlSession("full disconnect");

        Socket localData;
        Socket localCmd;
        synchronized (socketLock) {
            localData = dataSocket;
            localCmd = cmdSocket;
            dataSocket = null;
            cmdSocket = null;
            cmdOutput = null;
        }
        closeQuietly(localData);
        closeQuietly(localCmd);

        dataConnected = false;
        commandConnected = false;
        updateReportedConnectionState();
    }

    private void endDataSession(Socket expectedData, Socket expectedCmd) {
        sessionActive = false;

        boolean clearData = false;
        synchronized (socketLock) {
            if (dataSocket == expectedData) {
                dataSocket = null;
                clearData = true;
            }
            if (cmdSocket == expectedCmd) {
                cmdSocket = null;
                cmdOutput = null;
            }
        }
        closeQuietly(expectedData);
        closeQuietly(expectedCmd);

        if (clearData) {
            dataConnected = false;
            commandConnected = false;
            invalidateCurrentControlSession("data channel ended");
            updateReportedConnectionState();
        }
    }

    private void closeQuietly(Socket socket) {
        if (socket == null) return;
        try { socket.close(); } catch (Exception ignored) {}
    }

    private void sleepQuietly(long millis) {
        try {
            Thread.sleep(millis);
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }

    public boolean sendCommand(String cmd) {
        if (cmd == null || cmd.trim().isEmpty()) return false;
        OutputStream output;
        synchronized (socketLock) {
            output = cmdOutput;
        }
        if (output == null || !commandConnected) return false;

        try {
            output.write((cmd.trim() + "\n").getBytes("UTF-8"));
            output.flush();
            return true;
        } catch (Exception e) {
            commandConnected = false;
            Log.e(TAG, "sendCommand failed: " + e.getMessage());
            return false;
        }
    }

    /** Send a patient feedback message over the bidirectional 41006 control socket. */
    public boolean sendControlMessage(String line) {
        if (line == null || line.trim().isEmpty() || !controlConnected) return false;
        final Socket socket;
        final long generation;
        synchronized (socketLock) {
            socket = d2pSocket;
            generation = currentControlGeneration;
        }
        if (socket == null || socket.isClosed()) return false;
        final String normalized = line.trim();
        controlReplyWriter.execute(() -> sendControlLine(socket, generation, normalized));
        return true;
    }

    private boolean sendControlLine(Socket socket, long generation, String line) {
        synchronized (d2pWriteLock) {
            if (!isCurrentControlSession(socket, generation) || socket.isClosed()) return false;
            try {
                OutputStream output = socket.getOutputStream();
                output.write((line + "\n").getBytes("UTF-8"));
                output.flush();
                return true;
            } catch (Exception e) {
                Log.w(TAG, "send control line failed: " + e.getMessage());
                return false;
            }
        }
    }

    private void updateReportedConnectionState() {
        boolean ready = isConnected();
        boolean data = dataConnected;
        boolean control = controlConnected;
        if (ready == lastReportedConnected
                && data == lastReportedDataConnected
                && control == lastReportedControlConnected) {
            return;
        }
        lastReportedConnected = ready;
        lastReportedDataConnected = data;
        lastReportedControlConnected = control;
        notifyConnectionChanged(ready);
    }

    private void notifyConnectionChanged(boolean connected) {
        Log.i(TAG, "notifyConnectionChanged: " + connected
                + ", data=" + dataConnected
                + ", control=" + controlConnected
                + ", listeners=" + listeners.size());
        for (DataListener listener : listeners) {
            try {
                listener.onConnectionChanged(connected);
            } catch (Exception e) {
                Log.w(TAG, "connection listener error: " + e.getMessage());
            }
        }
    }

    private void replayState(DataListener listener) {
        final boolean connectedSnapshot = isConnected();
        final int pageSnapshot = currentPatientPage;
        final boolean stateKnownSnapshot = ssvepStateKnown;
        final boolean activeSnapshot = ssvepActive;
        final int freqSnapshot = ssvepFreqIndex;

        mainHandler.post(() -> {
            if (!listeners.contains(listener)) return;
            try {
                listener.onConnectionChanged(connectedSnapshot);
                listener.onPageSwitch(pageSnapshot);
                if (stateKnownSnapshot) {
                    if (activeSnapshot && freqSnapshot >= 0) {
                        listener.onSsvepStart(freqSnapshot);
                    } else {
                        listener.onSsvepStop();
                    }
                }
            } catch (Exception e) {
                Log.w(TAG, "replayState error: " + e.getMessage());
            }
        });
    }

    private int mapDoctorPageToPatient(int doctorPage) {
        switch (doctorPage) {
            case 4: return 1;
            case 5: return 2;
            case 6: return 3;
            default: return 0;
        }
    }

    private void parseD2pCommand(String line, Socket socket, long generation) {
        if (line == null || line.isEmpty()) return;

        if (line.equals("CONTROL_READY")) {
            lastControlRxAt = System.currentTimeMillis();
            setControlConnectedIfCurrent(socket, generation, true);
            Log.i(TAG, "D2P handshake completed generation=" + generation);
            return;
        }

        if (line.equals("PING")) {
            sendControlLine(socket, generation, "PONG");
            return;
        }

        // 握手完成前不执行页面和刺激控制，避免误把半连接当作可用控制通道。
        if (!controlConnected || !isCurrentControlSession(socket, generation)) {
            Log.w(TAG, "ignore control command before handshake: " + line);
            return;
        }

        if (line.startsWith("PAGE,")) {
            try {
                int doctorPage = Integer.parseInt(line.substring("PAGE,".length()).trim());
                dispatchPageSwitch(mapDoctorPageToPatient(doctorPage), "D2P");
            } catch (NumberFormatException e) {
                Log.w(TAG, "invalid PAGE command: " + line);
            }
            return;
        }

        if (line.startsWith("SSVEP,START")) {
            int freqIndex = 0;
            String[] parts = line.split(",");
            if (parts.length >= 3) {
                try {
                    freqIndex = Integer.parseInt(parts[2].trim());
                } catch (NumberFormatException e) {
                    Log.w(TAG, "invalid SSVEP frequency index: " + line);
                    return;
                }
            }
            if (freqIndex < 0 || freqIndex > 3) {
                Log.w(TAG, "SSVEP frequency index out of range: " + freqIndex);
                return;
            }

            ssvepStateKnown = true;
            ssvepActive = true;
            ssvepFreqIndex = freqIndex;
            if (currentPatientPage != 1) dispatchPageSwitch(1, "SSVEP_START");

            final int finalFreqIndex = freqIndex;
            Log.i(TAG, ">>> D2P SSVEP,START freqIndex=" + finalFreqIndex);
            for (DataListener listener : listeners) {
                try { listener.onSsvepStart(finalFreqIndex); }
                catch (Exception e) { Log.w(TAG, "onSsvepStart listener error: " + e.getMessage()); }
            }
            return;
        }

        if (line.startsWith("SSVEP,RESULT,")) {
            PatientSsvepResult result = PatientSsvepResult.fromCommand(line);
            if (result == null) {
                Log.w(TAG, "invalid SSVEP result command: " + line);
                return;
            }
            for (DataListener listener : listeners) {
                try { listener.onSsvepResult(result); }
                catch (Exception e) { Log.w(TAG, "onSsvepResult listener error: " + e.getMessage()); }
            }
            return;
        }

        if (line.equals("SSVEP,STOP")) {
            ssvepStateKnown = true;
            ssvepActive = false;
            ssvepFreqIndex = -1;
            Log.i(TAG, ">>> D2P SSVEP,STOP");
            for (DataListener listener : listeners) {
                try { listener.onSsvepStop(); }
                catch (Exception e) { Log.w(TAG, "onSsvepStop listener error: " + e.getMessage()); }
            }
            return;
        }

        if (line.equals("READY_TRAIN")) {
            Log.i(TAG, ">>> D2P READY_TRAIN");
            for (DataListener listener : listeners) {
                try { listener.onReadyTrain(); }
                catch (Exception e) { Log.w(TAG, "onReadyTrain listener error: " + e.getMessage()); }
            }
            emitPageSwitch(2);
            return;
        }

        if (line.equals("READY_TEST")) {
            Log.i(TAG, ">>> D2P READY_TEST");
            for (DataListener listener : listeners) {
                try { listener.onReadyTest(); }
                catch (Exception e) { Log.w(TAG, "onReadyTest listener error: " + e.getMessage()); }
            }
            return;
        }

        if (line.startsWith("TASK,")) {
            if (line.startsWith("TASK,DONE") || line.startsWith("TASK,STOPPED")) {
                Log.i(TAG, ">>> D2P " + line);
                for (DataListener listener : listeners) {
                    try { listener.onTaskDone(); }
                    catch (Exception e) { Log.w(TAG, "onTaskDone listener error: " + e.getMessage()); }
                }
            } else {
                java.util.regex.Matcher matcher = java.util.regex.Pattern
                        .compile("TASK,(LEFT|RIGHT),start")
                        .matcher(line);
                if (matcher.matches()) {
                    final String side = matcher.group(1);
                    Log.i(TAG, ">>> D2P TASK," + side + ",start");
                    for (DataListener listener : listeners) {
                        try { listener.onTaskStart(side); }
                        catch (Exception e) { Log.w(TAG, "onTaskStart listener error: " + e.getMessage()); }
                    }
                    emitPageSwitch(2);
                }
            }
            return;
        }

        if (line.startsWith("MODE_SET_OK,")) {
            try {
                int mode = Integer.parseInt(line.substring("MODE_SET_OK,".length()).trim());
                Log.i(TAG, ">>> D2P MODE_SET_OK," + mode);
                for (DataListener listener : listeners) {
                    try { listener.onModeSetOk(mode); }
                    catch (Exception e) { Log.w(TAG, "onModeSetOk listener error: " + e.getMessage()); }
                }
                if (mode == 2) emitPageSwitch(3);
            } catch (NumberFormatException ignored) {}
            return;
        }

        if (line.startsWith("RESULT,")) {
            InferenceResult result = InferenceResult.fromResultLine(line);
            if (result != null) {
                Log.i(TAG, ">>> D2P RESULT");
                for (DataListener listener : listeners) {
                    try { listener.onInferenceResult(result); }
                    catch (Exception e) { Log.w(TAG, "onInferenceResult listener error: " + e.getMessage()); }
                }
                emitPageSwitch(3);
            }
            return;
        }
    }

    private void dispatchPageSwitch(int page, String source) {
        currentPatientPage = page;
        Log.i(TAG, ">>> " + source + " PAGE -> patient=" + page);
        for (DataListener listener : listeners) {
            try { listener.onPageSwitch(page); }
            catch (Exception e) { Log.w(TAG, "onPageSwitch listener error: " + e.getMessage()); }
        }
    }

    private void emitPageSwitch(int page) {
        dispatchPageSwitch(page, "DATA_EVENT");
    }

    /** 41004 只解析数据和下位机状态，不再承担 PAGE/SSVEP 控制。 */
    private class FrameParser {
        private static final int STATE_HEADER = 0;
        private static final int STATE_PAYLOAD = 1;
        private static final int STATE_ESCAPE = 2;

        private int state = STATE_HEADER;
        private final byte[] payload = new byte[1500];
        private int payloadLen;
        private final StringBuilder textLineBuf = new StringBuilder(512);

        void parse(int rawByte) {
            switch (state) {
                case STATE_HEADER:
                    if (rawByte == 0x7E) {
                        state = STATE_PAYLOAD;
                        payloadLen = 0;
                        textLineBuf.setLength(0);
                    } else if (rawByte == '\n' || rawByte == '\r') {
                        if (textLineBuf.length() > 0) {
                            parseTextLine(textLineBuf.toString().trim());
                            textLineBuf.setLength(0);
                        }
                    } else if (rawByte >= 0x20 && rawByte < 0x7F) {
                        if (textLineBuf.length() < 4096) {
                            textLineBuf.append((char) rawByte);
                        } else {
                            textLineBuf.setLength(0);
                        }
                    }
                    break;

                case STATE_PAYLOAD:
                    if (rawByte == 0x7D) {
                        state = STATE_ESCAPE;
                    } else if (rawByte == 0x7E) {
                        if (payloadLen >= 1) parseBinaryFrame();
                        payloadLen = 0;
                        textLineBuf.setLength(0);
                        state = STATE_HEADER;
                    } else if (payloadLen < payload.length) {
                        payload[payloadLen++] = (byte) rawByte;
                    } else {
                        payloadLen = 0;
                        state = STATE_HEADER;
                    }
                    break;

                case STATE_ESCAPE:
                    if (payloadLen < payload.length) {
                        payload[payloadLen++] = (byte) (rawByte ^ 0x20);
                        state = STATE_PAYLOAD;
                    } else {
                        payloadLen = 0;
                        state = STATE_HEADER;
                    }
                    break;

                default:
                    state = STATE_HEADER;
                    payloadLen = 0;
                    break;
            }
        }

        private void parseBinaryFrame() {
            int cmd = payload[0] & 0xFF;
            int loadLen = payloadLen - 1;

            if ((cmd == 0x04 || cmd == 0x10) && loadLen == 8) {
                ByteBuffer buf = ByteBuffer.wrap(payload, 1, 8).order(ByteOrder.LITTLE_ENDIAN);
                float ch0 = buf.getFloat();
                float ch1 = buf.getFloat();
                for (DataListener listener : listeners) {
                    listener.onWaveData(cmd, ch0, ch1);
                }
            } else if (cmd == 0x05 && loadLen == 18) {
                ByteBuffer buf = ByteBuffer.wrap(payload, 1, 16).order(ByteOrder.LITTLE_ENDIAN);
                float a0 = buf.getFloat();
                float a1 = buf.getFloat();
                float e0 = buf.getFloat();
                float e1 = buf.getFloat();
                int trend = payload[17] & 0xFF;
                int instant = payload[18] & 0xFF;
                for (DataListener listener : listeners) {
                    listener.onFocusData(a0, a1, e0, e1, trend, instant);
                }
            }
        }

        private void parseTextLine(String line) {
            if (line.isEmpty()) return;
            Log.d(TAG, "DATA RECV: [" + line + "]");

            if (line.startsWith("DIRCSV,")) {
                EegFrame frame = EegFrame.fromDirCsv(line);
                if (frame != null) {
                    for (DataListener listener : listeners) listener.onEegFrame(frame);
                }
            } else if (line.startsWith("RESULT,")) {
                InferenceResult result = InferenceResult.fromResultLine(line);
                if (result != null) {
                    for (DataListener listener : listeners) listener.onInferenceResult(result);
                }
                emitPageSwitch(3);
            } else if (line.startsWith("IPCDIAG,")) {
                IpcDiagInfo diag = IpcDiagInfo.fromLine(line);
                if (diag != null) {
                    for (DataListener listener : listeners) listener.onIpcDiag(diag);
                }
            } else if (line.startsWith("TASK,")) {
                if (line.startsWith("TASK,DONE") || line.startsWith("TASK,STOPPED")) {
                    for (DataListener listener : listeners) listener.onTaskDone();
                } else {
                    java.util.regex.Matcher matcher = java.util.regex.Pattern
                            .compile("TASK,(LEFT|RIGHT),start")
                            .matcher(line);
                    if (matcher.matches()) {
                        for (DataListener listener : listeners) {
                            listener.onTaskStart(matcher.group(1));
                        }
                        emitPageSwitch(2);
                    }
                }
            } else if (line.equals("READY_TRAIN")) {
                for (DataListener listener : listeners) listener.onReadyTrain();
                emitPageSwitch(2);
            } else if (line.equals("READY_TEST")) {
                for (DataListener listener : listeners) listener.onReadyTest();
            } else if (line.startsWith("MODE_SET_OK,")) {
                try {
                    int mode = Integer.parseInt(line.substring("MODE_SET_OK,".length()).trim());
                    for (DataListener listener : listeners) listener.onModeSetOk(mode);
                    if (mode == 2) emitPageSwitch(3);
                } catch (NumberFormatException ignored) {}
            }
        }
    }
}
