package com.nihilisttt.eegpatient;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;
import androidx.viewpager2.adapter.FragmentStateAdapter;
import androidx.viewpager2.widget.ViewPager2;

public class MainActivity extends AppCompatActivity implements DoctorConnector.DataListener {

    private static final String TAG = "PATIENT";
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

        DoctorConnector.getInstance().addListener(this);
        DoctorConnector.getInstance().startAutoConnect();
        Log.i(TAG, "MainActivity: startAutoConnect called");
    }

    @Override
    protected void onResume() {
        super.onResume();
        updateConnectionState(DoctorConnector.getInstance().isConnected());
    }

    @Override
    protected void onDestroy() {
        TrainingModeCoordinator.getInstance().activate(TrainingModeCoordinator.Mode.NONE);
        super.onDestroy();
        DoctorConnector.getInstance().removeListener(this);
    }

    private void updateConnectionState(boolean conn) {
        if (statusDot == null || tvConnection == null) return;
        if (conn) {
            statusDot.setBackgroundColor(0xFF00C853);
            tvConnection.setText("数据与控制通道已连接");
            tvConnection.setTextColor(0xFF00C853);
        } else {
            statusDot.setBackgroundColor(0xFFFF5252);
            if (DoctorConnector.getInstance().isDataConnected()) {
                tvConnection.setText("数据已连接，等待控制通道");
            } else {
                tvConnection.setText("未连接");
            }
            tvConnection.setTextColor(0xFF607D8B);
        }
    }

    @Override
    public void onConnectionChanged(boolean connected) {
        Log.i(TAG, "onConnectionChanged: " + connected);
        runOnUiThread(() -> updateConnectionState(connected));
    }

    public void switchToPage(int position) {
        if (viewPager != null && position >= 0 && position < 4) {
            applyTrainingModeForPage(position);
            Log.i(TAG, "switchToPage: " + position + ", current=" + viewPager.getCurrentItem());
            viewPager.setCurrentItem(position, false);
        }
    }

    private void applyTrainingModeForPage(int position) {
        TrainingModeCoordinator.Mode mode;
        if (position == 1) mode = TrainingModeCoordinator.Mode.SSVEP;
        else if (position == 2) mode = TrainingModeCoordinator.Mode.MI;
        else mode = TrainingModeCoordinator.Mode.NONE;
        TrainingModeCoordinator.getInstance().activate(mode);
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
        Log.i(TAG, ">>> onPageSwitch: page=" + page);
        runOnUiThread(() -> switchToPage(page));
    }
}
