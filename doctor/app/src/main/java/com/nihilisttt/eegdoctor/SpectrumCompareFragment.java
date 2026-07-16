package com.nihilisttt.eegdoctor;

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

public class SpectrumCompareFragment extends Fragment implements DataListener {

    private static final int NUM_VIEWS = 4;
    private static final String[] CHANNEL_NAMES = {"OZ", "O1", "F3", "F4", "CP3", "CP4", "C3", "C4"};
    private static final String[] SPEC_TYPE_NAMES = {"原始频谱", "频域滤波频谱", "时域滤波频谱"};
    private static final int[] COLORS = {
        R.color.spectrum_bar, R.color.spectrum_bar_end,
        R.color.accent_info, R.color.accent_warning
    };

    private final SpectrumView[] specViews = new SpectrumView[NUM_VIEWS];
    private final TextView[] labelViews = new TextView[NUM_VIEWS];

    private TextView tvSpecRange;

    private int[] ch = {4, 4, 5, 5};
    private int[] specType = {0, 2, 0, 2};
    private boolean isPaused = false;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_spectrum_compare, container, false);

        ch[0] = SettingsStore.getScChA(requireContext(), 4);
        ch[1] = SettingsStore.getScChA(requireContext(), 4);
        ch[2] = SettingsStore.getScChB(requireContext(), 5);
        ch[3] = SettingsStore.getScChB(requireContext(), 5);

        LinearLayout grid = root.findViewById(R.id.grid_container);
        buildGrid(grid);

        tvSpecRange = root.findViewById(R.id.tv_spec_range);

        root.findViewById(R.id.btn_channel_cfg).setOnClickListener(v -> showChannelConfigDialog());
        tvSpecRange.setOnClickListener(v -> showSpecRangeDialog());

        com.google.android.material.button.MaterialButton btnPause = root.findViewById(R.id.btn_pause_resume);
        btnPause.setOnClickListener(v -> {
            isPaused = !isPaused;
            btnPause.setText(isPaused ? "继续" : "暂停");
        });

        float specRange = SettingsStore.getSpecRange(requireContext(), 500f);
        String specUnit = SettingsStore.getSpecUnit(requireContext(), "uV");
        for (SpectrumView sv : specViews) if (sv != null) sv.setRange(specRange, specUnit);
        tvSpecRange.setText(formatSpecRange(specRange, specUnit));

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

            SpectrumView sv = new SpectrumView(ctx, null);
            sv.setId(View.generateViewId());
            LinearLayout.LayoutParams svp = new LinearLayout.LayoutParams(
                    LinearLayout.LayoutParams.MATCH_PARENT, 0, 1f);
            sv.setLayoutParams(svp);
            inner.addView(sv);
            specViews[i] = sv;

            card.addView(inner);
            row.addView(card);
        }
        grid.addView(row);
    }

    private void updateLabels() {
        for (int i = 0; i < NUM_VIEWS; i++) {
            String chName = (ch[i] >= 0 && ch[i] < CHANNEL_NAMES.length) ? CHANNEL_NAMES[ch[i]] : "CH" + ch[i];
            String typeName = (specType[i] >= 0 && specType[i] < SPEC_TYPE_NAMES.length) ? SPEC_TYPE_NAMES[specType[i]] : "";
            if (labelViews[i] != null) labelViews[i].setText(chName + " " + typeName);
        }
    }

    private String formatSpecRange(float value, String unit) {
        return (Math.abs(value - Math.round(value)) < 0.001f
                ? String.format("%.0f", value) : String.format("%.3g", value)) + " " + unit;
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

        ArrayAdapter<String> specTypeAdapter = new ArrayAdapter<>(ctx,
                android.R.layout.simple_spinner_item, SPEC_TYPE_NAMES);
        specTypeAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        String[] slotNames = {"左上", "左下", "右上", "右下"};
        Spinner[] spCh = new Spinner[NUM_VIEWS];
        Spinner[] spSpecType = new Spinner[NUM_VIEWS];

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

                spSpecType[i] = new Spinner(ctx);
                spSpecType[i].setAdapter(specTypeAdapter);
                spSpecType[i].setSelection(specType[i]);
                addDialogRow(colLayout, "频谱", spSpecType[i]);

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
                        specType[i] = spSpecType[i].getSelectedItemPosition();
                    }
                    updateLabels();
                    SettingsStore.setScChA(requireContext(), ch[0]);
                    SettingsStore.setScChB(requireContext(), ch[2]);
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

    private void showSpecRangeDialog() {
        View dialogView = LayoutInflater.from(requireContext())
                .inflate(R.layout.dialog_range_input, null);
        EditText etValue = dialogView.findViewById(R.id.et_value);
        Spinner unitSpinner = dialogView.findViewById(R.id.unit_spinner);
        String[] units = {"uV", "mV", "V", "dBuV", "dBmV", "dBV"};
        ArrayAdapter<String> ua = new ArrayAdapter<>(requireContext(),
                android.R.layout.simple_spinner_item, units);
        ua.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        unitSpinner.setAdapter(ua);
        String cur = tvSpecRange.getText().toString();
        try {
            String[] parts = cur.split(" ");
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
                        float val = Float.parseFloat(str);
                        if (val <= 0) return;
                        String unit = (String) unitSpinner.getSelectedItem();
                        for (SpectrumView sv : specViews) if (sv != null) sv.setRange(val, unit);
                        tvSpecRange.setText(formatSpecRange(val, unit));
                        SettingsStore.setSpecRange(requireContext(), val);
                        SettingsStore.setSpecUnit(requireContext(), unit);
                    } catch (NumberFormatException ignored) {}
                })
                .setNegativeButton("取消", null)
                .show();
    }

    private void sendDisplayConfig() {
        String cmd = String.format(java.util.Locale.US,
                "DISPLAY_CFG,%d,0,%d,0,%d,%d,%d,0,%d,0,%d,%d",
                ch[0], ch[1], specType[0], specType[1], ch[2], ch[3], specType[2], specType[3]);
        TcpServerManager.getInstance().sendToDevice(cmd);
        Log.d("SpectrumCompare", "Sent: " + cmd);
    }

    @Override
    public void onResume() {
        super.onResume();
        DataDispatcher.getInstance().removeListener(this);
        DataDispatcher.getInstance().addListener(this);
        sendDisplayConfig();
        Log.d("SpectrumCompare", "onResume: listener refreshed");
    }

    @Override
    public void onPause() {
        super.onPause();
        DataDispatcher.getInstance().removeListener(this);
        Log.d("SpectrumCompare", "onPause: listener removed");
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        DataDispatcher.getInstance().removeListener(this);
        Log.d("SpectrumCompare", "onDestroyView: listener removed");
    }

    @Override
    public void onSpectrumData(int cmd, float[] mags) {
        if (isPaused) return;
        for (int i = 0; i < NUM_VIEWS; i++) {
            int chNum = -1;
            if (specType[i] == 0 && cmd >= 0x20 && cmd <= 0x27) {
                chNum = cmd - 0x20;
            } else if (specType[i] == 1 && cmd >= 0x30 && cmd <= 0x37) {
                chNum = cmd - 0x30;
            } else if (specType[i] == 2 && cmd >= 0x40 && cmd <= 0x47) {
                chNum = cmd - 0x40;
            }
            if (chNum == ch[i] && specViews[i] != null) {
                specViews[i].updateSpectrum(mags);
            }
        }
    }

    @Override
    public void onWaveData(int cmd, int ch, float val) {}

    @Override
    public void onFocusData(float attn0, float attn1, float ema0, float ema1, int trend, int instant) {}
}
