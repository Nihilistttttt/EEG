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

/** Mode-1 patient page: one doctor-selected full-screen SSVEP target. */
public class SsvepTrainingFragment extends Fragment implements DoctorConnector.DataListener, TrainingModeCoordinator.ModeController {

    private static final String TAG = "PATIENT_SSVEP";
    private static final float[] FREQS = {11.0f, 13.0f, 15.0f, 17.0f};
    private static final String[] ARROWS = {"↑", "↓", "←", "→"};
    private static final String[] DIRECTIONS = {"上", "下", "左", "右"};
    private static final int[] COLORS = {0xFF4CAF50, 0xFFFF9800, 0xFF1565C0, 0xFFE65100};

    private final Handler handler = new Handler(Looper.getMainLooper());
    private TextView tvStatus;
    private TextView tvResult;
    private TextView tvDetails;
    private SsvepStimulusView stimulusView;
    private boolean isBlinking;
    private int currentFreqIndex = -1;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_training, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        tvStatus = view.findViewById(R.id.tv_trial_direction);
        tvResult = view.findViewById(R.id.tv_ssvep_patient_result);
        tvDetails = view.findViewById(R.id.tv_ssvep_patient_details);
        stimulusView = view.findViewById(R.id.ssvep_stimulus_view);
        showWaiting();
        TrainingModeCoordinator.getInstance().registerController(
                TrainingModeCoordinator.Mode.SSVEP, this);
        DoctorConnector.getInstance().addListener(this);
    }

    private float getRefreshRate() {
        if (getActivity() == null) return 0f;
        try {
            return getActivity().getWindowManager().getDefaultDisplay().getRefreshRate();
        } catch (Exception ignored) {
            return 0f;
        }
    }

    private void sendStimulusStarted(int freqIndex, long frameTimeNanos) {
        float refresh = getRefreshRate();
        String line = String.format(Locale.US,
                "SSVEP,STIM_STARTED,%d,%.3f,%d", freqIndex, refresh, frameTimeNanos);
        DoctorConnector.getInstance().sendControlMessage(line);
        Log.i(TAG, "stimulus feedback: " + line);
    }

    private void showWaiting() {
        isBlinking = false;
        currentFreqIndex = -1;
        if (stimulusView != null) {
            stimulusView.stopStimulus();
            stimulusView.setVisibility(View.GONE);
        }
        if (tvStatus != null) {
            tvStatus.setVisibility(View.VISIBLE);
            tvStatus.setText(DoctorConnector.getInstance().isConnected()
                    ? "等待医生端选择单一刺激频率"
                    : "等待医生端数据与控制通道连接");
            tvStatus.setTextColor(0xFF90A4AE);
        }
        clearResult();
        TrainingModeCoordinator.getInstance().release(TrainingModeCoordinator.Mode.SSVEP);
    }

    private void clearResult() {
        if (tvResult != null) tvResult.setText("识别结果: --");
        if (tvDetails != null) tvDetails.setText("5秒窗口 / 2秒步长 / 3票确认");
    }

    private void startStimulus(int freqIndex) {
        if (!isAdded() || getView() == null) return;
        TrainingModeCoordinator.getInstance().activate(TrainingModeCoordinator.Mode.SSVEP);
        if (freqIndex < 0 || freqIndex >= FREQS.length) return;

        if (isBlinking && currentFreqIndex == freqIndex) {
            // State snapshot after reconnect: acknowledge the already-running stimulus again.
            sendStimulusStarted(freqIndex, System.nanoTime());
            return;
        }

        isBlinking = true;
        currentFreqIndex = freqIndex;
        clearResult();
        if (tvStatus != null) tvStatus.setVisibility(View.GONE);
        stimulusView.setVisibility(View.VISIBLE);
        stimulusView.setArrow(ARROWS[freqIndex], COLORS[freqIndex]);
        stimulusView.setFreqHz(FREQS[freqIndex]);
        stimulusView.setOnFirstFrameListener(frameTimeNanos ->
                sendStimulusStarted(freqIndex, frameTimeNanos));
        stimulusView.startStimulus();
        Log.i(TAG, "start mode-1 stimulus index=" + freqIndex + " freq=" + FREQS[freqIndex]);
    }

    private void showStopped() {
        isBlinking = false;
        currentFreqIndex = -1;
        if (stimulusView != null) {
            stimulusView.stopStimulus();
            stimulusView.setVisibility(View.GONE);
        }
        if (tvStatus != null) {
            tvStatus.setVisibility(View.VISIBLE);
            tvStatus.setText("刺激已停止，等待医生端指令");
            tvStatus.setTextColor(0xFF90A4AE);
        }
        TrainingModeCoordinator.getInstance().release(TrainingModeCoordinator.Mode.SSVEP);
    }

    private void displayResult(PatientSsvepResult result) {
        if (result == null || tvResult == null || tvDetails == null) return;
        int index = result.getFreqIndex();
        if (!result.isUsable()) {
            tvResult.setText("识别结果: 通道质量不合格");
            tvResult.setTextColor(0xFFFF5252);
        } else if (result.isUncertain() || index < 0) {
            tvResult.setText("识别结果: UNCERTAIN");
            tvResult.setTextColor(0xFFFFB300);
        } else {
            tvResult.setText(String.format(Locale.US,
                    "识别结果: %s（%.0f Hz）", DIRECTIONS[index], FREQS[index]));
            tvResult.setTextColor(0xFF69F0AE);
        }
        float[] scores = result.getScores();
        int[] votes = result.getVotes();
        tvDetails.setText(String.format(Locale.US,
                "raw=%s  ratio=%.3f  margin=%.5f%s\n"
                        + "score 11/13/15/17: %.3f / %.3f / %.3f / %.3f\n"
                        + "vote: %d / %d / %d / %d",
                result.getRawFreq(), result.getRatio(), result.getMargin(),
                result.isSynthetic() ? "  [无设备自测]" : "",
                scores[0], scores[1], scores[2], scores[3],
                votes[0], votes[1], votes[2], votes[3]));
    }

    @Override
    public void stopForModeSwitch(TrainingModeCoordinator.Mode nextMode) {
        showStopped();
    }

    @Override
    public void onSsvepStart(int freqIndex) {
        handler.post(() -> startStimulus(freqIndex));
    }

    @Override
    public void onSsvepStop() {
        handler.post(this::showStopped);
    }

    @Override
    public void onSsvepResult(PatientSsvepResult result) {
        handler.post(() -> displayResult(result));
    }

    @Override
    public void onConnectionChanged(boolean connected) {
        if (!connected) handler.post(this::showWaiting);
        else if (!DoctorConnector.getInstance().isSsvepActive()) handler.post(this::showWaiting);
    }

    @Override
    public void onDestroyView() {
        TrainingModeCoordinator.getInstance().unregisterController(
                TrainingModeCoordinator.Mode.SSVEP, this);
        DoctorConnector.getInstance().removeListener(this);
        TrainingModeCoordinator.getInstance().release(TrainingModeCoordinator.Mode.SSVEP);
        if (stimulusView != null) stimulusView.stopStimulus();
        isBlinking = false;
        currentFreqIndex = -1;
        tvStatus = null;
        tvResult = null;
        tvDetails = null;
        stimulusView = null;
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
    @Override public void onFocusData(float attn0, float attn1, float ema0, float ema1, int trend, int instant) {}
}
