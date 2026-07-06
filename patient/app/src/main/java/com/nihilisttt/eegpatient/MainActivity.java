package com.nihilisttt.eegpatient;

import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;
import androidx.viewpager2.adapter.FragmentStateAdapter;
import androidx.viewpager2.widget.ViewPager2;

import com.google.android.material.tabs.TabLayout;
import com.google.android.material.tabs.TabLayoutMediator;

public class MainActivity extends AppCompatActivity implements DoctorConnector.DataListener {

    private ViewPager2 viewPager;
    private TabLayout tabLayout;
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
        tabLayout = findViewById(R.id.tab_layout);

        String[] tabs = {"连接", "训练", "方向识别"};

        viewPager.setAdapter(new FragmentStateAdapter(this) {
        @NonNull
        @Override
        public Fragment createFragment(int position) {
            switch (position) {
                case 0: return new ConnectFragment();
                case 1: return new TrainingFragment();
                case 2: return new InferenceFragment();
                default: return new ConnectFragment();
            }
        }

        @Override
        public int getItemCount() { return tabs.length; }
        });

        new TabLayoutMediator(tabLayout, viewPager,
                (tab, position) -> tab.setText(tabs[position])
        ).attach();
    }

    @Override
    protected void onResume() {
        super.onResume();
        DoctorConnector.getInstance().addListener(this);
    }

    @Override
    protected void onPause() {
        super.onPause();
        DoctorConnector.getInstance().removeListener(this);
    }

    @Override
    public void onConnectionChanged(boolean connected) {
        runOnUiThread(() -> {
            if (connected) {
                statusDot.setBackgroundColor(ContextCompat.getColor(this, R.color.accent_success));
                tvConnection.setText("已连接");
                tvConnection.setTextColor(ContextCompat.getColor(this, R.color.accent_success));
            } else {
                statusDot.setBackgroundColor(ContextCompat.getColor(this, R.color.accent_error));
                tvConnection.setText("未连接");
                tvConnection.setTextColor(ContextCompat.getColor(this, R.color.text_secondary));
            }
        });
    }

    @Override public void onWaveData(int cmd, float ch0, float ch1) {}
    @Override public void onEegFrame(EegFrame frame) {}
    @Override public void onInferenceResult(InferenceResult result) {}
    @Override public void onIpcDiag(IpcDiagInfo diag) {}

    @Override
    protected void onDestroy() {
        super.onDestroy();
        DoctorConnector.getInstance().disconnect();
    }
}