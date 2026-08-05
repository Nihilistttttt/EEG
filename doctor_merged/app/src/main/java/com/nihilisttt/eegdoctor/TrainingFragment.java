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
    private boolean waitingReadyTrain = false;


    private TextView tvDirection;
    private TextView tvHint;
    private TextView tvTrialCount;
    private ProgressBar progressTrial;
    private View btnStartTraining;
    private View btnStopTraining;

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
        btnStartTraining = root.findViewById(R.id.btn_start_training);
        btnStopTraining = root.findViewById(R.id.btn_stop_training);

        layoutIdle = root.findViewById(R.id.layout_idle);
        layoutSsvep = root.findViewById(R.id.layout_ssvep);
        tvSsvepArrow = root.findViewById(R.id.tv_ssvep_arrow);
        tvSsvepFreq = root.findViewById(R.id.tv_ssvep_freq);
        tvSsvepHint = root.findViewById(R.id.tv_ssvep_hint);

        btnStartTraining.setOnClickListener(v -> startTraining());
        btnStopTraining.setOnClickListener(v -> stopTraining());

        return root;
    }

    private void startTraining() {
        if (isTraining) return;
        isTraining = true;
        currentTrialIndex = 0;
        waitingReadyTrain = true;


        btnStartTraining.setEnabled(false);
        btnStopTraining.setEnabled(true);

        showIdleLayout();
        tvDirection.setText("启动中...");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.accent_info));
        tvHint.setText("等待下位机就绪");
        tvTrialCount.setText("");
        progressTrial.setProgress(0);

        CommandSender.getInstance().setModeCollect();
        CommandSender.getInstance().startTraining();
    }

    private void stopTraining() {
        isTraining = false;
        waitingReadyTrain = false;

        currentState = STATE_IDLE;
        handler.removeCallbacksAndMessages(null);
        stopSsvepBlink();
        TcpServerManager.getInstance().sendBinaryToDevice(EegProtocol.CMD_STOP, null);
        TcpServerManager.getInstance().sendDisplayToOutput(EegProtocol.CMD_TRAIN_STOP, null);

        btnStartTraining.setEnabled(true);
        btnStopTraining.setEnabled(false);

        showIdleLayout();
        tvDirection.setText("已停止");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.training_rest));
        tvHint.setText("点击下方按钮开始训练");
        tvTrialCount.setText("");
        progressTrial.setProgress(0);
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
            if ("LEFT".equals(side)) {
                CommandSender.getInstance().trialLeft();
            } else {
                CommandSender.getInstance().trialRight();
            }
            tvDirection.setText("发送 " + side + " 试次...");
            tvHint.setText("等待下位机确认");
        }
    }

    private void onTaskStartInternal(String side) {
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

    private void onTaskDoneInternal() {
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

        CommandSender.getInstance().startTraining();
    }

    private void onReadyTrainInternal() {
        if (!isTraining) return;
        waitingReadyTrain = false;
        enterRestPhase();
    }

    private void onReadyTestInternal() {
        if (!isTraining || currentState != STATE_MODEL_TRAINING) return;
        CommandSender.getInstance().startTest();
        trainingComplete();
    }

    private void onModeSetOkInternal(int mode) {
    }

    private void trainingComplete() {
        isTraining = false;
        waitingReadyTrain = false;
        btnStartTraining.setEnabled(true);
        btnStopTraining.setEnabled(false);

        showIdleLayout();
        tvDirection.setText("训练完成");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.accent_success));
        tvHint.setText("可以前往方向推理页面测试");
        tvTrialCount.setText("");
        progressTrial.setIndeterminate(false);
        progressTrial.setProgress(progressTrial.getMax());
    }

    private void showIdleLayout() {
        layoutIdle.setVisibility(View.VISIBLE);
        layoutSsvep.setVisibility(View.GONE);
    }

    private void showSsvepLayout() {
        layoutIdle.setVisibility(View.GONE);
        layoutSsvep.setVisibility(View.VISIBLE);
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
        stopSsvepBlink();
    }

    @Override
    public void onTaskStart(String side) {
        handler.post(() -> onTaskStartInternal(side));
    }

    @Override
    public void onTaskDone() {
        handler.post(this::onTaskDoneInternal);
    }

    @Override
    public void onReadyTrain() {
        handler.post(this::onReadyTrainInternal);
    }

    @Override
    public void onReadyTest() {
        handler.post(this::onReadyTestInternal);
    }

    @Override
    public void onModeSetOk(int mode) {
        handler.post(() -> onModeSetOkInternal(mode));
    }

    @Override
    public void onInferenceResult(InferenceResult result) {
        if (currentState == STATE_MODEL_TRAINING && result.isTrained()) {
            handler.post(this::onReadyTestInternal);
        }
    }

    @Override public void onWaveData(int cmd, int ch, float val) {}
    @Override public void onSpectrumData(int cmd, float[] mags) {}
    @Override public void onFocusData(float attn0, float attn1, float ema0, float ema1, int trend, int instant) {}
    @Override public void onEegFrame(EegFrame frame) {}
    @Override public void onIpcDiag(IpcDiagInfo diag) {}
    @Override public void onDirConfig(String configJson) {}
}
