package com.nihilisttt.eegdoctor;

import com.nihilisttt.eegdoctor.R;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

public class SpectrumCompareFragment extends Fragment implements DataListener {

    private static final String[] CHANNEL_NAMES = {"OZ", "O1", "F3", "F4", "CP3", "CP4", "C3", "C4"};

    private SpectrumView rawSpecCh0;
    private SpectrumView filtSpecCh0;
    private SpectrumView rawSpecCh1;
    private SpectrumView filtSpecCh1;

    private TextView labelRawA, labelFiltA, labelRawB, labelFiltB;
    private Spinner spinnerChA, spinnerChB;

    private int chA = 2;
    private int chB = 2;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_spectrum_compare, container, false);

        chA = SettingsStore.getScChA(requireContext(), 2);
        chB = SettingsStore.getScChB(requireContext(), 2);

        rawSpecCh0  = root.findViewById(R.id.raw_spec_ch0);
        filtSpecCh0 = root.findViewById(R.id.filt_spec_ch0);
        rawSpecCh1  = root.findViewById(R.id.raw_spec_ch1);
        filtSpecCh1 = root.findViewById(R.id.filt_spec_ch1);

        labelRawA = root.findViewById(R.id.label_raw_a);
        labelFiltA = root.findViewById(R.id.label_filt_a);
        labelRawB = root.findViewById(R.id.label_raw_b);
        labelFiltB = root.findViewById(R.id.label_filt_b);

        spinnerChA = root.findViewById(R.id.spinner_ch_a);
        spinnerChB = root.findViewById(R.id.spinner_ch_b);

        ArrayAdapter<String> chAdapter = new ArrayAdapter<>(requireContext(),
                android.R.layout.simple_spinner_item, CHANNEL_NAMES);
        chAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinnerChA.setAdapter(chAdapter);
        spinnerChB.setAdapter(chAdapter);
        spinnerChA.setSelection(chA);
        spinnerChB.setSelection(chB);

        spinnerChA.setOnItemSelectedListener(new android.widget.AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(android.widget.AdapterView<?> parent, View view, int position, long id) {
                chA = position;
                updateLabels();
                SettingsStore.setScChA(requireContext(), chA);
            }
            @Override
            public void onNothingSelected(android.widget.AdapterView<?> parent) {}
        });
        spinnerChB.setOnItemSelectedListener(new android.widget.AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(android.widget.AdapterView<?> parent, View view, int position, long id) {
                chB = position;
                updateLabels();
                SettingsStore.setScChB(requireContext(), chB);
            }
            @Override
            public void onNothingSelected(android.widget.AdapterView<?> parent) {}
        });

        float specRange = SettingsStore.getSpecRange(requireContext(), 500f);
        String specUnit = SettingsStore.getSpecUnit(requireContext(), "uV");
        for (SpectrumView sv : new SpectrumView[]{rawSpecCh0, filtSpecCh0, rawSpecCh1, filtSpecCh1}) {
            if (sv != null) sv.setRange(specRange, specUnit);
        }

        updateLabels();

        return root;
    }

    private void updateLabels() {
        String nameA = (chA >= 0 && chA < CHANNEL_NAMES.length) ? CHANNEL_NAMES[chA] : "CH" + chA;
        String nameB = (chB >= 0 && chB < CHANNEL_NAMES.length) ? CHANNEL_NAMES[chB] : "CH" + chB;
        labelRawA.setText(nameA + " 原始频谱");
        labelFiltA.setText(nameA + " 时域滤波频谱");
        labelRawB.setText(nameB + " 原始频谱");
        labelFiltB.setText(nameB + " 时域滤波频谱");
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
        int ch = -1;
        if (cmd >= 0x20 && cmd <= 0x27) {
            ch = cmd - 0x20;
            if (ch == chA && rawSpecCh0 != null) rawSpecCh0.updateSpectrum(mags);
            if (ch == chB && rawSpecCh1 != null) rawSpecCh1.updateSpectrum(mags);
        } else if (cmd >= 0x40 && cmd <= 0x47) {
            ch = cmd - 0x40;
            if (ch == chA && filtSpecCh0 != null) filtSpecCh0.updateSpectrum(mags);
            if (ch == chB && filtSpecCh1 != null) filtSpecCh1.updateSpectrum(mags);
        }
    }

    @Override
    public void onWaveData(int cmd, int ch, float val) {
    }

    @Override
    public void onFocusData(float attn0, float attn1, float ema0, float ema1,
                            int trend, int instant) {
    }
}
