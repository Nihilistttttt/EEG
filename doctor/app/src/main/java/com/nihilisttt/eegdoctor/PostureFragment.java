package com.nihilisttt.eegdoctor;

import com.nihilisttt.eegdoctor.R;

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
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Locale;

public class PostureFragment extends Fragment implements DataListener {

    private TextView tvPostureState;
    private TextView tvEventLog;
    private TextView tvFallAlert;
    private TextView tvTurnAlert;
    private final List<String> eventLog = new ArrayList<>();
    private final Handler handler = new Handler(Looper.getMainLooper());
    private static final int MAX_LOG_LINES = 50;

    private String currentPosture = "UNKNOWN";
    private int currentTurnCount = 0;
    private boolean viewReady = false;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_posture, container, false);

        tvPostureState = root.findViewById(R.id.tv_posture_state);
        tvTurnAlert = root.findViewById(R.id.tv_turn_alert);
        tvFallAlert = root.findViewById(R.id.tv_fall_alert);
        tvEventLog = root.findViewById(R.id.tv_posture_detail);

        tvPostureState.setText("等待姿态数据...");
        tvPostureState.setTextColor(ContextCompat.getColor(requireContext(), R.color.text_secondary));
        tvFallAlert.setText("");
        tvTurnAlert.setText("");
        tvEventLog.setText("等待姿态事件...");

        root.findViewById(R.id.btn_posture_on).setVisibility(View.GONE);
        root.findViewById(R.id.btn_posture_off).setVisibility(View.GONE);

        viewReady = true;
        return root;
    }

    @Override
    public void onResume() {
        super.onResume();
        DataDispatcher.getInstance().removeListener(this);
        DataDispatcher.getInstance().addListener(this);
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        viewReady = false;
        DataDispatcher.getInstance().removeListener(this);
        handler.removeCallbacksAndMessages(null);
    }

    private static String postureToChinese(String posture) {
        switch (posture) {
            case "SUPINE": return "仰卧";
            case "PRONE": return "俯卧";
            case "LEFT": return "左侧卧";
            case "RIGHT": return "右侧卧";
            case "SITTING": return "坐姿";
            default: return posture;
        }
    }

    private void updatePostureDisplay() {
        if (!viewReady || tvPostureState == null) return;
        tvPostureState.setText(postureToChinese(currentPosture) + "  |  翻身: " + currentTurnCount);
        tvPostureState.setTextColor(ContextCompat.getColor(requireContext(), R.color.accent_success));
    }

    private void addEvent(String event) {
        if (!viewReady || tvEventLog == null) return;
        String time = new SimpleDateFormat("HH:mm:ss", Locale.getDefault()).format(new Date());
        String line = "[" + time + "] " + event;
        eventLog.add(0, line);
        while (eventLog.size() > MAX_LOG_LINES) eventLog.remove(eventLog.size() - 1);
        StringBuilder sb = new StringBuilder();
        for (String s : eventLog) {
            sb.append(s).append("\n");
        }
        tvEventLog.setText(sb.toString().trim());
    }

    @Override
    public void onPostureState(String posture, int turnCount) {
        handler.post(() -> {
            Log.d("Posture", "onPostureState: " + posture + " turns=" + turnCount);
            currentPosture = posture;
            currentTurnCount = turnCount;
            updatePostureDisplay();
        });
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
    @Override
    public void onTurnEvent(String from, String to) {
        handler.post(() -> {
            if (!viewReady || tvTurnAlert == null) return;
            tvTurnAlert.setText("翻身: " + postureToChinese(from) + " → " + postureToChinese(to));
            tvTurnAlert.setTextColor(ContextCompat.getColor(requireContext(), R.color.accent_info));
            addEvent("翻身: " + postureToChinese(from) + " → " + postureToChinese(to));
            handler.postDelayed(() -> { if (viewReady && tvTurnAlert != null) tvTurnAlert.setText(""); }, 5000);
        });
    }

    @Override
    public void onFallEvent() {
        handler.post(() -> {
            if (!viewReady || tvFallAlert == null) return;
            tvFallAlert.setText("坠床警报!");
            tvFallAlert.setTextColor(ContextCompat.getColor(requireContext(), R.color.accent_error));
            addEvent("坠床警报!");
        });
    }

    @Override
    public void onNoTurnAlert(long durationMin) {
        handler.post(() -> {
            if (!viewReady || tvTurnAlert == null) return;
            tvTurnAlert.setText("久未翻身: " + durationMin + "分钟");
            tvTurnAlert.setTextColor(ContextCompat.getColor(requireContext(), R.color.accent_warning));
            addEvent("久未翻身: " + durationMin + "分钟");
        });
    }
    @Override
    public void onTaskStart(String side) {}
    @Override
    public void onTaskDone() {}
    @Override
    public void onReadyTrain() {}
    @Override
    public void onReadyTest() {}
    @Override
    public void onModeSetOk(int mode) {}
}
