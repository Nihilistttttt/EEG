package com.nihilisttt.eegpatient;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;

public class ConnectFragment extends Fragment implements DoctorConnector.DataListener {

    private EditText etDoctorIp;
    private TextView tvConnectStatus;
    private View btnConnect;
    private View statusDot;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_connect, container, false);
        etDoctorIp = root.findViewById(R.id.et_doctor_ip);
        tvConnectStatus = root.findViewById(R.id.tv_connect_status);
        btnConnect = root.findViewById(R.id.btn_connect);
        statusDot = null;

        btnConnect.setOnClickListener(v -> {
            String ip = etDoctorIp.getText().toString().trim();
            if (ip.isEmpty()) {
                tvConnectStatus.setText("请输入IP地址");
                return;
            }
            tvConnectStatus.setText("正在连接...");
            DoctorConnector.getInstance().setDoctorIp(ip);
            DoctorConnector.getInstance().connect();
        });

        return root;
    }

    @Override
    public void onResume() {
        super.onResume();
        DoctorConnector.getInstance().addListener(this);
    }

    @Override
    public void onPause() {
        super.onPause();
        DoctorConnector.getInstance().removeListener(this);
    }

    @Override
    public void onConnectionChanged(boolean connected) {
        if (getActivity() == null) return;
        getActivity().runOnUiThread(() -> {
            if (connected) {
                tvConnectStatus.setText("已连接");
                tvConnectStatus.setTextColor(ContextCompat.getColor(requireContext(), R.color.accent_success));
            } else {
                tvConnectStatus.setText("连接断开");
                tvConnectStatus.setTextColor(ContextCompat.getColor(requireContext(), R.color.accent_error));
            }
        });
    }

    @Override public void onWaveData(int cmd, float ch0, float ch1) {}
    @Override public void onEegFrame(EegFrame frame) {}
    @Override public void onInferenceResult(InferenceResult result) {}
    @Override public void onIpcDiag(IpcDiagInfo diag) {}
}