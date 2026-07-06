package com.nihilisttt.eegdoctor;

import com.nihilisttt.eegdoctor.R;

import android.app.AlertDialog;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import java.util.Locale;

public class FocusHistoryFragment extends Fragment implements DataListener {

    private FocusChartView focusChart;
    private TextView tvFocusLabelCount, tvFocusRange, tvFocusXRange;
    private TextView tvAttnInfo;

    private static final int DEFAULT_LABEL_COUNT = 5;
    private static final float DEFAULT_Y_RANGE = 1.0f;
    private static final int DEFAULT_X_POINTS = 500;   // X轴默认点数

    private int currentLabelCount = DEFAULT_LABEL_COUNT;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_focus_history, container, false);
        focusChart = root.findViewById(R.id.focus_chart);
        tvFocusLabelCount = root.findViewById(R.id.tv_focus_label_count);
        tvFocusRange = root.findViewById(R.id.tv_focus_range);
        tvFocusXRange = root.findViewById(R.id.tv_focus_x_range);
        tvAttnInfo = root.findViewById(R.id.tv_attn_info);

        tvFocusLabelCount.setText(String.valueOf(DEFAULT_LABEL_COUNT));
        tvFocusRange.setText(String.format("%.2f", DEFAULT_Y_RANGE));
        tvFocusXRange.setText(String.format("%d 点", DEFAULT_X_POINTS));

        tvFocusLabelCount.setOnClickListener(v -> showLabelCountDialog());
        tvFocusRange.setOnClickListener(v -> showYRangeDialog());
        tvFocusXRange.setOnClickListener(v -> showXRangeDialog());

        applyYRange(DEFAULT_Y_RANGE, DEFAULT_LABEL_COUNT);
        applyXRange(DEFAULT_X_POINTS);

        return root;
    }

    private void applyYRange(float halfRange, int labelCount) {
        focusChart.setYRange(halfRange);
        focusChart.setLabelCount(labelCount);
        tvFocusRange.setText(String.format("%.2f", halfRange));
    }

    private void applyXRange(int points) {
        focusChart.setXMaxPoints(points);
        tvFocusXRange.setText(String.format("%d 点", points));
    }

    private void showYRangeDialog() {
        View dialogView = LayoutInflater.from(requireContext()).inflate(R.layout.dialog_range_input, null);
        EditText etValue = dialogView.findViewById(R.id.et_value);
        Spinner unitSpinner = dialogView.findViewById(R.id.unit_spinner);
        unitSpinner.setVisibility(View.GONE);
        etValue.setInputType(android.text.InputType.TYPE_CLASS_NUMBER | android.text.InputType.TYPE_NUMBER_FLAG_DECIMAL);
        etValue.setHint("如 1.5");

        String current = tvFocusRange.getText().toString().trim();
        etValue.setText(current);

        new AlertDialog.Builder(requireContext())
                .setTitle("设置 Y 轴半跨度")
                .setView(dialogView)
                .setPositiveButton("确定", (d, which) -> {
                    String str = etValue.getText().toString().trim();
                    if (str.isEmpty()) return;
                    try {
                        float halfRange = Float.parseFloat(str);
                        if (halfRange <= 0) return;
                        applyYRange(halfRange, currentLabelCount);
                    } catch (NumberFormatException ignored) {}
                })
                .setNegativeButton("取消", null)
                .show();
    }

    private void showXRangeDialog() {
        View dialogView = LayoutInflater.from(requireContext()).inflate(R.layout.dialog_range_input, null);
        EditText etValue = dialogView.findViewById(R.id.et_value);
        Spinner unitSpinner = dialogView.findViewById(R.id.unit_spinner);
        unitSpinner.setVisibility(View.GONE);
        etValue.setInputType(android.text.InputType.TYPE_CLASS_NUMBER);
        etValue.setHint("如 300");

        String current = tvFocusXRange.getText().toString().replace(" 点", "");
        etValue.setText(current);

        new AlertDialog.Builder(requireContext())
                .setTitle("设置显示点数")
                .setView(dialogView)
                .setPositiveButton("确定", (d, which) -> {
                    String str = etValue.getText().toString().trim();
                    if (str.isEmpty()) return;
                    try {
                        int points = Integer.parseInt(str);
                        if (points <= 0) return;
                        applyXRange(points);
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
                .setTitle("选择 Y 轴刻度数量")
                .setSingleChoiceItems(labels, currentIndex, (dialog, which) -> {
                    currentLabelCount = Integer.parseInt(labels[which]);
                    tvFocusLabelCount.setText(String.valueOf(currentLabelCount));
                    // 立即更新图表刻度
                    focusChart.setLabelCount(currentLabelCount);
                    // 重新应用Y范围（保持范围不变）
                    applyYRange(focusChart.getYRange(), currentLabelCount);
                    dialog.dismiss();
                })
                .setNegativeButton("取消", null)
                .show();
    }

    @Override
    public void onResume() {
        super.onResume();
        DataDispatcher.getInstance().addListener(this);
    }

    @Override
    public void onPause() {
        super.onPause();
        DataDispatcher.getInstance().removeListener(this);
    }

    @Override
    public void onFocusData(float attn0, float attn1, float ema0, float ema1,
                            int trend, int instant) {
        focusChart.addPoint(ema0);
        tvAttnInfo.setText(String.format(Locale.getDefault(),
                "Attn0: %.3f | Attn1: %.3f", attn0, attn1));
    }

    @Override
    public void onWaveData(int cmd, float ch0, float ch1) {}

    @Override
    public void onSpectrumData(int cmd, float[] mags) {}
}