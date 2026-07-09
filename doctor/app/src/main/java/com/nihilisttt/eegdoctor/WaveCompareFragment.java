package com.nihilisttt.eegdoctor;

import com.nihilisttt.eegdoctor.R;

import android.app.AlertDialog;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

public class WaveCompareFragment extends Fragment implements DataListener {

    private static final int DEFAULT_LABEL_COUNT = 7;
    private static final int DEFAULT_STEP = 500;
    private static final String DEFAULT_STEP_UNIT = "uV";
    private static final float DEFAULT_X_MAX = 2.048f;

    private static final String[] CHANNEL_NAMES = {"F3", "F4", "CP3", "CP4", "C3", "C4", "P3", "P4"};

    private WaveformView rawCh0, rawCh1;
    private WaveformView filtCh0, filtCh1;
    private TextView labelRawA, labelFiltA, labelRawB, labelFiltB;
    private Spinner spinnerChA, spinnerChB;

    private TextView tvXRange;
    private TextView tvWaveLabelCount;
    private TextView tvWaveRange;

    private int currentLabelCount = DEFAULT_LABEL_COUNT;
    private int currentStep = DEFAULT_STEP;
    private String currentStepUnit = DEFAULT_STEP_UNIT;
    private float currentXMax = DEFAULT_X_MAX;

    private int chA = 2;
    private int chB = 4;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_wave_compare, container, false);

        rawCh0 = root.findViewById(R.id.raw_wave_ch0);
        rawCh1 = root.findViewById(R.id.raw_wave_ch1);
        filtCh0 = root.findViewById(R.id.filt_wave_ch0);
        filtCh1 = root.findViewById(R.id.filt_wave_ch1);

        labelRawA  = root.findViewById(R.id.label_raw_a);
        labelFiltA = root.findViewById(R.id.label_filt_a);
        labelRawB  = root.findViewById(R.id.label_raw_b);
        labelFiltB = root.findViewById(R.id.label_filt_b);

        tvXRange = root.findViewById(R.id.tv_x_range);
        tvWaveLabelCount = root.findViewById(R.id.tv_wave_label_count);
        tvWaveRange = root.findViewById(R.id.tv_wave_range);

        ArrayAdapter<String> chAdapter = new ArrayAdapter<>(requireContext(),
                android.R.layout.simple_spinner_item, CHANNEL_NAMES);
        chAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        spinnerChA = root.findViewById(R.id.spinner_ch_a);
        spinnerChB = root.findViewById(R.id.spinner_ch_b);
        spinnerChA.setAdapter(chAdapter);
        spinnerChB.setAdapter(chAdapter);
        spinnerChA.setSelection(chA);
        spinnerChB.setSelection(chB);

        spinnerChA.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                if (position != chA) {
                    chA = position;
                    updateLabels();
                }
            }
            @Override
            public void onNothingSelected(AdapterView<?> parent) {}
        });

        spinnerChB.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                if (position != chB) {
                    chB = position;
                    updateLabels();
                }
            }
            @Override
            public void onNothingSelected(AdapterView<?> parent) {}
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

    private void updateLabels() {
        String nameA = CHANNEL_NAMES[chA];
        String nameB = CHANNEL_NAMES[chB];
        if (labelRawA != null)  labelRawA.setText(nameA + " 原始波形");
        if (labelFiltA != null) labelFiltA.setText(nameA + " 时域滤波波形");
        if (labelRawB != null)  labelRawB.setText(nameB + " 原始波形");
        if (labelFiltB != null) labelFiltB.setText(nameB + " 时域滤波波形");
    }

    @Override
    public void onResume() {
        super.onResume();
        DataDispatcher.getInstance().removeListener(this);
        DataDispatcher.getInstance().addListener(this);
        Log.d("WaveCompare", "onResume: listener refreshed");
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        DataDispatcher.getInstance().removeListener(this);
        Log.d("WaveCompare", "onDestroyView: listener removed");
    }

    private void applyXRange(float xMax) {
        rawCh0.setXMax(xMax);
        rawCh1.setXMax(xMax);
        filtCh0.setXMax(xMax);
        filtCh1.setXMax(xMax);
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
        rawCh0.setYRange(halfRange);
        rawCh1.setYRange(halfRange);
        filtCh0.setYRange(halfRange);
        filtCh1.setYRange(halfRange);
        rawCh0.setUnit(unit);
        rawCh1.setUnit(unit);
        filtCh0.setUnit(unit);
        filtCh1.setUnit(unit);

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
                    try {
                        float xMax = Float.parseFloat(str);
                        if (xMax <= 0) return;
                        applyXRange(xMax);
                    } catch (NumberFormatException ignored) {}
                })
                .setNegativeButton("取消", null)
                .show();
    }

    private void showLabelCountDialog() {
        final String[] labels = {"3", "5", "7", "9"};
        int currentIndex = 0;
        for (int i = 0; i < labels.length; i++) {
            if (labels[i].equals(String.valueOf(currentLabelCount))) {
                currentIndex = i;
                break;
            }
        }
        new AlertDialog.Builder(requireContext())
                .setTitle("选择 Y 轴标签数量")
                .setSingleChoiceItems(labels, currentIndex, (dialog, which) -> {
                    int newCount = Integer.parseInt(labels[which]);
                    applyWaveStep(currentStep, currentStepUnit, newCount);
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
        ArrayAdapter<String> unitAdapter = new ArrayAdapter<>(requireContext(),
                android.R.layout.simple_spinner_item, new String[]{"uV", "mV", "V"});
        unitAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        unitSpinner.setAdapter(unitAdapter);

        etValue.setText(String.valueOf(currentStep));
        int pos = unitAdapter.getPosition(currentStepUnit);
        if (pos >= 0) unitSpinner.setSelection(pos);

        new AlertDialog.Builder(requireContext())
                .setTitle("设置波形步长（每格刻度值）")
                .setMessage("输入正整数步长，例如：10、5、20")
                .setView(dialogView)
                .setPositiveButton("确定", (d, which) -> {
                    String str = etValue.getText().toString().trim();
                    if (str.isEmpty()) return;
                    try {
                        int stepInt = Integer.parseInt(str);
                        if (stepInt <= 0) return;
                        String unit = (String) unitSpinner.getSelectedItem();
                        applyWaveStep(stepInt, unit, currentLabelCount);
                    } catch (NumberFormatException ignored) {}
                })
                .setNegativeButton("取消", null)
                .show();
    }

    @Override
    public void onWaveData(int cmd, float ch0, float ch1) {}

    @Override
    public void onWaveData8ch(int cmd, float[] ch) {
        if (cmd == 0x20) {
            if (chA < ch.length && rawCh0 != null) rawCh0.addPoint(ch[chA]);
            if (chB < ch.length && rawCh1 != null) rawCh1.addPoint(ch[chB]);
        } else if (cmd == 0x21) {
            if (chA < ch.length && filtCh0 != null) filtCh0.addPoint(ch[chA]);
            if (chB < ch.length && filtCh1 != null) filtCh1.addPoint(ch[chB]);
        }
    }

    @Override
    public void onSpectrumData(int cmd, float[] mags) {}

    @Override
    public void onFocusData(float attn0, float attn1, float ema0, float ema1,
                            int trend, int instant) {}
}
