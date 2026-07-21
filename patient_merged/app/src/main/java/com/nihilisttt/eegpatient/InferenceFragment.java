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
import androidx.fragment.app.Fragment;

public class InferenceFragment extends Fragment implements DoctorConnector.DataListener {

    private static final int HIDE_DELAY_MS = 1500;
    private static final int COLOR_CORRECT = 0xFF00C853;
    private static final int COLOR_WRONG = 0xFFFF1744;
    private static final int COLOR_DIM = 0xFFFFFFFF;

    private TextView tvTarget;
    private TextView tvResult;
    private Handler handler = new Handler(Looper.getMainLooper());
    private String currentTarget = null;
    private Runnable hideRunnable = null;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_inference, container, false);
        tvTarget = root.findViewById(R.id.tv_target);
        tvResult = root.findViewById(R.id.tv_result);
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
        if (hideRunnable != null) handler.removeCallbacks(hideRunnable);
    }

    @Override
    public void onTargetDirection(String direction) {
        if (getActivity() == null || tvTarget == null) return;
        getActivity().runOnUiThread(() -> {
            currentTarget = direction;
            boolean isLeft = "LEFT".equals(direction);
            tvTarget.setText(isLeft ? "◀" : "▶");
            tvTarget.setTextColor(COLOR_DIM);
            tvResult.setText("--");
            tvResult.setTextColor(COLOR_DIM);
        });
    }

    @Override
    public void onInferenceResult(InferenceResult result) {
        if (getActivity() == null || tvResult == null) return;
        getActivity().runOnUiThread(() -> {
            String intent = result.getIntent();
            boolean isLeft = "LEFT".equals(intent);
            tvResult.setText(isLeft ? "◀" : "▶");

            if (currentTarget != null) {
                boolean correct = currentTarget.equals(intent);
                tvResult.setTextColor(correct ? COLOR_CORRECT : COLOR_WRONG);
                tvTarget.setTextColor(correct ? COLOR_CORRECT : COLOR_WRONG);
            } else {
                tvResult.setTextColor(COLOR_DIM);
            }

            if (hideRunnable != null) handler.removeCallbacks(hideRunnable);
            hideRunnable = () -> {
                tvTarget.setText("--");
                tvTarget.setTextColor(COLOR_DIM);
                tvResult.setText("--");
                tvResult.setTextColor(COLOR_DIM);
                currentTarget = null;
                hideRunnable = null;
            };
            handler.postDelayed(hideRunnable, HIDE_DELAY_MS);
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
