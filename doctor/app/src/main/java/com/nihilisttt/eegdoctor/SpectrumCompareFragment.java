package com.nihilisttt.eegdoctor;

import com.nihilisttt.eegdoctor.R;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

public class SpectrumCompareFragment extends Fragment implements DataListener {

    private SpectrumView rawSpecChA;
    private SpectrumView filtSpecChA;
    private SpectrumView rawSpecChB;
    private SpectrumView filtSpecChB;
    private TextView labelRawA;
    private TextView labelFiltA;
    private TextView labelRawB;
    private TextView labelFiltB;
    private Spinner spinnerChA;
    private Spinner spinnerChB;

    private static final String[] CHANNEL_NAMES = {"F3", "F4", "CP3", "CP4", "C3", "C4", "P3", "P4"};

    private static final int[] RAW_SPECTRUM_CMDS = {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
    };
    private static final int[] FILT_SPECTRUM_CMDS = {
        0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F
    };

    private int chA = 2;
    private int chB = 4;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_spectrum_compare, container, false);

        rawSpecChA  = root.findViewById(R.id.raw_spec_ch0);
        filtSpecChA = root.findViewById(R.id.filt_spec_ch0);
        rawSpecChB  = root.findViewById(R.id.raw_spec_ch1);
        filtSpecChB = root.findViewById(R.id.filt_spec_ch1);

        labelRawA  = root.findViewById(R.id.label_raw_a);
        labelFiltA = root.findViewById(R.id.label_filt_a);
        labelRawB  = root.findViewById(R.id.label_raw_b);
        labelFiltB = root.findViewById(R.id.label_filt_b);

        ArrayAdapter<String> chAdapter = new ArrayAdapter<>(requireContext(),
                android.R.layout.simple_spinner_item, CHANNEL_NAMES);
        chAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        spinnerChA = root.findViewById(R.id.spinner_ch_a);
        spinnerChB = root.findViewById(R.id.spinner_ch_b);
        spinnerChA.setAdapter(chAdapter);
        spinnerChB.setAdapter(chAdapter);
        spinnerChA.setSelection(chA);
        spinnerChB.setSelection(chB);

        spinnerChA.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                if (position != chA) {
                    chA = position;
                    updateLabels();
                }
            }
            @Override
            public void onNothingSelected(AdapterView<?> parent) {}
        });

        spinnerChB.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                if (position != chB) {
                    chB = position;
                    updateLabels();
                }
            }
            @Override
            public void onNothingSelected(AdapterView<?> parent) {}
        });

        updateLabels();
        return root;
    }

    private void updateLabels() {
        String nameA = CHANNEL_NAMES[chA];
        String nameB = CHANNEL_NAMES[chB];
        if (labelRawA != null)  labelRawA.setText(nameA + " 原始频谱");
        if (labelFiltA != null) labelFiltA.setText(nameA + " 时域滤波频谱");
        if (labelRawB != null)  labelRawB.setText(nameB + " 原始频谱");
        if (labelFiltB != null) labelFiltB.setText(nameB + " 时域滤波频谱");
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
        if (cmd == RAW_SPECTRUM_CMDS[chA] && rawSpecChA != null) rawSpecChA.updateSpectrum(mags);
        else if (cmd == FILT_SPECTRUM_CMDS[chA] && filtSpecChA != null) filtSpecChA.updateSpectrum(mags);
        else if (cmd == RAW_SPECTRUM_CMDS[chB] && rawSpecChB != null) rawSpecChB.updateSpectrum(mags);
        else if (cmd == FILT_SPECTRUM_CMDS[chB] && filtSpecChB != null) filtSpecChB.updateSpectrum(mags);
    }

    @Override
    public void onWaveData(int cmd, float ch0, float ch1) {}
    @Override
    public void onWaveData8ch(int cmd, float[] ch) {}
    @Override
    public void onFocusData(float attn0, float attn1, float ema0, float ema1,
                            int trend, int instant) {}
}
