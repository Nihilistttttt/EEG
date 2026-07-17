package com.nihilisttt.eegpatient;

import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ProgressBar;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

public class MiTrainingFragment extends Fragment implements DoctorConnector.DataListener, TrainingModeCoordinator.ModeController {

    private static final int STATE_IDLE = 0;
    private static final int STATE_REST = 1;
    private static final int STATE_TRIAL_ACTIVE = 2;
    private static final int STATE_MODEL_TRAINING = 3;

    private static final int REST_DURATION_MS = 5000;
    private static final int MI_TRIAL_DURATION_MS = 25000;
    private static final String[] TRIAL_SEQUENCE = {"LEFT", "RIGHT", "LEFT", "RIGHT"};

    private int currentState = STATE_IDLE;
    private int currentTrialIndex = 0;
    private boolean isTraining = false;

    private TextView tvDirection;
    private TextView tvHint;
    private TextView tvTrialCount;
    private ProgressBar progressTrial;

    private Handler handler = new Handler(Looper.getMainLooper());
    private long restStartTime;
    private long trialStartTime;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_mi_training, container, false);

        tvDirection = root.findViewById(R.id.tv_mi_direction);
        tvHint = root.findViewById(R.id.tv_mi_hint);
        tvTrialCount = root.findViewById(R.id.tv_mi_trial_count);
        progressTrial = root.findViewById(R.id.progress_mi_trial);

        tvDirection.setText("运动想象训练");
        tvHint.setText("训练由医生端控制");
        TrainingModeCoordinator.getInstance().registerController(
                TrainingModeCoordinator.Mode.MI, this);

        return root;
    }

    private void enterRestPhase() {
        if (!isTraining) return;
        currentState = STATE_REST;
        restStartTime = System.currentTimeMillis();

        tvDirection.setText("休息");
        tvDirection.setTextColor(0xFF607D8B);
        tvHint.setText("放松，准备下一个想象任务");
        tvTrialCount.setText("试次 " + (currentTrialIndex + 1) + " / " + TRIAL_SEQUENCE.length);
        progressTrial.setMax(REST_DURATION_MS / 100);
        progressTrial.setProgress(0);

        updateRestTimer();
    }

    private void updateRestTimer() {
        if (!isTraining || currentState != STATE_REST) return;
        long elapsed = System.currentTimeMillis() - restStartTime;
        int remaining = Math.max(0, (REST_DURATION_MS - (int) elapsed) / 1000);
        tvHint.setText("放松，" + remaining + " 秒后开始");
        progressTrial.setProgress((int) (elapsed / 100));
        if (elapsed >= REST_DURATION_MS) {
            progressTrial.setProgress(progressTrial.getMax());
        } else {
            handler.postDelayed(this::updateRestTimer, 100);
        }
    }

    private void handleTaskStart(String side) {
        if (!isTraining) return;
        currentState = STATE_TRIAL_ACTIVE;
        trialStartTime = System.currentTimeMillis();
        boolean isLeft = "LEFT".equals(side);

        tvDirection.setText(isLeft ? "←" : "→");
        tvDirection.setTextColor(isLeft ? 0xFF1565C0 : 0xFFE65100);
        tvHint.setText(isLeft ? "想象左手" : "想象右手");
        tvTrialCount.setText("试次 " + (currentTrialIndex + 1) + " / " + TRIAL_SEQUENCE.length);
        progressTrial.setMax(MI_TRIAL_DURATION_MS / 100);
        progressTrial.setProgress(0);

        updateTrialTimer();
    }

    private void updateTrialTimer() {
        if (!isTraining || currentState != STATE_TRIAL_ACTIVE) return;
        long elapsed = System.currentTimeMillis() - trialStartTime;
        progressTrial.setProgress((int) (elapsed / 100));
        if (elapsed >= MI_TRIAL_DURATION_MS) {
            progressTrial.setProgress(progressTrial.getMax());
        } else {
            handler.postDelayed(this::updateTrialTimer, 100);
        }
    }

    private void handleTaskDone() {
        if (!isTraining) return;
        currentTrialIndex++;

        if (currentTrialIndex < TRIAL_SEQUENCE.length) {
            enterRestPhase();
        } else {
            trainingComplete();
        }
    }

    private void startModelTraining() {
        currentState = STATE_MODEL_TRAINING;
        tvDirection.setText("训练中");
        tvDirection.setTextColor(0xFF40C4FF);
        tvHint.setText("正在训练模型，请稍候...");
        tvTrialCount.setText("");
        progressTrial.setIndeterminate(true);
    }

    private void handleReadyTrain() {
        TrainingModeCoordinator.getInstance().activate(TrainingModeCoordinator.Mode.MI);
        if (!isTraining) {
            isTraining = true;
            currentTrialIndex = 0;
        }
        enterRestPhase();
    }

    private void handleReadyTest() {
        if (!isTraining || currentState != STATE_MODEL_TRAINING) return;
        trainingComplete();
    }

    private void trainingComplete() {
        isTraining = false;
        currentState = STATE_IDLE;
        handler.removeCallbacksAndMessages(null);
        tvDirection.setText("训练完成");
        tvDirection.setTextColor(0xFF00C853);
        tvHint.setText("可以前往方向识别页面测试");
        tvTrialCount.setText("");
        progressTrial.setIndeterminate(false);
        progressTrial.setProgress(progressTrial.getMax());
    }

    private void resetForModeSwitch() {
        isTraining = false;
        currentState = STATE_IDLE;
        currentTrialIndex = 0;
        handler.removeCallbacksAndMessages(null);
        if (getView() != null) {
            tvDirection.setText("运动想象训练");
            tvDirection.setTextColor(0xFF607D8B);
            tvHint.setText("训练由医生端控制");
            tvTrialCount.setText("");
            progressTrial.setIndeterminate(false);
            progressTrial.setProgress(0);
        }
    }

    @Override
    public void stopForModeSwitch(TrainingModeCoordinator.Mode nextMode) {
        resetForModeSwitch();
    }

    @Override
    public void onResume() {
        super.onResume();
        DoctorConnector.getInstance().removeListener(this);
        DoctorConnector.getInstance().addListener(this);
    }

    @Override
    public void onDestroyView() {
        TrainingModeCoordinator.getInstance().unregisterController(
                TrainingModeCoordinator.Mode.MI, this);
        resetForModeSwitch();
        super.onDestroyView();
        DoctorConnector.getInstance().removeListener(this);
    }

    @Override
    public void onInferenceResult(InferenceResult result) {
        if (currentState == STATE_MODEL_TRAINING && result.isTrained()) {
            handler.post(this::handleReadyTest);
        }
    }

    @Override public void onWaveData(int cmd, float ch0, float ch1) {}
    @Override public void onEegFrame(EegFrame frame) {}
    @Override public void onIpcDiag(IpcDiagInfo diag) {}
    @Override
    public void onConnectionChanged(boolean connected) {
        if (!connected) handler.post(this::resetForModeSwitch);
    }

    @Override
    public void onTaskStart(String side) {
        handler.post(() -> handleTaskStart(side));
    }

    @Override
    public void onTaskDone() {
        handler.post(this::handleTaskDone);
    }

    @Override
    public void onReadyTrain() {
        handler.post(this::handleReadyTrain);
    }

    @Override
    public void onReadyTest() {
        handler.post(this::handleReadyTest);
    }

    @Override
    public void onModeSetOk(int mode) {
        handler.post(() -> {
            if (!isTraining) return;
            if (mode == 1) startModelTraining();
        });
    }

    @Override public void onPageSwitch(int page) {}
    @Override public void onFocusData(float attn0, float attn1, float ema0, float ema1, int trend, int instant) {}
}
