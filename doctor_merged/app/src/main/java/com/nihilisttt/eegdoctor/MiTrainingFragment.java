package com.nihilisttt.eegdoctor;

import com.nihilisttt.eegdoctor.R;

import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ProgressBar;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;

public class MiTrainingFragment extends Fragment implements DataListener, TrainingModeCoordinator.ModeController {

    private static final int STATE_IDLE = 0;
    private static final int STATE_WAIT_READY = 1;
    private static final int STATE_REST = 2;
    private static final int STATE_TRIAL_ACTIVE = 3;
    private static final int STATE_MODEL_TRAINING = 4;

    private static final int REST_DURATION_MS = 5000;
    private static final int MI_TRIAL_DURATION_MS = 25000;
    private static final String[] TRIAL_SEQUENCE = {"LEFT", "RIGHT", "LEFT", "RIGHT"};

    private int currentState = STATE_IDLE;
    private int currentTrialIndex = 0;
    private boolean isTraining = false;
    private boolean waitingReadyTrain = false;


    private TextView tvDirection;
    private TextView tvHint;
    private TextView tvTrialCount;
    private ProgressBar progressTrial;
    private View btnStartTraining;
    private View btnStopTraining;

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
        btnStartTraining = root.findViewById(R.id.btn_mi_start_training);
        btnStopTraining = root.findViewById(R.id.btn_mi_stop_training);

        btnStartTraining.setOnClickListener(v -> startTraining());
        btnStopTraining.setOnClickListener(v -> stopTraining());
        TrainingModeCoordinator.getInstance().registerController(
                TrainingModeCoordinator.Mode.MI, this);

        return root;
    }

    private void startTraining() {
        Log.i("MiTraining", "btnStartTraining clicked, isTraining=" + isTraining);
        if (isTraining) return;
        TrainingModeCoordinator.getInstance().activate(TrainingModeCoordinator.Mode.MI);
        isTraining = true;
        currentTrialIndex = 0;
        waitingReadyTrain = true;

        btnStartTraining.setEnabled(false);
        btnStopTraining.setEnabled(true);

        tvDirection.setText("启动中...");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.accent_info));
        tvHint.setText("等待下位机就绪");
        tvTrialCount.setText("");
        progressTrial.setProgress(0);

        CommandSender.getInstance().setModeCollect();
        CommandSender.getInstance().startTraining();
    }

    private void stopTraining() {
        stopTrainingInternal(true);
        TrainingModeCoordinator.getInstance().release(TrainingModeCoordinator.Mode.MI);
    }

    private void stopTrainingInternal(boolean updateUi) {
        Log.i("MiTraining", "stopTraining");
        boolean wasActive = isTraining || currentState != STATE_IDLE || waitingReadyTrain;
        isTraining = false;
        waitingReadyTrain = false;
        currentState = STATE_IDLE;
        handler.removeCallbacksAndMessages(null);
        if (wasActive) CommandSender.getInstance().sendCommand("STOP");
        if (updateUi && getView() != null) {
            btnStartTraining.setEnabled(true);
            btnStopTraining.setEnabled(false);
            tvDirection.setText("已停止");
            if (getContext() != null) {
                tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.training_rest));
            }
            tvHint.setText("点击下方按钮开始训练");
            tvTrialCount.setText("");
            progressTrial.setIndeterminate(false);
            progressTrial.setProgress(0);
        }
    }

    @Override
    public void stopForModeSwitch(TrainingModeCoordinator.Mode nextMode) {
        stopTrainingInternal(true);
    }

    private void enterRestPhase() {
        if (!isTraining) return;
        Log.i("MiTraining", "enterRestPhase: trialIndex=" + currentTrialIndex);
        currentState = STATE_REST;
        restStartTime = System.currentTimeMillis();

        tvDirection.setText("休息");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.training_rest));
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
            sendNextTrial();
        } else {
            handler.postDelayed(this::updateRestTimer, 100);
        }
    }

    private void sendNextTrial() {
        if (!isTraining) return;
        if (currentTrialIndex < TRIAL_SEQUENCE.length) {
            String side = TRIAL_SEQUENCE[currentTrialIndex];
            if ("LEFT".equals(side)) {
                CommandSender.getInstance().trialLeft();
            } else {
                CommandSender.getInstance().trialRight();
            }
            tvDirection.setText("发送 " + side + " 试次...");
            tvHint.setText("等待下位机确认");
        }
    }

    private void handleTaskStart(String side) {
        if (!isTraining) return;
        currentState = STATE_TRIAL_ACTIVE;
        trialStartTime = System.currentTimeMillis();
        boolean isLeft = "LEFT".equals(side);

        tvDirection.setText(isLeft ? "← 想象左手" : "想象右手 →");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(),
                isLeft ? R.color.direction_left : R.color.direction_right));
        tvHint.setText("运动想象（" + side + "）");
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

    private void trainingComplete() {
        isTraining = false;
        currentState = STATE_IDLE;
        handler.removeCallbacksAndMessages(null);

        btnStartTraining.setEnabled(true);
        btnStopTraining.setEnabled(false);

        tvDirection.setText("训练完成");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.accent_success));
        tvHint.setText("可以前往方向识别页面测试");
        tvTrialCount.setText("");
        progressTrial.setIndeterminate(false);
        progressTrial.setProgress(progressTrial.getMax());
    }

    private void handleReadyTrain() {
        Log.i("MiTraining", "handleReadyTrain: isTraining=" + isTraining + " waitingReadyTrain=" + waitingReadyTrain);
        if (!isTraining) {
            isTraining = true;
            currentTrialIndex = 0;
        }
        waitingReadyTrain = false;
        enterRestPhase();
    }


    private void handleReadyTest() {

    }

    @Override
    public void onResume() {
        super.onResume();
        DataDispatcher.getInstance().removeListener(this);
        DataDispatcher.getInstance().addListener(this);
    }

    @Override
    public void onPause() {
        super.onPause();
        DataDispatcher.getInstance().removeListener(this);
    }

    @Override
    public void onDestroyView() {
        TrainingModeCoordinator.getInstance().unregisterController(
                TrainingModeCoordinator.Mode.MI, this);
        stopTrainingInternal(false);
        super.onDestroyView();
        DataDispatcher.getInstance().removeListener(this);
        handler.removeCallbacksAndMessages(null);
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
    public void onInferenceResult(InferenceResult result) {
        if (currentState == STATE_MODEL_TRAINING && result.isTrained()) {
            handler.post(this::handleReadyTest);
        }
    }

    @Override public void onWaveData(int cmd, int ch, float val) {}
    @Override public void onSpectrumData(int cmd, float[] mags) {}
    @Override public void onFocusData(float attn0, float attn1, float ema0, float ema1, int trend, int instant) {}
    @Override public void onEegFrame(EegFrame frame) {}
    @Override public void onIpcDiag(IpcDiagInfo diag) {}
    @Override public void onDirConfig(String configJson) {}
    @Override public void onModeSetOk(int mode) {}
}