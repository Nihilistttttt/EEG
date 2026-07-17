package com.nihilisttt.eegdoctor;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ProgressBar;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;

import com.google.android.material.button.MaterialButton;

import java.util.Locale;

/** Mode 1: the doctor selects one target frequency and the patient displays one full-screen stimulus. */
public class SsvepTrainingFragment extends Fragment implements DataListener, TrainingModeCoordinator.ModeController {

    private static final float[] TARGET_FREQS = {11.0f, 13.0f, 15.0f, 17.0f};
    private static final String[] FREQ_LABELS = {"11 Hz", "13 Hz", "15 Hz", "17 Hz"};
    private static final String[] DIRECTIONS = {"↑ 上", "↓ 下", "← 左", "→ 右"};

    private SpectrumView spectrumO1;
    private SpectrumView spectrumOZ;
    private TextView tvStatus;
    private TextView tvSelectedFreq;
    private TextView tvResult;
    private TextView tvRawResult;
    private TextView tvScores;
    private TextView tvRatio;
    private TextView tvMargin;
    private TextView tvVote;
    private TextView tvProgress;
    private TextView tvWaveSource;
    private ProgressBar progressWindow;
    private MaterialButton btnFreq11;
    private MaterialButton btnFreq13;
    private MaterialButton btnFreq15;
    private MaterialButton btnFreq17;
    private MaterialButton btnStart;
    private MaterialButton btnSelfTest;
    private MaterialButton btnStop;

    private int selectedFreqIndex = -1;
    private boolean ssvepRunning;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_ssvep_training, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        spectrumO1 = view.findViewById(R.id.spectrum_o1);
        spectrumOZ = view.findViewById(R.id.spectrum_oz);
        tvStatus = view.findViewById(R.id.tv_ssvep_status);
        tvSelectedFreq = view.findViewById(R.id.tv_selected_freq);
        tvResult = view.findViewById(R.id.tv_ssvep_result);
        tvRawResult = view.findViewById(R.id.tv_ssvep_raw_result);
        tvScores = view.findViewById(R.id.tv_ssvep_scores);
        tvRatio = view.findViewById(R.id.tv_ssvep_ratio);
        tvMargin = view.findViewById(R.id.tv_ssvep_margin);
        tvVote = view.findViewById(R.id.tv_ssvep_vote);
        tvProgress = view.findViewById(R.id.tv_ssvep_progress);
        tvWaveSource = view.findViewById(R.id.tv_ssvep_wave_source);
        progressWindow = view.findViewById(R.id.progress_ssvep_window);
        btnFreq11 = view.findViewById(R.id.btn_freq_11);
        btnFreq13 = view.findViewById(R.id.btn_freq_13);
        btnFreq15 = view.findViewById(R.id.btn_freq_15);
        btnFreq17 = view.findViewById(R.id.btn_freq_17);
        btnStart = view.findViewById(R.id.btn_ssvep_start);
        btnSelfTest = view.findViewById(R.id.btn_ssvep_self_test);
        btnStop = view.findViewById(R.id.btn_ssvep_stop);

        spectrumO1.setFreqRange(0f, 45f);
        spectrumO1.setYMax(0.005f);
        spectrumOZ.setFreqRange(0f, 45f);
        spectrumOZ.setYMax(0.005f);

        btnFreq11.setOnClickListener(v -> selectFreq(0));
        btnFreq13.setOnClickListener(v -> selectFreq(1));
        btnFreq15.setOnClickListener(v -> selectFreq(2));
        btnFreq17.setOnClickListener(v -> selectFreq(3));
        btnStart.setOnClickListener(v -> startModeOne(false));
        btnSelfTest.setOnClickListener(v -> startModeOne(true));
        btnStop.setOnClickListener(v -> stopSsvep());

        updateFreqButtons();
        resetDisplay();
        DataDispatcher.getInstance().addListener(this);
        TrainingModeCoordinator.getInstance().registerController(
                TrainingModeCoordinator.Mode.SSVEP, this);
    }

    private void selectFreq(int index) {
        if (ssvepRunning) return;
        selectedFreqIndex = index;
        tvSelectedFreq.setText("当前目标: " + DIRECTIONS[index] + " / " + FREQ_LABELS[index]);
        updateFreqButtons();
    }

    private void updateFreqButtons() {
        MaterialButton[] buttons = {btnFreq11, btnFreq13, btnFreq15, btnFreq17};
        int selectedColor = ContextCompat.getColor(requireContext(), R.color.brand_primary);
        int normalColor = ContextCompat.getColor(requireContext(), R.color.text_secondary);
        for (int i = 0; i < buttons.length; i++) {
            if (i == selectedFreqIndex) {
                buttons[i].setBackgroundColor(selectedColor);
                buttons[i].setTextColor(0xFFFFFFFF);
            } else {
                buttons[i].setBackgroundColor(0x00000000);
                buttons[i].setTextColor(normalColor);
            }
        }
    }

    private void startModeOne(boolean selfTest) {
        if (ssvepRunning) return;
        TrainingModeCoordinator.getInstance().activate(TrainingModeCoordinator.Mode.SSVEP);
        if (selectedFreqIndex < 0) {
            setStatus("请先选择一个目标频率", true);
            return;
        }
        TcpServerManager server = TcpServerManager.getInstance();
        if (!server.isPatientControlConnected()) {
            setStatus("患者控制通道未连接", true);
            return;
        }

        final int freqIndex = selectedFreqIndex;
        SsvepAnalysisManager analysis = SsvepAnalysisManager.getInstance();
        analysis.prepareSession(freqIndex);
        server.setSsvepActive(true, freqIndex);
        ssvepRunning = true;
        setRunningUi(true);
        resetDisplay();
        setStatus(selfTest
                ? "正在启动患者刺激与无设备算法自测"
                : "正在启动患者刺激，等待首帧回执", false);

        boolean queued = server.sendToPatientAsync("SSVEP,START," + freqIndex, success -> {
            if (!isAdded()) return;
            if (!success) {
                analysis.stopSession();
                server.setSsvepActive(false, -1);
                ssvepRunning = false;
                setRunningUi(false);
                setStatus("患者刺激指令发送失败", true);
                return;
            }

            if (!ssvepRunning) return;
            // 41002 未连接时该命令只产生正常的未连接日志，不影响患者端和算法自测。
            CommandSender.getInstance().ssvepStart();
            if (selfTest) {
                analysis.startSyntheticTest(freqIndex);
                setStatus("模式一算法自测运行中（无需脑电设备）", false);
            } else {
                setStatus("患者刺激已下发；等待刺激首帧和脑电数据", false);
            }
        });

        if (!queued) {
            analysis.stopSession();
            server.setSsvepActive(false, -1);
            ssvepRunning = false;
            setRunningUi(false);
            setStatus("无法加入患者控制发送队列", true);
        }
    }

    private void stopSsvep() {
        TcpServerManager server = TcpServerManager.getInstance();
        boolean active = ssvepRunning
                || SsvepAnalysisManager.getInstance().isRunning()
                || server.isSsvepActive();
        if (!active) return;
        ssvepRunning = false;
        SsvepAnalysisManager.getInstance().stopSession();
        CommandSender.getInstance().ssvepStop();
        server.setSsvepActive(false, -1);
        server.sendToPatient("SSVEP,STOP");
        setRunningUi(false);
        setStatus("已停止", false);
        progressWindow.setProgress(0);
        TrainingModeCoordinator.getInstance().release(TrainingModeCoordinator.Mode.SSVEP);
    }

    @Override
    public void stopForModeSwitch(TrainingModeCoordinator.Mode nextMode) {
        stopSsvep();
    }

    private void setRunningUi(boolean running) {
        btnStart.setEnabled(!running);
        btnSelfTest.setEnabled(!running);
        btnStop.setEnabled(running);
        btnFreq11.setEnabled(!running);
        btnFreq13.setEnabled(!running);
        btnFreq15.setEnabled(!running);
        btnFreq17.setEnabled(!running);
    }

    private void setStatus(String message, boolean error) {
        tvStatus.setText("状态: " + message);
        tvStatus.setTextColor(error ? 0xFFFF5252 : 0xFF00A86B);
    }

    private void resetDisplay() {
        tvResult.setText("--");
        tvRawResult.setText("原始判决: --");
        tvScores.setText("11: --   13: --   15: --   17: --");
        tvRatio.setText("--");
        tvMargin.setText("--");
        tvVote.setText("投票: --");
        tvProgress.setText("窗口: 0 / " + FbccaConfig.WINDOW_SIZE);
        tvWaveSource.setText(String.format(Locale.US,
                "波形源: 优先 0x%02X，1秒后回退 0x%02X；输入比例 ×%.0f mV",
                FbccaConfig.PREFERRED_WAVE_CMD,
                FbccaConfig.FALLBACK_WAVE_CMD,
                FbccaConfig.INPUT_SCALE_TO_MV));
        progressWindow.setProgress(0);
    }

    @Override
    public void onSsvepProgress(SsvepProgress progress) {
        if (progress == null || getView() == null) return;
        progressWindow.setProgress(progress.getPercent());
        tvProgress.setText(String.format(Locale.US,
                "窗口: %d / %d（%d%%） %s",
                progress.getBufferedSamples(),
                progress.getWindowSamples(),
                progress.getPercent(),
                progress.getMessage()));
        if (progress.getWaveCommand() > 0) {
            tvWaveSource.setText(String.format(Locale.US,
                    "当前波形源: CMD=0x%02X，通道0，输入换算×%.0f mV",
                    progress.getWaveCommand(), FbccaConfig.INPUT_SCALE_TO_MV));
        } else if (progress.isSynthetic()) {
            tvWaveSource.setText("当前波形源: 合成 SSVEP 测试信号");
        }
    }

    @Override
    public void onSsvepResult(SsvepResult result) {
        if (result == null || getView() == null) return;
        int finalIndex = result.getFreqIndex();
        int rawIndex = result.getRawFreqIndex();
        if (!result.isChannelUsable()) {
            tvResult.setText("通道质量不合格");
            tvResult.setTextColor(0xFFD32F2F);
        } else if (result.isUncertain()) {
            tvResult.setText("UNCERTAIN");
            tvResult.setTextColor(0xFFFF9800);
        } else {
            tvResult.setText(DIRECTIONS[finalIndex] + " / " + result.getFreq() + " Hz");
            tvResult.setTextColor(0xFF00C853);
        }
        tvRawResult.setText("原始判决: "
                + (rawIndex >= 0 ? DIRECTIONS[rawIndex] + " / " + result.getRawFreq() + " Hz" : "UNCERTAIN")
                + (result.isSynthetic() ? "（自测）" : ""));
        tvScores.setText(String.format(Locale.US,
                "11: %.4f   13: %.4f\n15: %.4f   17: %.4f",
                result.getScore11(), result.getScore13(),
                result.getScore15(), result.getScore17()));
        tvRatio.setText(String.format(Locale.US, "%.3f", result.getRatio()));
        tvMargin.setText(String.format(Locale.US, "%.5f", result.getMargin()));
        tvVote.setText(String.format(Locale.US,
                "投票 11:%d  13:%d  15:%d  17:%d",
                result.getVote11(), result.getVote13(), result.getVote15(), result.getVote17()));
    }

    @Override
    public void onSpectrumData(int cmd, float[] mags) {
        int ch = -1;
        if (cmd >= 0x20 && cmd <= 0x27) ch = cmd - 0x20;
        else if (cmd >= 0x30 && cmd <= 0x37) ch = cmd - 0x30;
        else if (cmd >= 0x40 && cmd <= 0x47) ch = cmd - 0x40;
        if (ch == 1 && spectrumO1 != null) spectrumO1.updateSpectrum(mags);
        if (ch == 0 && spectrumOZ != null) spectrumOZ.updateSpectrum(mags);
    }

    @Override
    public void onDestroyView() {
        TrainingModeCoordinator.getInstance().unregisterController(
                TrainingModeCoordinator.Mode.SSVEP, this);
        DataDispatcher.getInstance().removeListener(this);
        stopSsvep();
        super.onDestroyView();
    }

    @Override public void onWaveData(int cmd, int ch, float val) {}
    @Override public void onFocusData(float attn0, float attn1, float ema0, float ema1, int trend, int instant) {}
}
