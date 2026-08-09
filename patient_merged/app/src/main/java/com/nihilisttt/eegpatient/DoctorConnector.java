package com.nihilisttt.eegpatient;

import android.os.Handler;
import android.os.Looper;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
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

 * 41004：医生端 -> 患者端，页面及 SSVEP 控制指令；
 * 41005：UDP 自动发现。
 *
 * v0.6.0：
 * 1. 41004 使用 CONTROL_READY 双向握手；
 * 2. 医生端每 2 秒发送 PING，患者端返回 PONG；
 * 3. 控制线程为永久监督线程，连接恢复后会自动重新建立 41004；
 * 4. 使用连接代次和 Socket 身份校验，旧线程不能覆盖新连接状态。
 */
public class DoctorConnector {
    private static final String TAG = "PATIENT";


    private static final int D2P_CMD_PORT = 41004;
    private static final int DISCOVERY_PORT = 41005;

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


    private Socket d2pSocket;

    private volatile boolean sessionActive;
    private volatile boolean autoConnecting;



    private volatile boolean controlConnected;
    private volatile long currentControlGeneration;
    private volatile long lastControlRxAt;

    private volatile boolean lastReportedConnected;

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
        default void onTargetDirection(String direction) {}
        default void onTrainStop() {}
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

    /** 完整连接：41004 已完成双向握手。 */
    public boolean isConnected() {
        return controlConnected;
    }

    public boolean isControlConnected() {
        return controlConnected;
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
                if (doctorIp == null || doctorIp.isEmpty()) {
                    String ip = discoverDoctorIp();
                    if (ip != null && !ip.trim().isEmpty()) {
                        doctorIp = ip.trim();
                        Log.i(TAG, "autoConnect: found doctor hotspot host at " + doctorIp);
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

    /**
     * 永久 41004 监督线程。它不会因一次控制连接失败而永久退出；
     * 只要患者仍在自动连接模式且 doctorIp 已知，就会持续重连 41004。
     */
    private void startControlSupervisor() {
        if (!controlSupervisorRunning.compareAndSet(false, true)) return;

        new Thread(() -> {
            try {
                while (autoConnecting) {
                    if (doctorIp == null || doctorIp.isEmpty()) {
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
                if (!autoConnecting) {
                    closeQuietly(socket);
                    return;
                }
                closeQuietly(d2pSocket);
                d2pSocket = socket;
                currentControlGeneration = generation;
            }

            // 此时只代表 TCP 已建立；收到医生端 CONTROL_READY 后才报告控制已连接。
            setControlConnectedIfCurrent(socket, generation, false);
            sendBinaryControl(socket, generation, EegProtocol.CMD_PATIENT_READY, null);
            Log.i(TAG, "D2P TCP connected, waiting CONTROL_READY: " + ip + ":" + D2P_CMD_PORT
                    + ", generation=" + generation);

            InputStream in = socket.getInputStream();
            byte[] buf = new byte[512];
            FrameDecoder decoder = new FrameDecoder(socket, generation);
            while (autoConnecting && isCurrentControlSession(socket, generation)) {
                int n;
                try {
                    n = in.read(buf);
                } catch (SocketTimeoutException timeout) {
                    Log.w(TAG, "D2P heartbeat timeout, reconnecting generation=" + generation);
                    break;
                } catch (IOException e) {
                    Log.w(TAG, "D2P read error generation=" + generation + ": " + e.getMessage());
                    break;
                }
                if (n < 0) {
                    Log.w(TAG, "D2P connection closed by doctor generation=" + generation);
                    break;
                }
                if (n == 0) continue;
                for (int i = 0; i < n; i++) {
                    decoder.parse(buf[i] & 0xFF);
                }
            }
        } catch (Exception e) {
            if (autoConnecting) {
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

        updateReportedConnectionState();
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

    /** Send a patient feedback message over the bidirectional 41004 control socket. */
    public boolean sendControlMessage(String line) {
        if (line == null || line.trim().isEmpty() || !controlConnected) return false;
        byte[] payload = controlTextToPayload(line);
        if (payload == null) {
            Log.w(TAG, "unsupported control message: " + line);
            return false;
        }
        final Socket socket;
        final long generation;
        synchronized (socketLock) {
            socket = d2pSocket;
            generation = currentControlGeneration;
        }
        if (socket == null || socket.isClosed()) return false;
        final int cmd = payload[0] & 0xFF;
        final byte[] data = new byte[payload.length - 1];
        System.arraycopy(payload, 1, data, 0, data.length);
        controlReplyWriter.execute(() -> sendBinaryControl(socket, generation, cmd, data));
        return true;
    }

    /** 患者端反馈文本 → 41004 上行二进制 payload（[cmd][data...]）。 */
    private static byte[] controlTextToPayload(String line) {
        String cmd = line == null ? "" : line.trim();
        if (cmd.equals("PONG")) return new byte[]{ (byte) EegProtocol.CMD_PONG };
        if (cmd.equals("PATIENT_READY")) return new byte[]{ (byte) EegProtocol.CMD_PATIENT_READY };
        if (cmd.startsWith("SSVEP,STIM_STARTED,")) {
            String[] parts = cmd.split(",");
            if (parts.length >= 4) {
                try {
                    int freqIndex = Integer.parseInt(parts[2].trim());
                    float refreshRate = Float.parseFloat(parts[3].trim());
                    ByteBuffer bb = ByteBuffer.allocate(5).order(ByteOrder.LITTLE_ENDIAN);
                    bb.put((byte) freqIndex);
                    bb.putFloat(refreshRate);
                    byte[] data = bb.array();
                    byte[] payload = new byte[data.length + 1];
                    payload[0] = (byte) EegProtocol.CMD_SSVEP_STIM_STARTED;
                    System.arraycopy(data, 0, payload, 1, data.length);
                    return payload;
                } catch (NumberFormatException e) {
                    return null;
                }
            }
        }
        return null;
    }

    private boolean sendBinaryControl(Socket socket, long generation, int cmd, byte[] data) {
        synchronized (d2pWriteLock) {
            if (!isCurrentControlSession(socket, generation) || socket.isClosed()) return false;
            try {
                int dLen = (data == null) ? 0 : data.length;
                byte[] payload = new byte[dLen + 1];
                payload[0] = (byte) cmd;
                if (dLen > 0) {
                    System.arraycopy(data, 0, payload, 1, dLen);
                }
                byte[] frame = EegProtocol.packFrame(EegProtocol.ADDR_PATIENT, cmd, payload);
                OutputStream output = socket.getOutputStream();
                output.write(frame);
                output.flush();
                return true;
            } catch (Exception e) {
                Log.w(TAG, "send control frame failed: " + e.getMessage());
                return false;
            }
        }
    }

    private void updateReportedConnectionState() {
        boolean ready = isConnected();
        boolean control = controlConnected;
        if (ready == lastReportedConnected
                && control == lastReportedControlConnected) {
            return;
        }
        lastReportedConnected = ready;
        lastReportedControlConnected = control;
        notifyConnectionChanged(ready);
    }

    private void notifyConnectionChanged(boolean connected) {
        Log.i(TAG, "notifyConnectionChanged: " + connected

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
            case 9: return 4;
            default: return 0;
        }
    }

    /** 41004 控制通道二进制帧解码器（医生端 → 患者端）。 */
    private class FrameDecoder {
        private static final int STATE_WAIT_AA = 0;
        private static final int STATE_WAIT_55 = 1;
        private static final int STATE_BODY = 2;
        private static final int STATE_ESCAPE = 3;

        private final Socket socket;
        private final long generation;
        private int state = STATE_WAIT_AA;
        private final byte[] body = new byte[256];
        private int bodyLen = 0;

        FrameDecoder(Socket socket, long generation) {
            this.socket = socket;
            this.generation = generation;
        }

        void parse(int rawByte) {
            switch (state) {
                case STATE_WAIT_AA:
                    if (rawByte == 0xAA) state = STATE_WAIT_55;
                    break;
                case STATE_WAIT_55:
                    if (rawByte == 0x55) {
                        state = STATE_BODY;
                        bodyLen = 0;
                    } else if (rawByte == 0xAA) {
                        state = STATE_WAIT_55;
                    } else {
                        state = STATE_WAIT_AA;
                    }
                    break;
                case STATE_BODY:
                    if (rawByte == 0x7D) {
                        state = STATE_ESCAPE;
                    } else if (rawByte == 0x7E) {
                        int minBody = EegProtocol.ADDR_LEN + EegProtocol.CMD_LEN
                                + EegProtocol.LEN_LEN + EegProtocol.TS_LEN + EegProtocol.CRC_LEN;
                        if (bodyLen >= minBody) {
                            dispatch();
                        }
                        state = STATE_WAIT_AA;
                        bodyLen = 0;
                    } else {
                        if (bodyLen < body.length) {
                            body[bodyLen++] = (byte) rawByte;
                        } else {
                            state = STATE_WAIT_AA;
                        }
                    }
                    break;
                case STATE_ESCAPE:
                    if (bodyLen < body.length) {
                        body[bodyLen++] = (byte) (rawByte ^ EegProtocol.ESCAPE_XOR);
                    }
                    state = STATE_BODY;
                    break;
            }
        }

        private void dispatch() {
            int cmd = body[1] & 0xFF;
            int payloadLen = EegProtocol.readU16LE(body, 2);
            int fixed = EegProtocol.ADDR_LEN + EegProtocol.CMD_LEN
                    + EegProtocol.LEN_LEN + EegProtocol.TS_LEN;
            if (bodyLen != fixed + payloadLen + EegProtocol.CRC_LEN) return;
            int calc = EegProtocol.checksum16(body, 0, fixed + payloadLen);
            int receivedCrc = EegProtocol.readU16LE(body, fixed + payloadLen);
            if (calc != receivedCrc) return;
            lastControlRxAt = System.currentTimeMillis();
            Log.i(TAG, "D2P RECV: cmd=0x" + Integer.toHexString(cmd) + " generation=" + generation);
            // 下行帧 payload 首字节为 cmd（对齐 MCU Parse_CommandBinary 约定），真实数据从 payload[1] 开始
            int dataLen = Math.max(0, payloadLen - 1);
            int dataOff = fixed + 1;
            handleFrame(cmd, dataLen, dataOff);
        }

        private void handleFrame(int cmd, int payloadLen, int off) {
            switch (cmd) {
                case EegProtocol.CMD_CONTROL_READY:
                    setControlConnectedIfCurrent(socket, generation, true);
                    Log.i(TAG, "D2P handshake completed generation=" + generation);
                    return;
                case EegProtocol.CMD_PING:
                    sendBinaryControl(socket, generation, EegProtocol.CMD_PONG, null);
                    return;
                default:
                    break;
            }

            // 握手完成前不执行页面和刺激控制，避免误把半连接当作可用控制通道。
            if (!controlConnected || !isCurrentControlSession(socket, generation)) {
                Log.w(TAG, "ignore control command before handshake: cmd=0x"
                        + Integer.toHexString(cmd));
                return;
            }

            switch (cmd) {
                case EegProtocol.CMD_PAGE:
                    if (payloadLen >= 1) {
                        dispatchPageSwitch(mapDoctorPageToPatient(body[off] & 0xFF), "D2P");
                    }
                    break;
                case EegProtocol.CMD_SSVEP_START_P:
                    handleSsvepStart(payloadLen >= 1 ? (body[off] & 0xFF) : 0);
                    break;
                case EegProtocol.CMD_SSVEP_RESULT: {
                    PatientSsvepResult result = decodeSsvepResult(payloadLen, off);
                    if (result != null) {
                        for (DataListener listener : listeners) {
                            try { listener.onSsvepResult(result); }
                            catch (Exception e) {
                                Log.w(TAG, "onSsvepResult listener error: " + e.getMessage());
                            }
                        }
                    }
                    break;
                }
                case EegProtocol.CMD_SSVEP_STOP_P:
                    handleSsvepStop();
                    break;
                case EegProtocol.CMD_READY_TRAIN:
                    if (ssvepActive) {
                        Log.i(TAG, "D2P READY_TRAIN ignored (SSVEP active)");
                        break;
                    }
                    Log.i(TAG, ">>> D2P READY_TRAIN");
                    for (DataListener listener : listeners) {
                        try { listener.onReadyTrain(); }
                        catch (Exception e) {
                            Log.w(TAG, "onReadyTrain listener error: " + e.getMessage());
                        }
                    }
                    emitPageSwitch(2);
                    break;
                case EegProtocol.CMD_TRAIN_STOP:
                    Log.i(TAG, ">>> D2P TRAIN_STOP");
                    for (DataListener listener : listeners) {
                        try { listener.onTrainStop(); }
                        catch (Exception e) {
                            Log.w(TAG, "onTrainStop listener error: " + e.getMessage());
                        }
                    }
                    break;
                case EegProtocol.CMD_READY_TEST:
                    Log.i(TAG, ">>> D2P READY_TEST");
                    for (DataListener listener : listeners) {
                        try { listener.onReadyTest(); }
                        catch (Exception e) {
                            Log.w(TAG, "onReadyTest listener error: " + e.getMessage());
                        }
                    }
                    break;
                case EegProtocol.CMD_TASK_START:
                    if (ssvepActive) {
                        Log.i(TAG, "D2P TASK_START ignored (SSVEP active)");
                        break;
                    }
                    if (payloadLen >= 1) {
                        final String side = (body[off] & 0xFF) == 0 ? "LEFT" : "RIGHT";
                        Log.i(TAG, ">>> D2P TASK," + side + ",start");
                        for (DataListener listener : listeners) {
                            try { listener.onTaskStart(side); }
                            catch (Exception e) {
                                Log.w(TAG, "onTaskStart listener error: " + e.getMessage());
                            }
                        }
                        emitPageSwitch(2);
                    }
                    break;
                case EegProtocol.CMD_TASK_DONE:
                case EegProtocol.CMD_TASK_STOPPED:
                    Log.i(TAG, ">>> D2P " + (cmd == EegProtocol.CMD_TASK_DONE
                            ? "TASK,DONE" : "TASK,STOPPED"));
                    for (DataListener listener : listeners) {
                        try { listener.onTaskDone(); }
                        catch (Exception e) {
                            Log.w(TAG, "onTaskDone listener error: " + e.getMessage());
                        }
                    }
                    break;
                case EegProtocol.CMD_MODE_SET_OK:
                    if (payloadLen >= 1) {
                        int mode = body[off] & 0xFF;
                        Log.i(TAG, ">>> D2P MODE_SET_OK," + mode);
                        for (DataListener listener : listeners) {
                            try { listener.onModeSetOk(mode); }
                            catch (Exception e) {
                                Log.w(TAG, "onModeSetOk listener error: " + e.getMessage());
                            }
                        }
                        if (mode == 2 && !ssvepActive) emitPageSwitch(3);
                    }
                    break;
                case EegProtocol.CMD_TARGET:
                    if (payloadLen >= 1) {
                        String dir = (body[off] & 0xFF) == 0 ? "LEFT" : "RIGHT";
                        Log.i(TAG, ">>> D2P TARGET," + dir);
                        for (DataListener listener : listeners) {
                            try { listener.onTargetDirection(dir); }
                            catch (Exception e) {
                                Log.w(TAG, "onTargetDirection listener error: " + e.getMessage());
                            }
                        }
                    }
                    break;
                case EegProtocol.CMD_RESULT_MI: {
                    if (ssvepActive) {
                        Log.i(TAG, "D2P RESULT_MI ignored (SSVEP active)");
                        break;
                    }
                    InferenceResult result = decodeResultMi(payloadLen, off);
                    if (result != null) {
                        Log.i(TAG, ">>> D2P RESULT");
                        for (DataListener listener : listeners) {
                            try { listener.onInferenceResult(result); }
                            catch (Exception e) {
                                Log.w(TAG, "onInferenceResult listener error: " + e.getMessage());
                            }
                        }
                        emitPageSwitch(3);
                    }
                    break;
                }
                default:
                    Log.i(TAG, "D2P unhandled cmd=0x" + Integer.toHexString(cmd)
                            + " len=" + payloadLen);
                    break;
            }
        }

        private InferenceResult decodeResultMi(int payloadLen, int off) {
            if (payloadLen < 16) return null;
            int pred = body[off + 1] & 0xFF;
            ByteBuffer buf = ByteBuffer.wrap(body, off + 2, 12).order(ByteOrder.LITTLE_ENDIAN);
            int scoreL = buf.getInt();
            int scoreR = buf.getInt();
            int conf = buf.getInt();
            boolean trained = (body[off + 14] & 0xFF) == 1;
            InferenceResult result = new InferenceResult();
            result.setIntent(pred == 0 ? "LEFT" : "RIGHT");
            result.setScoreLeft(scoreL / 10000f);
            result.setScoreRight(scoreR / 10000f);
            result.setConfidence(conf / 10000f);
            result.setTrained(trained);
            return result;
        }

        private PatientSsvepResult decodeSsvepResult(int payloadLen, int off) {
            if (payloadLen < EegProtocol.SSVEP_RESULT_PAYLOAD) return null;
            int seq = (int) EegProtocol.readU32LE(body, off);
            int freqIdx = body[off + 4] & 0xFF;
            int rawIdx = body[off + 5] & 0xFF;
            ByteBuffer buf = ByteBuffer.wrap(body, off + 6, 24).order(ByteOrder.LITTLE_ENDIAN);
            float ratio = buf.getFloat();
            float margin = buf.getFloat();
            float[] scores = new float[4];
            for (int i = 0; i < 4; i++) scores[i] = buf.getFloat();
            int[] votes = new int[4];
            for (int i = 0; i < 4; i++) votes[i] = body[off + 30 + i] & 0xFF;
            boolean synthetic = (body[off + 34] & 0xFF) == 1;
            boolean usable = (body[off + 35] & 0xFF) == 1;
            String freq = freqIndexToText(freqIdx);
            String raw = freqIndexToText(rawIdx);
            return new PatientSsvepResult(seq, freq, raw, ratio, margin,
                    scores, votes, synthetic, usable);
        }

        private String freqIndexToText(int idx) {
            if (idx == 0xFF) return "UNCERTAIN";
            String[] freqs = {"11.00", "13.00", "15.00", "17.00"};
            if (idx >= 0 && idx < freqs.length) return freqs[idx];
            return "UNCERTAIN";
        }

        private void handleSsvepStart(int freqIndex) {
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
        }

        private void handleSsvepStop() {
            ssvepStateKnown = true;
            ssvepActive = false;
            ssvepFreqIndex = -1;
            Log.i(TAG, ">>> D2P SSVEP,STOP");
            for (DataListener listener : listeners) {
                try { listener.onSsvepStop(); }
                catch (Exception e) { Log.w(TAG, "onSsvepStop listener error: " + e.getMessage()); }
            }
        }
    }

    private void dispatchPageSwitch(int page, String source) {
        if (currentPatientPage == page) return;
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


}
