package com.nihilisttt.eegdoctor;

import com.nihilisttt.eegdoctor.R;

import android.app.AlertDialog;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.Spinner;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.google.android.material.card.MaterialCardView;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class MiTrainingFragment extends Fragment implements DataListener, TrainingModeCoordinator.ModeController {

    private static final int STATE_IDLE = 0;
    private static final int STATE_WAIT_READY = 1;
    private static final int STATE_REST = 2;
    private static final int STATE_TRIAL_ACTIVE = 3;
    private static final int STATE_MODEL_TRAINING = 4;

    private static final int REST_DURATION_MS = 5000;
    private static final int MI_TRIAL_DURATION_MS = 15000;
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
    private EegStripView stripView;
    private LinearLayout gridContainer;
    private final WaveformView[] waveViews = new WaveformView[4];
    private final TextView[] labelViews = new TextView[4];
    private TextView tvWaveType;
    private TextView tvYRange;
    private TextView tvXRange;
    private View btnToggleLayout;

    private static final int[] MI_CHANNELS = {
            EegChannels.CH_C3, EegChannels.CH_C4,
            EegChannels.CH_CP3, EegChannels.CH_CP4
    };
    private static final int[] MI_COLORS = {
            R.color.wave_ch6, R.color.wave_ch7,
            R.color.wave_ch4, R.color.wave_ch5
    };

    private int currentWaveType = EegChannels.WAVE_TYPE_FILT;
    private float yRangeUv = 100f;
    private float splitXMax = 2.0f;
    private float xMax = 4.0f;
    private boolean isCombinedMode = true;
    private boolean isTrainMode = false;

    private View cardTraining;
    private View cardInference;
    private View btnModeToggle;

    private TextView tvInferDirection;

    private TextView tvInferTargetDir;
    private TextView tvInferConfidence;
    private TextView tvInferScores;
    private View btnStartInfer;
    private View btnStopInfer;
    private Spinner spinnerInferMode;
    private Spinner spinnerInferRounds;
    private TextView tvStatTotal;
    private TextView tvStatAccuracy;
    private TextView tvStatAvgConf;
    private TextView tvStatLeft;
    private TextView tvStatRight;
    private View btnExport;
    private View btnClearRecords;
    private RecyclerView rvRecords;
    private InferenceRecordAdapter inferAdapter;

    private boolean isInferencing = false;
    private int resultCountSinceLastRefresh = 0;
    private String currentTarget = null;
    private final ExecutorService recordWriter = Executors.newSingleThreadExecutor(r -> {
        Thread t = new Thread(r, "inference-record-writer");
        t.setDaemon(true);
        return t;
    });
    private final Handler targetHandler = new Handler(Looper.getMainLooper());

    private Handler handler = new Handler(Looper.getMainLooper());
    private long restStartTime;
    private long trialStartTime;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_mi_training, container, false);

        tvDirection = root.findViewById(R.id.tv_mi_direction);
        tvHint = root.findViewById(R.id.tv_mi_hint);
        tvTrialCount = root.findViewById(R.id.tv_mi_trial_count);
        progressTrial = root.findViewById(R.id.progress_mi_trial);
        btnStartTraining = root.findViewById(R.id.btn_mi_start_training);
        btnStopTraining = root.findViewById(R.id.btn_mi_stop_training);
        stripView = root.findViewById(R.id.strip_view_mi);
        stripView.setVisibleChannels(MI_CHANNELS);
        stripView.setYRange(yRangeUv / 1_000_000f);
        stripView.setXMax(xMax);

        gridContainer = root.findViewById(R.id.mi_grid_container);
        buildGrid();

        tvWaveType = root.findViewById(R.id.tv_mi_wave_type);
        tvYRange = root.findViewById(R.id.tv_mi_y_range);
        tvXRange = root.findViewById(R.id.tv_mi_x_range);
        btnToggleLayout = root.findViewById(R.id.btn_mi_toggle_layout);

        tvWaveType.setOnClickListener(v -> toggleWaveType());
        tvYRange.setOnClickListener(v -> showYRangeDialog());
        tvXRange.setOnClickListener(v -> showXRangeDialog());
        btnToggleLayout.setOnClickListener(v -> toggleLayout());

        btnStartTraining.setOnClickListener(v -> startTraining());
        btnStopTraining.setOnClickListener(v -> stopTraining());
        TrainingModeCoordinator.getInstance().registerController(
                TrainingModeCoordinator.Mode.MI, this);

        cardTraining = root.findViewById(R.id.card_mi_training);
        cardInference = root.findViewById(R.id.card_mi_inference);
        btnModeToggle = root.findViewById(R.id.btn_mi_mode_toggle);
        btnModeToggle.setOnClickListener(v -> toggleMode());

        tvInferDirection = root.findViewById(R.id.tv_infer_direction);

        tvInferTargetDir = root.findViewById(R.id.tv_infer_target_dir);
        tvInferConfidence = root.findViewById(R.id.tv_infer_confidence);
        tvInferScores = root.findViewById(R.id.tv_infer_scores);
        btnStartInfer = root.findViewById(R.id.btn_start_infer);
        btnStopInfer = root.findViewById(R.id.btn_stop_infer);
        spinnerInferMode = root.findViewById(R.id.spinner_infer_mode);
        spinnerInferRounds = root.findViewById(R.id.spinner_infer_rounds);
        tvStatTotal = root.findViewById(R.id.tv_stat_total);
        tvStatAccuracy = root.findViewById(R.id.tv_stat_accuracy);
        tvStatAvgConf = root.findViewById(R.id.tv_stat_avg_conf);
        tvStatLeft = root.findViewById(R.id.tv_stat_left);
        tvStatRight = root.findViewById(R.id.tv_stat_right);
        btnExport = root.findViewById(R.id.btn_export);
        btnClearRecords = root.findViewById(R.id.btn_clear_records);

        ArrayAdapter<String> modeAdapter = new ArrayAdapter<>(requireContext(),
                android.R.layout.simple_spinner_item, new String[]{"循环", "随机"});
        modeAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinnerInferMode.setAdapter(modeAdapter);
        ArrayAdapter<String> roundsAdapter = new ArrayAdapter<>(requireContext(),
                android.R.layout.simple_spinner_item, new String[]{"1次","2次","3次","4次","5次","6次","7次","8次","9次","10次"});
        roundsAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinnerInferRounds.setAdapter(roundsAdapter);
        spinnerInferRounds.setSelection(2);
        spinnerInferRounds.setEnabled(false);
        spinnerInferMode.setOnItemSelectedListener(new android.widget.AdapterView.OnItemSelectedListener() {
            @Override public void onItemSelected(android.widget.AdapterView<?> parent, View view, int position, long id) {
                spinnerInferRounds.setEnabled(position == 0);
            }
            @Override public void onNothingSelected(android.widget.AdapterView<?> parent) {}
        });

        btnStartInfer.setOnClickListener(v -> startInference());
        btnStopInfer.setOnClickListener(v -> stopInference());
        btnExport.setOnClickListener(v -> exportRecords());
        btnClearRecords.setOnClickListener(v -> confirmClearRecords());
        refreshStats();

        return root;
    }

    private void toggleMode() {
        isTrainMode = !isTrainMode;
        if (isTrainMode) {
            cardTraining.setVisibility(View.GONE);
            cardInference.setVisibility(View.VISIBLE);
            btnModeToggle.setSelected(true);
            ((com.google.android.material.button.MaterialButton) btnModeToggle).setText("切换到采集");
        } else {
            cardTraining.setVisibility(View.VISIBLE);
            cardInference.setVisibility(View.GONE);
            btnModeToggle.setSelected(false);
            ((com.google.android.material.button.MaterialButton) btnModeToggle).setText("切换到训练");
        }
        sendMiDisplayConfig();
    }

    private static final long READY_TRAIN_TIMEOUT_MS = 15000L;

    private void startTraining() {
        Log.i("MiTraining", "btnStartTraining clicked, isTraining=" + isTraining);
        if (isTraining) return;
        TrainingModeCoordinator.getInstance().activate(TrainingModeCoordinator.Mode.MI);
        isTraining = true;
        currentTrialIndex = 0;
        waitingReadyTrain = true;

        btnStartTraining.setEnabled(false);
        btnStopTraining.setEnabled(true);

        tvDirection.setText("启动中...");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.accent_info));
        tvHint.setText("等待下位机就绪");
        tvTrialCount.setText("");
        progressTrial.setProgress(0);

        CommandSender.getInstance().setModeCollect();
        CommandSender.getInstance().startTraining();

        handler.postDelayed(() -> {
            if (waitingReadyTrain) {
                stopTrainingInternal(true);
                tvDirection.setText("超时");
                tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.accent_error));
                tvHint.setText("下位机未响应，请检查连接");
            }
        }, READY_TRAIN_TIMEOUT_MS);
    }

    private void stopTraining() {
        stopTrainingInternal(true);
        TrainingModeCoordinator.getInstance().release(TrainingModeCoordinator.Mode.MI);
    }

    private void stopTrainingInternal(boolean updateUi) {
        Log.i("MiTraining", "stopTraining");
        boolean wasActive = isTraining || currentState != STATE_IDLE || waitingReadyTrain;
        isTraining = false;
        waitingReadyTrain = false;
        currentState = STATE_IDLE;
        handler.removeCallbacksAndMessages(null);
        if (wasActive) {
            TcpServerManager.getInstance().sendBinaryToDevice(EegProtocol.CMD_STOP, null);
            TcpServerManager.getInstance().sendDisplayToOutput(EegProtocol.CMD_TRAIN_STOP, null);
        }
        if (updateUi && getView() != null) {
            btnStartTraining.setEnabled(true);
            btnStopTraining.setEnabled(false);
            tvDirection.setText("已停止");
            if (getContext() != null) {
                tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.training_rest));
            }
            tvHint.setText("点击下方按钮开始训练");
            tvTrialCount.setText("");
            progressTrial.setIndeterminate(false);
            progressTrial.setProgress(0);
        }
    }

    @Override
    public void stopForModeSwitch(TrainingModeCoordinator.Mode nextMode) {
        isTraining = false;
        waitingReadyTrain = false;
        currentState = STATE_IDLE;
        handler.removeCallbacksAndMessages(null);
        if (getView() != null) {
            btnStartTraining.setEnabled(true);
            btnStopTraining.setEnabled(false);
        }
    }

    private void enterRestPhase() {
        if (!isTraining) return;
        Log.i("MiTraining", "enterRestPhase: trialIndex=" + currentTrialIndex);
        currentState = STATE_REST;
        restStartTime = System.currentTimeMillis();

        TcpServerManager.getInstance().sendBinaryToDevice(EegProtocol.CMD_MODE_SET, new byte[]{1});

        tvDirection.setText("休息");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.training_rest));
        tvHint.setText("放松，准备下一个想象任务");
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

    private void handleTaskStart(String side) {
        if (!isTraining) return;
        currentState = STATE_TRIAL_ACTIVE;
        trialStartTime = System.currentTimeMillis();
        boolean isLeft = "LEFT".equals(side);

        tvDirection.setText(isLeft ? "← 想象左手" : "想象右手 →");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(),
                isLeft ? R.color.direction_left : R.color.direction_right));
        tvHint.setText("运动想象（" + side + "）");
        tvTrialCount.setText("试次 " + (currentTrialIndex + 1) + " / " + TRIAL_SEQUENCE.length);
        progressTrial.setMax(MI_TRIAL_DURATION_MS / 100);
        progressTrial.setProgress(0);

        updateTrialTimer();
    }

    private void updateTrialTimer() {
        if (!isTraining || currentState != STATE_TRIAL_ACTIVE) return;
        long elapsed = System.currentTimeMillis() - trialStartTime;
        progressTrial.setProgress((int) (elapsed / 100));
        if (elapsed >= MI_TRIAL_DURATION_MS) {
            progressTrial.setProgress(progressTrial.getMax());
        } else {
            handler.postDelayed(this::updateTrialTimer, 100);
        }
    }

    private void handleTaskDone() {
        if (!isTraining) return;
        currentTrialIndex++;

        if (currentTrialIndex < TRIAL_SEQUENCE.length) {
            enterRestPhase();
        } else {
            trainingComplete();
        }
    }

    private void trainingComplete() {
        isTraining = false;
        currentState = STATE_IDLE;
        handler.removeCallbacksAndMessages(null);

        btnStartTraining.setEnabled(true);
        btnStopTraining.setEnabled(false);

        tvDirection.setText("训练完成");
        tvDirection.setTextColor(ContextCompat.getColor(requireContext(), R.color.accent_success));
        tvHint.setText("可以前往方向识别页面测试");
        tvTrialCount.setText("");
        progressTrial.setIndeterminate(false);
        progressTrial.setProgress(progressTrial.getMax());
    }

    private void handleReadyTrain() {
        Log.i("MiTraining", "handleReadyTrain: isTraining=" + isTraining + " waitingReadyTrain=" + waitingReadyTrain);
        if (!isTraining) {
            isTraining = true;
            currentTrialIndex = 0;
        }
        waitingReadyTrain = false;
        enterRestPhase();
    }


    private void handleReadyTest() {

    }

    @Override
    public void onResume() {
        super.onResume();
        DataDispatcher.getInstance().removeListener(this);
        DataDispatcher.getInstance().addListener(this);
        sendMiDisplayConfig();
    }

    @Override
    public void onPause() {
        super.onPause();
        DataDispatcher.getInstance().removeListener(this);
        TcpServerManager.getInstance().sendBinaryToDevice(EegProtocol.CMD_DISPLAY_CFG,
                EegChannels.buildAllNoneDisplayConfigData());
    }

    private void sendMiDisplayConfig() {
        int[] p = new int[24];
        for (int i = 0; i < 8; i++) {
            p[i * 3] = i;
            p[i * 3 + 1] = EegChannels.WAVE_TYPE_NONE;
            p[i * 3 + 2] = EegChannels.SPEC_TYPE_NONE;
        }
        for (int ch : MI_CHANNELS) {
            p[ch * 3 + 1] = currentWaveType;
        }
        TcpServerManager.getInstance().sendBinaryToDevice(EegProtocol.CMD_DISPLAY_CFG,
                EegChannels.buildDisplayConfigDataFromArray(p));
    }

    private void buildGrid() {
        Context ctx = requireContext();
        gridContainer.removeAllViews();

        LinearLayout row = new LinearLayout(ctx);
        row.setOrientation(LinearLayout.HORIZONTAL);
        row.setWeightSum(2f);
        row.setLayoutParams(new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT, 0, 1f));

        for (int i = 0; i < 4; i++) {
            if (i == 2) {
                gridContainer.addView(row);
                row = new LinearLayout(ctx);
                row.setOrientation(LinearLayout.HORIZONTAL);
                row.setWeightSum(2f);
                row.setLayoutParams(new LinearLayout.LayoutParams(
                        LinearLayout.LayoutParams.MATCH_PARENT, 0, 1f));
            }

            MaterialCardView card = new MaterialCardView(ctx);
            LinearLayout.LayoutParams cardParams = new LinearLayout.LayoutParams(
                    0, LinearLayout.LayoutParams.MATCH_PARENT, 1f);
            cardParams.setMargins(2, 2, 2, 2);
            card.setLayoutParams(cardParams);
            card.setCardElevation(1f);
            card.setRadius(6f);
            card.setCardBackgroundColor(ContextCompat.getColor(ctx, R.color.surface_low));
            card.setStrokeColor(ContextCompat.getColor(ctx, R.color.surface_overlay));
            card.setStrokeWidth(1);

            LinearLayout inner = new LinearLayout(ctx);
            inner.setOrientation(LinearLayout.VERTICAL);

            LinearLayout headerRow = new LinearLayout(ctx);
            headerRow.setOrientation(LinearLayout.HORIZONTAL);
            headerRow.setBackgroundColor(ContextCompat.getColor(ctx, R.color.surface_mid));
            headerRow.setGravity(Gravity.CENTER_VERTICAL);
            int hp = (int) (4 * getResources().getDisplayMetrics().density);
            headerRow.setPadding(hp * 2, 0, hp, 0);

            View dot = new View(ctx);
            LinearLayout.LayoutParams dotP = new LinearLayout.LayoutParams(6, 6);
            dotP.setMargins(0, 0, hp, 0);
            dot.setLayoutParams(dotP);
            dot.setBackgroundColor(ContextCompat.getColor(ctx, MI_COLORS[i]));
            headerRow.addView(dot);

            TextView label = new TextView(ctx);
            label.setTextColor(ContextCompat.getColor(ctx, R.color.text_secondary));
            label.setTextSize(10);
            label.setLayoutParams(new LinearLayout.LayoutParams(
                    0, LinearLayout.LayoutParams.WRAP_CONTENT, 1f));
            label.setText(EegChannels.nameOf(MI_CHANNELS[i]));
            headerRow.addView(label);
            labelViews[i] = label;

            inner.addView(headerRow);

            WaveformView wv = new WaveformView(ctx, null);
            wv.setId(View.generateViewId());
            wv.setLayoutParams(new LinearLayout.LayoutParams(
                    LinearLayout.LayoutParams.MATCH_PARENT, 0, 1f));
            wv.setWaveColor(ContextCompat.getColor(ctx, MI_COLORS[i]));
            wv.setYRange(yRangeUv / 1_000_000f);
            wv.setXMax(xMax);
            wv.setUnit("uV");
            inner.addView(wv);
            waveViews[i] = wv;

            card.addView(inner);
            row.addView(card);
        }
        gridContainer.addView(row);
    }

    private void toggleLayout() {
        isCombinedMode = !isCombinedMode;
        if (isCombinedMode) {
            stripView.setVisibility(View.VISIBLE);
            gridContainer.setVisibility(View.GONE);
            btnToggleLayout.setSelected(false);
            ((com.google.android.material.button.MaterialButton) btnToggleLayout).setText("分屏");
            xMax = splitXMax * 2f;
        } else {
            stripView.setVisibility(View.GONE);
            gridContainer.setVisibility(View.VISIBLE);
            btnToggleLayout.setSelected(true);
            ((com.google.android.material.button.MaterialButton) btnToggleLayout).setText("同屏");
            splitXMax = xMax / 2f;
            xMax = splitXMax;
        }
        stripView.setXMax(xMax);
        for (WaveformView wv : waveViews) if (wv != null) wv.setXMax(xMax);
        tvXRange.setText(String.format("%.0f s", xMax));
    }

    private void toggleWaveType() {
        if (currentWaveType == EegChannels.WAVE_TYPE_FILT) {
            currentWaveType = EegChannels.WAVE_TYPE_BASELINE;
            tvWaveType.setText("基线");
        } else {
            currentWaveType = EegChannels.WAVE_TYPE_FILT;
            tvWaveType.setText("滤波");
        }
        stripView.clear();
        for (WaveformView wv : waveViews) if (wv != null) wv.clear();
        sendMiDisplayConfig();
    }

    private void showYRangeDialog() {
        View dialogView = LayoutInflater.from(requireContext()).inflate(R.layout.dialog_range_input, null);
        EditText etValue = dialogView.findViewById(R.id.et_value);
        Spinner unitSpinner = dialogView.findViewById(R.id.unit_spinner);
        etValue.setInputType(android.text.InputType.TYPE_CLASS_NUMBER | android.text.InputType.TYPE_NUMBER_FLAG_DECIMAL);
        etValue.setHint("例如 100");
        ArrayAdapter<String> ua = new ArrayAdapter<>(requireContext(),
                android.R.layout.simple_spinner_item, new String[]{"μV", "mV"});
        ua.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        unitSpinner.setAdapter(ua);
        if (yRangeUv >= 1000f) {
            etValue.setText(String.format("%.0f", yRangeUv / 1000f));
            unitSpinner.setSelection(1);
        } else {
            etValue.setText(String.format("%.0f", yRangeUv));
            unitSpinner.setSelection(0);
        }
        new AlertDialog.Builder(requireContext())
                .setTitle("Y轴范围")
                .setView(dialogView)
                .setPositiveButton("确定", (d, which) -> {
                    String str = etValue.getText().toString().trim();
                    if (str.isEmpty()) return;
                    try {
                        float val = Float.parseFloat(str);
                        if (val <= 0) return;
                        String unit = (String) unitSpinner.getSelectedItem();
                        yRangeUv = "mV".equals(unit) ? val * 1000f : val;
                        stripView.setYRange(yRangeUv / 1_000_000f);
                        for (WaveformView wv : waveViews) if (wv != null) wv.setYRange(yRangeUv / 1_000_000f);
                        tvYRange.setText(yRangeUv >= 1000f
                                ? String.format("%.0fmV", yRangeUv / 1000f)
                                : String.format("%.0fμV", yRangeUv));
                    } catch (NumberFormatException ignored) {}
                })
                .setNegativeButton("取消", null)
                .show();
    }

    private void showXRangeDialog() {
        View dialogView = LayoutInflater.from(requireContext()).inflate(R.layout.dialog_range_input, null);
        EditText etValue = dialogView.findViewById(R.id.et_value);
        dialogView.findViewById(R.id.unit_spinner).setVisibility(View.GONE);
        etValue.setInputType(android.text.InputType.TYPE_CLASS_NUMBER | android.text.InputType.TYPE_NUMBER_FLAG_DECIMAL);
        etValue.setHint("例如 4");
        etValue.setText(String.format("%.1f", xMax));
        new AlertDialog.Builder(requireContext())
                .setTitle("时间范围 (秒)")
                .setView(dialogView)
                .setPositiveButton("确定", (d, which) -> {
                    String str = etValue.getText().toString().trim();
                    if (str.isEmpty()) return;
                    try {
                        float val = Math.round(Float.parseFloat(str) * 10f) / 10f;
                        if (val <= 0) return;
                        xMax = val;
                        stripView.setXMax(xMax);
                        for (WaveformView wv : waveViews) if (wv != null) wv.setXMax(xMax);
                        tvXRange.setText(String.format("%.0f s", xMax));
                    } catch (NumberFormatException ignored) {}
                })
                .setNegativeButton("取消", null)
                .show();
    }

    @Override
    public void onDestroyView() {
        TrainingModeCoordinator.getInstance().unregisterController(
                TrainingModeCoordinator.Mode.MI, this);
        stopTrainingInternal(false);
        super.onDestroyView();
        DataDispatcher.getInstance().removeListener(this);
        handler.removeCallbacksAndMessages(null);
    }

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
    public void onInferenceResult(InferenceResult result) {
        if (currentState == STATE_MODEL_TRAINING && result.isTrained()) {
            handler.post(this::handleReadyTest);
        }
        if (!isInferencing) return;
        resultCountSinceLastRefresh++;

        InferenceRecordStore.Record rec = new InferenceRecordStore.Record();
        rec.timestamp = System.currentTimeMillis();
        rec.intent = result.getIntent();
        rec.scoreLeft = result.getScoreLeft();
        rec.scoreRight = result.getScoreRight();
        rec.confidence = result.getConfidence();
        String gt = result.getGroundTruth();
        if (gt != null) {
            currentTarget = gt;
            rec.groundTruth = gt;
            rec.correct = gt.equals(result.getIntent());
        }
        recordWriter.execute(() -> InferenceRecordStore.addRecord(requireContext(), rec));

        if (resultCountSinceLastRefresh >= 5) {
            resultCountSinceLastRefresh = 0;
            refreshStats();
        }

        if (getActivity() != null) getActivity().runOnUiThread(() -> {
            tvInferConfidence.setText(String.format("置信度: %.1f%%", result.getConfidence() * 100));
            tvInferScores.setText(String.format("L:%.3f R:%.3f", result.getScoreLeft(), result.getScoreRight()));
            String intent = result.getIntent();
            String dirText = "LEFT".equals(intent) ? "←" : "→";
            tvInferDirection.setText(dirText);
            tvInferDirection.setTextColor(ContextCompat.getColor(requireContext(),
                    "LEFT".equals(intent) ? R.color.direction_left : R.color.direction_right));
            if (currentTarget != null) {
                String targetText = "LEFT".equals(currentTarget) ? "◀ 左" : "右 ▶";
                tvInferTargetDir.setText("目标: " + targetText);
                tvInferTargetDir.setTextColor(ContextCompat.getColor(requireContext(),
                        "LEFT".equals(currentTarget) ? R.color.direction_left : R.color.direction_right));
            }
        });
    }

    @Override public void onWaveData(int cmd, int ch, float val) {
        if (cmd != EegChannels.waveTypeToCmd(currentWaveType)) return;
        if (isCombinedMode) {
            stripView.addPoint(ch, val);
        } else {
            for (int i = 0; i < 4; i++) {
                if (ch == MI_CHANNELS[i] && waveViews[i] != null) {
                    waveViews[i].addPoint(val);
                    break;
                }
            }
        }
    }
    @Override public void onSpectrumData(int cmd, float[] mags) {}
    @Override public void onFocusData(float attn0, float attn1, float ema0, float ema1, int trend, int instant) {}
    @Override public void onEegFrame(EegFrame frame) {}
    @Override public void onIpcDiag(IpcDiagInfo diag) {}
    @Override public void onDirConfig(String configJson) {}
    @Override public void onModeSetOk(int mode) {}

    private void startInference() {
        if (isInferencing) return;
        isInferencing = true;
        resultCountSinceLastRefresh = 0;
        currentTarget = null;
        int mode = spinnerInferMode.getSelectedItemPosition();
        int rounds = mode == 0 ? spinnerInferRounds.getSelectedItemPosition() + 1 : 3;
        TcpServerManager.getInstance().sendBinaryToDevice(EegProtocol.CMD_INFER_CFG, new byte[]{(byte) mode, (byte) rounds});
        CommandSender.getInstance().setModeInfer();
        CommandSender.getInstance().startTest();
        btnStartInfer.setEnabled(false);
        btnStopInfer.setEnabled(true);
    }

    private void stopInference() {
        if (!isInferencing) return;
        isInferencing = false;
        TcpServerManager.getInstance().sendBinaryToDevice(EegProtocol.CMD_STOP, null);
        TcpServerManager.getInstance().sendDisplayToOutput(EegProtocol.CMD_TRAIN_STOP, null);
        btnStartInfer.setEnabled(true);
        btnStopInfer.setEnabled(false);
    }

    private void refreshStats() {
        java.util.List<InferenceRecordStore.Record> records = InferenceRecordStore.loadAll(requireContext());
        if (getActivity() != null) getActivity().runOnUiThread(() -> {
            tvStatTotal.setText("总推理: " + InferenceRecordStore.countTotal(records));
            float acc = InferenceRecordStore.calcAccuracy(records);
            tvStatAccuracy.setText(String.format("达标率: %.0f%%", acc * 100));
            tvStatAvgConf.setText(String.format("平均置信: %.1f%%", InferenceRecordStore.avgConfidence(records)));
            tvStatLeft.setText("←左: " + InferenceRecordStore.countLeft(records));
            tvStatRight.setText("右→: " + InferenceRecordStore.countRight(records));
        });
    }

    private void exportRecords() {
        java.util.List<InferenceRecordStore.Record> records = InferenceRecordStore.loadAll(requireContext());
        if (records.isEmpty()) return;
        String csv = InferenceRecordStore.exportCsv(records);
        try {
            java.io.File dir = new java.io.File(requireContext().getExternalFilesDir(null), "inference");
            if (!dir.exists()) dir.mkdirs();
            String ts = new java.text.SimpleDateFormat("yyyyMMdd_HHmmss", java.util.Locale.getDefault())
                    .format(new java.util.Date());
            java.io.File file = new java.io.File(dir, "inference_" + ts + ".csv");
            try (java.io.FileOutputStream fos = new java.io.FileOutputStream(file)) {
                fos.write(csv.getBytes("UTF-8"));
            }
        } catch (Exception e) {
            Log.e("MiTraining", "export failed", e);
        }
    }

    private void confirmClearRecords() {
        new AlertDialog.Builder(requireContext())
                .setTitle("清除记录")
                .setMessage("确定清除所有推理记录？")
                .setPositiveButton("确定", (d, w) -> {
                    InferenceRecordStore.clearAll(requireContext());
                    refreshStats();
                })
                .setNegativeButton("取消", null)
                .show();
    }
}