package com.nihilisttt.eegpatient;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

public class ConnectFragment extends Fragment implements DoctorConnector.DataListener {

    private View connectPanel;
    private TextView tvConnectStatus;
    private TextView tvConnectedHint;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_connect, container, false);
        connectPanel = root.findViewById(R.id.connect_panel);
        tvConnectStatus = root.findViewById(R.id.tv_connect_status);
        tvConnectedHint = root.findViewById(R.id.tv_connected_hint);
        updateUI();
        return root;
    }

    @Override
    public void onResume() {
        super.onResume();
        DoctorConnector.getInstance().removeListener(this);
        DoctorConnector.getInstance().addListener(this);
        updateUI();
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        DoctorConnector.getInstance().removeListener(this);
    }

    private void updateUI() {
        if (connectPanel == null) return;
        if (DoctorConnector.getInstance().isConnected()) {
            connectPanel.setVisibility(View.GONE);
            if (tvConnectedHint != null) tvConnectedHint.setVisibility(View.VISIBLE);
        } else {
            connectPanel.setVisibility(View.VISIBLE);
            if (tvConnectedHint != null) tvConnectedHint.setVisibility(View.GONE);
            if (tvConnectStatus != null) tvConnectStatus.setText("正在搜索医生端...");
        }
    }

    @Override
    public void onConnectionChanged(boolean connected) {
        if (getActivity() == null) return;
        getActivity().runOnUiThread(this::updateUI);
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
    @Override public void onFocusData(float attn0, float attn1, float ema0, float ema1, int trend, int instant) {}
}
