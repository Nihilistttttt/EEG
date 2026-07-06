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
    private final List<TextView> attnViews = new ArrayList<>();
    private final List<View> signalQualityViews = new ArrayList<>();

    private TextView tvStatus;
    private TextView tvInstantState;
    private TextView tvTrendState;
    private TextView tvWaveLabelCount;
    private TextView tvWaveRange;
    private TextView tvXRange;
    private TextView tvSpectrumRange;
    private TextView btnPauseResume;
    private boolean isPaused = false;
    private int currentLabelCount = DEFAULT_LABEL_COUNT;

    public MonitorFragment() {
        this.channels = ChannelConfig.getDefaultDualChannel();
    }

    public MonitorFragment(List<ChannelConfig> channels) {
        this.channels = channels != null ? channels : ChannelConfig.getDefaultDualChannel();
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_monitor, container, false);

        LinearLayout channelContainer = root.findViewById(R.id.channel_container);
        buildChannelViews(channelContainer);

        tvStatus = root.findViewById(R.id.tv_status);
        tvInstantState = root.findViewById(R.id.tv_instant_state);
        tvTrendState = root.findViewById(R.id.tv_trend_state);
        tvWaveLabelCount = root.findViewById(R.id.tv_wave_label_count);
        tvWaveRange = root.findViewById(R.id.tv_wave_range);
        tvXRange = root.findViewById(R.id.tv_x_range);
        tvSpectrumRange = root.findViewById(R.id.tv_spectrum_range);
        btnPauseResume = root.findViewById(R.id.btn_pause_resume);

        btnPauseResume.setOnClickListener(v -> {
            isPaused = !isPaused;
            btnPauseResume.setText(isPaused ? "继续" : "暂停");
            String ip = getWifiApIpAddress();
            tvStatus.setText(isPaused ? "本机IP: " + ip + "  端口: 41002  (已暂停)" : "本机IP: " + ip + "  端口: 41002");
        });

        root.findViewById(R.id.btn_set_ip).setOnClickListener(v -> showUdpIpDialog());
        root.findViewById(R.id.btn_training).setOnClickListener(v -> {
            if (getActivity() instanceof MainActivity) {
                ((MainActivity) requireActivity()).switchToPage(4);
            }
        });

        setupRangeSelectors();
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

            TextView attnView = new TextView(ctx);
            attnView.setText("专注度: --");
            attnView.setTextColor(ContextCompat.getColor(ctx, R.color.focus_line));
            attnView.setTextSize(10);
            headerRow.addView(attnView);
            attnViews.add(attnView);

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

        tvWaveLabelCount.setText(String.valueOf(DEFAULT_LABEL_COUNT));
        tvWaveRange.setText(DEFAULT_STEP + DEFAULT_STEP_UNIT);
        tvXRange.setText(String.format("%.3f s", DEFAULT_X_MAX));
        tvSpectrumRange.setText("500 uV");

        applyWaveStep(DEFAULT_STEP, DEFAULT_STEP_UNIT, DEFAULT_LABEL_COUNT);
        applyXRange(DEFAULT_X_MAX);
        for (SpectrumView sv : spectrumViews) {
            sv.setRange(500f, "uV");
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
                    } catch (NumberFormatException ignored) {}
                })
                .setNegativeButton("取消", null)
                .show();
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

    @Override
    public void onResume() {
        super.onResume();
        DataDispatcher.getInstance().addListener(this);
        Log.d("MonitorFragment", "onResume: listener added, waveViews=" + waveformViews.size());
    }

    @Override
    public void onPause() {
        super.onPause();
        DataDispatcher.getInstance().removeListener(this);
        Log.d("MonitorFragment", "onPause: listener removed");
    }

    @Override
    public void onWaveData(int cmd, float ch0, float ch1) {
        if (cmd == 0x04 && !isPaused) {
            if (waveformViews.size() > 0 && waveformViews.get(0) != null) waveformViews.get(0).addPoint(ch0);
            if (waveformViews.size() > 1 && waveformViews.get(1) != null) waveformViews.get(1).addPoint(ch1);
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
        if (!isPaused) {
            if (cmd == 0x07 && spectrumViews.size() > 0) spectrumViews.get(0).updateSpectrum(mags);
            else if (cmd == 0x06 && spectrumViews.size() > 1) spectrumViews.get(1).updateSpectrum(mags);
        }
    }

    @Override
    public void onFocusData(float attn0, float attn1, float ema0, float ema1,
                            int trend, int instant) {
        if (tvInstantState == null || tvTrendState == null) return;
        String[] stateText = {"放松", "平静", "专注"};
        int[] stateColors = {
            ContextCompat.getColor(requireContext(), R.color.accent_error),
            ContextCompat.getColor(requireContext(), R.color.accent_warning),
            ContextCompat.getColor(requireContext(), R.color.accent_success)
        };
        if (attnViews.size() > 0 && attnViews.get(0) != null)
            attnViews.get(0).setText(String.format(Locale.getDefault(), "专注度: %.3f", attn0));
        if (attnViews.size() > 1 && attnViews.get(1) != null)
            attnViews.get(1).setText(String.format(Locale.getDefault(), "专注度: %.3f", attn1));
        String instantStr = (instant >= 0 && instant < stateText.length) ? stateText[instant] : "?";
        String trendStr = (trend >= 0 && trend < stateText.length) ? stateText[trend] : "?";
        tvInstantState.setText("瞬时: " + instantStr);
        tvTrendState.setText("趋势: " + trendStr);
        if (instant >= 0 && instant < stateColors.length) tvInstantState.setTextColor(stateColors[instant]);
        if (trend >= 0 && trend < stateColors.length) tvTrendState.setTextColor(stateColors[trend]);
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