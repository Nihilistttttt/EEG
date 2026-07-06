package com.nihilisttt.eegdoctor;

import com.nihilisttt.eegdoctor.R;

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

import java.util.Locale;

public class InferenceFragment extends Fragment implements DataListener {

    private TextView tvDirection;
    private TextView tvConfidence;
    private ProgressBar progressConfidence;
    private TextView tvModelStatus;
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
        tvConfidence = root.findViewById(R.id.tv_confidence);
        progressConfidence = root.findViewById(R.id.progress_confidence);
        tvModelStatus = root.findViewById(R.id.tv_model_status);
        btnStartInfer = root.findViewById(R.id.btn_start_infer);
        btnStopInfer = root.findViewById(R.id.btn_stop_infer);

        btnStartInfer.setOnClickListener(v -> startInference());
        btnStopInfer.setOnClickListener(v -> stopInference());

        return root;
    }

    private void startInference() {
        if (isInferencing) return;
        isInferencing = true;
        CommandSender.getInstance().setModeInfer();
        btnStartInfer.setEnabled(false);
        btnStopInfer.setEnabled(true);
        tvDirection.setText("← →");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.text_secondary));
        tvConfidence.setText("置信度: --");
        tvConfidence.setTextColor(ContextCompat.getColor(requireContext(), R.color.text_secondary));
        progressConfidence.setProgress(0);
    }

    private void stopInference() {
        isInferencing = false;
        CommandSender.getInstance().setModeCollect();
        btnStartInfer.setEnabled(true);
        btnStopInfer.setEnabled(false);
        tvDirection.setText("← →");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.text_secondary));
        tvConfidence.setText("置信度: --");
        tvConfidence.setTextColor(ContextCompat.getColor(requireContext(), R.color.text_secondary));
        progressConfidence.setProgress(0);
    }

    @Override
    public void onResume() {
        super.onResume();
        DataDispatcher.getInstance().addListener(this);
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        DataDispatcher.getInstance().removeListener(this);
    }

    @Override
    public void onInferenceResult(InferenceResult result) {
        if (!isInferencing) return;

        String intent = result.getIntent();
        if ("LEFT".equals(intent)) {
            tvDirection.setText("←");
            tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.direction_left));
        } else if ("RIGHT".equals(intent)) {
            tvDirection.setText("→");
            tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.direction_right));
        }

        float confPct = result.getConfidence() * 100f;
        tvConfidence.setText(String.format(Locale.getDefault(), "置信度: %.0f%%", confPct));
        progressConfidence.setProgress((int) confPct);

        int confColor;
        if (confPct >= 80) confColor = ContextCompat.getColor(requireContext(), R.color.confidence_high);
        else if (confPct >= 50) confColor = ContextCompat.getColor(requireContext(), R.color.confidence_medium);
        else confColor = ContextCompat.getColor(requireContext(), R.color.confidence_low);
        tvConfidence.setTextColor(confColor);

        if (result.isTrained()) {
            tvModelStatus.setText("模型已就绪");
            tvModelStatus.setTextColor(ContextCompat.getColor(requireContext(), R.color.accent_success));
        } else {
            tvModelStatus.setText("请先完成训练");
            tvModelStatus.setTextColor(ContextCompat.getColor(requireContext(), R.color.accent_warning));
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
