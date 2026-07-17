package com.nihilisttt.eegdoctor;

import android.util.Log;

public class CommandSender {
    private static final String TAG = "CommandSender";
    private static final CommandSender INSTANCE = new CommandSender();

    public static CommandSender getInstance() { return INSTANCE; }

    public void setTarget(String ip, int port) {}
    public void setTarget(String ip) {}

    public boolean connect() {
        return TcpServerManager.getInstance().isDeviceConnected();
    }

    public void disconnect() {}

    public boolean isConnected() {
        return TcpServerManager.getInstance().isDeviceConnected();
    }

    public boolean sendCommand(String cmd) {
        Log.i(TAG, "sendCommand: [" + cmd + "]");
        TcpServerManager.getInstance().sendToDevice(cmd);
        return true;
    }

    public boolean setMode(int mode) {
        Log.i(TAG, "setMode: " + mode);
        return sendCommand("MODE,SET," + mode);
    }

    public boolean setModeCollect() { return setMode(1); }
    public boolean setModeInfer() { return setMode(2); }
    public boolean setModeCalibrate() { return setMode(3); }

    public boolean startTraining() {
        Log.i(TAG, "startTraining");
        return sendCommand("MODE,TRAIN");
    }

    public boolean startTest() {
        Log.i(TAG, "startTest");
        return sendCommand("MODE,TEST");
    }

    public boolean trialLeft() {
        Log.i(TAG, "trialLeft");
        return sendCommand("TRIAL,LEFT");
    }

    public boolean trialRight() {
        Log.i(TAG, "trialRight");
        return sendCommand("TRIAL,RIGHT");
    }

    public boolean sendPage(int page) {
        return sendCommand("PAGE," + page);
    }

    public boolean ssvepStart() {
        Log.i(TAG, "ssvepStart");
        return sendCommand("SSVEP,START");
    }

    public boolean ssvepStop() {
        Log.i(TAG, "ssvepStop");
        return sendCommand("SSVEP,STOP");
    }
}