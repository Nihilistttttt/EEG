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
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;

public class SsvepTrainingFragment extends Fragment implements DoctorConnector.DataListener {

    private static final int STATE_IDLE = 0;
    private static final int STATE_WAIT_READY = 1;
    private static final int STATE_REST = 2;
    private static final int STATE_TRIAL_ACTIVE = 3;
    private static final int STATE_MODEL_TRAINING = 4;

    private static final int REST_DURATION_MS = 3000;
    private static final int SSVEP_FREQ_LEFT_HZ = 8;
    private static final int SSVEP_FREQ_RIGHT_HZ = 12;
    private static final String[] TRIAL_SEQUENCE = {"LEFT", "RIGHT", "LEFT", "RIGHT"};

    private int currentState = STATE_IDLE;
    private int currentTrialIndex = 0;
    private boolean isTraining = false;

    private TextView tvDirection;
    private TextView tvHint;
    private TextView tvTrialCount;
    private ProgressBar progressTrial;

    private View layoutIdle;
    private View layoutSsvep;
    private TextView tvSsvepArrow;
    private TextView tvSsvepFreq;
    private TextView tvSsvepHint;

    private Handler handler = new Handler(Looper.getMainLooper());
    private Runnable ssvepBlinkRunnable;
    private boolean ssvepVisible = true;
    private long ssvepPeriodMs;
    private long restStartTime;

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

        layoutIdle = root.findViewById(R.id.layout_idle);
        layoutSsvep = root.findViewById(R.id.layout_ssvep);
        tvSsvepArrow = root.findViewById(R.id.tv_ssvep_arrow);
        tvSsvepFreq = root.findViewById(R.id.tv_ssvep_freq);
        tvSsvepHint = root.findViewById(R.id.tv_ssvep_hint);

        View btnStart = root.findViewById(R.id.btn_start_training);
        View btnStop = root.findViewById(R.id.btn_stop_training);
        if (btnStart != null) btnStart.setVisibility(View.GONE);
        if (btnStop != null) btnStop.setVisibility(View.GONE);

        showIdleLayout();
        tvDirection.setText("等待指令");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.training_rest));
        tvHint.setText("训练由医生端控制");

        return root;
    }

    private void sendCmd(String cmd) {
        DoctorConnector.getInstance().sendCommand(cmd);
    }

    private void startTraining() {
        if (isTraining) return;
        isTraining = true;
        currentTrialIndex = 0;

        showIdleLayout();
        tvDirection.setText("启动中...");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.accent_info));
        tvHint.setText("等待下位机就绪");
        tvTrialCount.setText("");
        progressTrial.setProgress(0);

        sendCmd("MODE,SET,1");
    }

    private void enterRestPhase() {
        if (!isTraining) return;
        currentState = STATE_REST;
        restStartTime = System.currentTimeMillis();

        showIdleLayout();
        tvDirection.setText("休息");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.training_rest));
        tvHint.setText("放松，准备注视下一个刺激");
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
            sendCmd("TRIAL," + side);
            tvDirection.setText("发送 " + side + " 试次...");
            tvHint.setText("等待下位机确认");
        }
    }

    private void handleTaskStart(String side) {
        if (!isTraining) return;
        currentState = STATE_TRIAL_ACTIVE;
        boolean isLeft = "LEFT".equals(side);

        showSsvepLayout();
        tvSsvepArrow.setText(isLeft ? "←" : "→");
        tvSsvepArrow.setTextColor(ContextCompat.getColor(requireContext(),
                isLeft ? R.color.direction_left : R.color.direction_right));

        int freqHz = isLeft ? SSVEP_FREQ_LEFT_HZ : SSVEP_FREQ_RIGHT_HZ;
        tvSsvepFreq.setText(freqHz + " Hz");
        tvSsvepHint.setText("注视闪烁箭头（" + side + "）");

        tvTrialCount.setText("试次 " + (currentTrialIndex + 1) + " / " + TRIAL_SEQUENCE.length);
        progressTrial.setMax(100);
        progressTrial.setProgress(0);

        startSsvepBlink(freqHz);
    }

    private void handleTaskDone() {
        if (!isTraining) return;
        stopSsvepBlink();
        currentTrialIndex++;

        if (currentTrialIndex < TRIAL_SEQUENCE.length) {
            enterRestPhase();
        } else {
            startModelTraining();
        }
    }

    private void startModelTraining() {
        currentState = STATE_MODEL_TRAINING;
        showIdleLayout();
        tvDirection.setText("训练模型");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.accent_info));
        tvHint.setText("正在训练模型，请稍候...");
        tvTrialCount.setText("");
        progressTrial.setIndeterminate(true);

        sendCmd("MODE,TRAIN");
    }

    private void handleReadyTrain() {
        if (!isTraining) {
            isTraining = true;
            currentTrialIndex = 0;
        }
        enterRestPhase();
    }

    private void handleReadyTest() {
        if (!isTraining || currentState != STATE_MODEL_TRAINING) return;
        sendCmd("MODE,TEST");
        trainingComplete();
    }

    private void handleModeSetOk(int mode) {
        if (!isTraining) return;
        if (mode == 1) {
            sendCmd("MODE,TRAIN");
        }
    }

    private void trainingComplete() {
        isTraining = false;
        showIdleLayout();
        tvDirection.setText("训练完成");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.accent_success));
        tvHint.setText("可以前往方向识别页面测试");
        tvTrialCount.setText("");
        progressTrial.setIndeterminate(false);
        progressTrial.setProgress(progressTrial.getMax());
    }

    private void showIdleLayout() {
        if (layoutIdle != null) layoutIdle.setVisibility(View.VISIBLE);
        if (layoutSsvep != null) layoutSsvep.setVisibility(View.GONE);
    }

    private void showSsvepLayout() {
        if (layoutIdle != null) layoutIdle.setVisibility(View.GONE);
        if (layoutSsvep != null) layoutSsvep.setVisibility(View.VISIBLE);
    }

    private void startSsvepBlink(int freqHz) {
        stopSsvepBlink();
        ssvepPeriodMs = 1000L / (freqHz * 2);
        ssvepVisible = true;
        ssvepBlinkRunnable = new Runnable() {
            @Override
            public void run() {
                if (!isTraining || currentState != STATE_TRIAL_ACTIVE) return;
                ssvepVisible = !ssvepVisible;
                tvSsvepArrow.setVisibility(ssvepVisible ? View.VISIBLE : View.INVISIBLE);
                handler.postDelayed(this, ssvepPeriodMs);
            }
        };
        handler.postDelayed(ssvepBlinkRunnable, ssvepPeriodMs);
    }

    private void stopSsvepBlink() {
        if (ssvepBlinkRunnable != null) {
            handler.removeCallbacks(ssvepBlinkRunnable);
            ssvepBlinkRunnable = null;
        }
        ssvepVisible = true;
        if (tvSsvepArrow != null) {
            tvSsvepArrow.setVisibility(View.VISIBLE);
        }
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
        handler.removeCallbacksAndMessages(null);
        stopSsvepBlink();
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
    @Override public void onConnectionChanged(boolean connected) {}

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
        handler.post(() -> handleModeSetOk(mode));
    }

    @Override public void onPageSwitch(int page) {}
    @Override public void onFocusData(float attn0, float attn1, float ema0, float ema1, int trend, int instant) {}
}
