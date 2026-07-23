package com.nihilisttt.eegdoctor;

import android.os.Handler;
import android.os.Looper;
import android.os.NetworkOnMainThreadException;
import android.util.Log;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Enumeration;
import java.util.List;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.RejectedExecutionException;
import java.util.concurrent.atomic.AtomicLong;
import java.util.concurrent.atomic.AtomicBoolean;

public class TcpServerManager {
    private static final int TCP_PORT = 41002;
    private static final int FORWARD_PORT = 41003;
    private static final int DOCTOR_TO_PATIENT_PORT = 41004;
    private static final int DISCOVERY_PORT = 41005;
    private static final long PATIENT_HEARTBEAT_INTERVAL_MS = 2000L;
    private static final long PATIENT_HEARTBEAT_TIMEOUT_MS = 7000L;
    private static final TcpServerManager INSTANCE = new TcpServerManager();

    private ServerSocket serverSocket;
    private boolean running;
    private DataDispatcher dispatcher = DataDispatcher.getInstance();

    private final UdpSender udpSender = new UdpSender();
    private ServerSocket forwardServerSocket;
    private TcpForwardManager forwardManager;


    private ServerSocket doctorToPatientServerSocket;
    private Socket deviceClient;
    private Socket doctorToPatientClient;
    private DatagramSocket discoverySocket;

    private final CopyOnWriteArrayList<ConnectionListener> connectionListeners = new CopyOnWriteArrayList<>();


    private final AtomicLong patientControlGenerationCounter = new AtomicLong(0);
    private volatile long currentPatientControlGeneration;
    private volatile long patientLastPongAt;
    private volatile boolean patientControlReady;
    private Thread patientControlHeartbeatThread;
    private final Object d2pWriteLock = new Object();
    private final Handler mainHandler = new Handler(Looper.getMainLooper());
    private final ExecutorService patientControlWriter = Executors.newSingleThreadExecutor(r -> {
        Thread thread = new Thread(r, "doctor-patient-control-writer");
        thread.setDaemon(true);
        return thread;
    });

    public interface PatientSendCallback {
        void onResult(boolean success);
    }

    private volatile boolean ssvepActive = false;
    private volatile int ssvepFreqIndex = -1;
    private volatile int currentDoctorPage = 0;

    public void setSsvepActive(boolean active, int freqIndex) {
        this.ssvepActive = active;
        this.ssvepFreqIndex = freqIndex;
        Log.i("TCP", "setSsvepActive: " + active + " freqIndex=" + freqIndex);
    }

    public boolean isSsvepActive() { return ssvepActive; }
    public int getSsvepFreqIndex() { return ssvepFreqIndex; }

    private Thread senderThread;
    private final BlockingQueue<byte[]> sendQueue = new ArrayBlockingQueue<>(256);

    public interface ConnectionListener {
        void onDeviceConnected(boolean connected);
        void onPatientConnected(boolean connected);
    }

    public void addConnectionListener(ConnectionListener l) { connectionListeners.add(l); }
    public void removeConnectionListener(ConnectionListener l) { connectionListeners.remove(l); }

    public boolean isDeviceConnected() {
        return deviceClient != null && !deviceClient.isClosed() && deviceClient.isConnected();
    }

    /**
     * 患者端是否已具备可用的医生->患者控制通道。

     */
    public boolean isPatientConnected() {
        return isPatientControlConnected();
    }

    public boolean isPatientControlConnected() {
        Socket client;
        boolean ready;
        long lastPong;
        synchronized (this) {
            client = doctorToPatientClient;
            ready = patientControlReady;
            lastPong = patientLastPongAt;
        }
        return client != null
                && !client.isClosed()
                && client.isConnected()
                && ready
                && System.currentTimeMillis() - lastPong <= PATIENT_HEARTBEAT_TIMEOUT_MS;
    }


    public static TcpServerManager getInstance() { return INSTANCE; }

    // ========== 新增UDP配置接口 ==========
    /** 设置Python上位机IP，开启UDP发送 */
    public void configUdpForPython(String pythonIp) {
        udpSender.setTargetIp(pythonIp);
        udpSender.setEnableUdp(true);
        Log.i("UDP", "UDP已启用，目标IP：" + pythonIp);
    }

    /** 关闭UDP发送 */
    public void disableUdp() {
        udpSender.setEnableUdp(false);
        Log.i("UDP", "UDP已关闭");
    }

    public void start() {
        if (running) return;
        running = true;
        // 启动原有主服务器
        new Thread(() -> {
            try {
                serverSocket = new ServerSocket(TCP_PORT);
                Log.i("TCP", "Main server started on port " + TCP_PORT);
                while (running) {
                    Socket client = serverSocket.accept();
                    Log.i("TCP", "Main client connected: " + client.getRemoteSocketAddress());
                    new Thread(() -> handleClient(client)).start();
                }
            } catch (Exception e) { Log.e("TCP", "Main server error", e); }
        }).start();

        // 启动转发服务器
        forwardManager = new TcpForwardManager();
        new Thread(() -> {
            try {
                forwardServerSocket = new ServerSocket(FORWARD_PORT);
                Log.i("TCP", "Forward server started on port " + FORWARD_PORT);
                while (running) {
                    Socket forwardClient = forwardServerSocket.accept();
                    Log.i("TCP", "Forward client connected: " + forwardClient.getRemoteSocketAddress());
                    forwardManager.addClient(forwardClient);
                }
            } catch (Exception e) { Log.e("TCP", "Forward server error", e); }
        }).start();



        // 启动医生端→患者端专用控制通道服务器。
        // 患者端连接后必须先发送 PATIENT_READY，医生端回 CONTROL_READY，
        // 双向握手完成后才将控制通道标记为已连接。
        new Thread(() -> {
            try {
                doctorToPatientServerSocket = new ServerSocket(DOCTOR_TO_PATIENT_PORT);
                Log.i("TCP", "Doctor-to-patient cmd server started on port " + DOCTOR_TO_PATIENT_PORT);
                while (running) {
                    Socket client = doctorToPatientServerSocket.accept();
                    client.setKeepAlive(true);
                    client.setTcpNoDelay(true);
                    final long generation = patientControlGenerationCounter.incrementAndGet();

                    Socket oldClient;
                    boolean oldWasReady;
                    synchronized (TcpServerManager.this) {
                        oldClient = doctorToPatientClient;
                        oldWasReady = patientControlReady;
                        doctorToPatientClient = client;
                        currentPatientControlGeneration = generation;
                        patientControlReady = false;
                        patientLastPongAt = System.currentTimeMillis();
                    }
                    if (oldClient != null && oldClient != client) {
                        try { oldClient.close(); } catch (Exception ignored) {}
                    }
                    if (oldWasReady) notifyPatientConnected(false);

                    Log.i("DOCTOR", ">>> Patient D2P TCP connected: "
                            + client.getRemoteSocketAddress() + ", generation=" + generation);

                    new Thread(() -> handlePatientControlClient(client, generation),
                            "doctor-d2p-reader-" + generation).start();
                }
            } catch (Exception e) {
                if (running) Log.e("TCP", "Doctor-to-patient server error", e);
            }
        }, "doctor-d2p-server").start();

        startPatientControlHeartbeat();

        // 启动UDP发现响应服务
        new Thread(() -> {
            try {
                discoverySocket = new DatagramSocket(DISCOVERY_PORT);
                discoverySocket.setBroadcast(true);
                Log.i("UDP", "Discovery server started on port " + DISCOVERY_PORT);
                byte[] buf = new byte[256];
                while (running) {
                    DatagramPacket packet = new DatagramPacket(buf, buf.length);
                    discoverySocket.receive(packet);
                    String msg = new String(packet.getData(), 0, packet.getLength()).trim();
                    if ("EEG_DOCTOR_DISCOVERY".equals(msg)) {
                        String localIp = getLocalHotspotIp();
                        String response = "EEG_DOCTOR:" + localIp;
                        byte[] respBytes = response.getBytes("UTF-8");
                        DatagramPacket respPacket = new DatagramPacket(
                                respBytes, respBytes.length,
                                packet.getAddress(), packet.getPort());
                        discoverySocket.send(respPacket);
                        Log.i("UDP", "Discovery response sent to " + packet.getAddress().getHostAddress());
                    }
                }
            } catch (Exception e) {
                if (running) Log.e("UDP", "Discovery server error", e);
            }
        }).start();
    }

    public void stop() {
        running = false;
        try { if (serverSocket != null) serverSocket.close(); } catch (Exception ignored) {}
        try { if (forwardServerSocket != null) forwardServerSocket.close(); } catch (Exception ignored) {}

        try { if (doctorToPatientServerSocket != null) doctorToPatientServerSocket.close(); } catch (Exception ignored) {}
        try { if (discoverySocket != null) discoverySocket.close(); } catch (Exception ignored) {}
        try { if (deviceClient != null) deviceClient.close(); } catch (Exception ignored) {}
        try { if (doctorToPatientClient != null) doctorToPatientClient.close(); } catch (Exception ignored) {}
        deviceClient = null;
        synchronized (this) {
            doctorToPatientClient = null;
            patientControlReady = false;
            currentPatientControlGeneration = patientControlGenerationCounter.incrementAndGet();
        }
        stopPatientControlHeartbeat();
        if (forwardManager != null) forwardManager.stopAll();

        udpSender.release();
        stopHeartbeat();
    }

    private void startHeartbeat() {
        startSender();
    }

    private void stopHeartbeat() {
        stopSender();
    }

    private void startSender() {
        stopSender();
        senderThread = new Thread(() -> {
            Log.i("TCP_SENDER", "Sender thread started");
            while (!Thread.currentThread().isInterrupted() && running) {
                try {
                    byte[] data = sendQueue.take();
                    Socket client = deviceClient;
                    if (client != null && !client.isClosed() && client.isConnected()) {
                        OutputStream os = client.getOutputStream();
                        os.write(data);
                        os.flush();
                        Log.i("TCP_SENDER", "Sent " + data.length + " bytes to " + client.getRemoteSocketAddress());
                    } else {
                        Log.w("TCP_SENDER", "Device not connected, dropping " + data.length + " bytes");
                    }
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                    break;
                } catch (Exception e) {
                    Log.e("TCP_SENDER", "Send failed", e);
                }
            }
            Log.i("TCP_SENDER", "Sender thread stopped");
        });
        senderThread.setDaemon(true);
        senderThread.start();
    }

    private void stopSender() {
        if (senderThread != null) {
            senderThread.interrupt();
            senderThread = null;
        }
        sendQueue.clear();
    }

    private int mLastProcessedSeq = -1;

    private boolean isDuplicateSeq(String line) {
        java.util.regex.Matcher m = java.util.regex.Pattern.compile("seq=(\\d+)").matcher(line);
        if (m.find()) {
            int seq = Integer.parseInt(m.group(1));
            if (seq == mLastProcessedSeq) return true;
            mLastProcessedSeq = seq;
        }
        return false;
    }

    private void sendAckForSeq(String line) {
        java.util.regex.Matcher m = java.util.regex.Pattern.compile("seq=(\\d+)").matcher(line);
        if (m.find()) {
            String ack = "ACK," + m.group(1);
            sendToDevice(ack);
        }
    }

    private void parseMcuTextResponses(String data) {
        for (String line : data.split("[\r\n]+")) {
            line = line.trim();
            if (line.isEmpty()) continue;
            switch (line) {
                case "READY_TRAIN":
                    Log.i("MCU_RESP", "READY_TRAIN");
                    dispatcher.postReadyTrain();
                    break;
                case "READY_TEST":
                    Log.i("MCU_RESP", "READY_TEST");
                    dispatcher.postReadyTest();
                    break;
                case "TASK,STOPPED":
                    Log.i("MCU_RESP", "TASK,STOPPED");
                    dispatcher.postTaskDone();
                    break;
                default:
                    if (line.startsWith("TASK,DONE")) {
                        Log.i("MCU_RESP", "TASK,DONE: " + line);
                        sendAckForSeq(line);
                        if (!isDuplicateSeq(line)) {
                            dispatcher.postTaskDone();
                        }
                    } else if (line.startsWith("MODE_SET_OK,")) {
                        try {
                            int mode = Integer.parseInt(line.substring(12).trim());
                            Log.i("MCU_RESP", "MODE_SET_OK," + mode);
                            dispatcher.postModeSetOk(mode);
                        } catch (NumberFormatException ignored) {}
                    } else if (line.startsWith("TASK,") && line.endsWith(",start")) {
                        String side = line.substring(5, line.length() - 6);
                        Log.i("MCU_RESP", "TASK," + side + ",start");
                        dispatcher.postTaskStart(side);
                    } else if (line.startsWith("TURN_EVENT,")) {
                        sendAckForSeq(line);
                        java.util.regex.Matcher m = java.util.regex.Pattern.compile("TURN_EVENT,(?:seq=\\d+,)?count=\\d+,from=(\\w+),to=(\\w+)").matcher(line);
                        if (m.find()) {
                            Log.i("MCU_RESP", "TURN_EVENT: " + m.group(1) + " -> " + m.group(2));
                            dispatcher.postTurnEvent(m.group(1), m.group(2));
                        }
                    } else if (line.startsWith("FALL_EVENT,")) {
                        sendAckForSeq(line);
                        Log.i("MCU_RESP", "FALL_EVENT");
                        dispatcher.postFallEvent();
                    } else if (line.startsWith("NO_TURN_ALERT,")) {
                        sendAckForSeq(line);
                        java.util.regex.Matcher m = java.util.regex.Pattern.compile("NO_TURN_ALERT,(?:seq=\\d+,)?duration_min=(\\d+)").matcher(line);
                        if (m.find()) {
                            long dur = Long.parseLong(m.group(1));
                            Log.i("MCU_RESP", "NO_TURN_ALERT: " + dur + " min");
                            dispatcher.postNoTurnAlert(dur);
                        }
                    } else if (line.startsWith("POSTURE_STATE,")) {
                        sendAckForSeq(line);
                        java.util.regex.Matcher m = java.util.regex.Pattern.compile("POSTURE_STATE,(?:seq=\\d+,)?(\\w+),turns=(\\d+)").matcher(line);
                        if (m.find()) {
                            Log.i("MCU_RESP", "POSTURE_STATE: " + m.group(1) + " turns=" + m.group(2));
                            dispatcher.postPostureState(m.group(1), Integer.parseInt(m.group(2)));
                        }
                    } else if (line.startsWith("MODE_DIAG,")) {
                        Log.i("MCU_RESP", "MODE_DIAG: " + line);
                    } else if (line.startsWith("V5F_DIAG,")) {
                        Log.i("MCU_RESP", "V5F_DIAG: " + line);
                    } else if (line.startsWith("V5F_RAW,")) {
                        Log.i("MCU_RESP", "V5F_RAW: " + line);
                    } else if (line.startsWith("RESULT,")) {
                        sendAckForSeq(line);
                        if (!isDuplicateSeq(line)) {
                            InferenceResult result = InferenceResult.fromResultLine(line);
                            if (result != null) {
                                Log.i("MCU_RESP", "RESULT: INTENT=" + result.getIntent() + " CONF=" + result.getConfidence());
                                dispatcher.postInferenceResult(result);
                            }
                        }
                    }
                    break;
            }
        }
    }

    private void notifyDeviceConnected(boolean connected) {
        for (ConnectionListener l : connectionListeners) {
            try { l.onDeviceConnected(connected); } catch (Exception ignored) {}
        }
    }

    private void notifyPatientConnected(boolean connected) {
        for (ConnectionListener l : connectionListeners) {
            try { l.onPatientConnected(connected); } catch (Exception ignored) {}
        }
    }

    /**
     * 获取本机热点 IP（通常为 192.168.43.1 或类似）
     */
    public String getLocalHotspotIp() {
        try {
            Enumeration<NetworkInterface> interfaces = NetworkInterface.getNetworkInterfaces();
            for (NetworkInterface intf : Collections.list(interfaces)) {
                if (intf.getName().contains("wlan") || intf.getName().contains("ap")) {
                    Enumeration<InetAddress> addresses = intf.getInetAddresses();
                    while (addresses.hasMoreElements()) {
                        InetAddress addr = addresses.nextElement();
                        if (!addr.isLoopbackAddress() && addr.getAddress().length == 4) {
                            String ip = addr.getHostAddress();
                            if (ip.startsWith("192.168.")) {
                                return ip;
                            }
                        }
                    }
                }
            }
        } catch (SocketException e) {
            Log.e("TCP", "Error getting hotspot IP", e);
        }
        return "192.168.43.1"; // 默认回退
    }

    public int getForwardPort() {
        return FORWARD_PORT;
    }



    public void sendToDevice(String command) {
        if (deviceClient == null || deviceClient.isClosed() || !deviceClient.isConnected()) {
            Log.w("TCP", "Cannot send to device (not connected): [" + command + "]");
            return;
        }
        try {
            byte[] data = (command + "\n").getBytes("UTF-8");
            sendQueue.offer(data);
            Log.i("TCP", "Queued to device: [" + command + "] bytes=" + data.length);
        } catch (Exception e) {
            Log.e("TCP", "Failed to queue command: [" + command + "]", e);
        }
    }

    // ========== V2帧下行发送（Android→ESP8266，无CRC） ==========
    // 格式: 0x7E [VER=0x01] [CMD] [LEN_LO] [LEN_HI] [PAYLOAD] 0x7E
    private static final byte V2_FRAME_CHAR = 0x7E;
    private static final byte V2_ESCAPE_CHAR = 0x7D;
    private static final byte V2_ESCAPE_XOR = 0x20;

    public void sendV2FrameToDevice(byte cmd, byte[] payload) {
        if (deviceClient == null || deviceClient.isClosed() || !deviceClient.isConnected()) {
            Log.w("TCP", "Cannot send v2 frame (not connected)");
            return;
        }
        try {
            byte[] frame = packV2Frame(cmd, payload);
            sendQueue.offer(frame);
            Log.i("TCP", "Queued v2 frame: cmd=0x" + String.format("%02X", cmd) + " len=" + payload.length);
        } catch (Exception e) {
            Log.e("TCP", "Failed to queue v2 frame", e);
        }
    }

    private static byte[] packV2Frame(byte cmd, byte[] payload) {
        int payLen = payload.length;
        byte[] buf = new byte[2 + 4 * 2 + payLen * 2 + 2];
        int idx = 0;
        buf[idx++] = V2_FRAME_CHAR;
        idx = v2EscapeWrite(buf, idx, (byte) 0x01);
        idx = v2EscapeWrite(buf, idx, cmd);
        idx = v2EscapeWrite(buf, idx, (byte) (payLen & 0xFF));
        idx = v2EscapeWrite(buf, idx, (byte) ((payLen >> 8) & 0xFF));
        for (byte b : payload) idx = v2EscapeWrite(buf, idx, b);
        buf[idx++] = V2_FRAME_CHAR;
        byte[] result = new byte[idx];
        System.arraycopy(buf, 0, result, 0, idx);
        return result;
    }

    private static int v2EscapeWrite(byte[] buf, int idx, byte b) {
        if ((b & 0xFF) == (V2_FRAME_CHAR & 0xFF) || (b & 0xFF) == (V2_ESCAPE_CHAR & 0xFF)) {
            buf[idx++] = V2_ESCAPE_CHAR;
            buf[idx++] = (byte) ((b & 0xFF) ^ (V2_ESCAPE_XOR & 0xFF));
        } else {
            buf[idx++] = b;
        }
        return idx;
    }

    public void sendWifiAdd(String ssid, String password) {
        byte[] ssidBytes = ssid.getBytes(java.nio.charset.StandardCharsets.UTF_8);
        byte[] pwdBytes = password.getBytes(java.nio.charset.StandardCharsets.UTF_8);
        byte[] payload = new byte[2 + ssidBytes.length + 1 + pwdBytes.length + 1];
        int off = 0;
        payload[off++] = (byte) 0xF0;
        payload[off++] = 0x01;
        System.arraycopy(ssidBytes, 0, payload, off, ssidBytes.length); off += ssidBytes.length;
        payload[off++] = 0;
        System.arraycopy(pwdBytes, 0, payload, off, pwdBytes.length); off += pwdBytes.length;
        payload[off++] = 0;
        byte[] trimmed = new byte[off];
        System.arraycopy(payload, 0, trimmed, 0, off);
        sendV2FrameToDevice((byte) 0xF0, trimmed);
    }

    public void sendWifiDelete(String ssid) {
        byte[] ssidBytes = ssid.getBytes(java.nio.charset.StandardCharsets.UTF_8);
        byte[] payload = new byte[2 + ssidBytes.length + 1];
        int off = 0;
        payload[off++] = (byte) 0xF0;
        payload[off++] = 0x02;
        System.arraycopy(ssidBytes, 0, payload, off, ssidBytes.length); off += ssidBytes.length;
        payload[off++] = 0;
        byte[] trimmed = new byte[off];
        System.arraycopy(payload, 0, trimmed, 0, off);
        sendV2FrameToDevice((byte) 0xF0, trimmed);
    }

    /**
     * 页面和 SSVEP 控制命令只允许走 41004。

     *
     * 该方法只负责将命令加入专用后台发送队列，不在调用线程执行网络 I/O。
     * 因此可以安全地从 Activity/Fragment 的主线程调用。
     *
     * @return true 表示命令已进入发送队列；false 表示命令无效或队列不可用。
     */
    public boolean sendToPatient(String command) {
        return enqueuePatientCommand(command, null);
    }

    /**
     * 异步发送患者控制命令，并在主线程回调实际写入结果。
     * 控制连接短暂重连时，后台任务会在 3 秒内等待新连接并重试。
     */
    public boolean sendToPatientAsync(String command, PatientSendCallback callback) {
        return enqueuePatientCommand(command, callback);
    }

    private boolean enqueuePatientCommand(String command, PatientSendCallback callback) {
        if (command == null || command.trim().isEmpty()) {
            postPatientSendResult(callback, false);
            return false;
        }

        final String normalized = command.trim();
        // 即使当前控制通道暂未连接，也保存期望状态。患者重连后的状态快照会恢复页面/SSVEP状态。
        updatePatientControlState(normalized);

        // 未完成握手时不把命令堆积在队列中；期望状态已经保存，重连后的状态快照会自动同步。
        if (!hasReadyPatientControlSocket()) {
            Log.w("DOCTOR", ">>> D2P NOT ENQUEUED (control not ready): [" + normalized + "]");
            postPatientSendResult(callback, false);
            return false;
        }

        Log.i("DOCTOR", ">>> D2P ENQUEUE: [" + normalized + "]");
        try {
            patientControlWriter.execute(() -> {
                boolean success = sendQueuedPatientCommand(normalized, 3000L);
                postPatientSendResult(callback, success);
            });
            return true;
        } catch (RejectedExecutionException e) {
            Log.e("DOCTOR", ">>> D2P QUEUE REJECTED: [" + normalized + "]", e);
            postPatientSendResult(callback, false);
            return false;
        }
    }

    private boolean hasReadyPatientControlSocket() {
        synchronized (this) {
            return doctorToPatientClient != null
                    && !doctorToPatientClient.isClosed()
                    && doctorToPatientClient.isConnected()
                    && patientControlReady;
        }
    }

    private void postPatientSendResult(PatientSendCallback callback, boolean success) {
        if (callback != null) {
            mainHandler.post(() -> callback.onResult(success));
        }
    }

    /**
     * 只在 doctor-patient-control-writer 后台线程执行。
     * 若连接正处于重连阶段，等待当前 generation 的可用控制通道，而不是立即判定失败。
     */
    private boolean sendQueuedPatientCommand(String command, long waitTimeoutMs) {
        long deadline = System.currentTimeMillis() + Math.max(0L, waitTimeoutMs);

        while (running && !Thread.currentThread().isInterrupted()) {
            Socket client;
            long generation;
            boolean ready;
            synchronized (this) {
                client = doctorToPatientClient;
                generation = currentPatientControlGeneration;
                ready = patientControlReady;
            }

            if (client != null && !client.isClosed() && client.isConnected() && ready) {
                if (sendLineToCurrentPatient(client, generation, command, true)) {
                    return true;
                }
                // 写失败时 sendLineToCurrentPatient 会清理当前连接；短暂等待患者端自动重连。
            }

            if (System.currentTimeMillis() >= deadline) {
                Log.w("DOCTOR", ">>> D2P SEND TIMEOUT: [" + command + "]");
                return false;
            }

            try {
                Thread.sleep(100L);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                return false;
            }
        }
        return false;
    }

    private boolean isCurrentPatientControl(Socket client, long generation) {
        synchronized (this) {
            return doctorToPatientClient == client
                    && currentPatientControlGeneration == generation;
        }
    }

    private boolean sendLineToCurrentPatient(Socket client,
                                             long generation,
                                             String line,
                                             boolean closeOnFailure) {
        synchronized (d2pWriteLock) {
            if (!isCurrentPatientControl(client, generation)
                    || client == null
                    || client.isClosed()) {
                return false;
            }
            try {
                OutputStream os = client.getOutputStream();
                os.write((line + "\n").getBytes("UTF-8"));
                os.flush();
                Log.i("DOCTOR", ">>> D2P OK generation=" + generation + ": [" + line + "]");
                return true;
            } catch (NetworkOnMainThreadException e) {
                // 编程线程错误不等于 Socket 失效，禁止因此主动关闭一条正常控制连接。
                Log.e("DOCTOR", ">>> D2P PROGRAMMING ERROR: network write on main thread: ["
                        + line + "]", e);
                return false;
            } catch (IOException e) {
                Log.e("DOCTOR", ">>> D2P NETWORK FAIL generation=" + generation + ": [" + line + "] "
                        + e.getClass().getSimpleName() + ": " + e.getMessage());
                if (closeOnFailure) {
                    closeCurrentPatientControl(client, generation, "network write failed");
                }
                return false;
            } catch (RuntimeException e) {
                Log.e("DOCTOR", ">>> D2P RUNTIME FAIL generation=" + generation + ": [" + line + "] "
                        + e.getClass().getSimpleName() + ": " + e.getMessage(), e);
                return false;
            }
        }
    }

    private void handlePatientControlClient(Socket client, long generation) {
        try {
            BufferedReader reader = new BufferedReader(
                    new InputStreamReader(client.getInputStream(), "UTF-8"));
            String line;
            while (running
                    && isCurrentPatientControl(client, generation)
                    && (line = reader.readLine()) != null) {
                line = line.trim();
                if (line.isEmpty()) continue;
                Log.i("DOCTOR", ">>> D2P RECV generation=" + generation + ": [" + line + "]");

                if (line.startsWith("PATIENT_READY")) {
                    boolean becameReady = false;
                    synchronized (this) {
                        if (doctorToPatientClient == client
                                && currentPatientControlGeneration == generation) {
                            patientLastPongAt = System.currentTimeMillis();
                            if (!patientControlReady) {
                                patientControlReady = true;
                                becameReady = true;
                            }
                        }
                    }
                    if (becameReady) {
                        // 先确认握手，再发送状态快照。患者端只有收到 CONTROL_READY 才显示完整连接。
                        if (sendLineToCurrentPatient(client, generation, "CONTROL_READY", true)
                                && sendPatientStateSnapshot(client, generation)) {
                            notifyPatientConnected(true);
                        }
                    }
                } else if (line.equals("PONG")) {
                    synchronized (this) {
                        if (doctorToPatientClient == client
                                && currentPatientControlGeneration == generation) {
                            patientLastPongAt = System.currentTimeMillis();
                        }
                    }
                } else if (line.startsWith("SSVEP,STIM_STARTED,")) {
                    String[] parts = line.split(",");
                    if (parts.length >= 4) {
                        try {
                            int freqIndex = Integer.parseInt(parts[2].trim());
                            float refreshRate = Float.parseFloat(parts[3].trim());
                            SsvepAnalysisManager.getInstance()
                                    .confirmStimulusStarted(freqIndex, refreshRate);
                        } catch (NumberFormatException e) {
                            Log.w("DOCTOR", "Invalid SSVEP stimulus feedback: " + line);
                        }
                    }
                }
            }
            Log.w("DOCTOR", ">>> Patient D2P reader ended generation=" + generation);
        } catch (IOException e) {
            if (running && isCurrentPatientControl(client, generation)) {
                Log.w("DOCTOR", ">>> Patient D2P read error generation=" + generation
                        + ": " + e.getMessage());
            }
        } finally {
            closeCurrentPatientControl(client, generation, "reader ended");
        }
    }

    private void startPatientControlHeartbeat() {
        stopPatientControlHeartbeat();
        patientControlHeartbeatThread = new Thread(() -> {
            Log.i("DOCTOR", "Patient control heartbeat started");
            while (running && !Thread.currentThread().isInterrupted()) {
                try {
                    Thread.sleep(PATIENT_HEARTBEAT_INTERVAL_MS);
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                    break;
                }

                Socket client;
                long generation;
                boolean ready;
                long lastPong;
                synchronized (TcpServerManager.this) {
                    client = doctorToPatientClient;
                    generation = currentPatientControlGeneration;
                    ready = patientControlReady;
                    lastPong = patientLastPongAt;
                }
                if (client == null || !ready) continue;

                long silentMs = System.currentTimeMillis() - lastPong;
                if (silentMs > PATIENT_HEARTBEAT_TIMEOUT_MS) {
                    Log.w("DOCTOR", "Patient control heartbeat timeout: " + silentMs
                            + "ms, generation=" + generation);
                    closeCurrentPatientControl(client, generation, "heartbeat timeout");
                    continue;
                }
                sendLineToCurrentPatient(client, generation, "PING", true);
            }
            Log.i("DOCTOR", "Patient control heartbeat stopped");
        }, "doctor-patient-heartbeat");
        patientControlHeartbeatThread.setDaemon(true);
        patientControlHeartbeatThread.start();
    }

    private void stopPatientControlHeartbeat() {
        if (patientControlHeartbeatThread != null) {
            patientControlHeartbeatThread.interrupt();
            patientControlHeartbeatThread = null;
        }
    }

    private void closeCurrentPatientControl(Socket client, long generation, String reason) {
        boolean wasCurrent = false;
        boolean wasReady = false;
        synchronized (this) {
            if (doctorToPatientClient == client
                    && currentPatientControlGeneration == generation) {
                doctorToPatientClient = null;
                wasReady = patientControlReady;
                patientControlReady = false;
                currentPatientControlGeneration = patientControlGenerationCounter.incrementAndGet();
                wasCurrent = true;
            }
        }
        try { if (client != null) client.close(); } catch (Exception ignored) {}
        if (wasCurrent) {
            Log.w("DOCTOR", ">>> Patient control cleared: " + reason
                    + ", generation=" + generation);
            if (wasReady) notifyPatientConnected(false);
        }
    }

    private void updatePatientControlState(String command) {
        if (command.startsWith("PAGE,")) {
            try {
                currentDoctorPage = Integer.parseInt(command.substring("PAGE,".length()).trim());
            } catch (NumberFormatException ignored) {}
        } else if (command.startsWith("SSVEP,START")) {
            String[] parts = command.split(",");
            if (parts.length >= 3) {
                try {
                    ssvepFreqIndex = Integer.parseInt(parts[2].trim());
                    ssvepActive = true;
                } catch (NumberFormatException ignored) {}
            }
        } else if (command.equals("SSVEP,STOP")) {
            ssvepActive = false;
            ssvepFreqIndex = -1;
        }
    }

    private boolean sendPatientStateSnapshot(Socket client, long generation) {
        if (!sendLineToCurrentPatient(client, generation,
                "PAGE," + currentDoctorPage, true)) return false;

        String ssvepState = ssvepActive && ssvepFreqIndex >= 0
                ? "SSVEP,START," + ssvepFreqIndex
                : "SSVEP,STOP";
        boolean ok = sendLineToCurrentPatient(client, generation, ssvepState, true);
        if (ok) {
            Log.i("DOCTOR", ">>> D2P STATE SNAPSHOT generation=" + generation
                    + ": page=" + currentDoctorPage
                    + ", ssvepActive=" + ssvepActive
                    + ", freqIndex=" + ssvepFreqIndex);
        }
        return ok;
    }

    private static final int BUFFER_POOL_SIZE = 16;
    private static final int BUFFER_SIZE = 512;

    private static final class RawPacket {
        byte[] buf;
        int len;
        RawPacket(byte[] buf, int len) { this.buf = buf; this.len = len; }
    }

    private void handleClient(Socket client) {
        synchronized (this) {
            this.deviceClient = client;
        }
        notifyDeviceConnected(true);
        startHeartbeat();

        ArrayBlockingQueue<RawPacket> rawPackets = new ArrayBlockingQueue<>(1024);
        ArrayBlockingQueue<byte[]> bufferPool = new ArrayBlockingQueue<>(BUFFER_POOL_SIZE);
        for (int i = 0; i < BUFFER_POOL_SIZE; i++) {
            bufferPool.offer(new byte[BUFFER_SIZE]);
        }

        AtomicLong totalPacketsRead = new AtomicLong(0);
        AtomicLong totalPacketsDropped = new AtomicLong(0);
        AtomicLong totalFramesParsed = new AtomicLong(0);
        AtomicLong totalInvalidFrames = new AtomicLong(0);

        Thread parserThread = new Thread(() -> {
            FrameParser parser = new FrameParser(totalFramesParsed, totalInvalidFrames, udpSender);
            while (!Thread.currentThread().isInterrupted()) {
                try {
                    RawPacket pkt = rawPackets.take();
                    try {
                        for (int i = 0; i < pkt.len; i++) {
                            parser.parse(pkt.buf[i] & 0xFF);
                        }
                    } finally {
                        bufferPool.offer(pkt.buf);
                    }
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                    break;
                }
            }
            Log.i("TCP", "Parser thread stopped, total frames parsed: " + totalFramesParsed.get() +
                    ", invalid: " + totalInvalidFrames.get());
        });
        parserThread.setDaemon(true);
        parserThread.start();

        try (InputStream in = client.getInputStream()) {
            byte[] buf = new byte[BUFFER_SIZE];
            int len;
            while ((len = in.read(buf)) != -1) {
                totalPacketsRead.addAndGet(1);

                if (forwardManager != null) {
                    forwardManager.broadcast(buf, len);
                }


                byte[] poolBuf = bufferPool.poll();
                if (poolBuf == null) {
                    totalPacketsDropped.incrementAndGet();
                    continue;
                }
                System.arraycopy(buf, 0, poolBuf, 0, len);
                if (!rawPackets.offer(new RawPacket(poolBuf, len))) {
                    totalPacketsDropped.incrementAndGet();
                    bufferPool.offer(poolBuf);
                }
            }
        } catch (Exception e) {
            Log.e("TCP", "Client error", e);
        } finally {
            stopHeartbeat();
            parserThread.interrupt();
            try { client.close(); } catch (Exception ignored) {}
            synchronized (this) {
                if (deviceClient == client) {
                    deviceClient = null;
                    notifyDeviceConnected(false);
                }
            }
        }
    }


    // ---------- 转发管理器（负责广播原始数据）----------
    private class TcpForwardManager {
        private final CopyOnWriteArrayList<Socket> forwardClients = new CopyOnWriteArrayList<>();
        private final CopyOnWriteArrayList<OutputStream> outputStreams = new CopyOnWriteArrayList<>();

        public boolean hasClients() {
            return !outputStreams.isEmpty();
        }

        public void addClient(Socket socket) {
            forwardClients.add(socket);
            try {
                outputStreams.add(socket.getOutputStream());
                new Thread(() -> {
                    try {
                        BufferedReader reader = new BufferedReader(
                                new InputStreamReader(socket.getInputStream(), "UTF-8"));
                        String line;
                        while ((line = reader.readLine()) != null) {
                            line = line.trim();
                            if (!line.isEmpty()) {
                                sendToDevice(line);
                                Log.i("FWD_CMD", "Python->MCU: " + line);
                            }
                        }
                    } catch (IOException ignored) {
                    } finally {
                        removeClient(socket);
                    }
                }).start();
            } catch (IOException e) {
                Log.e("TCP", "Failed to get output stream for forward client", e);
                removeClient(socket);
            }
        }

        public void broadcast(byte[] data, int len) {
            for (OutputStream os : outputStreams) {
                try {
                    os.write(data, 0, len);
                    os.flush();
                } catch (IOException e) {
                    Log.w("TCP", "Failed to send to forward client", e);
                }
            }
            cleanDisconnected();
        }

        public void broadcast(byte[] data) {
            broadcast(data, data.length);
        }

        private void cleanDisconnected() {
            List<Socket> toRemove = new ArrayList<>();
            for (Socket s : forwardClients) {
                if (s == null || s.isClosed() || !s.isConnected()) {
                    toRemove.add(s);
                }
            }
            for (Socket s : toRemove) {
                removeClient(s);
            }
        }

        private void removeClient(Socket socket) {
            forwardClients.remove(socket);
            try {
                socket.close();
            } catch (IOException ignored) {}
            List<OutputStream> rebuilt = new ArrayList<>();
            for (Socket s : forwardClients) {
                try {
                    OutputStream os = s.getOutputStream();
                    if (os != null) rebuilt.add(os);
                } catch (IOException e) {
                    Log.w("TCP", "Error re-adding output stream: " + e.getMessage());
                }
            }
            outputStreams.clear();
            outputStreams.addAll(rebuilt);
        }

        public void stopAll() {
            for (Socket s : forwardClients) {
                try { s.close(); } catch (IOException ignored) {}
            }
            forwardClients.clear();
            outputStreams.clear();
        }
    }

    // 以下为原有的 FrameParser 和 SpectrumReassembler，保持不变
    // ---------- 频谱重组器 ----------
    private static class SpectrumReassembler {
        private final java.util.Map<Integer, ReassemblyState> states = new java.util.HashMap<>();
        private static final long FRAG_TIMEOUT_MS = 5000;

        public float[] addFragment(int cmd, int fragIdx, int totalFrags,
                                   byte[] data, int offset, int len) {
            ReassemblyState state = states.get(cmd);
            long now = System.currentTimeMillis();
            if (state == null || fragIdx == 0
                || state.totalFrags != totalFrags
                || (now - state.createTime > FRAG_TIMEOUT_MS)) {
                state = new ReassemblyState(totalFrags);
                states.put(cmd, state);
            }
            int startFloatIdx = fragIdx * 4;
            long fragBit = 1L << fragIdx;
            if ((state.receivedMask & fragBit) != 0) {
                return null;
            }
            ByteBuffer bb = ByteBuffer.wrap(data, offset, len).order(ByteOrder.LITTLE_ENDIAN);
            for (int i = 0; i < len / 4; i++) {
                int idx = startFloatIdx + i;
                if (idx < state.mags.length) {
                    state.mags[idx] = bb.getFloat();
                }
            }
            state.receivedMask |= fragBit;
            state.receivedCount++;
            if (state.receivedCount == totalFrags) {
                states.remove(cmd);
                return state.mags;
            }
            return null;
        }

        private static class ReassemblyState {
            int totalFrags;
            float[] mags = new float[128];
            long receivedMask;
            int receivedCount;
            long createTime;
            ReassemblyState(int totalFrags) {
                this.totalFrags = totalFrags;
                this.receivedMask = 0;
                this.receivedCount = 0;
                this.createTime = System.currentTimeMillis();
            }
        }
    }

    private class FrameParser {
        private static final int STATE_HEADER = 0;
        private static final int STATE_PAYLOAD = 1;
        private static final int STATE_ESCAPE = 2;

        private int state = STATE_HEADER;
        private byte[] payload = new byte[1500];
        private int payloadLen = 0;
        private AtomicLong framesParsed;
        private AtomicLong invalidFrames;
        private SpectrumReassembler spectrumReassembler = new SpectrumReassembler();
        private final UdpSender udpSender;

        private final StringBuilder textLineBuf = new StringBuilder(512);

        FrameParser(AtomicLong framesParsed, AtomicLong invalidFrames, UdpSender udpSender) {
            this.framesParsed = framesParsed;
            this.invalidFrames = invalidFrames;
            this.udpSender = udpSender;
        }

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
                        textLineBuf.append((char) rawByte);
                    } else {
                        state = STATE_PAYLOAD;
                        payloadLen = 0;
                        if (payloadLen < payload.length) {
                            payload[payloadLen++] = (byte) rawByte;
                        }
                    }
                    break;
                case STATE_PAYLOAD:
                    if (rawByte == 0x7D) {
                        state = STATE_ESCAPE;
                    } else if (rawByte == 0x7E) {
                        if (payloadLen >= 1) {
                            parseBinaryFrame();
                        }
                        payloadLen = 0;
                        textLineBuf.setLength(0);
                        state = STATE_HEADER;
                    } else {
                        if (payloadLen < payload.length) {
                            payload[payloadLen++] = (byte) rawByte;
                        } else {
                            state = STATE_HEADER;
                        }
                    }
                    break;
                case STATE_ESCAPE:
                    byte unescaped = (byte) (rawByte ^ 0x20);
                    if (payloadLen < payload.length) {
                        payload[payloadLen++] = unescaped;
                    } else {
                        state = STATE_HEADER;
                    }
                    state = STATE_PAYLOAD;
                    break;
            }
        }

        private void parseBinaryFrame() {
            int cmd = payload[0] & 0xFF;
            int loadLen = payloadLen - 1;
            boolean valid = false;

            if (EegChannels.isWaveCmd(cmd)) {
                if (loadLen == 5) {
                    valid = true;
                    int ch = payload[1] & 0xFF;
                    ByteBuffer buf = ByteBuffer.wrap(payload, 2, 4).order(ByteOrder.LITTLE_ENDIAN);
                    float val = buf.getFloat();
                    dispatcher.postWaveData(cmd, ch, val);
                    if (ch == EegChannels.CH_O1) {
                        SsvepAnalysisManager.getInstance().offerWaveSample(cmd, val);
                    }
                    udpSender.sendWaveData(val, val);
                } else if (loadLen == 10) {
                    valid = true;
                    int chA = payload[1] & 0xFF;
                    int chB = payload[2] & 0xFF;
                    ByteBuffer buf = ByteBuffer.wrap(payload, 3, 8).order(ByteOrder.LITTLE_ENDIAN);
                    float valA = buf.getFloat();
                    float valB = buf.getFloat();
                    dispatcher.postWaveData(cmd, chA, valA);
                    dispatcher.postWaveData(cmd, chB, valB);
                    if (chA == EegChannels.CH_O1) {
                        SsvepAnalysisManager.getInstance().offerWaveSample(cmd, valA);
                    } else if (chB == EegChannels.CH_O1) {
                        SsvepAnalysisManager.getInstance().offerWaveSample(cmd, valB);
                    }
                    udpSender.sendWaveData(valA, valB);
                } else if (loadLen == 8) {
                    valid = true;
                    ByteBuffer buf = ByteBuffer.wrap(payload, 1, 8).order(ByteOrder.LITTLE_ENDIAN);
                    float valA = buf.getFloat();
                    float valB = buf.getFloat();
                    dispatcher.postWaveData(cmd, EegChannels.CH_OZ, valA);
                    dispatcher.postWaveData(cmd, EegChannels.CH_O1, valB);
                    SsvepAnalysisManager.getInstance().offerWaveSample(cmd, valB);
                    udpSender.sendWaveData(valA, valB);
                }
            } else if (cmd == 0x05) {
                if (loadLen == 18) {
                    valid = true;
                    ByteBuffer buf = ByteBuffer.wrap(payload, 1, 16).order(ByteOrder.LITTLE_ENDIAN);
                    float a0 = buf.getFloat(), a1 = buf.getFloat(), e0 = buf.getFloat(), e1 = buf.getFloat();
                    int trend = payload[1 + 16] & 0xFF;
                    int instant = payload[1 + 17] & 0xFF;
                    dispatcher.postFocusData(a0, a1, e0, e1, trend, instant);
                }
            } else if (isSpectrumCmd(cmd)) {
                if (loadLen >= 2) {
                    int fragIdx = payload[1] & 0xFF;
                    int totalFrags = payload[2] & 0xFF;
                    int dataLen = loadLen - 2;
                    if (dataLen == 16) {
                        float[] fullMags = spectrumReassembler.addFragment(cmd, fragIdx, totalFrags,
                                payload, 3, dataLen);
                        if (fullMags != null) {
                            dispatcher.postSpectrumData(cmd, fullMags);
                        }
                        valid = true;
                    } else {
                        Log.w("FrameRx", "Unexpected fragment data length: " + dataLen);
                    }
                } else {
                    Log.w("FrameRx", "Fragment header missing");
                }
            }
            if (!valid) {
                invalidFrames.incrementAndGet();
            } else {
                framesParsed.incrementAndGet();
            }
        }

        private boolean isSpectrumCmd(int cmd) {
            if (EegChannels.isSpectrumCmd(cmd)) return true;
            if (cmd == 0x02 || cmd == 0x03 || cmd == 0x06 ||
                cmd == 0x07 || cmd == 0x08 || cmd == 0x09) return true;
            return false;
        }

        private int textLineLogCounter = 0;
        private void parseTextLine(String line) {
            if (line.isEmpty()) return;
            if (line.startsWith("RX[")) return;
            textLineLogCounter++;
            if (textLineLogCounter % 250 == 0 || line.startsWith("TASK") || line.startsWith("READY") || line.startsWith("MODE_SET_OK")) {
                Log.i("TCP", "parseTextLine: " + line);
            }
            if (line.startsWith("DIRCSV,")) {
                EegFrame frame = EegFrame.fromDirCsv(line);
                if (frame != null) {
                    dispatcher.postEegFrame(frame);
                    framesParsed.incrementAndGet();
                } else {
                    invalidFrames.incrementAndGet();
                }
            } else if (line.startsWith("RESULT,")) {
                sendAckForSeq(line);
                if (!isDuplicateSeq(line)) {
                    InferenceResult result = InferenceResult.fromResultLine(line);
                    if (result != null) {
                        dispatcher.postInferenceResult(result);
                        framesParsed.incrementAndGet();
                    } else {
                        invalidFrames.incrementAndGet();
                    }
                }
            } else if (line.startsWith("IPCDIAG,")) {
                IpcDiagInfo diag = IpcDiagInfo.fromLine(line);
                if (diag != null) {
                    dispatcher.postIpcDiag(diag);
                    framesParsed.incrementAndGet();
                } else {
                    invalidFrames.incrementAndGet();
                }
            } else if (line.startsWith("DIRCFG,")) {
                String configJson = line.substring("DIRCFG,".length());
                dispatcher.postDirConfig(configJson);
                framesParsed.incrementAndGet();
            } else if (line.startsWith("TASK,")) {
                if (line.startsWith("TASK,DONE")) {
                    sendAckForSeq(line);
                    if (!isDuplicateSeq(line)) {
                        dispatcher.postTaskDone();
                    }
                    framesParsed.incrementAndGet();
                } else {
                    java.util.regex.Matcher m = java.util.regex.Pattern.compile("TASK,(LEFT|RIGHT),start").matcher(line);
                    if (m.matches()) {
                        dispatcher.postTaskStart(m.group(1));
                        framesParsed.incrementAndGet();
                    }
                }
            } else if (line.equals("READY_TRAIN")) {
                dispatcher.postReadyTrain();
                framesParsed.incrementAndGet();
            } else if (line.equals("READY_TEST")) {
                dispatcher.postReadyTest();
                framesParsed.incrementAndGet();
            } else if (line.startsWith("MODE_SET_OK,")) {
                try {
                    int mode = Integer.parseInt(line.substring("MODE_SET_OK,".length()));
                    dispatcher.postModeSetOk(mode);
                    framesParsed.incrementAndGet();
                } catch (NumberFormatException ignored) {}
            } else if (line.startsWith("TURN_EVENT,")) {
                sendAckForSeq(line);
                java.util.regex.Matcher m = java.util.regex.Pattern.compile("TURN_EVENT,(?:seq=\\d+,)?count=\\d+,from=(\\w+),to=(\\w+)").matcher(line);
                if (m.find()) {
                    dispatcher.postTurnEvent(m.group(1), m.group(2));
                    framesParsed.incrementAndGet();
                }
            } else if (line.startsWith("FALL_EVENT,")) {
                sendAckForSeq(line);
                dispatcher.postFallEvent();
                framesParsed.incrementAndGet();
            } else if (line.startsWith("NO_TURN_ALERT,")) {
                sendAckForSeq(line);
                java.util.regex.Matcher m = java.util.regex.Pattern.compile("NO_TURN_ALERT,(?:seq=\\d+,)?duration_min=(\\d+)").matcher(line);
                if (m.find()) {
                    dispatcher.postNoTurnAlert(Long.parseLong(m.group(1)));
                    framesParsed.incrementAndGet();
                }
            } else if (line.startsWith("POSTURE_STATE,")) {
                sendAckForSeq(line);
                java.util.regex.Matcher m = java.util.regex.Pattern.compile("POSTURE_STATE,(?:seq=\\d+,)?(\\w+),turns=(\\d+)").matcher(line);
                if (m.find()) {
                    dispatcher.postPostureState(m.group(1), Integer.parseInt(m.group(2)));
                    framesParsed.incrementAndGet();
                }
            } else if (line.startsWith("MODE_DIAG,")) {
                Log.i("TCP", "parseTextLine: " + line);
            } else if (line.startsWith("V5F_DIAG,")) {
                Log.i("TCP", "parseTextLine: " + line);
            } else if (line.startsWith("V5F_RAW,")) {
                Log.i("TCP", "parseTextLine: " + line);
            }

            if (line.equals("READY_TRAIN") || line.equals("READY_TEST")
                    || line.startsWith("TASK,") || line.startsWith("MODE_SET_OK,")
                    || line.startsWith("RESULT,")) {
                TcpServerManager.this.sendToPatient(line);
            }
        }
    }
}