package com.nihilisttt.eegdoctor;

import com.nihilisttt.eegdoctor.R;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;
import androidx.viewpager2.adapter.FragmentStateAdapter;
import androidx.viewpager2.widget.ViewPager2;

import com.google.android.material.button.MaterialButton;

import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

public class MainActivity extends AppCompatActivity implements TcpServerManager.ConnectionListener, DataListener {

    private ViewPager2 viewPager;
    private TcpServerManager tcpServer;
    private final String[] pageTitles = {"脑电监测", "专注度", "波形对比", "频谱对比", "SSVEP训练", "MI训练", "方向识别", "姿态监护", "系统配置"};

    private TextView tvBarFocus;
    private TextView tvBarRelax;
    private TextView tvBarState;
    private TextView tvBarPosture;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getSupportActionBar() != null) {
            getSupportActionBar().hide();
        }
        setContentView(R.layout.activity_main);

        viewPager = findViewById(R.id.view_pager2);

        tvBarFocus = findViewById(R.id.tv_bar_focus);
        tvBarRelax = findViewById(R.id.tv_bar_relax);
        tvBarState = findViewById(R.id.tv_bar_state);
        tvBarPosture = findViewById(R.id.tv_bar_posture);

        List<Fragment> fragments = new ArrayList<>();
        fragments.add(new MonitorFragment());
        fragments.add(new FocusHistoryFragment());
        fragments.add(new WaveCompareFragment());
        fragments.add(new SpectrumCompareFragment());
        fragments.add(new SsvepTrainingFragment());
        fragments.add(new MiTrainingFragment());
        fragments.add(new InferenceFragment());
        fragments.add(new PostureFragment());
        fragments.add(new ConfigFragment());

        viewPager.setAdapter(new FragmentStateAdapter(this) {
            @NonNull
            @Override
            public Fragment createFragment(int position) {
                return fragments.get(position);
            }

            @Override
            public int getItemCount() {
                return fragments.size();
            }
        });

        viewPager.setUserInputEnabled(true);
        viewPager.setOffscreenPageLimit(pageTitles.length);

        int initialPage = getIntent().getIntExtra("initial_page", 0);
        if (initialPage >= 0 && initialPage < fragments.size()) {
            viewPager.setCurrentItem(initialPage, false);
        }

        tcpServer = TcpServerManager.getInstance();
        tcpServer.start();
        tcpServer.addConnectionListener(this);

        MaterialButton btnHome = findViewById(R.id.btn_home);
        btnHome.setOnClickListener(v -> {
            Intent intent = new Intent(MainActivity.this, HomeActivity.class);
            intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
            startActivity(intent);
        });

        viewPager.registerOnPageChangeCallback(new ViewPager2.OnPageChangeCallback() {
            @Override
            public void onPageSelected(int position) {
                super.onPageSelected(position);
                updatePageTitle(position);
                Log.i("DOCTOR", ">>> onPageSelected: " + position + " -> sending PAGE," + position);
                TcpServerManager.getInstance().sendToPatient("PAGE," + position);
            }
        });

        updatePageTitle(viewPager.getCurrentItem());

        DataDispatcher.getInstance().addListener(this);

        new android.os.Handler(android.os.Looper.getMainLooper()).postDelayed(() -> {
            TcpServerManager.getInstance().sendToPatient("PAGE," + viewPager.getCurrentItem());
        }, 2000);
    }

    private void updatePageTitle(int position) {
        TextView tvTitle = findViewById(R.id.tv_page_title);
        if (tvTitle != null && position >= 0 && position < pageTitles.length) {
            tvTitle.setText(pageTitles[position]);
        }
    }

    public void switchToPage(int position) {
        if (viewPager != null) {
            viewPager.setCurrentItem(position, true);
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        tcpServer.removeConnectionListener(this);
        DataDispatcher.getInstance().removeListener(this);
    }

    @Override
    public void onDeviceConnected(boolean connected) {}

    @Override
    public void onPatientConnected(boolean connected) {
        if (connected) {
            runOnUiThread(() -> sendCurrentPage());
        }
    }

    public void sendCurrentPage() {
        if (viewPager != null) {
            int page = viewPager.getCurrentItem();
            Log.i("DOCTOR", ">>> sendCurrentPage: " + page);
            TcpServerManager.getInstance().sendToPatient("PAGE," + page);
        }
    }

    @Override
    public void onFocusData(float attn0, float attn1, float ema0, float ema1, int trend, int instant) {
        runOnUiThread(() -> {
            if (tvBarFocus != null) {
                tvBarFocus.setText(String.format(Locale.getDefault(), "专注:%.0f%%", ema0 * 100));
            }
            if (tvBarRelax != null) {
                tvBarRelax.setText(String.format(Locale.getDefault(), "放松:%.0f%%", (1.0f - ema0) * 100));
            }
            if (tvBarState != null) {
                String[] states = {"放松", "平静", "专注"};
                int[] colors = {R.color.accent_error, R.color.accent_warning, R.color.accent_success};
                String stateStr = (instant >= 0 && instant < states.length) ? states[instant] : "--";
                tvBarState.setText("状态:" + stateStr);
                if (instant >= 0 && instant < colors.length) {
                    tvBarState.setTextColor(ContextCompat.getColor(this, colors[instant]));
                }
            }
        });
    }

    @Override
    public void onPostureState(String posture, int turnCount) {
        runOnUiThread(() -> {
            if (tvBarPosture != null) {
                String cn = postureToChinese(posture);
                tvBarPosture.setText("姿态:" + cn);
            }
        });
    }

    private static String postureToChinese(String posture) {
        switch (posture) {
            case "SUPINE": return "仰卧";
            case "PRONE": return "俯卧";
            case "LEFT": return "左侧卧";
            case "RIGHT": return "右侧卧";
            case "SITTING": return "坐姿";
            default: return posture;
        }
    }

    @Override
    public void onWaveData(int cmd, float ch0, float ch1) {}
    @Override
    public void onSpectrumData(int cmd, float[] mags) {}
    @Override
    public void onEegFrame(EegFrame frame) {}
    @Override
    public void onInferenceResult(InferenceResult result) {}
    @Override
    public void onIpcDiag(IpcDiagInfo diag) {}
    @Override
    public void onDirConfig(String configJson) {}
    @Override
    public void onTaskStart(String side) {}
    @Override
    public void onTaskDone() {}
    @Override
    public void onReadyTrain() {}
    @Override
    public void onReadyTest() {}
    @Override
    public void onModeSetOk(int mode) {}
    @Override
    public void onTurnEvent(String from, String to) {}
    @Override
    public void onFallEvent() {}
    @Override
    public void onNoTurnAlert(long durationMin) {}
}
