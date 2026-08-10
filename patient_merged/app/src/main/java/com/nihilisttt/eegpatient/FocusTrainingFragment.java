package com.nihilisttt.eegpatient;

import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import java.util.Locale;
import java.util.Random;

public class FocusTrainingFragment extends Fragment implements DoctorConnector.DataListener, TrainingModeCoordinator.ModeController {

    private static final String TAG = "PATIENT_FOCUS";
    private static final String[] WORDS = {"红", "黄", "绿", "蓝", "白", "黑"};
    private static final int[] COLORS = {
        0xFFFF0000, 0xFFFFFF00, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF5A5A5A
    };
    private static final long SWITCH_PERIOD_MS = 1000;

    private final Handler handler = new Handler(Looper.getMainLooper());
    private final Random rng = new Random();
    private TextView tvStroopWord;
    private TextView btnStart;
    private TextView btnStop;
    private TextView tvStatus;
    private TextView tvFocusData;
    private boolean running;
    private int wordIdx;
    private int colorIdx;

    private final Runnable switchRunnable = new Runnable() {
        @Override
        public void run() {
            if (!running) return;
            stroopPick();
            tvStroopWord.setText(WORDS[wordIdx]);
            tvStroopWord.setTextColor(COLORS[colorIdx]);
            handler.postDelayed(this, SWITCH_PERIOD_MS);
        }
    };

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_focus_training, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        tvStroopWord = view.findViewById(R.id.tv_stroop_word);
        btnStart = view.findViewById(R.id.btn_focus_start);
        btnStop = view.findViewById(R.id.btn_focus_stop);
        tvStatus = view.findViewById(R.id.tv_focus_status);
        tvFocusData = view.findViewById(R.id.tv_focus_data);

        btnStart.setOnClickListener(v -> startStroop());
        btnStop.setOnClickListener(v -> stopStroop());

        TrainingModeCoordinator.getInstance().registerController(
                TrainingModeCoordinator.Mode.FOCUS, this);
        DoctorConnector.getInstance().addListener(this);
    }

    private void stroopPick() {
        wordIdx = rng.nextInt(WORDS.length);
        int c;
        do { c = rng.nextInt(COLORS.length); }
        while (c == wordIdx);
        colorIdx = c;
    }

    private void startStroop() {
        if (running) return;
        running = true;
        TrainingModeCoordinator.getInstance().activate(TrainingModeCoordinator.Mode.FOCUS);
        btnStart.setAlpha(0.5f);
        btnStop.setAlpha(1.0f);
        tvStatus.setText("训练中");
        tvStatus.setTextColor(0xFF69F0AE);
        DoctorConnector.getInstance().sendControlMessage("FOCUS,START");
        handler.post(switchRunnable);
        Log.i(TAG, "Stroop started");
    }

    private void stopStroop() {
        if (!running) return;
        running = false;
        handler.removeCallbacks(switchRunnable);
        btnStart.setAlpha(1.0f);
        btnStop.setAlpha(0.5f);
        tvStatus.setText("未开始");
        tvStatus.setTextColor(0xFFEEEEEE);
        tvStroopWord.setText("等待开始");
        tvStroopWord.setTextColor(0xFF90A4AE);
        tvFocusData.setText("专注度: --");
        DoctorConnector.getInstance().sendControlMessage("FOCUS,STOP");
        TrainingModeCoordinator.getInstance().release(TrainingModeCoordinator.Mode.FOCUS);
        Log.i(TAG, "Stroop stopped");
    }

    @Override
    public void stopForModeSwitch(TrainingModeCoordinator.Mode nextMode) {
        stopStroop();
    }

    @Override
    public void onFocusData(float attn0, float attn1, float ema0, float ema1, int trend, int instant) {
        handler.post(() -> {
            if (tvFocusData != null && running) {
                tvFocusData.setText(String.format(Locale.US,
                    "专注度: %.1f / %.1f  EMA: %.1f / %.1f  趋势:%d",
                    attn0, attn1, ema0, ema1, trend));
            }
        });
    }

    @Override
    public void onConnectionChanged(boolean connected) {
        if (!connected) handler.post(this::stopStroop);
    }

    @Override
    public void onDestroyView() {
        handler.removeCallbacks(switchRunnable);
        running = false;
        TrainingModeCoordinator.getInstance().unregisterController(
                TrainingModeCoordinator.Mode.FOCUS, this);
        DoctorConnector.getInstance().removeListener(this);
        TrainingModeCoordinator.getInstance().release(TrainingModeCoordinator.Mode.FOCUS);
        tvStroopWord = null;
        btnStart = null;
        btnStop = null;
        tvStatus = null;
        tvFocusData = null;
        super.onDestroyView();
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
    @Override public void onSsvepStart(int freqIndex) {}
    @Override public void onSsvepStop() {}
    @Override public void onSsvepResult(PatientSsvepResult result) {}
    @Override public void onTargetDirection(String direction) {}
    @Override public void onTrainStop() {}
}