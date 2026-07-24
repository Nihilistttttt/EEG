package com.nihilisttt.eegdoctor;

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

public class CloudForwardFragment extends Fragment {

    private Handler statusHandler = new Handler(Looper.getMainLooper());
    private Runnable statusRunnable;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_cloud_forward, container, false);

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