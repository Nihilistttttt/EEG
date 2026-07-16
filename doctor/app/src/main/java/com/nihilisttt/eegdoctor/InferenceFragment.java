package com.nihilisttt.eegdoctor;

import android.app.AlertDialog;
import android.content.Intent;
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
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import java.io.File;
import java.io.FileOutputStream;
import java.util.List;
import java.util.Locale;

public class InferenceFragment extends Fragment implements DataListener {

    private TextView tvDirection;
    private TextView tvConfidence;
    private TextView tvScores;
    private TextView tvStatTotal;
    private TextView tvStatVerified;
    private TextView tvStatAccuracy;
    private TextView tvStatAvgConf;
    private TextView tvStatLeft;
    private TextView tvStatRight;

    private View btnStartInfer;
    private View btnStopInfer;
    private View btnVerifyLeft;
    private View btnVerifyRight;
    private View btnVerifySkip;
    private View btnExport;
    private View btnClearRecords;

    private RecyclerView rvRecords;
    private InferenceRecordAdapter adapter;

    private boolean isInferencing = false;
    private InferenceRecordStore.Record lastResult = null;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_inference, container, false);

        tvDirection = root.findViewById(R.id.tv_direction);
        tvConfidence = root.findViewById(R.id.tv_confidence);
        tvScores = root.findViewById(R.id.tv_scores);
        tvStatTotal = root.findViewById(R.id.tv_stat_total);
        tvStatVerified = root.findViewById(R.id.tv_stat_verified);
        tvStatAccuracy = root.findViewById(R.id.tv_stat_accuracy);
        tvStatAvgConf = root.findViewById(R.id.tv_stat_avg_conf);
        tvStatLeft = root.findViewById(R.id.tv_stat_left);
        tvStatRight = root.findViewById(R.id.tv_stat_right);

        btnStartInfer = root.findViewById(R.id.btn_start_infer);
        btnStopInfer = root.findViewById(R.id.btn_stop_infer);
        btnVerifyLeft = root.findViewById(R.id.btn_verify_left);
        btnVerifyRight = root.findViewById(R.id.btn_verify_right);
        btnVerifySkip = root.findViewById(R.id.btn_verify_skip);
        btnExport = root.findViewById(R.id.btn_export);
        btnClearRecords = root.findViewById(R.id.btn_clear_records);

        rvRecords = root.findViewById(R.id.rv_records);
        adapter = new InferenceRecordAdapter();
        rvRecords.setLayoutManager(new LinearLayoutManager(requireContext()));
        rvRecords.setAdapter(adapter);

        btnStartInfer.setOnClickListener(v -> startInference());
        btnStopInfer.setOnClickListener(v -> stopInference());

        btnVerifyLeft.setOnClickListener(v -> verifyResult("LEFT"));
        btnVerifyRight.setOnClickListener(v -> verifyResult("RIGHT"));
        btnVerifySkip.setOnClickListener(v -> skipVerification());

        btnExport.setOnClickListener(v -> exportRecords());
        btnClearRecords.setOnClickListener(v -> confirmClearRecords());

        refreshStats();
        return root;
    }

    private void startInference() {
        if (isInferencing) return;
        isInferencing = true;
        lastResult = null;
        CommandSender.getInstance().setModeInfer();
        CommandSender.getInstance().startTest();
        btnStartInfer.setEnabled(false);
        btnStopInfer.setEnabled(true);
        tvDirection.setText("← →");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.text_secondary));
        tvConfidence.setText("置信度: --");
        tvScores.setText("L:-- R:--");
        updateVerifyButtons();
    }

    private void stopInference() {
        isInferencing = false;
        lastResult = null;
        CommandSender.getInstance().sendCommand("STOP");
        btnStartInfer.setEnabled(true);
        btnStopInfer.setEnabled(false);
        tvDirection.setText("← →");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.text_secondary));
        updateVerifyButtons();
    }

    private void verifyResult(String groundTruth) {
        if (lastResult == null) return;
        lastResult.groundTruth = groundTruth;
        lastResult.correct = lastResult.intent.equals(groundTruth);
        InferenceRecordStore.addRecord(requireContext(), lastResult);
        lastResult = null;
        refreshStats();
        updateVerifyButtons();
    }

    private void skipVerification() {
        if (lastResult == null) return;
        InferenceRecordStore.addRecord(requireContext(), lastResult);
        lastResult = null;
        refreshStats();
        updateVerifyButtons();
    }

    private void updateVerifyButtons() {
        boolean hasResult = lastResult != null;
        btnVerifyLeft.setEnabled(hasResult);
        btnVerifyRight.setEnabled(hasResult);
        btnVerifySkip.setEnabled(hasResult);
    }

    private void refreshStats() {
        List<InferenceRecordStore.Record> records = InferenceRecordStore.loadAll(requireContext());
        tvStatTotal.setText("总推理: " + InferenceRecordStore.countTotal(records));
        tvStatVerified.setText("已验证: " + InferenceRecordStore.countWithGroundTruth(records));
        float acc = InferenceRecordStore.calcAccuracy(records);
        tvStatAccuracy.setText(String.format(Locale.getDefault(), "达标率: %.1f%%", acc));
        tvStatAvgConf.setText(String.format(Locale.getDefault(), "平均置信: %.1f%%", InferenceRecordStore.avgConfidence(records)));
        tvStatLeft.setText("←左: " + InferenceRecordStore.countLeft(records));
        tvStatRight.setText("右→: " + InferenceRecordStore.countRight(records));
        adapter.setRecords(records);
        if (rvRecords.getAdapter() != null) rvRecords.scrollToPosition(0);
    }

    private void exportRecords() {
        List<InferenceRecordStore.Record> records = InferenceRecordStore.loadAll(requireContext());
        if (records.isEmpty()) {
            new AlertDialog.Builder(requireContext())
                    .setTitle("导出")
                    .setMessage("暂无记录")
                    .setPositiveButton("确定", null)
                    .show();
            return;
        }
        String csv = InferenceRecordStore.exportCsv(records);
        try {
            File dir = new File(requireContext().getExternalFilesDir(null), "inference");
            if (!dir.exists()) dir.mkdirs();
            String ts = new java.text.SimpleDateFormat("yyyyMMdd_HHmmss", Locale.getDefault())
                    .format(new java.util.Date());
            File file = new File(dir, "inference_" + ts + ".csv");
            FileOutputStream fos = new FileOutputStream(file);
            fos.write(csv.getBytes("UTF-8"));
            fos.close();

            Intent share = new Intent(Intent.ACTION_SEND);
            share.setType("text/csv");
            share.putExtra(Intent.EXTRA_STREAM, androidx.core.content.FileProvider.getUriForFile(
                    requireContext(), requireContext().getPackageName() + ".fileprovider", file));
            share.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
            startActivity(Intent.createChooser(share, "导出推理记录"));

            new AlertDialog.Builder(requireContext())
                    .setTitle("已导出")
                    .setMessage("保存到: " + file.getName() + "\n共 " + records.size() + " 条记录")
                    .setPositiveButton("确定", null)
                    .show();
        } catch (Exception e) {
            Log.e("Inference", "export failed", e);
            new AlertDialog.Builder(requireContext())
                    .setTitle("导出失败")
                    .setMessage(e.getMessage())
                    .setPositiveButton("确定", null)
                    .show();
        }
    }

    private void confirmClearRecords() {
        new AlertDialog.Builder(requireContext())
                .setTitle("清除记录")
                .setMessage("确定清除所有推理记录？此操作不可恢复。")
                .setPositiveButton("清除", (d, w) -> {
                    InferenceRecordStore.clearAll(requireContext());
                    refreshStats();
                })
                .setNegativeButton("取消", null)
                .show();
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
        DataDispatcher.getInstance().removeListener(this);
    }

    @Override
    public void onInferenceResult(InferenceResult result) {
        if (tvDirection == null) return;

        InferenceRecordStore.Record rec = new InferenceRecordStore.Record();
        rec.timestamp = System.currentTimeMillis();
        rec.intent = result.getIntent();
        rec.scoreLeft = result.getScoreLeft();
        rec.scoreRight = result.getScoreRight();
        rec.confidence = result.getConfidence();
        lastResult = rec;

        String intent = result.getIntent();
        if ("LEFT".equals(intent)) {
            tvDirection.setText("←");
            tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.direction_left));
        } else if ("RIGHT".equals(intent)) {
            tvDirection.setText("→");
            tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.direction_right));
        }
        tvConfidence.setText(String.format(Locale.getDefault(), "置信度: %.1f%%", result.getConfidence() * 100));
        tvScores.setText(String.format(Locale.getDefault(), "L:%.3f R:%.3f", result.getScoreLeft(), result.getScoreRight()));
        updateVerifyButtons();
    }

    @Override
    public void onWaveData(int cmd, int ch, float val) {}
    @Override
    public void onSpectrumData(int cmd, float[] mags) {}
    @Override
    public void onFocusData(float attn0, float attn1, float ema0, float ema1, int trend, int instant) {}
}
