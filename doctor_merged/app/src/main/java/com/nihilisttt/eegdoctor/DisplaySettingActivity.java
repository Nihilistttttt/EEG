package com.nihilisttt.eegdoctor;

import android.os.Bundle;
import android.widget.TextView;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;

import com.google.android.material.button.MaterialButtonToggleGroup;

public class DisplaySettingActivity extends AppCompatActivity implements TcpServerManager.ConnectionListener {

    private TextView tvDisplayHint;
    private TextView tvMcuStatus;
    private TextView tvPatientStatus;
    private TextView tvGlxssStatus;
    private View dotMcu;
    private View dotPatient;
    private View dotGlxss;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getSupportActionBar() != null) getSupportActionBar().hide();
        setContentView(R.layout.activity_display_setting);

        findViewById(R.id.btn_back).setOnClickListener(v -> finish());

        MaterialButtonToggleGroup tg = findViewById(R.id.tg_display_output);
        int current = SettingsStore.getDisplayOutput(this);
        if (current == SettingsStore.DISPLAY_OUTPUT_GLXSS) {
            tg.check(R.id.btn_display_glxss);
        } else {
            tg.check(R.id.btn_display_patient);
        }
        tg.addOnButtonCheckedListener((group, checkedId, isChecked) -> {
            if (!isChecked) return;
            if (checkedId == R.id.btn_display_glxss) {
                SettingsStore.setDisplayOutput(this, SettingsStore.DISPLAY_OUTPUT_GLXSS);
            } else {
                SettingsStore.setDisplayOutput(this, SettingsStore.DISPLAY_OUTPUT_PATIENT);
            }
            updateHint();
        });

        tvDisplayHint = findViewById(R.id.tv_display_hint);
        tvMcuStatus = findViewById(R.id.tv_mcu_status);
        tvPatientStatus = findViewById(R.id.tv_patient_status);
        tvGlxssStatus = findViewById(R.id.tv_glxss_status);
        dotMcu = findViewById(R.id.dot_mcu);
        dotPatient = findViewById(R.id.dot_patient);
        dotGlxss = findViewById(R.id.dot_glxss);

        TcpServerManager.getInstance().addConnectionListener(this);
        updateHint();
        updateStatusDots();
    }

    @Override
    protected void onDestroy() {
        TcpServerManager.getInstance().removeConnectionListener(this);
        super.onDestroy();
    }

    @Override
    public void onDeviceConnected(boolean connected) {
        runOnUiThread(this::updateStatusDots);
    }

    @Override
    public void onPatientConnected(boolean connected) {
        runOnUiThread(this::updateStatusDots);
    }

    private void updateHint() {
        boolean glxss = SettingsStore.isGlxssOutput(this);
        if (glxss) {
            tvDisplayHint.setText("AR眼镜模式：刺激画面通过MCU发送至AR眼镜，无需患者端安卓连接");
            tvDisplayHint.setTextColor(ContextCompat.getColor(this, R.color.accent_info));
        } else {
            tvDisplayHint.setText("患者端安卓模式：刺激画面发送至患者端安卓APP，需41004端口连接");
            tvDisplayHint.setTextColor(ContextCompat.getColor(this, R.color.text_secondary));
        }
    }

    private void updateStatusDots() {
        TcpServerManager tcp = TcpServerManager.getInstance();
        boolean mcuConn = tcp.isDeviceConnected();
        boolean patientConn = tcp.isPatientControlConnected();
        boolean glxss = SettingsStore.isGlxssOutput(this);

        tvMcuStatus.setText(mcuConn ? "MCU: 已连接" : "MCU: 未连接");
        dotMcu.setBackgroundColor(ContextCompat.getColor(this,
                mcuConn ? R.color.accent_success : R.color.accent_error));

        tvPatientStatus.setText(patientConn ? "患者端安卓: 已连接" : "患者端安卓: 未连接");
        dotPatient.setBackgroundColor(ContextCompat.getColor(this,
                patientConn ? R.color.accent_success : R.color.accent_error));

        if (glxss) {
            tvGlxssStatus.setText(mcuConn ? "AR眼镜: 可用（MCU已连接）" : "AR眼镜: 不可用（MCU未连接）");
            dotGlxss.setBackgroundColor(ContextCompat.getColor(this,
                    mcuConn ? R.color.accent_success : R.color.accent_warning));
        } else {
            tvGlxssStatus.setText("AR眼镜: 未启用");
            dotGlxss.setBackgroundColor(ContextCompat.getColor(this, R.color.text_secondary));
        }
    }
}