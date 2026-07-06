package com.nihilisttt.eegpatient;

import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

public class ConnectFragment extends Fragment implements DoctorConnector.DataListener {

    private TextView tvConnectStatus;
    private Handler handler = new Handler(Looper.getMainLooper());
    private boolean isConnecting = false;
    private static final int DOCTOR_DISCOVERY_PORT = 41006;
    private static final int DISCOVERY_TIMEOUT_MS = 5000;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_connect, container, false);
        tvConnectStatus = root.findViewById(R.id.tv_connect_status);

        if (!DoctorConnector.getInstance().isConnected()) {
            startAutoDiscovery();
        } else {
            showConnected();
        }

        return root;
    }

    @Override
    public void onResume() {
        super.onResume();
        DoctorConnector.getInstance().addListener(this);
        if (DoctorConnector.getInstance().isConnected()) {
            showConnected();
        }
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        handler.removeCallbacksAndMessages(null);
    }

    private void startAutoDiscovery() {
        if (isConnecting) return;
        isConnecting = true;
        tvConnectStatus.setText("正在搜索医生端...");
        tvConnectStatus.setTextColor(ContextCompat.getColor(requireContext(), R.color.accent_info));

        new Thread(() -> {
            String gatewayIp = discoverGatewayIp();
            if (gatewayIp != null) {
                handler.post(() -> attemptConnect(gatewayIp));
            } else {
                handler.post(() -> {
                    tvConnectStatus.setText("未找到医生端，正在重试...");
                    tvConnectStatus.setTextColor(ContextCompat.getColor(requireContext(), R.color.accent_warning));
                    isConnecting = false;
                    handler.postDelayed(this::startAutoDiscovery, 3000);
                });
            }
        }).start();
    }

    private String discoverGatewayIp() {
        try {
            DatagramSocket socket = new DatagramSocket();
            socket.setSoTimeout(DISCOVERY_TIMEOUT_MS);
            byte[] requestData = "EEG_DOCTOR_DISCOVERY".getBytes("UTF-8");
            String broadcastAddr = getBroadcastAddress();
            if (broadcastAddr == null) broadcastAddr = "255.255.255.255";
            DatagramPacket packet = new DatagramPacket(
                    requestData, requestData.length,
                    InetAddress.getByName(broadcastAddr), DOCTOR_DISCOVERY_PORT);
            socket.send(packet);

            byte[] buf = new byte[256];
            DatagramPacket response = new DatagramPacket(buf, buf.length);
            socket.receive(response);
            String resp = new String(response.getData(), 0, response.getLength()).trim();
            socket.close();

            if (resp.startsWith("EEG_DOCTOR:")) {
                return resp.substring("EEG_DOCTOR:".length());
            }
        } catch (Exception ignored) {}

        try {
            java.util.Enumeration<java.net.NetworkInterface> interfaces =
                    java.util.Collections.list(java.net.NetworkInterface.getNetworkInterfaces());
            for (java.net.NetworkInterface intf : interfaces) {
                java.util.Enumeration<java.net.InetAddress> addrs =
                        java.util.Collections.list(intf.getInetAddresses());
                for (java.net.InetAddress addr : addrs) {
                    if (!addr.isLoopbackAddress() && addr.getAddress().length == 4) {
                        String ip = addr.getHostAddress();
                        if (ip.startsWith("192.168.")) {
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
            java.util.Enumeration<java.net.NetworkInterface> interfaces =
                    java.util.Collections.list(java.net.NetworkInterface.getNetworkInterfaces());
            for (java.net.NetworkInterface intf : interfaces) {
                java.util.Enumeration<java.net.InetAddress> addrs =
                        java.util.Collections.list(intf.getInetAddresses());
                for (java.net.InetAddress addr : addrs) {
                    if (!addr.isLoopbackAddress() && addr.getAddress().length == 4) {
                        String ip = addr.getHostAddress();
                        if (ip.startsWith("192.168.")) {
                            String[] parts = ip.split("\\.");
                            return parts[0] + "." + parts[1] + "." + parts[2] + ".255";
                        }
                    }
                }
            }
        } catch (Exception ignored) {}
        return null;
    }

    private void attemptConnect(String ip) {
        tvConnectStatus.setText("正在连接 " + ip + "...");
        tvConnectStatus.setTextColor(ContextCompat.getColor(requireContext(), R.color.accent_info));
        DoctorConnector.getInstance().setDoctorIp(ip);
        DoctorConnector.getInstance().connect();
    }

    private void showConnected() {
        if (tvConnectStatus == null) return;
        isConnecting = false;
        tvConnectStatus.setText("已连接到医生端");
        tvConnectStatus.setTextColor(ContextCompat.getColor(requireContext(), R.color.accent_success));
    }

    @Override
    public void onConnectionChanged(boolean connected) {
        if (getActivity() == null) return;
        getActivity().runOnUiThread(() -> {
            if (connected) {
                showConnected();
            } else {
                isConnecting = false;
                tvConnectStatus.setText("连接断开，正在重连...");
                tvConnectStatus.setTextColor(ContextCompat.getColor(requireContext(), R.color.accent_error));
                handler.postDelayed(this::startAutoDiscovery, 3000);
            }
        });
    }

    @Override public void onWaveData(int cmd, float ch0, float ch1) {}
    @Override public void onEegFrame(EegFrame frame) {}
    @Override public void onInferenceResult(InferenceResult result) {}
    @Override public void onIpcDiag(IpcDiagInfo diag) {}
    @Override public void onTaskStart(String side) {}
    @Override public void onTaskDone() {}
    @Override public void onReadyTrain() {}
    @Override public void onReadyTest() {}
    @Override public void onModeSetOk(int mode) {}
    @Override public void onPageSwitch(int page) {}
}
