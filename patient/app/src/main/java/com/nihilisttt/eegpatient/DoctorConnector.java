package com.nihilisttt.eegpatient;

import android.util.Log;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.concurrent.CopyOnWriteArrayList;

public class DoctorConnector {
    private static final String TAG = "DoctorConnector";
    private static final int DATA_PORT = 41004;
    private static final int CMD_PORT = 41005;
    private static final DoctorConnector INSTANCE = new DoctorConnector();

    private String doctorIp;
    private Socket dataSocket;
    private Socket cmdSocket;
    private OutputStream cmdOutput;
    private volatile boolean connected;
    private Thread dataThread;

    private final CopyOnWriteArrayList<DataListener> listeners = new CopyOnWriteArrayList<>();

    public interface DataListener {
        void onWaveData(int cmd, float ch0, float ch1);
        void onEegFrame(EegFrame frame);
        void onInferenceResult(InferenceResult result);
        void onIpcDiag(IpcDiagInfo diag);
        void onConnectionChanged(boolean connected);
    }

    public static DoctorConnector getInstance() { return INSTANCE; }

    public void setDoctorIp(String ip) { this.doctorIp = ip; }

    public void addListener(DataListener l) { listeners.add(l); }
    public void removeListener(DataListener l) { listeners.remove(l); }

    public boolean isConnected() { return connected; }

    public void connect() {
        if (doctorIp == null || doctorIp.isEmpty()) {
            Log.e(TAG, "Doctor IP not set");
            return;
        }
        new Thread(() -> {
            try {
                cmdSocket = new Socket(doctorIp, CMD_PORT);
                cmdOutput = cmdSocket.getOutputStream();
                Log.i(TAG, "Cmd socket connected to " + doctorIp + ":" + CMD_PORT);

                dataSocket = new Socket(doctorIp, DATA_PORT);
                Log.i(TAG, "Data socket connected to " + doctorIp + ":" + DATA_PORT);
                connected = true;
                notifyConnectionChanged(true);

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
                Log.e(TAG, "Connection error: " + e.getMessage());
            } finally {
                connected = false;
                notifyConnectionChanged(false);
                disconnect();
            }
        }).start();
    }

    public void disconnect() {
        connected = false;
        try { if (dataSocket != null) dataSocket.close(); } catch (Exception ignored) {}
        try { if (cmdSocket != null) cmdSocket.close(); } catch (Exception ignored) {}
        dataSocket = null;
        cmdSocket = null;
        cmdOutput = null;
    }

    public boolean sendCommand(String cmd) {
        if (cmdOutput == null || !connected) {
            Log.w(TAG, "Not connected, cannot send: " + cmd);
            return false;
        }
        try {
            cmdOutput.write((cmd + "\n").getBytes("UTF-8"));
            cmdOutput.flush();
            Log.i(TAG, "Sent command: " + cmd);
            return true;
        } catch (Exception e) {
            Log.e(TAG, "Send failed: " + e.getMessage());
            return false;
        }
    }

    private void notifyConnectionChanged(boolean c) {
        for (DataListener l : listeners) l.onConnectionChanged(c);
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
            }
        }

        private void parseTextLine(String line) {
            if (line.isEmpty()) return;
            if (line.startsWith("DIRCSV,")) {
                EegFrame frame = EegFrame.fromDirCsv(line);
                if (frame != null) for (DataListener l : listeners) l.onEegFrame(frame);
            } else if (line.startsWith("RESULT,")) {
                InferenceResult r = InferenceResult.fromResultLine(line);
                if (r != null) for (DataListener l : listeners) l.onInferenceResult(r);
            } else if (line.startsWith("IPCDIAG,")) {
                IpcDiagInfo d = IpcDiagInfo.fromLine(line);
                if (d != null) for (DataListener l : listeners) l.onIpcDiag(d);
            }
        }
    }
}