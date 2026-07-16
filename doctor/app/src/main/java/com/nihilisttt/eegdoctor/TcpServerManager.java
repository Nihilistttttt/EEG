package com.nihilisttt.eegdoctor;

import android.util.Log;
import java.io.IOException;
import java.io.InputStream;
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
import java.util.Collections;
import java.util.Enumeration;
import java.util.List;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.atomic.AtomicLong;
import java.util.concurrent.atomic.AtomicBoolean;

public class TcpServerManager {
    private static final int TCP_PORT = 41002;
    private static final int FORWARD_PORT = 41003;
    private static final int PATIENT_DATA_PORT = 41004;
    private static final int PATIENT_CMD_PORT = 41005;
    private static final int DOCTOR_TO_PATIENT_PORT = 41006;
    private static final int DISCOVERY_PORT = 41007;
    private static final TcpServerManager INSTANCE = new TcpServerManager();

    private ServerSocket serverSocket;
    private boolean running;
    private DataDispatcher dispatcher = DataDispatcher.getInstance();

    private final UdpSender udpSender = new UdpSender();
    private ServerSocket forwardServerSocket;
    private TcpForwardManager forwardManager;
    private ServerSocket patientDataServerSocket;
    private TcpForwardManager patientDataManager;
    private ServerSocket patientCmdServerSocket;
    private Socket deviceClient;
    private Socket doctorToPatientClient;
    private DatagramSocket discoverySocket;

    private final CopyOnWriteArrayList<ConnectionListener> connectionListeners = new CopyOnWriteArrayList<>();

    private final AtomicBoolean pongReceived = new AtomicBoolean(false);
    private Thread heartbeatThread;
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

    public boolean isPatientConnected() {
        return doctorToPatientClient != null && !doctorToPatientClient.isClosed() && doctorToPatientClient.isConnected();
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

        // 启动患者端数据转发服务器
        patientDataManager = new TcpForwardManager();
        new Thread(() -> {
            try {
                patientDataServerSocket = new ServerSocket(PATIENT_DATA_PORT);
                Log.i("TCP", "Patient data server started on port " + PATIENT_DATA_PORT);
                while (running) {
                    Socket pClient = patientDataServerSocket.accept();
                    Log.i("TCP", "Patient data client connected: " + pClient.getRemoteSocketAddress());
                    patientDataManager.addClient(pClient);
                }
            } catch (Exception e) { Log.e("TCP", "Patient data server error", e); }
        }).start();

        // 启动患者端命令接收服务器
        new Thread(() -> {
            try {
                patientCmdServerSocket = new ServerSocket(PATIENT_CMD_PORT);
                Log.i("TCP", "Patient cmd server started on port " + PATIENT_CMD_PORT);
                while (running) {
                    Socket cmdClient = patientCmdServerSocket.accept();
                    Log.i("TCP", "Patient cmd client connected: " + cmdClient.getRemoteSocketAddress());
                    new Thread(() -> handlePatientCommand(cmdClient)).start();
                }
            } catch (Exception e) { Log.e("TCP", "Patient cmd server error", e); }
        }).start();

        // 启动医生端→患者端命令通道服务器
        new Thread(() -> {
            try {
                ServerSocket d2pServer = new ServerSocket(DOCTOR_TO_PATIENT_PORT);
                Log.i("TCP", "Doctor-to-patient cmd server started on port " + DOCTOR_TO_PATIENT_PORT);
                while (running) {
                    Socket client = d2pServer.accept();
                    Log.i("DOCTOR", ">>> Patient D2P connected: " + client.getRemoteSocketAddress());
                    doctorToPatientClient = client;
                    notifyPatientConnected(true);
                    new Thread(() -> {

                        try {
                            client.getInputStream().read();
                        } catch (IOException ignored) {
                        } finally {
                            Log.w("DOCTOR", ">>> Patient D2P disconnected");
                            doctorToPatientClient = null;
                            notifyPatientConnected(false);
                        }
                    }).start();
                }
            } catch (Exception e) { Log.e("TCP", "Doctor-to-patient server error", e); }
        }).start();

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
        try { if (patientDataServerSocket != null) patientDataServerSocket.close(); } catch (Exception ignored) {}
        try { if (patientCmdServerSocket != null) patientCmdServerSocket.close(); } catch (Exception ignored) {}
        try { if (discoverySocket != null) discoverySocket.close(); } catch (Exception ignored) {}
        try { if (deviceClient != null) deviceClient.close(); } catch (Exception ignored) {}
        try { if (doctorToPatientClient != null) doctorToPatientClient.close(); } catch (Exception ignored) {}
        deviceClient = null;
        doctorToPatientClient = null;
        if (forwardManager != null) forwardManager.stopAll();
        if (patientDataManager != null) patientDataManager.stopAll();
        udpSender.release();
        stopHeartbeat();
    }

    private void startHeartbeat() {
        stopHeartbeat();
        startSender();
        heartbeatThread = new Thread(() -> {
            Log.i("HEARTBEAT", "Heartbeat thread started");
            while (!Thread.currentThread().isInterrupted() && running) {
                try {
                    Thread.sleep(3000);
                } catch (InterruptedException e) {
                    break;
                }
                if (!isDeviceConnected()) continue;
                pongReceived.set(false);
                sendToDevice("PING");
                try {
                    Thread.sleep(2000);
                } catch (InterruptedException e) {
                    break;
                }
                if (!pongReceived.get()) {
                    Log.w("HEARTBEAT", "PONG not received! ESP8266 may not be responding to PING");
                } else {
                    Log.i("HEARTBEAT", "Heartbeat OK");
                }
            }
            Log.i("HEARTBEAT", "Heartbeat thread stopped");
        });
        heartbeatThread.setDaemon(true);
        heartbeatThread.start();
    }

    private void stopHeartbeat() {
        if (heartbeatThread != null) {
            heartbeatThread.interrupt();
            heartbeatThread = null;
        }
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

    private static final java.util.regex.Pattern SEQ_PATTERN = java.util.regex.Pattern.compile("seq=(\\d+)");
    private static final java.util.regex.Pattern TURN_EVENT_PATTERN = java.util.regex.Pattern.compile("TURN_EVENT,(?:seq=\\d+,)?count=\\d+,from=(\\w+),to=(\\w+)");
    private static final java.util.regex.Pattern FALL_EVENT_PATTERN = java.util.regex.Pattern.compile("FALL_EVENT,(?:seq=\\d+)?");
    private static final java.util.regex.Pattern NO_TURN_ALERT_PATTERN = java.util.regex.Pattern.compile("NO_TURN_ALERT,(?:seq=\\d+,)?duration_min=(\\d+)");
    private static final java.util.regex.Pattern POSTURE_STATE_PATTERN = java.util.regex.Pattern.compile("POSTURE_STATE,(?:seq=\\d+,)?(\\w+),turns=(\\d+)");

    private boolean isDuplicateSeq(String line) {
        java.util.regex.Matcher m = SEQ_PATTERN.matcher(line);
        if (m.find()) {
            int seq = Integer.parseInt(m.group(1));
            if (seq == mLastProcessedSeq) return true;
            mLastProcessedSeq = seq;
        }
        return false;
    }

    private void sendAckForSeq(String line) {
        java.util.regex.Matcher m = SEQ_PATTERN.matcher(line);
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
                        java.util.regex.Matcher m = TURN_EVENT_PATTERN.matcher(line);
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
                        java.util.regex.Matcher m = NO_TURN_ALERT_PATTERN.matcher(line);
                        if (m.find()) {
                            long dur = Long.parseLong(m.group(1));
                            Log.i("MCU_RESP", "NO_TURN_ALERT: " + dur + " min");
                            dispatcher.postNoTurnAlert(dur);
                        }
                    } else if (line.startsWith("POSTURE_STATE,")) {
                        sendAckForSeq(line);
                        java.util.regex.Matcher m = POSTURE_STATE_PATTERN.matcher(line);
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

    public int getPatientDataPort() {
        return PATIENT_DATA_PORT;
    }

    public int getPatientCmdPort() {
        return PATIENT_CMD_PORT;
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

    public void sendToPatient(String command) {
        Log.i("DOCTOR", ">>> sendToPatient: [" + command + "]");
        sendToPatientViaD2p(command);
        sendToPatientViaData(command);
    }

    private void sendToPatientViaD2p(String command) {
        if (doctorToPatientClient != null && !doctorToPatientClient.isClosed()
                && doctorToPatientClient.isConnected()) {
            try {
                OutputStream os = doctorToPatientClient.getOutputStream();
                os.write((command + "\n").getBytes("UTF-8"));
                os.flush();
                Log.i("DOCTOR", ">>> D2P OK: [" + command + "]");
            } catch (Exception e) {
                Log.e("DOCTOR", ">>> D2P FAIL: [" + command + "] " + e.getMessage());
            }
        } else {
            Log.w("DOCTOR", ">>> D2P SKIP (no patient connected)");
        }
    }

    public void sendToPatientViaData(String command) {
        if (patientDataManager != null) {
            try {
                byte[] data = (command + "\n").getBytes("UTF-8");
                patientDataManager.broadcast(data);
                Log.i("DOCTOR", ">>> DATA OK: [" + command + "]");
            } catch (Exception e) {
                Log.e("DOCTOR", ">>> DATA FAIL: [" + command + "] " + e.getMessage());
            }
        } else {
            Log.w("DOCTOR", ">>> DATA SKIP (patientDataManager null)");
        }
    }

    // 处理原有主服务器客户端连接（保持不变，但增加转发广播）
    private void handleClient(Socket client) {
        synchronized (this) {
            this.deviceClient = client;
        }
        notifyDeviceConnected(true);
        startHeartbeat();
        BlockingQueue<byte[]> rawPackets = new ArrayBlockingQueue<>(1024);
        AtomicLong totalPacketsRead = new AtomicLong(0);
        AtomicLong totalPacketsDropped = new AtomicLong(0);
        AtomicLong totalFramesParsed = new AtomicLong(0);
        AtomicLong totalInvalidFrames = new AtomicLong(0);

        Thread parserThread = new Thread(() -> {
            FrameParser parser = new FrameParser(totalFramesParsed, totalInvalidFrames, udpSender);
            while (!Thread.currentThread().isInterrupted()) {
                try {
                    byte[] chunk = rawPackets.take();
                    for (int i = 0; i < chunk.length; i++) {
                        parser.parse(chunk[i] & 0xFF);
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
            byte[] buf = new byte[512];
            int len;
            while ((len = in.read(buf)) != -1) {
                totalPacketsRead.addAndGet(1);

                boolean hasText = false;
                for (int i = 0; i < len; i++) {
                    if ((buf[i] & 0xFF) >= 0x20 && (buf[i] & 0xFF) < 0x7F) {
                        hasText = true;
                        break;
                    }
                    if (buf[i] == '\n' || buf[i] == '\r' || buf[i] == 'P' || buf[i] == 'O') {
                        hasText = true;
                        break;
                    }
                }
                if (hasText) {
                    String peek = new String(buf, 0, len, "UTF-8");
                    if (peek.contains("PONG")) {
                        pongReceived.set(true);
                        Log.i("HEARTBEAT", "PONG received from ESP8266");
                    }
                    parseMcuTextResponses(peek);
                }

                byte[] copy = new byte[len];
                System.arraycopy(buf, 0, copy, 0, len);
                if (forwardManager != null) {
                    forwardManager.broadcast(copy);
                }
                if (patientDataManager != null) {
                    patientDataManager.broadcast(copy);
                }
                if (!rawPackets.offer(copy)) {
                    totalPacketsDropped.incrementAndGet();
                    rawPackets.poll();
                    rawPackets.offer(copy);
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

    private void handlePatientCommand(Socket cmdClient) {
        try (InputStream in = cmdClient.getInputStream()) {
            byte[] buf = new byte[256];
            int len;
            while ((len = in.read(buf)) != -1) {
                String cmd = new String(buf, 0, len, "UTF-8").trim();
                Log.i("TCP", "Patient command received: " + cmd);
                sendToDevice(cmd);
            }
        } catch (Exception e) {
            Log.e("TCP", "Patient cmd client error", e);
        } finally {
            try { cmdClient.close(); } catch (Exception ignored) {}
        }
    }

    // ---------- 转发管理器（负责广播原始数据）----------
    private class TcpForwardManager {
        private final CopyOnWriteArrayList<Socket> forwardClients = new CopyOnWriteArrayList<>();
        private final CopyOnWriteArrayList<OutputStream> outputStreams = new CopyOnWriteArrayList<>();

        public void addClient(Socket socket) {
            forwardClients.add(socket);
            try {
                outputStreams.add(socket.getOutputStream());
                // 监控客户端断开
                new Thread(() -> {
                    try {
                        socket.getInputStream().read(); // 阻塞直到断开
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

        public void broadcast(byte[] data) {
            for (OutputStream os : outputStreams) {
                try {
                    os.write(data);
                    os.flush();
                } catch (IOException e) {
                    // 发送失败，该客户端可能已断开，稍后会被清理
                    Log.w("TCP", "Failed to send to forward client", e);
                }
            }
            // 清理断开连接的输出流
            cleanDisconnected();
        }

        private void cleanDisconnected() {
            for (Socket s : forwardClients) {
                if (s.isClosed() || !s.isConnected()) {
                    removeClient(s);
                }
            }
        }

        private void removeClient(Socket socket) {
            forwardClients.remove(socket);
            try {
                socket.close();
            } catch (IOException ignored) {}
            // 重新构建输出流列表
            outputStreams.clear();
            for (Socket s : forwardClients) {
                try {
                    outputStreams.add(s.getOutputStream());
                } catch (IOException e) {
                    Log.e("TCP", "Error re-adding output stream", e);
                }
            }
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
            if (state == null || state.totalFrags != totalFrags
                || (now - state.createTime > FRAG_TIMEOUT_MS)) {
                state = new ReassemblyState(totalFrags);
                states.put(cmd, state);
            }
            int startFloatIdx = fragIdx * 4;
            ByteBuffer bb = ByteBuffer.wrap(data, offset, len).order(ByteOrder.LITTLE_ENDIAN);
            for (int i = 0; i < len / 4; i++) {
                int idx = startFloatIdx + i;
                if (idx < state.mags.length) {
                    state.mags[idx] = bb.getFloat();
                }
            }
            state.receivedMask |= (1L << fragIdx);
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

            if (cmd == 0x04 || cmd == 0x10 || cmd == 0x11) {
                if (loadLen == 5) {
                    valid = true;
                    int ch = payload[1] & 0xFF;
                    ByteBuffer buf = ByteBuffer.wrap(payload, 2, 4).order(ByteOrder.LITTLE_ENDIAN);
                    float val = buf.getFloat();
                    dispatcher.postWaveData(cmd, ch, val);
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
                    udpSender.sendWaveData(valA, valB);
                } else if (loadLen == 8) {
                    valid = true;
                    ByteBuffer buf = ByteBuffer.wrap(payload, 1, 8).order(ByteOrder.LITTLE_ENDIAN);
                    float valA = buf.getFloat();
                    float valB = buf.getFloat();
                    dispatcher.postWaveData(cmd, 0, valA);
                    dispatcher.postWaveData(cmd, 1, valB);
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
            if (cmd >= 0x20 && cmd <= 0x27) return true;
            if (cmd >= 0x30 && cmd <= 0x37) return true;
            if (cmd >= 0x40 && cmd <= 0x47) return true;
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
                    java.util.regex.Matcher m = TURN_EVENT_PATTERN.matcher(line);
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
                    java.util.regex.Matcher m = NO_TURN_ALERT_PATTERN.matcher(line);
                if (m.find()) {
                    dispatcher.postNoTurnAlert(Long.parseLong(m.group(1)));
                    framesParsed.incrementAndGet();
                }
            } else if (line.startsWith("POSTURE_STATE,")) {
                sendAckForSeq(line);
                    java.util.regex.Matcher m = POSTURE_STATE_PATTERN.matcher(line);
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
        }
    }
}