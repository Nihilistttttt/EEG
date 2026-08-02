package com.nihilisttt.eegdoctor;

import com.nihilisttt.eegdoctor.R;

import android.app.AlertDialog;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

public class TopoMapFragment extends Fragment implements DataListener {

    private static final float DEFAULT_MAX_UV = 50f;

    private TopoMapView topoMapView;
    private TextView tvWaveType;
    private TextView tvRange;
    private TextView btnPauseResume;

    private boolean isPaused = false;
    private int currentWaveType = EegChannels.WAVE_TYPE_FILT;
    private float amplitudeMaxUv = DEFAULT_MAX_UV;

    private TcpServerManager.ConnectionListener connectionListener;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_topomap, container, false);

        topoMapView = root.findViewById(R.id.topo_map_view);
        tvWaveType = root.findViewById(R.id.tv_wave_type);
        tvRange = root.findViewById(R.id.tv_range);
        btnPauseResume = root.findViewById(R.id.btn_pause_resume);

        topoMapView.setAmplitudeMaxUv(amplitudeMaxUv);

        tvWaveType.setOnClickListener(v -> toggleWaveType());
        tvRange.setOnClickListener(v -> showRangeDialog());

        btnPauseResume.setOnClickListener(v -> {
            isPaused = !isPaused;
            btnPauseResume.setText(isPaused ? "继续" : "暂停");
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
        topoMapView.clear();
        sendDisplayConfig();
    }

    private void showRangeDialog() {
        String[] presets = {"10μV", "20μV", "50μV", "100μV", "200μV", "500μV"};
        float[] values = {10f, 20f, 50f, 100f, 200f, 500f};
        int current = 2;
        for (int i = 0; i < values.length; i++) {
            if (Math.abs(amplitudeMaxUv - values[i]) < 0.5f) { current = i; break; }
        }
        new AlertDialog.Builder(requireContext())
                .setTitle("振幅范围")
                .setSingleChoiceItems(presets, current, (dialog, which) -> {
                    amplitudeMaxUv = values[which];
                    topoMapView.setAmplitudeMaxUv(amplitudeMaxUv);
                    tvRange.setText(presets[which]);
                    dialog.dismiss();
                })
                .setNegativeButton("取消", null)
                .show();
    }

    private void sendDisplayConfig() {
        byte[] data = EegChannels.build8ChannelDisplayConfigData(
                currentWaveType, EegChannels.SPEC_TYPE_NONE);
        TcpServerManager.getInstance().sendBinaryToDevice(EegProtocol.CMD_DISPLAY_CFG, data);
        Log.i("TopoMap", "Sent 8-ch DISPLAY_CFG waveType=" + currentWaveType);
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
            topoMapView.addSample(ch, val);
        }
    }

    @Override
    public void onSpectrumData(int cmd, float[] mags) {}

    @Override
    public void onFocusData(float attn0, float attn1, float ema0, float ema1,
                            int trend, int instant) {}
}