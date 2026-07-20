package com.nihilisttt.eegdoctor;

import com.nihilisttt.eegdoctor.R;

import android.app.AlertDialog;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;

import java.util.Locale;

public class FocusHistoryFragment extends Fragment implements DataListener {

    private FocusChartView focusChart;
    private TextView tvFocusXRange;

    private TextView btnToggleFocus;
    private TextView btnToggleRelax;

    private static final int DEFAULT_X_POINTS = 500;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_focus_history, container, false);
        focusChart = root.findViewById(R.id.focus_chart);
        tvFocusXRange = root.findViewById(R.id.tv_focus_x_range);

        btnToggleFocus = root.findViewById(R.id.btn_toggle_focus);
        btnToggleRelax = root.findViewById(R.id.btn_toggle_relax);

        View tvFocusLabelCount = root.findViewById(R.id.tv_focus_label_count);
        View tvFocusRange = root.findViewById(R.id.tv_focus_range);
        if (tvFocusLabelCount != null) tvFocusLabelCount.setVisibility(View.GONE);
        if (tvFocusRange != null) tvFocusRange.setVisibility(View.GONE);

        btnToggleFocus.setOnClickListener(v -> {
            boolean visible = !focusChart.isFocusVisible();
            focusChart.setFocusVisible(visible);
            updateToggleButton(btnToggleFocus, visible, R.color.focus_line);
        });
        btnToggleRelax.setOnClickListener(v -> {
            boolean visible = !focusChart.isRelaxVisible();
            focusChart.setRelaxVisible(visible);
            updateToggleButton(btnToggleRelax, visible, R.color.accent_info);
        });

        int xPoints = SettingsStore.getFocusXPoints(requireContext(), DEFAULT_X_POINTS);
        tvFocusXRange.setText(String.format("%d 点", xPoints));
        tvFocusXRange.setOnClickListener(v -> showXRangeDialog());

        applyXRange(xPoints);

        return root;
    }

    private void updateToggleButton(TextView btn, boolean visible, int activeColorRes) {
        if (visible) {
            btn.setTextColor(ContextCompat.getColor(requireContext(), activeColorRes));
            btn.setAlpha(1.0f);
        } else {
            btn.setTextColor(ContextCompat.getColor(requireContext(), R.color.text_secondary));
            btn.setAlpha(0.4f);
        }
    }

    private void applyXRange(int points) {
        focusChart.setXMax(points / 10.0f);
        tvFocusXRange.setText(String.format("%d 点", points));
        SettingsStore.setFocusXPoints(requireContext(), points);
    }

    private void showXRangeDialog() {
        View dialogView = LayoutInflater.from(requireContext()).inflate(R.layout.dialog_range_input, null);
        EditText etValue = dialogView.findViewById(R.id.et_value);
        Spinner unitSpinner = dialogView.findViewById(R.id.unit_spinner);
        unitSpinner.setVisibility(View.GONE);
        etValue.setInputType(android.text.InputType.TYPE_CLASS_NUMBER);
        etValue.setHint("默认 " + DEFAULT_X_POINTS);

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

    @Override
    public void onResume() {
        super.onResume();
        DataDispatcher.getInstance().removeListener(this);
        DataDispatcher.getInstance().addListener(this);
        TcpServerManager.getInstance().sendToDevice(EegChannels.buildAllNoneDisplayConfig());
        if (focusChart != null) focusChart.loadFromStore();
        Log.d("FocusHistory", "onResume: listener refreshed");
    }

    @Override
    public void onPause() {
        super.onPause();
        DataDispatcher.getInstance().removeListener(this);
        Log.d("FocusHistory", "onPause: listener removed");
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        DataDispatcher.getInstance().removeListener(this);
        Log.d("FocusHistory", "onDestroyView: listener removed");
    }

    @Override
    public void onFocusData(float attn0, float attn1, float ema0, float ema1,
                            int trend, int instant) {
        if (focusChart != null) focusChart.addPoint(ema0, ema1);
    }

    @Override
    public void onWaveData(int cmd, int ch, float val) {}

    @Override
    public void onSpectrumData(int cmd, float[] mags) {}
}
