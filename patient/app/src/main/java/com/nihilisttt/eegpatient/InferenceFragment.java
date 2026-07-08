package com.nihilisttt.eegpatient;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

public class InferenceFragment extends Fragment implements DoctorConnector.DataListener {

    private TextView tvDirection;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_inference, container, false);
        tvDirection = root.findViewById(R.id.tv_direction);
        tvDirection.setText("← →");
        return root;
    }

    @Override
    public void onResume() {
        super.onResume();
        DoctorConnector.getInstance().removeListener(this);
        DoctorConnector.getInstance().addListener(this);
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        DoctorConnector.getInstance().removeListener(this);
    }

    @Override
    public void onInferenceResult(InferenceResult result) {
        if (getActivity() == null || tvDirection == null) return;
        getActivity().runOnUiThread(() -> {
            String intent = result.getIntent();
            if ("LEFT".equals(intent)) {
                tvDirection.setText("←");
                tvDirection.setTextColor(0xFF1565C0);
            } else if ("RIGHT".equals(intent)) {
                tvDirection.setText("→");
                tvDirection.setTextColor(0xFFE65100);
            }
        });
    }

    @Override public void onWaveData(int cmd, float ch0, float ch1) {}
    @Override public void onEegFrame(EegFrame frame) {}
    @Override public void onIpcDiag(IpcDiagInfo diag) {}
    @Override public void onConnectionChanged(boolean connected) {}
    @Override public void onTaskStart(String side) {}
    @Override public void onTaskDone() {}
    @Override public void onReadyTrain() {}
    @Override public void onReadyTest() {}
    @Override public void onModeSetOk(int mode) {}
    @Override public void onPageSwitch(int page) {}
    @Override public void onFocusData(float attn0, float attn1, float ema0, float ema1, int trend, int instant) {}
}
