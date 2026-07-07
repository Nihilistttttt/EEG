package com.nihilisttt.eegpatient;

import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.text.format.Formatter;
import android.view.View;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;
import androidx.viewpager2.adapter.FragmentStateAdapter;
import androidx.viewpager2.widget.ViewPager2;

public class MainActivity extends AppCompatActivity implements DoctorConnector.DataListener {

    private ViewPager2 viewPager;
    private View statusDot;
    private TextView tvConnection;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getSupportActionBar() != null) getSupportActionBar().hide();
        setContentView(R.layout.activity_main);

        statusDot = findViewById(R.id.status_dot);
        tvConnection = findViewById(R.id.tv_connection);
        viewPager = findViewById(R.id.view_pager);

        viewPager.setUserInputEnabled(false);
        viewPager.setOffscreenPageLimit(4);

        viewPager.setAdapter(new FragmentStateAdapter(this) {
            @NonNull
            @Override
            public Fragment createFragment(int position) {
                switch (position) {
                    case 0: return new ConnectFragment();
                    case 1: return new SsvepTrainingFragment();
                    case 2: return new MiTrainingFragment();
                    case 3: return new InferenceFragment();
                    default: return new ConnectFragment();
                }
            }

            @Override
            public int getItemCount() { return 4; }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
        DoctorConnector.getInstance().addListener(this);
        updateConnectionState(DoctorConnector.getInstance().isConnected());
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        DoctorConnector.getInstance().removeListener(this);
        DoctorConnector.getInstance().disconnect();
    }

    private void updateConnectionState(boolean connected) {
        if (statusDot == null || tvConnection == null) return;
        if (connected) {
            statusDot.setBackgroundColor(ContextCompat.getColor(this, R.color.accent_success));
            tvConnection.setText("已连接");
            tvConnection.setTextColor(ContextCompat.getColor(this, R.color.accent_success));
        } else {
            statusDot.setBackgroundColor(ContextCompat.getColor(this, R.color.accent_error));
            tvConnection.setText("未连接");
            tvConnection.setTextColor(ContextCompat.getColor(this, R.color.text_secondary));
        }
    }

    @Override
    public void onConnectionChanged(boolean connected) {
        runOnUiThread(() -> updateConnectionState(connected));
    }

    public void switchToPage(int position) {
        if (viewPager != null && position >= 0 && position < 4) {
            viewPager.setCurrentItem(position, true);
        }
    }

    @Override public void onWaveData(int cmd, float ch0, float ch1) {}
    @Override public void onEegFrame(EegFrame frame) {}
    @Override public void onInferenceResult(InferenceResult result) {}
    @Override public void onIpcDiag(IpcDiagInfo diag) {}
    @Override public void onTaskStart(String side) {}
    @Override public void onTaskDone() {}
    @Override public void onReadyTrain() {}
    @Override public void onReadyTest() {}
    @Override public void onModeSetOk(int mode) {}
    @Override public void onFocusData(float attn0, float attn1, float ema0, float ema1, int trend, int instant) {}

    @Override
    public void onPageSwitch(int page) {
        runOnUiThread(() -> switchToPage(page));
    }
}
