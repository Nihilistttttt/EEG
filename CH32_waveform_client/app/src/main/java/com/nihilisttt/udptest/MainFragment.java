package com.nihilisttt.udptest;

import android.app.AlertDialog;
import android.graphics.Color;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;

import java.util.Locale;

public class MainFragment extends Fragment implements DataListener {
    // 默认步长：500 uV
    private static final int DEFAULT_STEP = 500;
    private static final String DEFAULT_STEP_UNIT = "uV";
    private static final int DEFAULT_LABEL_COUNT = 7;   // 标签数量
    private static final float DEFAULT_X_MAX = 2.048f;  // 默认 X 轴范围（秒）
    private static final float DEFAULT_SPEC_RANGE_VOLT = 0.0005f;   // 500μV

    // 视图
    private WaveformView waveCh0, waveCh1;
    private SpectrumView spectrumCh0, spectrumCh1;
    private TextView attnCh0, attnCh1;
    private TextView tvStatus;
    private TextView tvInstantState;
    private TextView tvTrendState;
    private Button btnCarGame;
    private Button btnSetIp;
    private TextView tvWaveLabelCount;   // 标签数量显示
    private TextView tvWaveRange;        // 波形步长显示
    private TextView tvXRange;           // X轴范围显示
    private TextView tvSpectrumRange;    // 频谱量程显示
    private Button btnPauseResume;
    private boolean isPaused = false;  // 暂停标志

    private int currentLabelCount = DEFAULT_LABEL_COUNT;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_main, container, false);
        waveCh0 = root.findViewById(R.id.wave_ch0);
        waveCh1 = root.findViewById(R.id.wave_ch1);
        spectrumCh0 = root.findViewById(R.id.spectrum_ch0);
        spectrumCh1 = root.findViewById(R.id.spectrum_ch1);
        attnCh0 = root.findViewById(R.id.attn_ch0);
        attnCh1 = root.findViewById(R.id.attn_ch1);
        tvStatus = root.findViewById(R.id.tv_status);
        tvInstantState = root.findViewById(R.id.tv_instant_state);
        tvTrendState = root.findViewById(R.id.tv_trend_state);
        btnCarGame = root.findViewById(R.id.btn_car_game);
        tvWaveLabelCount = root.findViewById(R.id.tv_wave_label_count);
        tvWaveRange = root.findViewById(R.id.tv_wave_range);
        tvXRange = root.findViewById(R.id.tv_x_range);
        tvSpectrumRange = root.findViewById(R.id.tv_spectrum_range);

        btnCarGame.setOnClickListener(v -> {
            if (getActivity() instanceof MainActivity) {
                ((MainActivity) requireActivity()).switchToPage(3);
            }
        });

        String ip = getWifiApIpAddress();

        btnPauseResume = root.findViewById(R.id.btn_pause_resume);
        btnPauseResume.setOnClickListener(v -> {
            isPaused = !isPaused;
            btnPauseResume.setText(isPaused ? "继续" : "暂停");
            // 可选：在暂停时显示提示
            if (isPaused) {
                // 例如在状态栏显示"已暂停"
                tvStatus.setText("本机IP: " + ip + "  端口: 41002  (已暂停)");
            } else {
                tvStatus.setText("本机IP: " + ip + "  端口: 41002");
            }
        });

        btnSetIp = root.findViewById(R.id.btn_set_ip);
        btnSetIp.setOnClickListener(v -> showUdpIpDialog());
        setupRangeSelectors();
        return root;
    }
    private void showUdpIpDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(requireContext());
        builder.setTitle("设置 Python 上位机 IP");
        final EditText input = new EditText(requireContext());
        input.setHint("例如 192.168.1.100");
        builder.setView(input);
        builder.setPositiveButton("确定", (dialog, which) -> {
            String ip = input.getText().toString().trim();
            if (!ip.isEmpty()) {
                TcpServerManager.getInstance().configUdpForPython(ip);
                Toast.makeText(getContext(), "UDP 目标 IP 已设置为 " + ip, Toast.LENGTH_SHORT).show();
            }
        });
        builder.setNegativeButton("取消", null);
        builder.show();
    }
    private void setupRangeSelectors() {
        // 标签数量点击
        tvWaveLabelCount.setOnClickListener(v -> showLabelCountDialog());
        // 波形步长点击
        tvWaveRange.setOnClickListener(v -> showWaveRangeDialog());
        // X轴范围点击
        tvXRange.setOnClickListener(v -> showXRangeDialog());
        // 频谱量程点击
        tvSpectrumRange.setOnClickListener(v -> showSpectrumRangeDialog());

        // 初始化默认值
        tvWaveLabelCount.setText(String.valueOf(DEFAULT_LABEL_COUNT));
        tvWaveRange.setText(DEFAULT_STEP + DEFAULT_STEP_UNIT);
        tvXRange.setText(String.format("%.3f s", DEFAULT_X_MAX));
        tvSpectrumRange.setText("500 uV");

        applyWaveStep(DEFAULT_STEP, DEFAULT_STEP_UNIT, DEFAULT_LABEL_COUNT);
        applyXRange(DEFAULT_X_MAX);
        spectrumCh0.setRange(500f, "uV");
        spectrumCh1.setRange(500f, "uV");
    }

    // ==================== 波形：标签数量 ====================
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
                    currentLabelCount = newCount;
                    tvWaveLabelCount.setText(String.valueOf(newCount));
                    // 重新应用当前步长
                    String stepText = tvWaveRange.getText().toString();
                    String numericPart = stepText.replaceAll("[^0-9]", "");
                    String unitPart = stepText.replaceAll("[0-9]", "");
                    if (!numericPart.isEmpty()) {
                        int step = Integer.parseInt(numericPart);
                        applyWaveStep(step, unitPart, newCount);
                    }
                    dialog.dismiss();
                })
                .setNegativeButton("取消", null)
                .show();
    }

    // ==================== 波形：步长 ====================
    private void applyWaveStep(int step, String unit, int labelCount) {
        float stepVolt;
        switch (unit) {
            case "uV": stepVolt = step / 1_000_000f; break;
            case "mV": stepVolt = step / 1000f; break;
            default:   stepVolt = step; break;
        }
        float halfRange = stepVolt * ((labelCount - 1) / 2.0f);
        waveCh0.setYRange(halfRange);
        waveCh1.setYRange(halfRange);
        waveCh0.setUnit(unit);
        waveCh1.setUnit(unit);
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
                        tvWaveRange.setText(stepInt + unit);
                    } catch (NumberFormatException ignored) {}
                })
                .setNegativeButton("取消", null)
                .show();
    }

    // ==================== X轴范围 ====================
    private void applyXRange(float xMax) {
        waveCh0.setXMax(xMax);
        waveCh1.setXMax(xMax);
        tvXRange.setText(String.format("%.3f s", xMax));
    }

    private void showXRangeDialog() {
        View dialogView = LayoutInflater.from(requireContext()).inflate(R.layout.dialog_range_input, null);
        EditText etValue = dialogView.findViewById(R.id.et_value);
        Spinner unitSpinner = dialogView.findViewById(R.id.unit_spinner);
        unitSpinner.setVisibility(View.GONE); // 隐藏单位选择
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
                    } catch (NumberFormatException ignored) {}
                })
                .setNegativeButton("取消", null)
                .show();
    }

    // ==================== 频谱量程 ====================
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
                float curVal = Float.parseFloat(parts[0]);
                String curUnit = parts[1];
                etValue.setText(String.valueOf(curVal));
                int pos = java.util.Arrays.asList(units).indexOf(curUnit);
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
                        spectrumCh0.setRange(value, unit);
                        spectrumCh1.setRange(value, unit);
                        String displayValue;
                        if (Math.abs(value - Math.round(value)) < 0.001f) {
                            displayValue = String.format("%.0f", value);
                        } else {
                            displayValue = String.format("%.3g", value);
                        }
                        tvSpectrumRange.setText(displayValue + " " + unit);
                    } catch (NumberFormatException ignored) {}
                })
                .setNegativeButton("取消", null)
                .show();
    }

    // ==================== Fragment 生命周期 ====================
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

    // ==================== DataListener 回调 ====================
    @Override
    public void onWaveData(int cmd, float ch0, float ch1) {
        if (cmd == 0x04 && !isPaused) {
            waveCh0.addPoint(ch0);
            waveCh1.addPoint(ch1);
        }
    }

    @Override
    public void onSpectrumData(int cmd, float[] mags) {
        if (!isPaused) {
            if (cmd == 0x07) {
                spectrumCh0.updateSpectrum(mags);
            } else if (cmd == 0x06) {
                spectrumCh1.updateSpectrum(mags);
            }
        }
    }

    @Override
    public void onFocusData(float attn0, float attn1, float ema0, float ema1,
                            int trend, int instant) {
        String[] stateText = {"放松", "平静", "专注"};
        int[] stateColors = {
            ContextCompat.getColor(requireContext(), R.color.accent_error),
            ContextCompat.getColor(requireContext(), R.color.accent_warning),
            ContextCompat.getColor(requireContext(), R.color.accent_success)
        };

        String trendStr = (trend >= 0 && trend < stateText.length) ? stateText[trend] : "?";
        String instantStr = (instant >= 0 && instant < stateText.length) ? stateText[instant] : "?";

        attnCh0.setText(String.format(Locale.getDefault(), "Ch0 Focus: %.3f (EMA: %.3f)", attn0, ema0));
        attnCh1.setText(String.format(Locale.getDefault(), "Ch1 Focus: %.3f (EMA: %.3f)", attn1, ema1));
        tvInstantState.setText("瞬时: " + instantStr);
        tvTrendState.setText("趋势: " + trendStr);

        if (instant >= 0 && instant < stateColors.length) {
            tvInstantState.setTextColor(stateColors[instant]);
        }
        if (trend >= 0 && trend < stateColors.length) {
            tvTrendState.setTextColor(stateColors[trend]);
        }
    }

    // ==================== 工具方法 ====================
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
                        if (ip.startsWith("192.168.")) {
                            return ip;
                        }
                    }
                }
            }
        } catch (Exception ignored) {
        }
        return "192.168.43.1";
    }
}