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

    // 通道0
    private SpectrumView rawSpecCh0;   // 0x03
    private SpectrumView freqSpecCh0;  // 0x07
    private SpectrumView filtSpecCh0;  // 0x09

    // 通道1
    private SpectrumView rawSpecCh1;   // 0x02
    private SpectrumView freqSpecCh1;  // 0x06
    private SpectrumView filtSpecCh1;  // 0x08

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_spectrum_compare, container, false);

        rawSpecCh0  = root.findViewById(R.id.raw_spec_ch0);
        freqSpecCh0 = root.findViewById(R.id.freq_spec_ch0);
        filtSpecCh0 = root.findViewById(R.id.filt_spec_ch0);

        rawSpecCh1  = root.findViewById(R.id.raw_spec_ch1);
        freqSpecCh1 = root.findViewById(R.id.freq_spec_ch1);
        filtSpecCh1 = root.findViewById(R.id.filt_spec_ch1);

        return root;
    }

    @Override
    public void onResume() {
        super.onResume();
        DataDispatcher.getInstance().addListener(this);
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        DataDispatcher.getInstance().removeListener(this);
    }

    /**
     * 频谱数据回调，根据命令字区分不同处理方式的频谱
     * @param cmd  命令字 (0x03/0x02: 原始, 0x07/0x06: 频域滤波, 0x09/0x08: 时域滤波)
     * @param mags 128点幅度谱
     */
    @Override
    public void onSpectrumData(int cmd, float[] mags) {
        //Log.d("Spectrum", "cmd " + Integer.toHexString(cmd) + " len=" + mags.length + " first=" + mags[0]);
        switch (cmd) {
            // 原始频谱
            case 0x03: rawSpecCh0.updateSpectrum(mags); break;
            case 0x02: rawSpecCh1.updateSpectrum(mags); break;
            // 频域滤波频谱
            case 0x07: freqSpecCh0.updateSpectrum(mags); break;
            case 0x06: freqSpecCh1.updateSpectrum(mags); break;
            // 时域滤波频谱
            case 0x09: filtSpecCh0.updateSpectrum(mags); break;
            case 0x08: filtSpecCh1.updateSpectrum(mags); break;
        }
    }

    @Override
    public void onWaveData(int cmd, float ch0, float ch1) {
        // 本页面不处理波形数据
    }

    @Override
    public void onFocusData(float attn0, float attn1, float ema0, float ema1,
                            int trend, int instant) {
        // 本页面不处理专注度数据
    }
}