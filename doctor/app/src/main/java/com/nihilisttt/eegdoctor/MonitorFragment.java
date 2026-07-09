package com.nihilisttt.eegdoctor;

import com.nihilisttt.eegdoctor.R;

import android.app.AlertDialog;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.AdapterView;

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
    private static final String[] CHANNEL_NAMES = {"F3", "F4", "CP3", "CP4", "C3", "C4", "P3", "P4"};

    private final List<ChannelConfig> channels;
    private WaveformView waveformViewA;
    private WaveformView waveformViewB;
    private SpectrumView spectrumViewA;
    private SpectrumView spectrumViewB;
    private TextView labelChA;
    private TextView labelChB;
    private TextView specLabelA;
    private TextView specLabelB;

    private View statusIndicator;
    private TextView tvStatus;
    private TextView tvWaveLabelCount;
    private TextView tvWaveRange;
    private TextView tvXRange;
    private TextView tvSpectrumRange;
    private TextView tvSpectrumMode;
    private TextView btnPauseResume;
    private Spinner spinnerChA;
    private Spinner spinnerChB;
    private boolean isPaused = false;
    private int currentLabelCount = DEFAULT_LABEL_COUNT;
    private boolean deviceConnected = false;
    private boolean patientConnected = false;

    private int selectedChA = 2;
    private int selectedChB = 4;
    private int spectrumMode = 0;

    private static final int SPECTRUM_MODE_RAW = 0;
    private static final int SPECTRUM_MODE_FILT = 1;

    private static final int[] RAW_SPECTRUM_CMDS = {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
    };
    private static final int[] FILT_SPECTRUM_CMDS = {
        0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F
    };

    private static final int[] CHANNEL_COLORS = {
        R.color.wave_ch0, R.color.wave_ch1, R.color.wave_filtered,
        R.color.focus_line, R.color.spectrum_bar, R.color.accent_info,
        R.color.accent_success, R.color.accent_warning
    };

    public MonitorFragment() {
        this.channels = ChannelConfig.getDefault8Channel();
    }

    public MonitorFragment(List<ChannelConfig> channels) {
        this.channels = channels != null ? channels : ChannelConfig.getDefault8Channel();
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_monitor, container, false);

        LinearLayout channelContainer = root.findViewById(R.id.channel_container);
        buildChannelViews(channelContainer);

        statusIndicator = root.findViewById(R.id.status_indicator);
        tvStatus = root.findViewById(R.id.tv_status);
        tvWaveLabelCount = root.findViewById(R.id.tv_wave_label_count);
        tvWaveRange = root.findViewById(R.id.tv_wave_range);
        tvXRange = root.findViewById(R.id.tv_x_range);
        tvSpectrumRange = root.findViewById(R.id.tv_spectrum_range);
        tvSpectrumMode = root.findViewById(R.id.tv_spectrum_mode);
        btnPauseResume = root.findViewById(R.id.btn_pause_resume);

        ArrayAdapter<String> chAdapter = new ArrayAdapter<>(requireContext(),
                android.R.layout.simple_spinner_item, CHANNEL_NAMES);
        chAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        spinnerChA = root.findViewById(R.id.spinner_ch_a);
        spinnerChB = root.findViewById(R.id.spinner_ch_b);
        spinnerChA.setAdapter(chAdapter);
        spinnerChB.setAdapter(chAdapter);
        spinnerChA.setSelection(selectedChA);
        spinnerChB.setSelection(selectedChB);

        spinnerChA.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                if (position != selectedChA) {
                    selectedChA = position;
                    updateChannelLabels();
                }
            }
            @Override
            public void onNothingSelected(AdapterView<?> parent) {}
        });

        spinnerChB.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                if (position != selectedChB) {
                    selectedChB = position;
                    updateChannelLabels();
                }
            }
            @Override
            public void onNothingSelected(AdapterView<?> parent) {}
        });

        tvSpectrumMode.setOnClickListener(v -> {
            spectrumMode = (spectrumMode + 1) % 2;
            tvSpectrumMode.setText(spectrumMode == SPECTRUM_MODE_RAW ? "原始" : "时域滤波");
        });

        btnPauseResume.setOnClickListener(v -> {
            isPaused = !isPaused;
            btnPauseResume.setText(isPaused ? "继续" : "暂停");
            updateConnectionStatus();
        });

        root.findViewById(R.id.btn_set_ip).setOnClickListener(v -> showWifiConfigDialog());
        root.findViewById(R.id.btn_training).setOnClickListener(v -> {
            if (getActivity() instanceof MainActivity) {
                ((MainActivity) requireActivity()).switchToPage(4);
            }
        });

        TcpServerManager.getInstance().addConnectionListener(new TcpServerManager.ConnectionListener() {
            @Override
            public void onDeviceConnected(boolean connected) {
                deviceConnected = connected;
                if (getActivity() != null) getActivity().runOnUiThread(() -> updateConnectionStatus());
            }
            @Override
            public void onPatientConnected(boolean connected) {
                patientConnected = connected;
                if (getActivity() != null) getActivity().runOnUiThread(() -> updateConnectionStatus());
            }
        });

        setupRangeSelectors();
        return root;
    }

    private void updateChannelLabels() {
        if (labelChA != null) labelChA.setText(CHANNEL_NAMES[selectedChA]);
        if (labelChB != null) labelChB.setText(CHANNEL_NAMES[selectedChB]);
        if (specLabelA != null) specLabelA.setText("频谱 " + CHANNEL_NAMES[selectedChA]);
        if (specLabelB != null) specLabelB.setText("频谱 " + CHANNEL_NAMES[selectedChB]);
        if (waveformViewA != null) {
            waveformViewA.setWaveColor(ContextCompat.getColor(requireContext(), CHANNEL_COLORS[selectedChA]));
        }
        if (waveformViewB != null) {
            waveformViewB.setWaveColor(ContextCompat.getColor(requireContext(), CHANNEL_COLORS[selectedChB]));
        }
    }

    private void buildChannelViews(LinearLayout container) {
        Context ctx = requireContext();

        LinearLayout row = new LinearLayout(ctx);
        row.setOrientation(LinearLayout.HORIZONTAL);
        row.setWeightSum(4f);

        for (int i = 0; i < 2; i++) {
            int chIdx = (i == 0) ? selectedChA : selectedChB;

            MaterialCardView waveCard = new MaterialCardView(ctx);
            LinearLayout.LayoutParams waveCardParams = new LinearLayout.LayoutParams(
                    0, LinearLayout.LayoutParams.MATCH_PARENT, 3f);
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
            colorDot.setBackgroundColor(ContextCompat.getColor(ctx, CHANNEL_COLORS[chIdx]));
            headerRow.addView(colorDot);

            TextView labelView = new TextView(ctx);
            labelView.setText(CHANNEL_NAMES[chIdx]);
            labelView.setTextColor(ContextCompat.getColor(ctx, R.color.text_secondary));
            labelView.setTextSize(10);
            LinearLayout.LayoutParams labelParams = new LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.WRAP_CONTENT, 1f);
            labelView.setLayoutParams(labelParams);
            headerRow.addView(labelView);

            if (i == 0) labelChA = labelView; else labelChB = labelView;

            waveInner.addView(headerRow);

            WaveformView wv = new WaveformView(ctx, null);
            wv.setId(View.generateViewId());
            LinearLayout.LayoutParams wvParams = new LinearLayout.LayoutParams(
                    LinearLayout.LayoutParams.MATCH_PARENT, 0, 1f);
            wv.setLayoutParams(wvParams);
            wv.setWaveColor(ContextCompat.getColor(ctx, CHANNEL_COLORS[chIdx]));
            wv.setYRange(0.5f);
            wv.setUnit("uV");
            waveInner.addView(wv);

            if (i == 0) waveformViewA = wv; else waveformViewB = wv;

            waveCard.addView(waveInner);
            row.addView(waveCard);

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
            specLabel.setText("频谱 " + CHANNEL_NAMES[chIdx]);
            specLabel.setTextColor(ContextCompat.getColor(ctx, R.color.text_secondary));
            specLabel.setTextSize(10);
            specLabel.setBackgroundColor(ContextCompat.getColor(ctx, R.color.surface_mid));
            int slp = (int) (8 * getResources().getDisplayMetrics().density);
            specLabel.setPadding(slp, 0, 0, 0);
            specInner.addView(specLabel);

            if (i == 0) specLabelA = specLabel; else specLabelB = specLabel;

            SpectrumView sv = new SpectrumView(ctx, null);
            sv.setId(View.generateViewId());
            LinearLayout.LayoutParams svParams = new LinearLayout.LayoutParams(
                    LinearLayout.LayoutParams.MATCH_PARENT, 0, 1f);
            sv.setLayoutParams(svParams);
            specInner.addView(sv);

            if (i == 0) spectrumViewA = sv; else spectrumViewB = sv;

            specCard.addView(specInner);
            row.addView(specCard);
        }

        LinearLayout.LayoutParams rowParams = new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.MATCH_PARENT);
        container.addView(row, rowParams);
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
        if (spectrumViewA != null) spectrumViewA.setRange(specRange, specUnit);
        if (spectrumViewB != null) spectrumViewB.setRange(specRange, specUnit);
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
        if (waveformViewA != null) { waveformViewA.setYRange(halfRange); waveformViewA.setUnit(unit); }
        if (waveformViewB != null) { waveformViewB.setYRange(halfRange); waveformViewB.setUnit(unit); }
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
        if (waveformViewA != null) waveformViewA.setXMax(xMax);
        if (waveformViewB != null) waveformViewB.setXMax(xMax);
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
                        if (spectrumViewA != null) spectrumViewA.setRange(value, unit);
                        if (spectrumViewB != null) spectrumViewB.setRange(value, unit);
                        tvSpectrumRange.setText((Math.abs(value - Math.round(value)) < 0.001f ? String.format("%.0f", value) : String.format("%.3g", value)) + " " + unit);
                        SettingsStore.setSpecRange(requireContext(), value);
                        SettingsStore.setSpecUnit(requireContext(), unit);
                    } catch (NumberFormatException ignored) {}
                })
                .setNegativeButton("取消", null)
                .show();
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
        Log.d("MonitorFragment", "onResume: listener refreshed");
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        DataDispatcher.getInstance().removeListener(this);
        Log.d("MonitorFragment", "onDestroyView: listener removed");
    }

    @Override
    public void onWaveData(int cmd, float ch0, float ch1) {}

    @Override
    public void onWaveData8ch(int cmd, float[] ch) {
        if (isPaused) return;
        if (cmd != 0x20 && cmd != 0x21) return;
        if (selectedChA < ch.length && waveformViewA != null)
            waveformViewA.addPoint(ch[selectedChA]);
        if (selectedChB < ch.length && waveformViewB != null)
            waveformViewB.addPoint(ch[selectedChB]);
    }

    @Override
    public void onEegFrame(EegFrame frame) {
        if (isPaused) return;
        float[] ch = frame.getChannels();
        if (selectedChA < ch.length && waveformViewA != null)
            waveformViewA.addPoint(ch[selectedChA]);
        if (selectedChB < ch.length && waveformViewB != null)
            waveformViewB.addPoint(ch[selectedChB]);
    }

    @Override
    public void onSpectrumData(int cmd, float[] mags) {
        if (isPaused) return;
        int[] cmds = (spectrumMode == SPECTRUM_MODE_RAW) ? RAW_SPECTRUM_CMDS : FILT_SPECTRUM_CMDS;
        if (cmd == cmds[selectedChA] && spectrumViewA != null) {
            spectrumViewA.updateSpectrum(mags);
        } else if (cmd == cmds[selectedChB] && spectrumViewB != null) {
            spectrumViewB.updateSpectrum(mags);
        }
    }

    @Override
    public void onFocusData(float attn0, float attn1, float ema0, float ema1,
                            int trend, int instant) {
    }

    private void updateConnectionStatus() {
        StringBuilder sb = new StringBuilder();
        if (!deviceConnected) sb.append("采集端未连接");
        if (!patientConnected) {
            if (sb.length() > 0) sb.append("  ");
            sb.append("患者端未连接");
        }
        if (deviceConnected && patientConnected) sb.append("全部已连接");
        if (isPaused) {
            if (sb.length() > 0) sb.append("  ");
            sb.append("(已暂停)");
        }
        tvStatus.setText(sb.toString());
        int dotColor;
        if (deviceConnected && patientConnected) {
            dotColor = ContextCompat.getColor(requireContext(), R.color.accent_success);
        } else if (deviceConnected || patientConnected) {
            dotColor = ContextCompat.getColor(requireContext(), R.color.accent_warning);
        } else {
            dotColor = ContextCompat.getColor(requireContext(), R.color.accent_error);
        }
        statusIndicator.setBackgroundColor(dotColor);
    }
}
