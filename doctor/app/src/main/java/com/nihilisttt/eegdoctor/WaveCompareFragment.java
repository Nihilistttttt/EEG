package com.nihilisttt.eegdoctor;

import com.nihilisttt.eegdoctor.R;

import android.app.AlertDialog;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

/**
 * 波形对比页面：展示双通道原始波形与 IIR 时域滤波波形
 * 支持独立的 X 轴范围、Y 轴标签数量和步长控制
 */
public class WaveCompareFragment extends Fragment implements DataListener {

    // 默认参数（与主界面保持一致）
    private static final int DEFAULT_LABEL_COUNT = 7;
    private static final int DEFAULT_STEP = 500;          // 单位 uV
    private static final String DEFAULT_STEP_UNIT = "uV";
    private static final float DEFAULT_X_MAX = 2.048f;     // 秒

    // 波形视图
    private WaveformView rawCh0, rawCh1;
    private WaveformView filtCh0, filtCh1;

    // 控制 UI
    private TextView tvXRange;
    private TextView tvWaveLabelCount;
    private TextView tvWaveRange;

    // 当前参数
    private int currentLabelCount = DEFAULT_LABEL_COUNT;
    private int currentStep = DEFAULT_STEP;          // 数值部分
    private String currentStepUnit = DEFAULT_STEP_UNIT;
    private float currentXMax = DEFAULT_X_MAX;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_wave_compare, container, false);

        // 波形视图
        rawCh0 = root.findViewById(R.id.raw_wave_ch0);
        rawCh1 = root.findViewById(R.id.raw_wave_ch1);
        filtCh0 = root.findViewById(R.id.filt_wave_ch0);
        filtCh1 = root.findViewById(R.id.filt_wave_ch1);

        // 控制控件
        tvXRange = root.findViewById(R.id.tv_x_range);
        tvWaveLabelCount = root.findViewById(R.id.tv_wave_label_count);
        tvWaveRange = root.findViewById(R.id.tv_wave_range);

        // 设置点击监听
        tvXRange.setOnClickListener(v -> showXRangeDialog());
        tvWaveLabelCount.setOnClickListener(v -> showLabelCountDialog());
        tvWaveRange.setOnClickListener(v -> showWaveRangeDialog());

        // 应用持久化参数
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

        return root;
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

    // ==================== 波形控制方法 ====================

    /**
     * 统一设置所有波形视图的 X 轴时间范围
     */
    private void applyXRange(float xMax) {
        rawCh0.setXMax(xMax);
        rawCh1.setXMax(xMax);
        filtCh0.setXMax(xMax);
        filtCh1.setXMax(xMax);
        tvXRange.setText(String.format("%.1f s", xMax));
        currentXMax = xMax;
        SettingsStore.setWaveXMax(requireContext(), xMax);
    }

    /**
     * 根据步长数值、单位和标签数量，计算 Y 轴半跨度并应用到所有波形视图
     */
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
        // 设置电压单位（用于左侧标签）
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

    // ==================== 对话框设置 ====================

    /**
     * 修改 X 轴范围（秒）
     */
    private void showXRangeDialog() {
        View dialogView = LayoutInflater.from(requireContext())
                .inflate(R.layout.dialog_range_input, null);
        EditText etValue = dialogView.findViewById(R.id.et_value);
        Spinner unitSpinner = dialogView.findViewById(R.id.unit_spinner);
        unitSpinner.setVisibility(View.GONE);   // 隐藏单位选择

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

    /**
     * 修改 Y 轴标签数量（3,5,7,9）
     */
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

    /**
     * 修改波形步长（每格刻度对应的电压值）
     */
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

        // 预填当前值
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

    // ==================== DataListener 回调 ====================

    @Override
    public void onWaveData(int cmd, float ch0, float ch1) {
        if (cmd == 0x04) {
            if (rawCh0 != null) rawCh0.addPoint(ch0);
            if (rawCh1 != null) rawCh1.addPoint(ch1);
        } else if (cmd == 0x10) {
            if (filtCh0 != null) filtCh0.addPoint(ch0);
            if (filtCh1 != null) filtCh1.addPoint(ch1);
        }
    }

    @Override
    public void onSpectrumData(int cmd, float[] mags) {
        // 本页面不处理频谱数据
    }

    @Override
    public void onFocusData(float attn0, float attn1, float ema0, float ema1,
                            int trend, int instant) {
        // 本页面不处理专注度数据
    }
}