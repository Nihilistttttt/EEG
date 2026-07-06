package com.nihilisttt.eegdoctor;

import com.nihilisttt.eegdoctor.R;

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
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;

public class TrainingFragment extends Fragment implements DataListener {

    private static final int STATE_IDLE = 0;
    private static final int STATE_REST = 1;
    private static final int STATE_TRIAL_LEFT = 2;
    private static final int STATE_TRIAL_RIGHT = 3;
    private static final int STATE_TRAINING = 4;

    private static final int REST_DURATION_MS = 5000;
    private static final int TRIAL_DURATION_MS = 2000;
    private static final int DEFAULT_TOTAL_TRIALS = 10;

    private int currentState = STATE_IDLE;
    private int currentTrial = 0;
    private int totalTrials = DEFAULT_TOTAL_TRIALS;
    private boolean isTraining = false;

    private TextView tvDirection;
    private TextView tvHint;
    private TextView tvTrialCount;
    private ProgressBar progressTrial;
    private View btnStartTraining;
    private View btnStopTraining;

    private Handler handler = new Handler(Looper.getMainLooper());
    private long phaseStartTime;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_training, container, false);

        tvDirection = root.findViewById(R.id.tv_trial_direction);
        tvHint = root.findViewById(R.id.tv_trial_hint);
        tvTrialCount = root.findViewById(R.id.tv_trial_count);
        progressTrial = root.findViewById(R.id.progress_trial);
        btnStartTraining = root.findViewById(R.id.btn_start_training);
        btnStopTraining = root.findViewById(R.id.btn_stop_training);

        btnStartTraining.setOnClickListener(v -> startTraining());
        btnStopTraining.setOnClickListener(v -> stopTraining());

        return root;
    }

    private void startTraining() {
        if (isTraining) return;
        isTraining = true;
        currentTrial = 0;
        CommandSender.getInstance().setModeCollect();
        btnStartTraining.setEnabled(false);
        btnStopTraining.setEnabled(true);
        enterRestPhase();
    }

    private void stopTraining() {
        isTraining = false;
        currentState = STATE_IDLE;
        handler.removeCallbacksAndMessages(null);
        CommandSender.getInstance().setModeCollect();
        btnStartTraining.setEnabled(true);
        btnStopTraining.setEnabled(false);
        tvDirection.setText("准备开始");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.training_rest));
        tvHint.setText("点击下方按钮开始训练");
        tvTrialCount.setText("");
        progressTrial.setProgress(0);
    }

    private void enterRestPhase() {
        if (!isTraining) return;
        currentState = STATE_REST;
        phaseStartTime = System.currentTimeMillis();
        tvDirection.setText("休息");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.training_rest));
        tvHint.setText("放松，准备下一个动作");
        tvTrialCount.setText("试次 " + (currentTrial + 1) + " / " + totalTrials);
        progressTrial.setMax(REST_DURATION_MS / 100);
        progressTrial.setProgress(0);
        updateRestTimer();
    }

    private void updateRestTimer() {
        if (!isTraining || currentState != STATE_REST) return;
        long elapsed = System.currentTimeMillis() - phaseStartTime;
        int remaining = Math.max(0, (REST_DURATION_MS - (int) elapsed) / 1000);
        tvHint.setText("放松，" + remaining + " 秒后开始");
        progressTrial.setProgress((int) (elapsed / 100));
        if (elapsed >= REST_DURATION_MS) {
            enterTrialPhase();
        } else {
            handler.postDelayed(this::updateRestTimer, 100);
        }
    }

    private void enterTrialPhase() {
        if (!isTraining) return;
        boolean isLeft = (currentTrial % 2 == 0);
        currentState = isLeft ? STATE_TRIAL_LEFT : STATE_TRIAL_RIGHT;
        phaseStartTime = System.currentTimeMillis();

        if (isLeft) {
            CommandSender.getInstance().trialLeft();
            tvDirection.setText("← 左手");
            tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.direction_left));
            tvHint.setText("想象左手运动");
        } else {
            CommandSender.getInstance().trialRight();
            tvDirection.setText("右手 →");
            tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.direction_right));
            tvHint.setText("想象右手运动");
        }

        tvTrialCount.setText("试次 " + (currentTrial + 1) + " / " + totalTrials);
        progressTrial.setMax(TRIAL_DURATION_MS / 100);
        progressTrial.setProgress(0);
        updateTrialTimer();
    }

    private void updateTrialTimer() {
        if (!isTraining || (currentState != STATE_TRIAL_LEFT && currentState != STATE_TRIAL_RIGHT)) return;
        long elapsed = System.currentTimeMillis() - phaseStartTime;
        progressTrial.setProgress((int) (elapsed / 100));
        if (elapsed >= TRIAL_DURATION_MS) {
            currentTrial++;
            if (currentTrial >= totalTrials) {
                startModelTraining();
            } else {
                enterRestPhase();
            }
        } else {
            handler.postDelayed(this::updateTrialTimer, 100);
        }
    }

    private void startModelTraining() {
        currentState = STATE_TRAINING;
        CommandSender.getInstance().startTraining();
        tvDirection.setText("训练中");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.accent_info));
        tvHint.setText("正在训练模型，请稍候...");
        tvTrialCount.setText("");
        progressTrial.setProgress(0);
        handler.postDelayed(() -> {
            if (isTraining && currentState == STATE_TRAINING) {
                CommandSender.getInstance().startTest();
                trainingComplete();
            }
        }, 5000);
    }

    private void trainingComplete() {
        isTraining = false;
        btnStartTraining.setEnabled(true);
        btnStopTraining.setEnabled(false);
        tvDirection.setText("训练完成");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.accent_success));
        tvHint.setText("可以前往方向推理页面测试");
        tvTrialCount.setText("");
        progressTrial.setProgress(progressTrial.getMax());
    }

    @Override
    public void onResume() {
        super.onResume();
        DataDispatcher.getInstance().addListener(this);
    }

    @Override
    public void onPause() {
        super.onPause();
        DataDispatcher.getInstance().removeListener(this);
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        handler.removeCallbacksAndMessages(null);
    }

    @Override
    public void onInferenceResult(InferenceResult result) {
        if (currentState == STATE_TRAINING && result.isTrained()) {
            handler.post(this::trainingComplete);
        }
    }

    @Override
    public void onWaveData(int cmd, float ch0, float ch1) {}
    @Override
    public void onSpectrumData(int cmd, float[] mags) {}
    @Override
    public void onFocusData(float attn0, float attn1, float ema0, float ema1, int trend, int instant) {}
    @Override
    public void onEegFrame(EegFrame frame) {}
    @Override
    public void onIpcDiag(IpcDiagInfo diag) {}
    @Override
    public void onDirConfig(String configJson) {}
}
