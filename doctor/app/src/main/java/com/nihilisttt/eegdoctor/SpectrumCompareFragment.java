package com.nihilisttt.eegdoctor;

import com.nihilisttt.eegdoctor.R;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

/**
 * 频谱对比页面：展示双通道的原始频谱、频域滤波频谱、时域滤波频谱 (共6个图)
 */
public class SpectrumCompareFragment extends Fragment implements DataListener {

    private SpectrumView rawSpecCh0;
    private SpectrumView filtSpecCh0;

    private SpectrumView rawSpecCh1;
    private SpectrumView filtSpecCh1;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_spectrum_compare, container, false);

        rawSpecCh0  = root.findViewById(R.id.raw_spec_ch0);
        filtSpecCh0 = root.findViewById(R.id.filt_spec_ch0);

        rawSpecCh1  = root.findViewById(R.id.raw_spec_ch1);
        filtSpecCh1 = root.findViewById(R.id.filt_spec_ch1);

        return root;
    }

    @Override
    public void onResume() {
        super.onResume();
        DataDispatcher.getInstance().removeListener(this);
        DataDispatcher.getInstance().addListener(this);
        Log.d("SpectrumCompare", "onResume: listener refreshed");
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        DataDispatcher.getInstance().removeListener(this);
        Log.d("SpectrumCompare", "onDestroyView: listener removed");
    }

    @Override
    public void onSpectrumData(int cmd, float[] mags) {
        switch (cmd) {
            case 0x03: if (rawSpecCh0 != null) rawSpecCh0.updateSpectrum(mags); break;
            case 0x02: if (rawSpecCh1 != null) rawSpecCh1.updateSpectrum(mags); break;
            case 0x09: if (filtSpecCh0 != null) filtSpecCh0.updateSpectrum(mags); break;
            case 0x08: if (filtSpecCh1 != null) filtSpecCh1.updateSpectrum(mags); break;
        }
    }

    @Override
    public void onWaveData(int cmd, int ch, float val) {
        // 本页面不处理波形数据
    }

    @Override
    public void onFocusData(float attn0, float attn1, float ema0, float ema1,
                            int trend, int instant) {
        // 本页面不处理专注度数据
    }
}