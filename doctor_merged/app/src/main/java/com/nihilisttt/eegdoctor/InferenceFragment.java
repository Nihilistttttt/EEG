package com.nihilisttt.eegdoctor;

import android.app.AlertDialog;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
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
import java.util.Random;

public class InferenceFragment extends Fragment implements DataListener {

    private static final int DEFAULT_CYCLE_ROUNDS = 3;
    private static final int RESULT_TO_NEXT_TARGET_MS = 2000;

    private TextView tvDirection;
    private TextView tvTargetDir;
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
    private View btnExport;
    private View btnClearRecords;

    private Spinner spinnerMode;
    private Spinner spinnerRounds;

    private RecyclerView rvRecords;
    private InferenceRecordAdapter adapter;

    private boolean isInferencing = false;

    private String currentTarget = null;
    private boolean targetIsLeft = true;
    private int cycleCount = 0;
    private final Random random = new Random();
    private final Handler targetHandler = new Handler(Looper.getMainLooper());
    private Runnable nextTargetRunnable = null;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_inference, container, false);

        tvDirection = root.findViewById(R.id.tv_direction);
        tvTargetDir = root.findViewById(R.id.tv_target_dir);
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
        btnExport = root.findViewById(R.id.btn_export);
        btnClearRecords = root.findViewById(R.id.btn_clear_records);

        spinnerMode = root.findViewById(R.id.spinner_target_mode);
        spinnerRounds = root.findViewById(R.id.spinner_target_sec);

        ArrayAdapter<String> modeAdapter = new ArrayAdapter<>(requireContext(),
                android.R.layout.simple_spinner_item, new String[]{"循环", "随机"});
        modeAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinnerMode.setAdapter(modeAdapter);

        String[] roundOptions = new String[10];
        for (int i = 0; i < 10; i++) roundOptions[i] = (i + 1) + " 次";
        ArrayAdapter<String> roundAdapter = new ArrayAdapter<>(requireContext(),
                android.R.layout.simple_spinner_item, roundOptions);
        roundAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinnerRounds.setAdapter(roundAdapter);
        spinnerRounds.setSelection(DEFAULT_CYCLE_ROUNDS - 1);
        spinnerRounds.setEnabled(false);

        spinnerMode.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                spinnerRounds.setEnabled(position == 0);
            }
            @Override
            public void onNothingSelected(AdapterView<?> parent) {}
        });

        rvRecords = root.findViewById(R.id.rv_records);
        adapter = new InferenceRecordAdapter();
        rvRecords.setLayoutManager(new LinearLayoutManager(requireContext()));
        rvRecords.setAdapter(adapter);

        btnStartInfer.setOnClickListener(v -> startInference());
        btnStopInfer.setOnClickListener(v -> stopInference());

        btnExport.setOnClickListener(v -> exportRecords());
        btnClearRecords.setOnClickListener(v -> confirmClearRecords());

        refreshStats();
        return root;
    }

    private boolean isCycleMode() {
        return spinnerMode != null && spinnerMode.getSelectedItemPosition() == 0;
    }

    private int getCycleRounds() {
        if (spinnerRounds == null) return DEFAULT_CYCLE_ROUNDS;
        return spinnerRounds.getSelectedItemPosition() + 1;
    }

    private String generateNextTarget() {
        if (isCycleMode()) {
            cycleCount++;
            if (cycleCount > getCycleRounds()) {
                cycleCount = 1;
                targetIsLeft = !targetIsLeft;
            }
            return targetIsLeft ? "LEFT" : "RIGHT";
        } else {
            return random.nextBoolean() ? "LEFT" : "RIGHT";
        }
    }

    private void sendTarget(String direction) {
        currentTarget = direction;
        boolean isLeft = "LEFT".equals(direction);
        tvTargetDir.setText(isLeft ? "◀ 左" : "右 ▶");
        tvTargetDir.setTextColor(ContextCompat.getColor(requireContext(),
                isLeft ? R.color.direction_left : R.color.direction_right));
        TcpServerManager.getInstance().sendToPatient("TARGET," + direction);
    }

    private void startInference() {
        if (isInferencing) return;
        isInferencing = true;
        currentTarget = null;
        targetIsLeft = true;
        cycleCount = 0;
        CommandSender.getInstance().setModeInfer();
        CommandSender.getInstance().startTest();
        btnStartInfer.setEnabled(false);
        btnStopInfer.setEnabled(true);
        tvDirection.setText("← →");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.text_secondary));
        tvTargetDir.setText("--");
        tvConfidence.setText("置信度: --");
        tvScores.setText("L:-- R:--");

        String target = generateNextTarget();
        sendTarget(target);
    }

    private void stopInference() {
        isInferencing = false;
        currentTarget = null;
        if (nextTargetRunnable != null) {
            targetHandler.removeCallbacks(nextTargetRunnable);
            nextTargetRunnable = null;
        }
        CommandSender.getInstance().sendCommand("STOP");
        btnStartInfer.setEnabled(true);
        btnStopInfer.setEnabled(false);
        tvDirection.setText("← →");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.text_secondary));
        tvTargetDir.setText("--");
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
            try (FileOutputStream fos = new FileOutputStream(file)) {
                fos.write(csv.getBytes("UTF-8"));
            }

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
        TcpServerManager.getInstance().sendToDevice(EegChannels.buildAllNoneDisplayConfig());
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
        if (nextTargetRunnable != null) {
            targetHandler.removeCallbacks(nextTargetRunnable);
            nextTargetRunnable = null;
        }
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
        if (currentTarget != null) {
            rec.groundTruth = currentTarget;
            rec.correct = currentTarget.equals(result.getIntent());
        }
        InferenceRecordStore.addRecord(requireContext(), rec);
        refreshStats();

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

        if (isInferencing) {
            if (nextTargetRunnable != null) targetHandler.removeCallbacks(nextTargetRunnable);
            nextTargetRunnable = () -> {
                if (!isInferencing) return;
                String target = generateNextTarget();
                sendTarget(target);
            };
            targetHandler.postDelayed(nextTargetRunnable, RESULT_TO_NEXT_TARGET_MS);
        }
    }

    @Override
    public void onWaveData(int cmd, int ch, float val) {}
    @Override
    public void onSpectrumData(int cmd, float[] mags) {}
    @Override
    public void onFocusData(float attn0, float attn1, float ema0, float ema1, int trend, int instant) {}
}
