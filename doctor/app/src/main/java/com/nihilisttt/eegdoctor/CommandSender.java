package com.nihilisttt.eegdoctor;

import android.util.Log;
import java.io.OutputStream;
import java.net.Socket;

public class CommandSender {
    private static final String TAG = "CommandSender";
    private static final int DEFAULT_PORT = 41002;
    private static final CommandSender INSTANCE = new CommandSender();

    private Socket commandSocket;
    private OutputStream outputStream;
    private String targetIp;
    private int targetPort = DEFAULT_PORT;

    public static CommandSender getInstance() { return INSTANCE; }

    public void setTarget(String ip, int port) {
        this.targetIp = ip;
        this.targetPort = port;
    }

    public void setTarget(String ip) {
        setTarget(ip, DEFAULT_PORT);
    }

    public boolean connect() {
        if (targetIp == null || targetIp.isEmpty()) {
            Log.e(TAG, "Target IP not set");
            return false;
        }
        try {
            if (commandSocket != null && !commandSocket.isClosed()) {
                commandSocket.close();
            }
            commandSocket = new Socket(targetIp, targetPort);
            outputStream = commandSocket.getOutputStream();
            Log.i(TAG, "Connected to " + targetIp + ":" + targetPort);
            return true;
        } catch (Exception e) {
            Log.e(TAG, "Connect failed: " + e.getMessage());
            outputStream = null;
            return false;
        }
    }

    public void disconnect() {
        try {
            if (outputStream != null) outputStream.close();
            if (commandSocket != null) commandSocket.close();
        } catch (Exception ignored) {}
        outputStream = null;
        commandSocket = null;
    }

    public boolean isConnected() {
        return commandSocket != null && commandSocket.isConnected() && !commandSocket.isClosed();
    }

    private boolean sendCommand(String cmd) {
        if (outputStream == null) {
            Log.w(TAG, "Not connected, cannot send: " + cmd);
            return false;
        }
        try {
            outputStream.write((cmd + "\n").getBytes("UTF-8"));
            outputStream.flush();
            Log.i(TAG, "Sent: " + cmd);
            return true;
        } catch (Exception e) {
            Log.e(TAG, "Send failed: " + e.getMessage());
            return false;
        }
    }

    public boolean setMode(int mode) {
        return sendCommand("MODE,SET," + mode);
    }

    public boolean setModeCollect() { return setMode(1); }
    public boolean setModeInfer() { return setMode(2); }
    public boolean setModeCalibrate() { return setMode(3); }

    public boolean startTraining() {
        return sendCommand("MODE,TRAIN");
    }

    public boolean startTest() {
        return sendCommand("MODE,TEST");
    }

    public boolean trialLeft() {
        return sendCommand("TRIAL,LEFT");
    }

    public boolean trialRight() {
        return sendCommand("TRIAL,RIGHT");
    }
}