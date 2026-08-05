package com.nihilisttt.eegdoctor;

import com.nihilisttt.eegdoctor.R;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.util.Log;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import com.google.android.material.textfield.TextInputEditText;

import com.google.android.material.button.MaterialButtonToggleGroup;

public class ConfigFragment extends Fragment {

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_config, container, false);

        TextInputEditText etSsid = root.findViewById(R.id.et_wifi_ssid);
        TextInputEditText etPassword = root.findViewById(R.id.et_wifi_password);

        root.findViewById(R.id.btn_wifi_add).setOnClickListener(v -> {
            String ssid = etSsid.getText() != null ? etSsid.getText().toString().trim() : "";
            String pwd = etPassword.getText() != null ? etPassword.getText().toString().trim() : "";
            Log.i("Config", "btn_wifi_add clicked, ssid=[" + ssid + "]");
            if (ssid.isEmpty()) {
                Toast.makeText(requireContext(), "请输入SSID", Toast.LENGTH_SHORT).show();
                return;
            }
            TcpServerManager.getInstance().sendWifiAdd(ssid, pwd);
            Toast.makeText(requireContext(), "WiFi配置已发送: " + ssid, Toast.LENGTH_SHORT).show();
        });

        root.findViewById(R.id.btn_wifi_delete).setOnClickListener(v -> {
            String ssid = etSsid.getText() != null ? etSsid.getText().toString().trim() : "";
            Log.i("Config", "btn_wifi_delete clicked, ssid=[" + ssid + "]");
            if (ssid.isEmpty()) {
                Toast.makeText(requireContext(), "请输入要删除的SSID", Toast.LENGTH_SHORT).show();
                return;
            }
            TcpServerManager.getInstance().sendWifiDelete(ssid);
            Toast.makeText(requireContext(), "WiFi删除已发送: " + ssid, Toast.LENGTH_SHORT).show();
        });

        MaterialButtonToggleGroup tgDisplay = root.findViewById(R.id.tg_display_output);
        if (tgDisplay != null) {
            int current = SettingsStore.getDisplayOutput(requireContext());
            if (current == SettingsStore.DISPLAY_OUTPUT_GLXSS) {
                tgDisplay.check(R.id.btn_display_glxss);
            } else {
                tgDisplay.check(R.id.btn_display_patient);
            }
            tgDisplay.addOnButtonCheckedListener((group, checkedId, isChecked) -> {
                if (!isChecked) return;
                if (checkedId == R.id.btn_display_glxss) {
                    SettingsStore.setDisplayOutput(requireContext(), SettingsStore.DISPLAY_OUTPUT_GLXSS);
                } else {
                    SettingsStore.setDisplayOutput(requireContext(), SettingsStore.DISPLAY_OUTPUT_PATIENT);
                }
            });
        }

        return root;
    }

    @Override
    public void onResume() {
        super.onResume();
        TcpServerManager.getInstance().sendBinaryToDevice(EegProtocol.CMD_DISPLAY_CFG,
                EegChannels.buildAllNoneDisplayConfigData());
    }
}