package com.nihilisttt.eegdoctor;

import com.nihilisttt.eegdoctor.R;

import android.app.AlertDialog;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

public class Eeg8ChFragment extends Fragment implements DataListener {

    private static final float DEFAULT_Y_RANGE_UV = 100f;
    private static final float DEFAULT_X_MAX = 2.048f;

    private EegStripView stripView;
    private TextView tvWaveType;
    private TextView tvYRange;
    private TextView tvXRange;
    private TextView btnPauseResume;

    private boolean isPaused = false;
    private int currentWaveType = EegChannels.WAVE_TYPE_FILT;
    private float yRangeUv = DEFAULT_Y_RANGE_UV;
    private float xMax = DEFAULT_X_MAX;

    private TcpServerManager.ConnectionListener connectionListener;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_eeg8ch, container, false);

        stripView = root.findViewById(R.id.eeg_strip_view);
        tvWaveType = root.findViewById(R.id.tv_wave_type);
        tvYRange = root.findViewById(R.id.tv_y_range);
        tvXRange = root.findViewById(R.id.tv_x_range);
        btnPauseResume = root.findViewById(R.id.btn_pause_resume);

        stripView.setYRange(yRangeUv / 1_000_000f);
        stripView.setXMax(xMax);

        tvWaveType.setOnClickListener(v -> toggleWaveType());
        tvYRange.setOnClickListener(v -> showYRangeDialog());
        tvXRange.setOnClickListener(v -> showXRangeDialog());

        btnPauseResume.setOnClickListener(v -> {
            isPaused = !isPaused;
            btnPauseResume.setText(isPaused ? "继续" : "暂停");
        });

        root.findViewById(R.id.btn_set_ip).setOnClickListener(v -> showWifiConfigDialog());
        root.findViewById(R.id.btn_training).setOnClickListener(v -> {
            if (getActivity() instanceof MainActivity) {
                ((MainActivity) requireActivity()).switchToPage(5);
            }
        });

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

        return root;
    }

    private void toggleWaveType() {
        if (currentWaveType == EegChannels.WAVE_TYPE_FILT) {
            currentWaveType = EegChannels.WAVE_TYPE_BASELINE;
            tvWaveType.setText("基线");
        } else {
            currentWaveType = EegChannels.WAVE_TYPE_FILT;
            tvWaveType.setText("滤波");
        }
        stripView.clear();
        sendDisplayConfig();
    }

    private void showYRangeDialog() {
        String[] presets = {"50μV", "100μV", "200μV", "500μV", "1mV"};
        float[] values = {50f, 100f, 200f, 500f, 1000f};
        int current = 1;
        for (int i = 0; i < values.length; i++) {
            if (Math.abs(yRangeUv - values[i]) < 1f) { current = i; break; }
        }
        new AlertDialog.Builder(requireContext())
                .setTitle("Y轴范围")
                .setSingleChoiceItems(presets, current, (dialog, which) -> {
                    yRangeUv = values[which];
                    stripView.setYRange(yRangeUv / 1_000_000f);
                    tvYRange.setText(presets[which]);
                    dialog.dismiss();
                })
                .setNegativeButton("取消", null)
                .show();
    }

    private void showXRangeDialog() {
        View dialogView = LayoutInflater.from(requireContext()).inflate(R.layout.dialog_range_input, null);
        EditText etValue = dialogView.findViewById(R.id.et_value);
        dialogView.findViewById(R.id.unit_spinner).setVisibility(View.GONE);
        etValue.setInputType(android.text.InputType.TYPE_CLASS_NUMBER | android.text.InputType.TYPE_NUMBER_FLAG_DECIMAL);
        etValue.setHint("例如 2.048");
        etValue.setText(String.format("%.3f", xMax));
        new AlertDialog.Builder(requireContext())
                .setTitle("X轴时间范围 (秒)")
                .setView(dialogView)
                .setPositiveButton("确定", (d, which) -> {
                    String str = etValue.getText().toString().trim();
                    if (str.isEmpty()) return;
                    try {
                        float val = Float.parseFloat(str);
                        if (val <= 0) return;
                        xMax = val;
                        stripView.setXMax(xMax);
                        tvXRange.setText(String.format("%.2f s", xMax));
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

    private void sendDisplayConfig() {
        byte[] data = EegChannels.build8ChannelDisplayConfigData(
                currentWaveType, EegChannels.SPEC_TYPE_NONE);
        TcpServerManager.getInstance().sendBinaryToDevice(EegProtocol.CMD_DISPLAY_CFG, data);
        Log.i("Eeg8Ch", "Sent 8-ch DISPLAY_CFG waveType=" + currentWaveType);
    }

    @Override
    public void onResume() {
        super.onResume();
        DataDispatcher.getInstance().removeListener(this);
        DataDispatcher.getInstance().addListener(this);
        sendDisplayConfig();
    }

    @Override
    public void onPause() {
        super.onPause();
        DataDispatcher.getInstance().removeListener(this);
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        DataDispatcher.getInstance().removeListener(this);
        if (connectionListener != null) {
            TcpServerManager.getInstance().removeConnectionListener(connectionListener);
            connectionListener = null;
        }
    }

    @Override
    public void onWaveData(int cmd, int ch, float val) {
        if (isPaused) return;
        if (EegChannels.isWaveCmd(cmd) && cmd == EegChannels.waveTypeToCmd(currentWaveType)) {
            stripView.addPoint(ch, val);
        }
    }

    @Override
    public void onSpectrumData(int cmd, float[] mags) {}

    @Override
    public void onFocusData(float attn0, float attn1, float ema0, float ema1,
                            int trend, int instant) {}
}