package com.nihilisttt.eegpatient;

import android.graphics.Color;
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

import java.util.Random;

public class FocusTrainingFragment extends Fragment implements DoctorConnector.DataListener {

    private static final String[] STROOP_WORDS = {"红", "黄", "绿", "蓝", "白", "黑"};
    private static final int[] STROOP_COLORS = {
            Color.rgb(255, 0, 0),
            Color.rgb(255, 255, 0),
            Color.rgb(0, 255, 0),
            Color.rgb(0, 0, 255),
            Color.rgb(255, 255, 255),
            Color.rgb(90, 90, 90)
    };
    private static final long STROOP_PERIOD_MS = 1000L;

    private TextView tvStroopWord;
    private TextView tvFocusVal;
    private TextView tvRelaxVal;
    private TextView tvFocusStatus;
    private final Handler stroopHandler = new Handler(Looper.getMainLooper());
    private final Random rng = new Random();
    private boolean running = false;

    private final Runnable stroopTick = new Runnable() {
        @Override
        public void run() {
            if (!running) return;
            int word = rng.nextInt(STROOP_WORDS.length);
            int color;
            do {
                color = rng.nextInt(STROOP_COLORS.length);
            } while (color == word);
            tvStroopWord.setText(STROOP_WORDS[word]);
            tvStroopWord.setTextColor(STROOP_COLORS[color]);
            stroopHandler.postDelayed(this, STROOP_PERIOD_MS);
        }
    };

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_focus_training, container, false);
        tvStroopWord = root.findViewById(R.id.tv_stroop_word);
        tvFocusVal = root.findViewById(R.id.tv_focus_val);
        tvRelaxVal = root.findViewById(R.id.tv_relax_val);
        tvFocusStatus = root.findViewById(R.id.tv_focus_status);
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
        stopStroop();
    }

    private void startStroop() {
        if (running) return;
        running = true;
        tvFocusStatus.setText("训练中");
        stroopHandler.post(stroopTick);
    }

    private void stopStroop() {
        running = false;
        stroopHandler.removeCallbacks(stroopTick);
        tvStroopWord.setText("");
        tvFocusStatus.setText("未开始");
    }

    @Override
    public void onFocusTrainingStart() {
        if (getActivity() != null) getActivity().runOnUiThread(this::startStroop);
    }

    @Override
    public void onFocusTrainingStop() {
        if (getActivity() != null) getActivity().runOnUiThread(this::stopStroop);
    }

    @Override
    public void onFocusData(float attn0, float attn1, float ema0, float ema1, int trend, int instant) {
        if (getActivity() != null) getActivity().runOnUiThread(() -> {
            tvFocusVal.setText(String.format("专注: %.0f%%", ema0 * 100));
            tvRelaxVal.setText(String.format("放松: %.0f%%", ema1 * 100));
        });
    }

    @Override public void onWaveData(int cmd, float ch0, float ch1) {}
    @Override public void onEegFrame(EegFrame frame) {}
    @Override public void onInferenceResult(InferenceResult result) {}
    @Override public void onIpcDiag(IpcDiagInfo diag) {}
    @Override public void onConnectionChanged(boolean connected) {}
    @Override public void onTaskStart(String side) {}
    @Override public void onTaskDone() {}
    @Override public void onReadyTrain() {}
    @Override public void onReadyTest() {}
    @Override public void onModeSetOk(int mode) {}
    @Override public void onPageSwitch(int page) {}
}
