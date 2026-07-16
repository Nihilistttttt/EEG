package com.nihilisttt.eegdoctor;

import com.nihilisttt.eegdoctor.R;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

public class SsvepTrainingFragment extends Fragment implements DataListener {

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        TextView tv = new TextView(requireContext());
        tv.setText("SSVEP训练功能开发中\n\n目标频率: 11/13/15/17 Hz\n检测方法: FBCCA\n闪烁刺激将由患者端提供");
        tv.setTextSize(18);
        tv.setTextColor(getResources().getColor(R.color.text_secondary, null));
        tv.setPadding(48, 48, 48, 48);
        return tv;
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
        super.onDestroyView();
        DataDispatcher.getInstance().removeListener(this);
    }

    @Override public void onWaveData(int cmd, int ch, float val) {}
    @Override public void onSpectrumData(int cmd, float[] mags) {}
    @Override public void onFocusData(float attn0, float attn1, float ema0, float ema1, int trend, int instant) {}
    @Override public void onEegFrame(EegFrame frame) {}
    @Override public void onIpcDiag(IpcDiagInfo diag) {}
    @Override public void onDirConfig(String configJson) {}
    @Override public void onTaskStart(String side) {}
    @Override public void onTaskDone() {}
    @Override public void onReadyTrain() {}
    @Override public void onReadyTest() {}
    @Override public void onModeSetOk(int mode) {}
    @Override public void onInferenceResult(InferenceResult result) {}
    @Override public void onTurnEvent(String from, String to) {}
    @Override public void onFallEvent() {}
    @Override public void onNoTurnAlert(long durationMin) {}
    @Override public void onPostureState(String posture, int turnCount) {}
}
