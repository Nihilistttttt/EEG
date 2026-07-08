package com.nihilisttt.eegdoctor;

import com.nihilisttt.eegdoctor.R;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;


public class InferenceFragment extends Fragment implements DataListener {

    private TextView tvDirection;

    private View btnStartInfer;
    private View btnStopInfer;

    private boolean isInferencing = false;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_inference, container, false);

        tvDirection = root.findViewById(R.id.tv_direction);

        btnStartInfer = root.findViewById(R.id.btn_start_infer);
        btnStopInfer = root.findViewById(R.id.btn_stop_infer);

        btnStartInfer.setOnClickListener(v -> startInference());
        btnStopInfer.setOnClickListener(v -> stopInference());

        return root;
    }

    private void startInference() {
        Log.i("Inference", "btnStartInfer clicked, isInferencing=" + isInferencing);
        if (isInferencing) return;
        isInferencing = true;
        CommandSender.getInstance().setModeInfer();
        CommandSender.getInstance().startTest();
        btnStartInfer.setEnabled(false);
        btnStopInfer.setEnabled(true);
        tvDirection.setText("← →");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.text_secondary));
    }

    private void stopInference() {
        Log.i("Inference", "btnStopInfer clicked");
        isInferencing = false;
        CommandSender.getInstance().sendCommand("STOP");
        btnStartInfer.setEnabled(true);
        btnStopInfer.setEnabled(false);
        tvDirection.setText("← →");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.text_secondary));
    }

    @Override
    public void onResume() {
        super.onResume();
        DataDispatcher.getInstance().removeListener(this);
        DataDispatcher.getInstance().addListener(this);
        Log.d("Inference", "onResume: listener refreshed");
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        DataDispatcher.getInstance().removeListener(this);
        Log.d("Inference", "onDestroyView: listener removed");
    }

    @Override
    public void onInferenceResult(InferenceResult result) {
        if (tvDirection == null) return;

        String intent = result.getIntent();
        if ("LEFT".equals(intent)) {
            tvDirection.setText("←");
            tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.direction_left));
        } else if ("RIGHT".equals(intent)) {
            tvDirection.setText("→");
            tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.direction_right));
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
    @Override
    public void onTurnEvent(String from, String to) {}
    @Override
    public void onFallEvent() {}
    @Override
    public void onNoTurnAlert(long durationMin) {}
    @Override
    public void onPostureState(String posture, int turnCount) {}
}
