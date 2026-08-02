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

    private static final int COMPARE_DISPLAY_MS = 200;
    private static final int COMPARE_GAP_MS = 200;
    private static final int COLOR_CORRECT = 0xFF00C853;
    private static final int COLOR_WRONG = 0xFFFF1744;
    private static final int COLOR_DIM = 0xFFFFFFFF;

    private TextView tvTarget;
    private TextView tvResult;
    private Handler handler = new Handler(Looper.getMainLooper());
    private String currentTarget = null;


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

        handler.removeCallbacksAndMessages(null);
    }

    @Override
    public void onTargetDirection(String direction) {
        if (getActivity() == null || tvTarget == null) return;
        getActivity().runOnUiThread(() -> {
            currentTarget = direction;
            boolean isLeft = "LEFT".equals(direction);
            tvTarget.setText(isLeft ? "◀" : "▶");
            tvTarget.setTextColor(COLOR_DIM);

        });
    }

    @Override
    public void onInferenceResult(InferenceResult result) {
        if (getActivity() == null || tvResult == null) return;
        getActivity().runOnUiThread(() -> {
            if (currentTarget == null) return;
            handler.removeCallbacksAndMessages(null);

            String intent = result.getIntent();
            boolean isLeft = "LEFT".equals(intent);
            boolean correct = currentTarget.equals(intent);
            int color = correct ? COLOR_CORRECT : COLOR_WRONG;
            tvTarget.setTextColor(color);
            tvResult.setText(isLeft ? "◀" : "▶");
            tvResult.setTextColor(color);
            tvResult.setVisibility(View.VISIBLE);

            handler.postDelayed(() -> {
                if (getActivity() == null) return;
                tvTarget.setVisibility(View.INVISIBLE);
                tvResult.setVisibility(View.INVISIBLE);
                tvTarget.setTextColor(COLOR_DIM);
            }, COMPARE_DISPLAY_MS);

            handler.postDelayed(() -> {
                if (getActivity() == null) return;
                tvTarget.setVisibility(View.VISIBLE);
                tvResult.setVisibility(View.INVISIBLE);
            }, COMPARE_DISPLAY_MS + COMPARE_GAP_MS);
        });
    }

    @Override
    public void onTrainStop() {
        if (getActivity() == null || tvTarget == null) return;
        getActivity().runOnUiThread(() -> {
            currentTarget = null;

            handler.removeCallbacksAndMessages(null);
            tvTarget.setText("--");
            tvTarget.setTextColor(COLOR_DIM);
            tvTarget.setVisibility(View.VISIBLE);
            tvResult.setText("--");
            tvResult.setTextColor(COLOR_DIM);
            tvResult.setVisibility(View.INVISIBLE);
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
