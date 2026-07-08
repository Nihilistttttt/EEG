package com.nihilisttt.eegpatient;

import android.util.Log;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.concurrent.CopyOnWriteArrayList;

public class DoctorConnector {
    private static final String TAG = "PATIENT";
    private static final int DATA_PORT = 41004;
    private static final int CMD_PORT = 41005;
    private static final int D2P_CMD_PORT = 41006;
    private static final int DISCOVERY_PORT = 41007;
    private static final int DISCOVERY_TIMEOUT_MS = 5000;
    private static final DoctorConnector INSTANCE = new DoctorConnector();

    private String doctorIp;
    private Socket dataSocket;
    private Socket cmdSocket;
    private OutputStream cmdOutput;
    private volatile boolean connected;
    private volatile boolean autoConnecting;

    private final CopyOnWriteArrayList<DataListener> listeners = new CopyOnWriteArrayList<>();

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
    }

    public static DoctorConnector getInstance() { return INSTANCE; }

    public void setDoctorIp(String ip) { this.doctorIp = ip; }

    public void addListener(DataListener l) { listeners.add(l); }
    public void removeListener(DataListener l) { listeners.remove(l); }

    public boolean isConnected() { return connected; }

    public void startAutoConnect() {
        if (autoConnecting) return;
        autoConnecting = true;
        Log.i(TAG, "startAutoConnect: begin background auto-connect");
        new Thread(() -> {
            while (autoConnecting) {
                if (!connected) {
                    String ip = discoverDoctorIp();
                    if (ip != null) {
                        Log.i(TAG, "autoConnect: found doctor at " + ip);
                        doctorIp = ip;
                        doConnect();
                        Log.w(TAG, "autoConnect: doConnect returned, will retry in 3s");
                    } else {
                        Log.w(TAG, "autoConnect: discovery failed, retry in 3s");
                    }
                }
                if (autoConnecting) {
                    try { Thread.sleep(3000); } catch (InterruptedException ie) { break; }
                }
            }
            Log.i(TAG, "startAutoConnect: exited");
        }).start();
    }

    public void stopAutoConnect() {
        autoConnecting = false;
    }

    private String discoverDoctorIp() {
        try {
            DatagramSocket socket = new DatagramSocket();
            socket.setSoTimeout(DISCOVERY_TIMEOUT_MS);
            byte[] requestData = "EEG_DOCTOR_DISCOVERY".getBytes("UTF-8");
            String broadcastAddr = getBroadcastAddress();
            if (broadcastAddr == null) broadcastAddr = "255.255.255.255";
            DatagramPacket packet = new DatagramPacket(
                    requestData, requestData.length,
                    InetAddress.getByName(broadcastAddr), DISCOVERY_PORT);
            socket.send(packet);
            byte[] buf = new byte[256];
            DatagramPacket response = new DatagramPacket(buf, buf.length);
            socket.receive(response);
            String resp = new String(response.getData(), 0, response.getLength()).trim();
            socket.close();
            if (resp.startsWith("EEG_DOCTOR:")) {
                return resp.substring("EEG_DOCTOR:".length());
            }
        } catch (Exception e) {
            Log.d(TAG, "discovery: " + e.getMessage());
        }
        try {
            java.util.List<java.net.NetworkInterface> ifaces =
                    java.util.Collections.list(java.net.NetworkInterface.getNetworkInterfaces());
            for (java.net.NetworkInterface intf : ifaces) {
                for (java.net.InetAddress addr : java.util.Collections.list(intf.getInetAddresses())) {
                    if (!addr.isLoopbackAddress() && addr.getAddress().length == 4) {
                        String ip = addr.getHostAddress();
                        if (ip.startsWith("192.168.")) {
                            String[] p = ip.split("\\.");
                            return p[0] + "." + p[1] + "." + p[2] + ".1";
                        }
                    }
                }
            }
        } catch (Exception ignored) {}
        return null;
    }

    private String getBroadcastAddress() {
        try {
            for (java.net.NetworkInterface intf : java.util.Collections.list(java.net.NetworkInterface.getNetworkInterfaces())) {
                for (java.net.InetAddress addr : java.util.Collections.list(intf.getInetAddresses())) {
                    if (!addr.isLoopbackAddress() && addr.getAddress().length == 4) {
                        String ip = addr.getHostAddress();
                        if (ip.startsWith("192.168.")) {
                            String[] p = ip.split("\\.");
                            return p[0] + "." + p[1] + "." + p[2] + ".255";
                        }
                    }
                }
            }
        } catch (Exception ignored) {}
        return null;
    }

    private boolean doConnect() {
        if (doctorIp == null || doctorIp.isEmpty()) return false;
        try {
            cmdSocket = new Socket(doctorIp, CMD_PORT);
            cmdOutput = cmdSocket.getOutputStream();
            Log.i(TAG, "CMD socket OK: " + doctorIp + ":" + CMD_PORT);

            dataSocket = new Socket(doctorIp, DATA_PORT);
            Log.i(TAG, "DATA socket OK: " + doctorIp + ":" + DATA_PORT);
            connected = true;
            notifyConnectionChanged(true);

            startD2pConnection();

            InputStream in = dataSocket.getInputStream();
            FrameParser parser = new FrameParser();
            byte[] buf = new byte[512];
            int len;
            while ((len = in.read(buf)) != -1) {
                for (int i = 0; i < len; i++) {
                    parser.parse(buf[i] & 0xFF);
                }
            }
        } catch (Exception e) {
            Log.e(TAG, "doConnect error: " + e.getMessage());
        } finally {
            connected = false;
            notifyConnectionChanged(false);
            doDisconnect();
        }
        return false;
    }

    private void startD2pConnection() {
        new Thread(() -> {
            while (connected) {
                Socket s = null;
                try {
                    s = new Socket(doctorIp, D2P_CMD_PORT);
                    Log.i(TAG, "D2P socket OK: " + doctorIp + ":" + D2P_CMD_PORT);
                    InputStream d2pIn = s.getInputStream();
                    byte[] d2pBuf = new byte[256];
                    int d2pLen;
                    while ((d2pLen = d2pIn.read(d2pBuf)) != -1) {
                        String line = new String(d2pBuf, 0, d2pLen, "UTF-8").trim();
                        Log.i(TAG, "D2P RECV: [" + line + "]");
                        parseD2pCommand(line);
                    }
                } catch (Exception e) {
                    Log.w(TAG, "D2P error: " + e.getMessage());
                } finally {
                    try { if (s != null) s.close(); } catch (Exception ignored) {}
                }
                if (connected) {
                    try { Thread.sleep(3000); } catch (InterruptedException ie) { break; }
                }
            }
        }).start();
    }

    public void disconnect() {
        autoConnecting = false;
        connected = false;
        doDisconnect();
    }

    private void doDisconnect() {
        try { if (dataSocket != null) dataSocket.close(); } catch (Exception ignored) {}
        try { if (cmdSocket != null) cmdSocket.close(); } catch (Exception ignored) {}
        dataSocket = null;
        cmdSocket = null;
        cmdOutput = null;
    }

    public boolean sendCommand(String cmd) {
        if (cmdOutput == null || !connected) return false;
        try {
            cmdOutput.write((cmd + "\n").getBytes("UTF-8"));
            cmdOutput.flush();
            return true;
        } catch (Exception e) {
            return false;
        }
    }

    private void notifyConnectionChanged(boolean c) {
        Log.i(TAG, "notifyConnectionChanged: " + c + ", listeners=" + listeners.size());
        for (DataListener l : listeners) l.onConnectionChanged(c);
    }

    private int mapDoctorPageToPatient(int doctorPage) {
        switch (doctorPage) {
            case 4: return 1;
            case 5: return 2;
            case 6: return 3;
            default: return 0;
        }
    }

    private void parseD2pCommand(String line) {
        if (line == null || line.isEmpty()) return;
        if (line.startsWith("PAGE,")) {
            try {
                int doctorPage = Integer.parseInt(line.substring("PAGE,".length()));
                int patientPage = mapDoctorPageToPatient(doctorPage);
                Log.i(TAG, ">>> D2P PAGE: doctor=" + doctorPage + " -> patient=" + patientPage);
                for (DataListener l : listeners) l.onPageSwitch(patientPage);
            } catch (NumberFormatException ignored) {}
        }
    }

    private void emitPageSwitch(int page) {
        Log.i(TAG, ">>> emitPageSwitch: page=" + page);
        for (DataListener l : listeners) l.onPageSwitch(page);
    }

    private class FrameParser {
        private static final int STATE_HEADER = 0;
        private static final int STATE_PAYLOAD = 1;
        private static final int STATE_ESCAPE = 2;

        private int state = STATE_HEADER;
        private byte[] payload = new byte[1500];
        private int payloadLen = 0;
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
                        textLineBuf.append((char) rawByte);
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
                    } else {
                        if (payloadLen < payload.length) {
                            payload[payloadLen++] = (byte) rawByte;
                        } else {
                            state = STATE_HEADER;
                        }
                    }
                    break;
                case STATE_ESCAPE:
                    if (payloadLen < payload.length) {
                        payload[payloadLen++] = (byte) (rawByte ^ 0x20);
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
            if ((cmd == 0x04 || cmd == 0x10) && loadLen == 8) {
                ByteBuffer buf = ByteBuffer.wrap(payload, 1, 8).order(ByteOrder.LITTLE_ENDIAN);
                float ch0 = buf.getFloat();
                float ch1 = buf.getFloat();
                for (DataListener l : listeners) l.onWaveData(cmd, ch0, ch1);
            } else if (cmd == 0x05 && loadLen == 18) {
                ByteBuffer buf = ByteBuffer.wrap(payload, 1, 16).order(ByteOrder.LITTLE_ENDIAN);
                float a0 = buf.getFloat(), a1 = buf.getFloat(), e0 = buf.getFloat(), e1 = buf.getFloat();
                int trend = payload[1 + 16] & 0xFF;
                int instant = payload[1 + 17] & 0xFF;
                for (DataListener l : listeners) l.onFocusData(a0, a1, e0, e1, trend, instant);
            }
        }

        private void parseTextLine(String line) {
            if (line.isEmpty()) return;
            Log.d(TAG, "DATA RECV: [" + line + "]");
            if (line.startsWith("DIRCSV,")) {
                EegFrame frame = EegFrame.fromDirCsv(line);
                if (frame != null) for (DataListener l : listeners) l.onEegFrame(frame);
            } else if (line.startsWith("RESULT,")) {
                InferenceResult r = InferenceResult.fromResultLine(line);
                if (r != null) for (DataListener l : listeners) l.onInferenceResult(r);
                emitPageSwitch(3);
            } else if (line.startsWith("IPCDIAG,")) {
                IpcDiagInfo d = IpcDiagInfo.fromLine(line);
                if (d != null) for (DataListener l : listeners) l.onIpcDiag(d);
            } else if (line.startsWith("TASK,")) {
                if (line.startsWith("TASK,DONE")) {
                    for (DataListener l : listeners) l.onTaskDone();
                } else if (line.startsWith("TASK,STOPPED")) {
                    for (DataListener l : listeners) l.onTaskDone();
                } else {
                    java.util.regex.Matcher m = java.util.regex.Pattern.compile("TASK,(LEFT|RIGHT),start").matcher(line);
                    if (m.matches()) {
                        for (DataListener l : listeners) l.onTaskStart(m.group(1));
                        emitPageSwitch(2);
                    }
                }
            } else if (line.equals("READY_TRAIN")) {
                for (DataListener l : listeners) l.onReadyTrain();
                emitPageSwitch(2);
            } else if (line.equals("READY_TEST")) {
                for (DataListener l : listeners) l.onReadyTest();
            } else if (line.startsWith("MODE_SET_OK,")) {
                try {
                    int mode = Integer.parseInt(line.substring("MODE_SET_OK,".length()));
                    for (DataListener l : listeners) l.onModeSetOk(mode);
                    if (mode == 2) emitPageSwitch(3);
                } catch (NumberFormatException ignored) {}
            } else if (line.startsWith("PAGE,")) {
                try {
                    int doctorPage = Integer.parseInt(line.substring("PAGE,".length()));
                    int patientPage = mapDoctorPageToPatient(doctorPage);
                    Log.i(TAG, ">>> DATA PAGE: doctor=" + doctorPage + " -> patient=" + patientPage);
                    for (DataListener l : listeners) l.onPageSwitch(patientPage);
                } catch (NumberFormatException ignored) {}
            }
        }
    }
}
