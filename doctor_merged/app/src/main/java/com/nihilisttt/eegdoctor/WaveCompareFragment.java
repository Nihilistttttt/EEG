package com.nihilisttt.eegdoctor;

import com.google.android.material.button.MaterialButton;
import com.nihilisttt.eegdoctor.R;

import android.app.AlertDialog;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Spinner;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;

import com.google.android.material.card.MaterialCardView;

public class WaveCompareFragment extends Fragment implements DataListener {

    private static final int DEFAULT_LABEL_COUNT = 7;
    private static final int DEFAULT_STEP = 500;
    private static final String DEFAULT_STEP_UNIT = "uV";
    private static final float DEFAULT_X_MAX = 2.048f;
    private static final int NUM_VIEWS = 4;

    private static final String[] CHANNEL_NAMES = EegChannels.NAMES;
    private static final String[] WAVE_TYPE_NAMES = EegChannels.WAVE_TYPE_NAMES;
    private static final int[] COLORS = {
        R.color.wave_ch0, R.color.wave_filtered,
        R.color.wave_ch1, R.color.accent_info
    };

    private final WaveformView[] waveViews = new WaveformView[NUM_VIEWS];
    private final TextView[] labelViews = new TextView[NUM_VIEWS];

    private TextView tvXRange;
    private TextView tvWaveLabelCount;
    private TextView tvWaveRange;

    private int currentLabelCount = DEFAULT_LABEL_COUNT;
    private int currentStep = DEFAULT_STEP;
    private String currentStepUnit = DEFAULT_STEP_UNIT;
    private float currentXMax = DEFAULT_X_MAX;

    private int[] ch = {6, 7, 4, 5};
    private int[] waveType = {0, 0, 0, 0};
    private boolean isPaused = false;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_wave_compare, container, false);

        ch[0] = SettingsStore.getWcChA(requireContext(), 6);
        ch[1] = SettingsStore.getWcChA(requireContext(), 7);
        ch[2] = SettingsStore.getWcChB(requireContext(), 4);
        ch[3] = SettingsStore.getWcChB(requireContext(), 5);

        LinearLayout grid = root.findViewById(R.id.grid_container);
        buildGrid(grid);

        tvXRange = root.findViewById(R.id.tv_x_range);
        tvWaveLabelCount = root.findViewById(R.id.tv_wave_label_count);
        tvWaveRange = root.findViewById(R.id.tv_wave_range);

        root.findViewById(R.id.btn_channel_cfg).setOnClickListener(v -> showChannelConfigDialog());

        MaterialButton btnPause = root.findViewById(R.id.btn_pause_resume);
        btnPause.setOnClickListener(v -> {
            isPaused = !isPaused;
            btnPause.setText(isPaused ? "继续" : "暂停");
        });

        tvXRange.setOnClickListener(v -> showXRangeDialog());
        tvWaveLabelCount.setOnClickListener(v -> showLabelCountDialog());
        tvWaveRange.setOnClickListener(v -> showWaveRangeDialog());

        int step = SettingsStore.getWaveStep(requireContext(), DEFAULT_STEP);
        String stepUnit = SettingsStore.getWaveStepUnit(requireContext(), DEFAULT_STEP_UNIT);
        int labelCount = SettingsStore.getWaveLabelCount(requireContext(), DEFAULT_LABEL_COUNT);
        float xMax = SettingsStore.getWaveXMax(requireContext(), DEFAULT_X_MAX);

        currentLabelCount = labelCount;
        currentStep = step;
        currentStepUnit = stepUnit;
        currentXMax = xMax;

        applyXRange(currentXMax);
        applyWaveStep(currentStep, currentStepUnit, currentLabelCount);
        updateLabels();

        return root;
    }

    private void buildGrid(LinearLayout grid) {
        Context ctx = requireContext();
        grid.removeAllViews();

        LinearLayout row = new LinearLayout(ctx);
        row.setOrientation(LinearLayout.HORIZONTAL);
        row.setWeightSum(2f);
        LinearLayout.LayoutParams rowParams = new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT, 0, 1f);
        row.setLayoutParams(rowParams);

        for (int i = 0; i < NUM_VIEWS; i++) {
            if (i == 2) {
                grid.addView(row);
                row = new LinearLayout(ctx);
                row.setOrientation(LinearLayout.HORIZONTAL);
                row.setWeightSum(2f);
                LinearLayout.LayoutParams rp2 = new LinearLayout.LayoutParams(
                        LinearLayout.LayoutParams.MATCH_PARENT, 0, 1f);
                row.setLayoutParams(rp2);
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
            dot.setBackgroundColor(ContextCompat.getColor(ctx, COLORS[i]));
            headerRow.addView(dot);

            TextView label = new TextView(ctx);
            label.setTextColor(ContextCompat.getColor(ctx, R.color.text_secondary));
            label.setTextSize(10);
            LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(
                    0, LinearLayout.LayoutParams.WRAP_CONTENT, 1f);
            label.setLayoutParams(lp);
            headerRow.addView(label);
            labelViews[i] = label;

            inner.addView(headerRow);

            WaveformView wv = new WaveformView(ctx, null);
            wv.setId(View.generateViewId());
            LinearLayout.LayoutParams wvp = new LinearLayout.LayoutParams(
                    LinearLayout.LayoutParams.MATCH_PARENT, 0, 1f);
            wv.setLayoutParams(wvp);
            wv.setWaveColor(ContextCompat.getColor(ctx, COLORS[i]));
            inner.addView(wv);
            waveViews[i] = wv;

            card.addView(inner);
            row.addView(card);
        }
        grid.addView(row);
    }

    private void updateLabels() {
        for (int i = 0; i < NUM_VIEWS; i++) {
            String chName = EegChannels.nameOf(ch[i]);
            String typeName = (waveType[i] >= 0 && waveType[i] < WAVE_TYPE_NAMES.length) ? WAVE_TYPE_NAMES[waveType[i]] : "";
            if (labelViews[i] != null) labelViews[i].setText(chName + " " + typeName);
        }
    }

    private void showChannelConfigDialog() {
        Context ctx = requireContext();
        int dp4 = (int) (4 * getResources().getDisplayMetrics().density);
        int dp8 = (int) (8 * getResources().getDisplayMetrics().density);

        android.widget.ScrollView scrollView = new android.widget.ScrollView(ctx);
        LinearLayout rootLayout = new LinearLayout(ctx);
        rootLayout.setOrientation(LinearLayout.VERTICAL);
        rootLayout.setPadding(dp4 * 3, dp4 * 2, dp4 * 3, 0);
        rootLayout.setMinimumWidth((int) (480 * getResources().getDisplayMetrics().density));

        ArrayAdapter<String> chAdapter = new ArrayAdapter<>(ctx,
                android.R.layout.simple_spinner_item, CHANNEL_NAMES);
        chAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        ArrayAdapter<String> waveTypeAdapter = new ArrayAdapter<>(ctx,
                android.R.layout.simple_spinner_item, WAVE_TYPE_NAMES);
        waveTypeAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        String[] slotNames = {"左上", "左下", "右上", "右下"};
        Spinner[] spCh = new Spinner[NUM_VIEWS];
        Spinner[] spWaveType = new Spinner[NUM_VIEWS];

        for (int row = 0; row < 2; row++) {
            LinearLayout colRow = new LinearLayout(ctx);
            colRow.setOrientation(LinearLayout.HORIZONTAL);
            colRow.setWeightSum(2f);

            for (int col = 0; col < 2; col++) {
                int i = row * 2 + col;

                LinearLayout colLayout = new LinearLayout(ctx);
                colLayout.setOrientation(LinearLayout.VERTICAL);
                LinearLayout.LayoutParams colP = new LinearLayout.LayoutParams(
                        0, LinearLayout.LayoutParams.WRAP_CONTENT, 1f);
                colLayout.setLayoutParams(colP);

                TextView title = new TextView(ctx);
                title.setText(slotNames[i]);
                title.setTextSize(13);
                title.setTypeface(null, android.graphics.Typeface.BOLD);
                title.setPadding(0, dp4, 0, dp4);
                colLayout.addView(title);

                spCh[i] = new Spinner(ctx);
                spCh[i].setAdapter(chAdapter);
                spCh[i].setSelection(ch[i]);
                addDialogRow(colLayout, "电极", spCh[i]);

                spWaveType[i] = new Spinner(ctx);
                spWaveType[i].setAdapter(waveTypeAdapter);
                spWaveType[i].setSelection(waveType[i]);
                addDialogRow(colLayout, "波形", spWaveType[i]);

                colRow.addView(colLayout);

                if (col == 0) {
                    View vDiv = new View(ctx);
                    LinearLayout.LayoutParams vdp = new LinearLayout.LayoutParams(
                            1, LinearLayout.LayoutParams.MATCH_PARENT);
                    vdp.setMargins(dp8, 0, dp8, 0);
                    vDiv.setLayoutParams(vdp);
                    vDiv.setBackgroundColor(ContextCompat.getColor(ctx, R.color.surface_overlay));
                    colRow.addView(vDiv);
                }
            }

            rootLayout.addView(colRow);

            if (row == 0) {
                View hDiv = new View(ctx);
                hDiv.setBackgroundColor(ContextCompat.getColor(ctx, R.color.surface_overlay));
                LinearLayout.LayoutParams hdp = new LinearLayout.LayoutParams(
                        LinearLayout.LayoutParams.MATCH_PARENT, 1);
                hdp.setMargins(0, dp8, 0, dp8);
                hDiv.setLayoutParams(hdp);
                rootLayout.addView(hDiv);
            }
        }

        scrollView.addView(rootLayout);

        new AlertDialog.Builder(ctx)
                .setTitle("通道配置")
                .setView(scrollView)
                .setPositiveButton("确定", (d, id) -> {
                    for (int i = 0; i < NUM_VIEWS; i++) {
                        ch[i] = spCh[i].getSelectedItemPosition();
                        waveType[i] = spWaveType[i].getSelectedItemPosition();
                    }
                    updateLabels();
                    clearAll();
                    SettingsStore.setWcChA(requireContext(), ch[0]);
                    SettingsStore.setWcChB(requireContext(), ch[2]);
                    sendDisplayConfig();
                })
                .setNegativeButton("取消", null)
                .show();
    }

    private void addDialogRow(LinearLayout parent, String label, Spinner spinner) {
        LinearLayout row = new LinearLayout(requireContext());
        row.setOrientation(LinearLayout.HORIZONTAL);
        row.setGravity(Gravity.CENTER_VERTICAL);
        TextView tv = new TextView(requireContext());
        tv.setText(label);
        tv.setTextSize(13);
        int dp8 = (int) (8 * getResources().getDisplayMetrics().density);
        tv.setPadding(0, 0, dp8, 0);
        LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);
        tv.setLayoutParams(lp);
        row.addView(tv);
        LinearLayout.LayoutParams sp = new LinearLayout.LayoutParams(
                0, LinearLayout.LayoutParams.WRAP_CONTENT, 1f);
        spinner.setLayoutParams(sp);
        row.addView(spinner);
        parent.addView(row);
    }

    private void sendDisplayConfig() {
        int[] p = new int[24];
        p[0] = ch[0]; p[1] = waveType[0]; p[2] = EegChannels.SPEC_TYPE_NONE;
        p[3] = ch[1]; p[4] = waveType[1]; p[5] = EegChannels.SPEC_TYPE_NONE;
        p[6] = ch[2]; p[7] = waveType[2]; p[8] = EegChannels.SPEC_TYPE_NONE;
        p[9] = ch[3]; p[10] = waveType[3]; p[11] = EegChannels.SPEC_TYPE_NONE;
        for (int i = 4; i < 8; i++) {
            p[i * 3] = 0;
            p[i * 3 + 1] = EegChannels.WAVE_TYPE_NONE;
            p[i * 3 + 2] = EegChannels.SPEC_TYPE_NONE;
        }
        byte[] data = EegChannels.buildDisplayConfigDataFromArray(p);
        TcpServerManager.getInstance().sendBinaryToDevice(EegProtocol.CMD_DISPLAY_CFG, data);
        Log.d("WaveCompare", "Sent DISPLAY_CFG binary");
    }

    private void clearAll() {
        for (WaveformView wv : waveViews) {
            if (wv != null) wv.clear();
        }
    }

    private TcpServerManager.ConnectionListener connectionListener;

    @Override
    public void onResume() {
        super.onResume();
        DataDispatcher.getInstance().removeListener(this);
        DataDispatcher.getInstance().addListener(this);
        sendDisplayConfig();
        if (connectionListener == null) {
            connectionListener = new TcpServerManager.ConnectionListener() {
                @Override
                public void onDeviceConnected(boolean connected) {
                    if (connected && getActivity() != null) {
                        getActivity().runOnUiThread(() -> sendDisplayConfig());
                    }
                }
                @Override
                public void onPatientConnected(boolean connected) {}
            };
            TcpServerManager.getInstance().addConnectionListener(connectionListener);
        }
        Log.d("WaveCompare", "onResume: listener refreshed");
    }

    @Override
    public void onPause() {
        super.onPause();
        DataDispatcher.getInstance().removeListener(this);
        Log.d("WaveCompare", "onPause: listener removed");
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        DataDispatcher.getInstance().removeListener(this);
        if (connectionListener != null) {
            TcpServerManager.getInstance().removeConnectionListener(connectionListener);
            connectionListener = null;
        }
        Log.d("WaveCompare", "onDestroyView: listener removed");
    }

    private void applyXRange(float xMax) {
        for (WaveformView wv : waveViews) if (wv != null) wv.setXMax(xMax);
        tvXRange.setText(String.format("%.1f s", xMax));
        currentXMax = xMax;
        SettingsStore.setWaveXMax(requireContext(), xMax);
    }

    private void applyWaveStep(int step, String unit, int labelCount) {
        float stepVolt;
        switch (unit) {
            case "uV": stepVolt = step / 1_000_000f; break;
            case "mV": stepVolt = step / 1000f; break;
            default:   stepVolt = step; break;
        }
        float halfRange = stepVolt * ((labelCount - 1) / 2.0f);
        for (WaveformView wv : waveViews) {
            if (wv != null) {
                wv.setYRange(halfRange);
                wv.setUnit(unit);
            }
        }
        tvWaveRange.setText(step + unit);
        tvWaveLabelCount.setText(String.valueOf(labelCount));
        currentStep = step;
        currentStepUnit = unit;
        currentLabelCount = labelCount;
        SettingsStore.setWaveStep(requireContext(), step);
        SettingsStore.setWaveStepUnit(requireContext(), unit);
        SettingsStore.setWaveLabelCount(requireContext(), labelCount);
    }

    private void showXRangeDialog() {
        View dialogView = LayoutInflater.from(requireContext())
                .inflate(R.layout.dialog_range_input, null);
        EditText etValue = dialogView.findViewById(R.id.et_value);
        Spinner unitSpinner = dialogView.findViewById(R.id.unit_spinner);
        unitSpinner.setVisibility(View.GONE);
        etValue.setInputType(android.text.InputType.TYPE_CLASS_NUMBER
                | android.text.InputType.TYPE_NUMBER_FLAG_DECIMAL);
        etValue.setHint("例如 2.048");
        etValue.setText(String.valueOf(currentXMax));
        new AlertDialog.Builder(requireContext())
                .setTitle("设置 X 轴时间范围 (秒)")
                .setView(dialogView)
                .setPositiveButton("确定", (d, which) -> {
                    String str = etValue.getText().toString().trim();
                    if (str.isEmpty()) return;
                    try { float xMax = Float.parseFloat(str); if (xMax <= 0) return; applyXRange(xMax); }
                    catch (NumberFormatException ignored) {}
                })
                .setNegativeButton("取消", null)
                .show();
    }

    private void showLabelCountDialog() {
        final String[] labels = {"3", "5", "7", "9"};
        int ci = 0;
        for (int i = 0; i < labels.length; i++) if (labels[i].equals(String.valueOf(currentLabelCount))) { ci = i; break; }
        new AlertDialog.Builder(requireContext())
                .setTitle("选择 Y 轴标签数量")
                .setSingleChoiceItems(labels, ci, (dialog, which) -> {
                    applyWaveStep(currentStep, currentStepUnit, Integer.parseInt(labels[which]));
                    dialog.dismiss();
                })
                .setNegativeButton("取消", null)
                .show();
    }

    private void showWaveRangeDialog() {
        View dialogView = LayoutInflater.from(requireContext())
                .inflate(R.layout.dialog_range_input, null);
        EditText etValue = dialogView.findViewById(R.id.et_value);
        Spinner unitSpinner = dialogView.findViewById(R.id.unit_spinner);
        etValue.setInputType(android.text.InputType.TYPE_CLASS_NUMBER);
        ArrayAdapter<String> ua = new ArrayAdapter<>(requireContext(),
                android.R.layout.simple_spinner_item, new String[]{"uV", "mV", "V"});
        ua.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        unitSpinner.setAdapter(ua);
        etValue.setText(String.valueOf(currentStep));
        int pos = ua.getPosition(currentStepUnit);
        if (pos >= 0) unitSpinner.setSelection(pos);
        new AlertDialog.Builder(requireContext())
                .setTitle("设置波形步长")
                .setView(dialogView)
                .setPositiveButton("确定", (d, which) -> {
                    String str = etValue.getText().toString().trim();
                    if (str.isEmpty()) return;
                    try { int si = Integer.parseInt(str); if (si <= 0) return; applyWaveStep(si, (String) unitSpinner.getSelectedItem(), currentLabelCount); }
                    catch (NumberFormatException ignored) {}
                })
                .setNegativeButton("取消", null)
                .show();
    }

    private static int waveTypeToCmd(int wt) {
        switch (wt) { case 1: return EegChannels.CMD_WAVE_FILT; case 2: return EegChannels.CMD_WAVE_BASELINE; default: return EegChannels.CMD_WAVE_RAW; }
    }

    @Override
    public void onWaveData(int cmd, int chNum, float val) {
        if (isPaused) return;
        for (int i = 0; i < NUM_VIEWS; i++) {
            if (cmd == waveTypeToCmd(waveType[i]) && chNum == ch[i] && waveViews[i] != null)
                waveViews[i].addPoint(val);
        }
    }

    @Override
    public void onSpectrumData(int cmd, float[] mags) {}

    @Override
    public void onFocusData(float attn0, float attn1, float ema0, float ema1, float blinkCount, int trend, int instant) {}
}
