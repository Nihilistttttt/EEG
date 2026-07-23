package com.nihilisttt.eegdoctor;

import com.nihilisttt.eegdoctor.R;

import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.util.Log;
import android.widget.Toast;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import com.google.android.material.textfield.TextInputEditText;
import com.google.android.material.button.MaterialButton;

public class ConfigFragment extends Fragment {

    private Handler statusHandler = new Handler(Looper.getMainLooper());
    private Runnable statusRunnable;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_config, container, false);

        TextInputEditText etSsid = root.findViewById(R.id.et_wifi_ssid);
        TextInputEditText etPassword = root.findViewById(R.id.et_wifi_password);

        root.findViewById(R.id.btn_wifi_add).setOnClickListener(v -> {
            String ssid = etSsid.getText() != null ? etSsid.getText().toString().trim() : "";
            String pwd = etPassword.getText() != null ? etPassword.getText().toString().trim() : "";
            Log.i("Config", "btn_wifi_add clicked, ssid=[" + ssid + "]");
            if (ssid.isEmpty()) {
                Toast.makeText(requireContext(), "请输入SSID", Toast.LENGTH_SHORT).show();
                return;
            }
            TcpServerManager.getInstance().sendWifiAdd(ssid, pwd);
            Toast.makeText(requireContext(), "WiFi配置已发送: " + ssid, Toast.LENGTH_SHORT).show();
        });

        root.findViewById(R.id.btn_wifi_delete).setOnClickListener(v -> {
            String ssid = etSsid.getText() != null ? etSsid.getText().toString().trim() : "";
            Log.i("Config", "btn_wifi_delete clicked, ssid=[" + ssid + "]");
            if (ssid.isEmpty()) {
                Toast.makeText(requireContext(), "请输入要删除的SSID", Toast.LENGTH_SHORT).show();
                return;
            }
            TcpServerManager.getInstance().sendWifiDelete(ssid);
            Toast.makeText(requireContext(), "WiFi删除已发送: " + ssid, Toast.LENGTH_SHORT).show();
        });

        TextInputEditText etCloudIp = root.findViewById(R.id.et_cloud_ip);
        TextInputEditText etCloudPort = root.findViewById(R.id.et_cloud_port);
        MaterialButton btnCloudEnable = root.findViewById(R.id.btn_cloud_enable);
        TextView tvCloudStatus = root.findViewById(R.id.tv_cloud_status);

        btnCloudEnable.setOnClickListener(v -> {
            String ip = etCloudIp.getText() != null ? etCloudIp.getText().toString().trim() : "";
            String portStr = etCloudPort.getText() != null ? etCloudPort.getText().toString().trim() : "8888";
            
            if (ip.isEmpty()) {
                Toast.makeText(requireContext(), "请输入ESP8266 IP地址", Toast.LENGTH_SHORT).show();
                return;
            }

            int port;
            try {
                port = Integer.parseInt(portStr);
            } catch (NumberFormatException e) {
                port = 8888;
            }

            boolean isEnabled = TcpServerManager.getInstance().isCloudForwardEnabled();
            if (!isEnabled) {
                TcpServerManager.getInstance().setCloudForwardAddress(ip, port);
                TcpServerManager.getInstance().setCloudForwardEnabled(true);
                btnCloudEnable.setText("禁用转发");
                Toast.makeText(requireContext(), "华为云转发已启用: " + ip + ":" + port, Toast.LENGTH_SHORT).show();
            } else {
                TcpServerManager.getInstance().setCloudForwardEnabled(false);
                btnCloudEnable.setText("启用转发");
                Toast.makeText(requireContext(), "华为云转发已禁用", Toast.LENGTH_SHORT).show();
            }
        });

        statusRunnable = new Runnable() {
            @Override
            public void run() {
                if (TcpServerManager.getInstance().isCloudForwardEnabled()) {
                    if (TcpServerManager.getInstance().isCloudForwardConnected()) {
                        tvCloudStatus.setText("已连接");
                        tvCloudStatus.setTextColor(getResources().getColor(android.R.color.holo_green_dark));
                    } else {
                        tvCloudStatus.setText("连接中...");
                        tvCloudStatus.setTextColor(getResources().getColor(android.R.color.holo_orange_dark));
                    }
                    btnCloudEnable.setText("禁用转发");
                } else {
                    tvCloudStatus.setText("未启用");
                    tvCloudStatus.setTextColor(getResources().getColor(android.R.color.darker_gray));
                    btnCloudEnable.setText("启用转发");
                }
                statusHandler.postDelayed(this, 1000);
            }
        };

        return root;
    }

    @Override
    public void onResume() {
        super.onResume();
        TcpServerManager.getInstance().sendToDevice(EegChannels.buildAllNoneDisplayConfig());
        if (statusRunnable != null) {
            statusHandler.post(statusRunnable);
        }
    }

    @Override
    public void onPause() {
        super.onPause();
        if (statusRunnable != null) {
            statusHandler.removeCallbacks(statusRunnable);
        }
    }
}