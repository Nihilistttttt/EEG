package com.nihilisttt.eegdoctor;

import android.util.Log;
import java.io.IOException;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.atomic.AtomicBoolean;

public class CloudForwardClient {
    private static final String TAG = "CloudForwardClient";
    private static final int CONNECT_TIMEOUT = 5000;
    private static final int SO_TIMEOUT = 10000;
    private static final int QUEUE_SIZE = 512;

    private volatile String serverIp;
    private volatile int serverPort = 8888;
    private volatile boolean enabled = false;

    private Socket socket;
    private OutputStream outputStream;
    private Thread sendThread;
    private final BlockingQueue<byte[]> sendQueue = new ArrayBlockingQueue<>(QUEUE_SIZE);
    private final AtomicBoolean running = new AtomicBoolean(false);
    private volatile boolean connected = false;

    private Thread connectThread;
    private volatile long lastConnectAttempt = 0;
    private static final long RECONNECT_INTERVAL = 5000;

    public void setServerAddress(String ip, int port) {
        this.serverIp = ip;
        this.serverPort = port;
        Log.i(TAG, "Server address set to " + ip + ":" + port);
    }

    public void setEnabled(boolean enable) {
        this.enabled = enable;
        if (enable && !running.get()) {
            start();
        } else if (!enable && running.get()) {
            stop();
        }
        Log.i(TAG, "Enabled: " + enable);
    }

    public boolean isEnabled() {
        return enabled;
    }

    public boolean isConnected() {
        return connected && socket != null && socket.isConnected() && !socket.isClosed();
    }

    public void start() {
        if (running.get()) return;
        running.set(true);

        connectThread = new Thread(() -> {
            while (running.get()) {
                if (!enabled || serverIp == null || serverIp.isEmpty()) {
                    try { Thread.sleep(1000); } catch (InterruptedException e) { break; }
                    continue;
                }

                if (!isConnected()) {
                    long now = System.currentTimeMillis();
                    if (now - lastConnectAttempt >= RECONNECT_INTERVAL) {
                        lastConnectAttempt = now;
                        connect();
                    }
                }

                try { Thread.sleep(1000); } catch (InterruptedException e) { break; }
            }
        });
        connectThread.setDaemon(true);
        connectThread.start();

        sendThread = new Thread(() -> {
            while (running.get() || !sendQueue.isEmpty()) {
                try {
                    byte[] data = sendQueue.poll();
                    if (data == null) {
                        try { Thread.sleep(10); } catch (InterruptedException e) { break; }
                        continue;
                    }

                    if (isConnected() && outputStream != null) {
                        try {
                            outputStream.write(data);
                            outputStream.flush();
                        } catch (IOException e) {
                            Log.e(TAG, "Send failed, disconnecting", e);
                            disconnect();
                        }
                    }
                } catch (Exception e) {
                    Log.e(TAG, "Send thread error", e);
                }
            }
            Log.i(TAG, "Send thread stopped");
        });
        sendThread.setDaemon(true);
        sendThread.start();

        Log.i(TAG, "Started");
    }

    public void stop() {
        running.set(false);
        disconnect();

        if (connectThread != null) {
            connectThread.interrupt();
            connectThread = null;
        }

        if (sendThread != null) {
            sendThread.interrupt();
            sendThread = null;
        }

        sendQueue.clear();
        Log.i(TAG, "Stopped");
    }

    private synchronized void connect() {
        if (isConnected()) return;

        try {
            Log.i(TAG, "Connecting to " + serverIp + ":" + serverPort);
            socket = new Socket();
            socket.connect(new InetSocketAddress(serverIp, serverPort), CONNECT_TIMEOUT);
            socket.setSoTimeout(SO_TIMEOUT);
            socket.setTcpNoDelay(true);
            outputStream = socket.getOutputStream();
            connected = true;
            Log.i(TAG, "Connected to " + serverIp + ":" + serverPort);
        } catch (Exception e) {
            Log.e(TAG, "Connect failed: " + e.getMessage());
            disconnect();
        }
    }

    private synchronized void disconnect() {
        connected = false;
        if (outputStream != null) {
            try { outputStream.close(); } catch (Exception ignored) {}
            outputStream = null;
        }
        if (socket != null) {
            try { socket.close(); } catch (Exception ignored) {}
            socket = null;
        }
    }

    public void forward(byte[] data, int len) {
        if (!enabled || !isConnected()) return;

        byte[] copy = new byte[len];
        System.arraycopy(data, 0, copy, 0, len);

        if (!sendQueue.offer(copy)) {
            Log.w(TAG, "Send queue full, dropping data");
        }
    }

    public void forward(byte[] data) {
        forward(data, data.length);
    }
}