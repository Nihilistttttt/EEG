package com.nihilisttt.eegdoctor;

import android.util.Log;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
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

public class TcpServerManager {
    private static final int TCP_PORT = 41002;
    private static final int FORWARD_PORT = 41003;
    private static final int PATIENT_DATA_PORT = 41004;
    private static final int PATIENT_CMD_PORT = 41005;
    private static final int DOCTOR_TO_PATIENT_PORT = 41006;
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
                    Log.i("TCP", "Doctor-to-patient client connected: " + client.getRemoteSocketAddress());
                    doctorToPatientClient = client;
                }
            } catch (Exception e) { Log.e("TCP", "Doctor-to-patient server error", e); }
        }).start();
    }

    public void stop() {
        running = false;
        try { if (serverSocket != null) serverSocket.close(); } catch (Exception ignored) {}
        try { if (forwardServerSocket != null) forwardServerSocket.close(); } catch (Exception ignored) {}
        try { if (patientDataServerSocket != null) patientDataServerSocket.close(); } catch (Exception ignored) {}
        try { if (patientCmdServerSocket != null) patientCmdServerSocket.close(); } catch (Exception ignored) {}
        if (forwardManager != null) forwardManager.stopAll();
        if (patientDataManager != null) patientDataManager.stopAll();
        udpSender.release();
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
        if (deviceClient != null && !deviceClient.isClosed() && deviceClient.isConnected()) {
            try {
                OutputStream os = deviceClient.getOutputStream();
                os.write((command + "\n").getBytes("UTF-8"));
                os.flush();
                Log.i("TCP", "Sent to device: " + command);
            } catch (Exception e) {
                Log.e("TCP", "Failed to send to device: " + e.getMessage());
            }
        }
    }

    public void sendToPatient(String command) {
        if (doctorToPatientClient != null && !doctorToPatientClient.isClosed()
                && doctorToPatientClient.isConnected()) {
            try {
                OutputStream os = doctorToPatientClient.getOutputStream();
                os.write((command + "\n").getBytes("UTF-8"));
                os.flush();
                Log.i("TCP", "Sent to patient: " + command);
            } catch (Exception e) {
                Log.e("TCP", "Failed to send to patient: " + e.getMessage());
            }
        }
    }

    // 处理原有主服务器客户端连接（保持不变，但增加转发广播）
    private void handleClient(Socket client) {
        this.deviceClient = client;
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
                byte[] copy = new byte[len];
                System.arraycopy(buf, 0, copy, 0, len);
                // 转发给所有转发客户端（原始数据）
                if (forwardManager != null) {
                    forwardManager.broadcast(copy);
                }
                // 转发给患者端
                if (patientDataManager != null) {
                    patientDataManager.broadcast(copy);
                }
                // 原有解析逻辑
                if (!rawPackets.offer(copy)) {
                    totalPacketsDropped.incrementAndGet();
                    rawPackets.poll();
                    rawPackets.offer(copy);
                }
            }
        } catch (Exception e) {
            Log.e("TCP", "Client error", e);
        } finally {
            parserThread.interrupt();
            try { client.close(); } catch (Exception ignored) {}
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

        public float[] addFragment(int cmd, int fragIdx, int totalFrags,
                                   byte[] data, int offset, int len) {
            ReassemblyState state = states.get(cmd);
            if (state == null) {
                state = new ReassemblyState(totalFrags);
                states.put(cmd, state);
            }
            if (state.totalFrags != totalFrags) {
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
            ReassemblyState(int totalFrags) {
                this.totalFrags = totalFrags;
                this.receivedMask = 0;
                this.receivedCount = 0;
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
            if (cmd == 0x04 || cmd == 0x10) {
                if (loadLen == 8) {
                    valid = true;
                    ByteBuffer buf = ByteBuffer.wrap(payload, 1, 8).order(ByteOrder.LITTLE_ENDIAN);
                    float ch0 = buf.getFloat();
                    float ch1 = buf.getFloat();
                    dispatcher.postWaveData(cmd, ch0, ch1);
                    udpSender.sendWaveData(ch0, ch1);
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
            } else if (cmd == 0x02 || cmd == 0x03 || cmd == 0x06 ||
                    cmd == 0x07 || cmd == 0x08 || cmd == 0x09) {
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

        private void parseTextLine(String line) {
            if (line.isEmpty()) return;
            if (line.startsWith("DIRCSV,")) {
                EegFrame frame = EegFrame.fromDirCsv(line);
                if (frame != null) {
                    dispatcher.postEegFrame(frame);
                    framesParsed.incrementAndGet();
                } else {
                    invalidFrames.incrementAndGet();
                }
            } else if (line.startsWith("RESULT,")) {
                InferenceResult result = InferenceResult.fromResultLine(line);
                if (result != null) {
                    dispatcher.postInferenceResult(result);
                    framesParsed.incrementAndGet();
                } else {
                    invalidFrames.incrementAndGet();
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
                if (line.equals("TASK,DONE")) {
                    dispatcher.postTaskDone();
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
            }
        }
    }
}