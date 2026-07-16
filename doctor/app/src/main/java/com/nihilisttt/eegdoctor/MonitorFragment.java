package com.nihilisttt.eegdoctor;

import com.nihilisttt.eegdoctor.R;

import android.app.AlertDialog;
import android.content.Context;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;

import com.google.android.material.card.MaterialCardView;

import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

public class MonitorFragment extends Fragment implements DataListener {

    private static final int DEFAULT_STEP = 500;
    private static final String DEFAULT_STEP_UNIT = "uV";
    private static final int DEFAULT_LABEL_COUNT = 7;
    private static final float DEFAULT_X_MAX = 2.048f;

    private final List<ChannelConfig> channels;
    private final List<WaveformView> waveformViews = new ArrayList<>();
    private final List<SpectrumView> spectrumViews = new ArrayList<>();
    private final List<View> signalQualityViews = new ArrayList<>();


    private TextView tvWaveLabelCount;
    private TextView tvWaveRange;
    private TextView tvXRange;
    private TextView tvSpectrumRange;
    private TextView btnPauseResume;
    private boolean isPaused = false;
    private int currentLabelCount = DEFAULT_LABEL_COUNT;
    private boolean deviceConnected = false;
    private boolean patientConnected = false;

    private static final String[] CHANNEL_NAMES = {"OZ", "O1", "F3", "F4", "CP3", "CP4", "C3", "C4"};
    private static final String[] WAVE_TYPE_NAMES = {"原始波形", "滤波波形", "基线修复"};
    private static final String[] SPEC_TYPE_NAMES = {"原始频谱", "频域滤波频谱", "时域滤波频谱"};

    private int[] waveCh = {2, 2};
    private int[] waveType = {0, 1};
    private int[] specType = {0, 2};
    private int[] waveMode = {0, 0};

    public MonitorFragment() {
        this.channels = ChannelConfig.getDefaultDualChannel();
    }

    public MonitorFragment(List<ChannelConfig> channels) {
        this.channels = channels != null ? channels : ChannelConfig.getDefaultDualChannel();
    }

    private void loadDisplayConfigFromStore() {
        waveCh[0] = SettingsStore.getWaveChA(requireContext(), 2);
        waveCh[1] = SettingsStore.getWaveChB(requireContext(), 2);
        waveType[0] = SettingsStore.getWaveTypeA(requireContext(), 0);
        waveType[1] = SettingsStore.getWaveTypeB(requireContext(), 1);
        specType[0] = SettingsStore.getSpecTypeA(requireContext(), 0);
        specType[1] = SettingsStore.getSpecTypeB(requireContext(), 2);
        waveMode[0] = SettingsStore.getWaveModeA(requireContext(), 0);
        waveMode[1] = SettingsStore.getWaveModeB(requireContext(), 0);
    }

    private void saveDisplayConfigToStore() {
        SettingsStore.setWaveChA(requireContext(), waveCh[0]);
        SettingsStore.setWaveChB(requireContext(), waveCh[1]);
        SettingsStore.setWaveTypeA(requireContext(), waveType[0]);
        SettingsStore.setWaveTypeB(requireContext(), waveType[1]);
        SettingsStore.setSpecTypeA(requireContext(), specType[0]);
        SettingsStore.setSpecTypeB(requireContext(), specType[1]);
        for (int i = 0; i < waveformViews.size() && i < 2; i++) {
            if (waveformViews.get(i) != null) {
                waveMode[i] = waveformViews.get(i).getMode();
            }
        }
        SettingsStore.setWaveModeA(requireContext(), waveMode[0]);
        SettingsStore.setWaveModeB(requireContext(), waveMode[1]);
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_monitor, container, false);

        loadDisplayConfigFromStore();

        rebuildChannelsList();

        LinearLayout channelContainer = root.findViewById(R.id.channel_container);
        buildChannelViews(channelContainer);


        tvWaveLabelCount = root.findViewById(R.id.tv_wave_label_count);
        tvWaveRange = root.findViewById(R.id.tv_wave_range);
        tvXRange = root.findViewById(R.id.tv_x_range);
        tvSpectrumRange = root.findViewById(R.id.tv_spectrum_range);
        btnPauseResume = root.findViewById(R.id.btn_pause_resume);

        btnPauseResume.setOnClickListener(v -> {
            isPaused = !isPaused;
            btnPauseResume.setText(isPaused ? "继续" : "暂停");
            updateConnectionStatus();
        });

        root.findViewById(R.id.btn_set_ip).setOnClickListener(v -> showWifiConfigDialog());
        root.findViewById(R.id.btn_display_cfg).setOnClickListener(v -> showDisplayConfigDialog());
        root.findViewById(R.id.btn_training).setOnClickListener(v -> {
            if (getActivity() instanceof MainActivity) {
                ((MainActivity) requireActivity()).switchToPage(4);
            }
        });

        TcpServerManager.getInstance().addConnectionListener(new TcpServerManager.ConnectionListener() {
            @Override
            public void onDeviceConnected(boolean connected) {
                deviceConnected = connected;
                if (connected && getActivity() != null) {
                    getActivity().runOnUiThread(() -> {
                        updateConnectionStatus();
                        sendDisplayConfig();
                    });
                } else if (getActivity() != null) {
                    getActivity().runOnUiThread(() -> updateConnectionStatus());
                }
            }
            @Override
            public void onPatientConnected(boolean connected) {
                patientConnected = connected;
                if (getActivity() != null) getActivity().runOnUiThread(() -> updateConnectionStatus());
            }
        });

        setupRangeSelectors();
        sendDisplayConfig();
        return root;
    }

    private void buildChannelViews(LinearLayout container) {
        Context ctx = requireContext();
        int channelCount = channels.size();
        boolean hasSpectrum = true;

        for (int i = 0; i < channelCount; i++) {
            ChannelConfig ch = channels.get(i);

            LinearLayout row = new LinearLayout(ctx);
            row.setOrientation(LinearLayout.HORIZONTAL);
            row.setWeightSum(hasSpectrum ? 4f : 1f);

            MaterialCardView waveCard = new MaterialCardView(ctx);
            LinearLayout.LayoutParams waveCardParams = new LinearLayout.LayoutParams(
                    0, LinearLayout.LayoutParams.MATCH_PARENT, hasSpectrum ? 3f : 1f);
            waveCardParams.setMargins(2, 2, 2, 2);
            waveCard.setLayoutParams(waveCardParams);
            waveCard.setCardElevation(1f);
            waveCard.setRadius(6f);
            waveCard.setCardBackgroundColor(ContextCompat.getColor(ctx, R.color.surface_low));
            waveCard.setStrokeColor(ContextCompat.getColor(ctx, R.color.surface_overlay));
            waveCard.setStrokeWidth(1);

            LinearLayout waveInner = new LinearLayout(ctx);
            waveInner.setOrientation(LinearLayout.VERTICAL);

            LinearLayout headerRow = new LinearLayout(ctx);
            headerRow.setOrientation(LinearLayout.HORIZONTAL);
            headerRow.setBackgroundColor(ContextCompat.getColor(ctx, R.color.surface_mid));
            headerRow.setGravity(android.view.Gravity.CENTER_VERTICAL);
            int headerPad = (int) (4 * getResources().getDisplayMetrics().density);
            headerRow.setPadding(headerPad * 2, 0, headerPad, 0);

            View colorDot = new View(ctx);
            LinearLayout.LayoutParams dotParams = new LinearLayout.LayoutParams(6, 6);
            dotParams.setMargins(0, 0, headerPad, 0);
            colorDot.setLayoutParams(dotParams);
            colorDot.setBackgroundColor(ContextCompat.getColor(ctx, ch.getColorResId()));
            headerRow.addView(colorDot);

            TextView labelView = new TextView(ctx);
            labelView.setText(ch.getElectrodeName());
            labelView.setTextColor(ContextCompat.getColor(ctx, R.color.text_secondary));
            labelView.setTextSize(10);
            LinearLayout.LayoutParams labelParams = new LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.WRAP_CONTENT, 1f);
            labelView.setLayoutParams(labelParams);
            headerRow.addView(labelView);


            View sqView = new View(ctx);
            LinearLayout.LayoutParams sqParams = new LinearLayout.LayoutParams(20, 4);
            sqParams.setMargins(headerPad, 0, 0, 0);
            sqView.setLayoutParams(sqParams);
            sqView.setBackgroundColor(ContextCompat.getColor(ctx, R.color.accent_success));
            headerRow.addView(sqView);
            signalQualityViews.add(sqView);

            waveInner.addView(headerRow);

            WaveformView wv = new WaveformView(ctx, null);
            wv.setId(View.generateViewId());
            LinearLayout.LayoutParams wvParams = new LinearLayout.LayoutParams(
                    LinearLayout.LayoutParams.MATCH_PARENT, 0, 1f);
            wv.setLayoutParams(wvParams);
            wv.setWaveColor(ContextCompat.getColor(ctx, ch.getColorResId()));
            wv.setYRange(ch.getYRange());
            wv.setUnit(ch.getUnit());
            waveInner.addView(wv);
            waveformViews.add(wv);

            waveCard.addView(waveInner);
            row.addView(waveCard);

            if (hasSpectrum) {
                MaterialCardView specCard = new MaterialCardView(ctx);
                LinearLayout.LayoutParams specCardParams = new LinearLayout.LayoutParams(
                        0, LinearLayout.LayoutParams.MATCH_PARENT, 1f);
                specCardParams.setMargins(2, 2, 2, 2);
                specCard.setLayoutParams(specCardParams);
                specCard.setCardElevation(1f);
                specCard.setRadius(6f);
                specCard.setCardBackgroundColor(ContextCompat.getColor(ctx, R.color.surface_low));
                specCard.setStrokeColor(ContextCompat.getColor(ctx, R.color.surface_overlay));
                specCard.setStrokeWidth(1);

                LinearLayout specInner = new LinearLayout(ctx);
                specInner.setOrientation(LinearLayout.VERTICAL);

                TextView specLabel = new TextView(ctx);
                specLabel.setText("频谱 " + ch.getLabel());
                specLabel.setTextColor(ContextCompat.getColor(ctx, R.color.text_secondary));
                specLabel.setTextSize(10);
                specLabel.setBackgroundColor(ContextCompat.getColor(ctx, R.color.surface_mid));
                int slp = (int) (8 * getResources().getDisplayMetrics().density);
                specLabel.setPadding(slp, 0, 0, 0);
                specInner.addView(specLabel);

                SpectrumView sv = new SpectrumView(ctx, null);
                sv.setId(View.generateViewId());
                LinearLayout.LayoutParams svParams = new LinearLayout.LayoutParams(
                        LinearLayout.LayoutParams.MATCH_PARENT, 0, 1f);
                sv.setLayoutParams(svParams);
                specInner.addView(sv);
                spectrumViews.add(sv);

                specCard.addView(specInner);
                row.addView(specCard);
            }

            LinearLayout.LayoutParams rowParams = new LinearLayout.LayoutParams(
                    LinearLayout.LayoutParams.MATCH_PARENT, 0, 1f);
            container.addView(row, rowParams);
        }
    }

    private void setupRangeSelectors() {
        tvWaveLabelCount.setOnClickListener(v -> showLabelCountDialog());
        tvWaveRange.setOnClickListener(v -> showWaveRangeDialog());
        tvXRange.setOnClickListener(v -> showXRangeDialog());
        tvSpectrumRange.setOnClickListener(v -> showSpectrumRangeDialog());

        int step = SettingsStore.getWaveStep(requireContext(), DEFAULT_STEP);
        String stepUnit = SettingsStore.getWaveStepUnit(requireContext(), DEFAULT_STEP_UNIT);
        int labelCount = SettingsStore.getWaveLabelCount(requireContext(), DEFAULT_LABEL_COUNT);
        float xMax = SettingsStore.getWaveXMax(requireContext(), DEFAULT_X_MAX);
        float specRange = SettingsStore.getSpecRange(requireContext(), 500f);
        String specUnit = SettingsStore.getSpecUnit(requireContext(), "uV");

        currentLabelCount = labelCount;
        tvWaveLabelCount.setText(String.valueOf(labelCount));
        tvWaveRange.setText(step + stepUnit);
        tvXRange.setText(String.format("%.3f s", xMax));
        tvSpectrumRange.setText((Math.abs(specRange - Math.round(specRange)) < 0.001f ? String.format("%.0f", specRange) : String.format("%.3g", specRange)) + " " + specUnit);

        applyWaveStep(step, stepUnit, labelCount);
        applyXRange(xMax);
        for (SpectrumView sv : spectrumViews) {
            sv.setRange(specRange, specUnit);
        }
        for (int i = 0; i < waveformViews.size() && i < 2; i++) {
            waveformViews.get(i).setMode(waveMode[i]);
        }
    }

    private void showLabelCountDialog() {
        final String[] labels = {"3", "5", "7", "9"};
        int currentIndex = 0;
        for (int i = 0; i < labels.length; i++) {
            if (labels[i].equals(String.valueOf(currentLabelCount))) { currentIndex = i; break; }
        }
        new AlertDialog.Builder(requireContext())
                .setTitle("选择 Y 轴标签数量")
                .setSingleChoiceItems(labels, currentIndex, (dialog, which) -> {
                    int newCount = Integer.parseInt(labels[which]);
                    currentLabelCount = newCount;
                    tvWaveLabelCount.setText(String.valueOf(newCount));
                    String stepText = tvWaveRange.getText().toString();
                    String numericPart = stepText.replaceAll("[^0-9]", "");
                    String unitPart = stepText.replaceAll("[0-9]", "");
                    if (!numericPart.isEmpty()) {
                        applyWaveStep(Integer.parseInt(numericPart), unitPart, newCount);
                    }
                    SettingsStore.setWaveLabelCount(requireContext(), newCount);
                    dialog.dismiss();
                })
                .setNegativeButton("取消", null)
                .show();
    }

    private void applyWaveStep(int step, String unit, int labelCount) {
        float stepVolt;
        switch (unit) {
            case "uV": stepVolt = step / 1_000_000f; break;
            case "mV": stepVolt = step / 1000f; break;
            default:   stepVolt = step; break;
        }
        float halfRange = stepVolt * ((labelCount - 1) / 2.0f);
        for (WaveformView wv : waveformViews) {
            wv.setYRange(halfRange);
            wv.setUnit(unit);
        }
    }

    private void showWaveRangeDialog() {
        View dialogView = LayoutInflater.from(requireContext()).inflate(R.layout.dialog_range_input, null);
        EditText etValue = dialogView.findViewById(R.id.et_value);
        Spinner unitSpinner = dialogView.findViewById(R.id.unit_spinner);
        etValue.setInputType(android.text.InputType.TYPE_CLASS_NUMBER);
        ArrayAdapter<String> unitAdapter = new ArrayAdapter<>(requireContext(),
                android.R.layout.simple_spinner_item, new String[]{"uV", "mV", "V"});
        unitAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        unitSpinner.setAdapter(unitAdapter);
        String currentText = tvWaveRange.getText().toString();
        try {
            String numericPart = currentText.replaceAll("[^0-9]", "");
            if (!numericPart.isEmpty()) {
                etValue.setText(numericPart);
                String curUnit = currentText.replaceAll("[0-9]", "");
                int pos = unitAdapter.getPosition(curUnit);
                if (pos >= 0) unitSpinner.setSelection(pos);
            }
        } catch (Exception ignored) {}
        new AlertDialog.Builder(requireContext())
                .setTitle("设置波形步长")
                .setView(dialogView)
                .setPositiveButton("确定", (d, which) -> {
                    String str = etValue.getText().toString().trim();
                    if (str.isEmpty()) return;
                    try {
                        int stepInt = Integer.parseInt(str);
                        if (stepInt <= 0) return;
                        String unit = (String) unitSpinner.getSelectedItem();
                        applyWaveStep(stepInt, unit, currentLabelCount);
                        tvWaveRange.setText(stepInt + unit);
                        SettingsStore.setWaveStep(requireContext(), stepInt);
                        SettingsStore.setWaveStepUnit(requireContext(), unit);
                    } catch (NumberFormatException ignored) {}
                })
                .setNegativeButton("取消", null)
                .show();
    }

    private void applyXRange(float xMax) {
        for (WaveformView wv : waveformViews) { wv.setXMax(xMax); }
        tvXRange.setText(String.format("%.3f s", xMax));
    }

    private void showXRangeDialog() {
        View dialogView = LayoutInflater.from(requireContext()).inflate(R.layout.dialog_range_input, null);
        EditText etValue = dialogView.findViewById(R.id.et_value);
        Spinner unitSpinner = dialogView.findViewById(R.id.unit_spinner);
        unitSpinner.setVisibility(View.GONE);
        etValue.setInputType(android.text.InputType.TYPE_CLASS_NUMBER | android.text.InputType.TYPE_NUMBER_FLAG_DECIMAL);
        etValue.setHint("例如 2.048");
        String current = tvXRange.getText().toString().replace(" s", "");
        etValue.setText(current);
        new AlertDialog.Builder(requireContext())
                .setTitle("设置 X 轴时间范围 (秒)")
                .setView(dialogView)
                .setPositiveButton("确定", (d, which) -> {
                    String str = etValue.getText().toString().trim();
                    if (str.isEmpty()) return;
                    try {
                        float xMax = Float.parseFloat(str);
                        if (xMax <= 0) return;
                        applyXRange(xMax);
                        SettingsStore.setWaveXMax(requireContext(), xMax);
                    } catch (NumberFormatException ignored) {}
                })
                .setNegativeButton("取消", null)
                .show();
    }

    private void showSpectrumRangeDialog() {
        View dialogView = LayoutInflater.from(requireContext()).inflate(R.layout.dialog_range_input, null);
        EditText etValue = dialogView.findViewById(R.id.et_value);
        Spinner unitSpinner = dialogView.findViewById(R.id.unit_spinner);
        String[] units = {"uV", "mV", "V", "dBuV", "dBmV", "dBV"};
        ArrayAdapter<String> unitAdapter = new ArrayAdapter<>(requireContext(),
                android.R.layout.simple_spinner_item, units);
        unitAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        unitSpinner.setAdapter(unitAdapter);
        String currentText = tvSpectrumRange.getText().toString();
        try {
            String[] parts = currentText.split(" ");
            if (parts.length >= 2) {
                etValue.setText(parts[0]);
                int pos = java.util.Arrays.asList(units).indexOf(parts[1]);
                if (pos >= 0) unitSpinner.setSelection(pos);
            }
        } catch (Exception ignored) {}
        new AlertDialog.Builder(requireContext())
                .setTitle("设置频谱 Y 轴最大值")
                .setView(dialogView)
                .setPositiveButton("确定", (d, which) -> {
                    String str = etValue.getText().toString().trim();
                    if (str.isEmpty()) return;
                    try {
                        float value = Float.parseFloat(str);
                        if (value <= 0) return;
                        String unit = (String) unitSpinner.getSelectedItem();
                        for (SpectrumView sv : spectrumViews) { sv.setRange(value, unit); }
                        tvSpectrumRange.setText((Math.abs(value - Math.round(value)) < 0.001f ? String.format("%.0f", value) : String.format("%.3g", value)) + " " + unit);
                        SettingsStore.setSpecRange(requireContext(), value);
                        SettingsStore.setSpecUnit(requireContext(), unit);
                    } catch (NumberFormatException ignored) {}
                })
                .setNegativeButton("取消", null)
                .show();
    }

    private void showDisplayConfigDialog() {
        Context ctx = requireContext();
        int dp8 = (int) (8 * getResources().getDisplayMetrics().density);
        int dp4 = (int) (4 * getResources().getDisplayMetrics().density);
        int dp12 = (int) (12 * getResources().getDisplayMetrics().density);

        ScrollView scrollView = new ScrollView(ctx);
        LinearLayout rootLayout = new LinearLayout(ctx);
        rootLayout.setOrientation(LinearLayout.VERTICAL);
        rootLayout.setPadding(dp12, dp8, dp12, dp8);

        ArrayAdapter<String> chAdapter = new ArrayAdapter<>(ctx,
                android.R.layout.simple_spinner_item, CHANNEL_NAMES);
        chAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        ArrayAdapter<String> waveTypeAdapter = new ArrayAdapter<>(ctx,
                android.R.layout.simple_spinner_item, WAVE_TYPE_NAMES);
        waveTypeAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        ArrayAdapter<String> specTypeAdapter = new ArrayAdapter<>(ctx,
                android.R.layout.simple_spinner_item, SPEC_TYPE_NAMES);
        specTypeAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        String[] labels = {"A", "B"};
        Spinner[] spWaveCh = new Spinner[2];
        Spinner[] spWaveType = new Spinner[2];
        Spinner[] spSpecType = new Spinner[2];

        for (int i = 0; i < 2; i++) {
            TextView chTitle = new TextView(ctx);
            chTitle.setText("通道 " + labels[i]);
            chTitle.setTextSize(13);
            chTitle.setTypeface(null, android.graphics.Typeface.BOLD);
            chTitle.setPadding(0, dp4, 0, dp4);
            rootLayout.addView(chTitle);

            spWaveCh[i] = new Spinner(ctx);
            spWaveCh[i].setAdapter(chAdapter);
            spWaveCh[i].setSelection(waveCh[i]);
            addRow(rootLayout, "电极", spWaveCh[i]);

            spWaveType[i] = new Spinner(ctx);
            spWaveType[i].setAdapter(waveTypeAdapter);
            spWaveType[i].setSelection(waveType[i]);
            addRow(rootLayout, "波形", spWaveType[i]);

            spSpecType[i] = new Spinner(ctx);
            spSpecType[i].setAdapter(specTypeAdapter);
            spSpecType[i].setSelection(specType[i]);
            addRow(rootLayout, "频谱", spSpecType[i]);

            if (i == 0) {
                View divider = new View(ctx);
                divider.setBackgroundColor(ContextCompat.getColor(ctx, R.color.surface_overlay));
                LinearLayout.LayoutParams divParams = new LinearLayout.LayoutParams(
                        LinearLayout.LayoutParams.MATCH_PARENT, 1);
                divParams.setMargins(0, dp8, 0, dp8);
                divider.setLayoutParams(divParams);
                rootLayout.addView(divider);
            }
        }

        scrollView.addView(rootLayout);

        new AlertDialog.Builder(ctx)
                .setTitle("显示配置")
                .setView(scrollView)
                .setPositiveButton("确定", (d, which) -> {
                    for (int i = 0; i < 2; i++) {
                        waveCh[i] = spWaveCh[i].getSelectedItemPosition();
                        waveType[i] = spWaveType[i].getSelectedItemPosition();
                        specType[i] = spSpecType[i].getSelectedItemPosition();
                    }
                    sendDisplayConfig();
                    rebuildChannelViews();
                    saveDisplayConfigToStore();
                })
                .setNegativeButton("取消", null)
                .show();
    }

    private void addRow(LinearLayout parent, String label, Spinner spinner) {
        LinearLayout row = new LinearLayout(requireContext());
        row.setOrientation(LinearLayout.HORIZONTAL);
        row.setGravity(android.view.Gravity.CENTER_VERTICAL);
        TextView tv = new TextView(requireContext());
        tv.setText(label);
        tv.setTextSize(13);
        tv.setPadding(0, 0, (int) (8 * getResources().getDisplayMetrics().density), 0);
        LinearLayout.LayoutParams labelParams = new LinearLayout.LayoutParams(
                (int) (48 * getResources().getDisplayMetrics().density),
                LinearLayout.LayoutParams.WRAP_CONTENT);
        tv.setLayoutParams(labelParams);
        row.addView(tv);
        LinearLayout.LayoutParams spParams = new LinearLayout.LayoutParams(
                0, LinearLayout.LayoutParams.WRAP_CONTENT, 1f);
        spinner.setLayoutParams(spParams);
        row.addView(spinner);
        parent.addView(row);
    }

    private void addDualRow(LinearLayout parent, String label, Spinner spA, Spinner spB) {
        LinearLayout row = new LinearLayout(requireContext());
        row.setOrientation(LinearLayout.HORIZONTAL);
        row.setGravity(android.view.Gravity.CENTER_VERTICAL);
        TextView tv = new TextView(requireContext());
        tv.setText(label);
        tv.setTextSize(13);
        tv.setPadding(0, 0, (int) (8 * getResources().getDisplayMetrics().density), 0);
        LinearLayout.LayoutParams labelParams = new LinearLayout.LayoutParams(
                (int) (48 * getResources().getDisplayMetrics().density),
                LinearLayout.LayoutParams.WRAP_CONTENT);
        tv.setLayoutParams(labelParams);
        row.addView(tv);
        LinearLayout.LayoutParams spParams = new LinearLayout.LayoutParams(
                0, LinearLayout.LayoutParams.WRAP_CONTENT, 1f);
        spA.setLayoutParams(spParams);
        row.addView(spA);
        TextView slash = new TextView(requireContext());
        slash.setText(" / ");
        slash.setTextSize(13);
        row.addView(slash);
        LinearLayout.LayoutParams spBParams = new LinearLayout.LayoutParams(
                0, LinearLayout.LayoutParams.WRAP_CONTENT, 1f);
        spB.setLayoutParams(spBParams);
        row.addView(spB);
        parent.addView(row);
    }

    private void sendDisplayConfig() {
        String cmd = String.format(Locale.US,
                "DISPLAY_CFG,WAVE_CH_A=%d,WAVE_TYPE_A=%d,WAVE_CH_B=%d,WAVE_TYPE_B=%d,SPEC_TYPE_A=%d,SPEC_TYPE_B=%d",
                waveCh[0], waveType[0], waveCh[1], waveType[1], specType[0], specType[1]);
        TcpServerManager.getInstance().sendToDevice(cmd);
        Log.i("DisplayConfig", "Sent: " + cmd);
    }

    private void rebuildChannelsList() {
        channels.clear();
        int[] colors = {
            R.color.wave_ch0, R.color.wave_ch1, R.color.wave_filtered,
            R.color.focus_line, R.color.spectrum_bar, R.color.accent_info,
            R.color.accent_success, R.color.accent_warning
        };
        for (int i = 0; i < 2; i++) {
            int ch = waveCh[i];
            String name = (ch >= 0 && ch < CHANNEL_NAMES.length) ? CHANNEL_NAMES[ch] : "CH" + ch;
            channels.add(new ChannelConfig("CH " + ch, name, ChannelConfig.ChannelType.EEG,
                    colors[ch % colors.length], 0.5f, "uV", 0x04));
        }
    }

    private void rebuildChannelViews() {
        int[] savedModes = new int[waveformViews.size()];
        for (int i = 0; i < waveformViews.size(); i++) {
            savedModes[i] = waveformViews.get(i).getMode();
        }

        LinearLayout container = requireView().findViewById(R.id.channel_container);
        container.removeAllViews();
        waveformViews.clear();
        spectrumViews.clear();

        signalQualityViews.clear();

        rebuildChannelsList();
        buildChannelViews(container);

        for (int i = 0; i < waveformViews.size(); i++) {
            WaveformView wv = waveformViews.get(i);
            wv.clear();
            if (i < savedModes.length) {
                wv.setMode(savedModes[i]);
            }
        }

        int step = SettingsStore.getWaveStep(requireContext(), DEFAULT_STEP);
        String stepUnit = SettingsStore.getWaveStepUnit(requireContext(), DEFAULT_STEP_UNIT);
        int labelCount = SettingsStore.getWaveLabelCount(requireContext(), DEFAULT_LABEL_COUNT);
        float xMax = SettingsStore.getWaveXMax(requireContext(), DEFAULT_X_MAX);
        float specRange = SettingsStore.getSpecRange(requireContext(), 500f);
        String specUnit = SettingsStore.getSpecUnit(requireContext(), "uV");

        applyWaveStep(step, stepUnit, labelCount);
        applyXRange(xMax);
        for (SpectrumView sv : spectrumViews) {
            sv.setRange(specRange, specUnit);
        }

        for (int i = 0; i < 2; i++) {
            int sCh = waveCh[i];
            String specName = (sCh >= 0 && sCh < CHANNEL_NAMES.length) ? CHANNEL_NAMES[sCh] : "CH" + sCh;
            String typeName = (specType[i] >= 0 && specType[i] < SPEC_TYPE_NAMES.length) ? SPEC_TYPE_NAMES[specType[i]] : "";
            updateSpectrumLabel(container, i, specName + " " + typeName);
        }
    }

    private void updateSpectrumLabel(LinearLayout container, int rowIndex, String name) {
        if (rowIndex >= container.getChildCount()) return;
        View row = container.getChildAt(rowIndex);
        if (!(row instanceof LinearLayout)) return;
        LinearLayout rowLayout = (LinearLayout) row;
        for (int j = 0; j < rowLayout.getChildCount(); j++) {
            View child = rowLayout.getChildAt(j);
            if (child instanceof MaterialCardView) {
                MaterialCardView card = (MaterialCardView) child;
                if (card.getChildCount() > 0 && card.getChildAt(0) instanceof LinearLayout) {
                    LinearLayout inner = (LinearLayout) card.getChildAt(0);
                    for (int k = 0; k < inner.getChildCount(); k++) {
                        View innerChild = inner.getChildAt(k);
                        if (innerChild instanceof TextView && !(innerChild instanceof EditText)) {
                            TextView tv = (TextView) innerChild;
                            if (tv.getText().toString().startsWith("频谱")) {
                                tv.setText("频谱 " + name);
                                return;
                            }
                        }
                    }
                }
            }
        }
    }

    private void showWifiConfigDialog() {
        LinearLayout layout = new LinearLayout(requireContext());
        layout.setOrientation(LinearLayout.VERTICAL);
        int pad = (int) (12 * getResources().getDisplayMetrics().density);
        layout.setPadding(pad, pad, pad, pad);
        EditText etSsid = new EditText(requireContext());
        etSsid.setHint("SSID");
        layout.addView(etSsid);
        EditText etPassword = new EditText(requireContext());
        etPassword.setHint("密码");
        layout.addView(etPassword);
        new AlertDialog.Builder(requireContext())
                .setTitle("WiFi 配置")
                .setView(layout)
                .setPositiveButton("添加/修改", (d, which) -> {
                    String ssid = etSsid.getText().toString().trim();
                    String pwd = etPassword.getText().toString().trim();
                    if (ssid.isEmpty()) return;
                    TcpServerManager.getInstance().sendWifiAdd(ssid, pwd);
                    Toast.makeText(getContext(), "WiFi配置已发送: " + ssid, Toast.LENGTH_SHORT).show();
                })
                .setNeutralButton("删除", (d, which) -> {
                    String ssid = etSsid.getText().toString().trim();
                    if (ssid.isEmpty()) return;
                    TcpServerManager.getInstance().sendWifiDelete(ssid);
                    Toast.makeText(getContext(), "WiFi删除已发送: " + ssid, Toast.LENGTH_SHORT).show();
                })
                .setNegativeButton("取消", null)
                .show();
    }

    @Override
    public void onResume() {
        super.onResume();
        DataDispatcher.getInstance().removeListener(this);
        DataDispatcher.getInstance().addListener(this);
        Log.d("MonitorFragment", "onResume: listener refreshed, waveViews=" + waveformViews.size());
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        saveDisplayConfigToStore();
        DataDispatcher.getInstance().removeListener(this);
        Log.d("MonitorFragment", "onDestroyView: listener removed, config saved");
    }

    private static int waveTypeToCmd(int waveType) {
        switch (waveType) {
            case 1: return 0x10;
            case 2: return 0x11;
            default: return 0x04;
        }
    }

    @Override
    public void onWaveData(int cmd, int ch, float val) {
        if (isPaused) return;
        if (cmd == 0x04 || cmd == 0x10 || cmd == 0x11) {
            for (int i = 0; i < 2; i++) {
                if (ch == waveCh[i] && cmd == waveTypeToCmd(waveType[i])
                        && waveformViews.size() > i && waveformViews.get(i) != null)
                    waveformViews.get(i).addPoint(val);
            }
        }
    }

    @Override
    public void onEegFrame(EegFrame frame) {
        if (isPaused) return;
        if (waveformViews.isEmpty()) return;
        float[] ch = frame.getChannels();
        for (int i = 0; i < waveformViews.size() && i < ch.length; i++) {
            if (waveformViews.get(i) != null) waveformViews.get(i).addPoint(ch[i]);
        }
    }

    @Override
    public void onSpectrumData(int cmd, float[] mags) {
        if (isPaused) return;
        int ch = -1;
        if (cmd >= 0x20 && cmd <= 0x27) ch = cmd - 0x20;
        else if (cmd >= 0x30 && cmd <= 0x37) ch = cmd - 0x30;
        else if (cmd >= 0x40 && cmd <= 0x47) ch = cmd - 0x40;
        else if (cmd == 0x07 || cmd == 0x03) ch = 0;
        else if (cmd == 0x06 || cmd == 0x02) ch = 1;
        for (int i = 0; i < 2; i++) {
            if (ch == waveCh[i] && spectrumViews.size() > i) {
                spectrumViews.get(i).updateSpectrum(mags);
            }
        }
    }

    @Override
    public void onFocusData(float attn0, float attn1, float ema0, float ema1,
                            int trend, int instant) {
    }

    private void updateConnectionStatus() {
    }

    private String getWifiApIpAddress() {
        try {
            java.util.List<java.net.NetworkInterface> interfaces =
                    java.util.Collections.list(java.net.NetworkInterface.getNetworkInterfaces());
            for (java.net.NetworkInterface intf : interfaces) {
                java.util.List<java.net.InetAddress> addrs =
                        java.util.Collections.list(intf.getInetAddresses());
                for (java.net.InetAddress addr : addrs) {
                    if (!addr.isLoopbackAddress() && addr.getAddress().length == 4) {
                        String ip = addr.getHostAddress();
                        if (ip.startsWith("192.168.")) return ip;
                    }
                }
            }
        } catch (Exception ignored) {}
        return "192.168.43.1";
    }
}