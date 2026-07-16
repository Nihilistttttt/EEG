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

public class MainActivity extends AppCompatActivity implements TcpServerManager.ConnectionListener, DataListener {

    private ViewPager2 viewPager;
    private TcpServerManager tcpServer;
    private final String[] pageTitles = {"脑电监测", "专注度", "波形对比", "频谱对比", "SSVEP训练", "MI训练", "方向识别", "姿态监护", "系统配置"};

    private TextView tvBarFocus, tvBarRelax, tvBarInstant, tvBarTrend, tvBarPosture;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getSupportActionBar() != null) {
            getSupportActionBar().hide();
        }
        setContentView(R.layout.activity_main);

        viewPager = findViewById(R.id.view_pager2);

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

        tvBarFocus = findViewById(R.id.tv_bar_focus);
        tvBarRelax = findViewById(R.id.tv_bar_relax);
        tvBarInstant = findViewById(R.id.tv_bar_instant);
        tvBarTrend = findViewById(R.id.tv_bar_trend);
        tvBarPosture = findViewById(R.id.tv_bar_posture);
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
    public void onDeviceConnected(boolean connected) {
        runOnUiThread(this::updateStatusBar);
    }

    @Override
    public void onPatientConnected(boolean connected) {
        if (connected) {
            runOnUiThread(this::sendCurrentPage);
        }
        runOnUiThread(this::updateStatusBar);
    }

    private void updateStatusBar() {
        TextView tvBar = findViewById(R.id.tv_status_bar);
        View dot = findViewById(R.id.status_indicator);
        boolean devConn = tcpServer.isDeviceConnected();
        boolean patConn = tcpServer.isPatientConnected();
        StringBuilder sb = new StringBuilder();
        if (!devConn) sb.append("采集未连接");
        if (!patConn) {
            if (sb.length() > 0) sb.append(" ");
            sb.append("患者未连接");
        }
        if (devConn && patConn) sb.append("已连接");
        if (tvBar != null) tvBar.setText(sb.toString());
        if (dot != null) {
            int color;
            if (devConn && patConn) color = ContextCompat.getColor(this, R.color.accent_success);
            else if (devConn || patConn) color = ContextCompat.getColor(this, R.color.accent_warning);
            else color = ContextCompat.getColor(this, R.color.accent_error);
            dot.setBackgroundColor(color);
        }
    }

    public void sendCurrentPage() {
        if (viewPager != null) {
            int page = viewPager.getCurrentItem();
            Log.i("DOCTOR", ">>> sendCurrentPage: " + page);
            TcpServerManager.getInstance().sendToPatient("PAGE," + page);
        }
    }

    private static final String[] STATE_TEXT = {"放松", "平静", "专注"};
    private static final int[] STATE_COLORS_RES = {
        com.nihilisttt.eegdoctor.R.color.accent_error,
        com.nihilisttt.eegdoctor.R.color.accent_warning,
        com.nihilisttt.eegdoctor.R.color.accent_success
    };

    private static String postureToChinese(String posture) {
        if (posture == null) return "-";
        switch (posture) {
            case "SUPINE": return "仰卧";
            case "PRONE": return "俯卧";
            case "LEFT": return "左侧卧";
            case "RIGHT": return "右侧卧";
            case "SITTING": return "坐姿";
            case "UNKNOWN": return "未知";
            default: return posture;
        }
    }

    @Override
    public void onWaveData(int cmd, int ch, float val) {}

    @Override
    public void onSpectrumData(int cmd, float[] mags) {}

    @Override
    public void onFocusData(float attn0, float attn1, float ema0, float ema1, int trend, int instant) {
        runOnUiThread(() -> {
            float focus = ema0;
            float relax = ema1;
            if (tvBarFocus != null) tvBarFocus.setText(String.format(java.util.Locale.getDefault(), "专注:%.0f%%", focus * 100));
            if (tvBarRelax != null) tvBarRelax.setText(String.format(java.util.Locale.getDefault(), "放松:%.0f%%", relax * 100));
            String instantStr = (instant >= 0 && instant < STATE_TEXT.length) ? STATE_TEXT[instant] : "-";
            String trendStr = (trend >= 0 && trend < STATE_TEXT.length) ? STATE_TEXT[trend] : "-";
            if (tvBarInstant != null) {
                tvBarInstant.setText("瞬时:" + instantStr);
                if (instant >= 0 && instant < STATE_COLORS_RES.length)
                    tvBarInstant.setTextColor(getResources().getColor(STATE_COLORS_RES[instant]));
            }
            if (tvBarTrend != null) {
                tvBarTrend.setText("趋势:" + trendStr);
                if (trend >= 0 && trend < STATE_COLORS_RES.length)
                    tvBarTrend.setTextColor(getResources().getColor(STATE_COLORS_RES[trend]));
            }
        });
    }

    @Override
    public void onPostureState(String posture, int turnCount) {
        runOnUiThread(() -> {
            if (tvBarPosture != null)
                tvBarPosture.setText("姿态:" + postureToChinese(posture) + " 翻身:" + turnCount);
        });
    }

    @Override
    public void onTurnEvent(String from, String to) {
        runOnUiThread(() -> {
            if (tvBarPosture != null)
                tvBarPosture.setText(postureToChinese(from) + "→" + postureToChinese(to));
        });
    }

    @Override
    public void onFallEvent() {
        runOnUiThread(() -> {
            if (tvBarPosture != null)
                tvBarPosture.setText("跌倒!");
        });
    }

    @Override
    public void onNoTurnAlert(long durationMin) {
        runOnUiThread(() -> {
            if (tvBarPosture != null)
                tvBarPosture.setText("未翻身:" + durationMin + "分钟");
        });
    }
}
