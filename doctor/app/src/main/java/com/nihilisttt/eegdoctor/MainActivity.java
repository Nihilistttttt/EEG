package com.nihilisttt.eegdoctor;

import com.nihilisttt.eegdoctor.R;

import android.content.Intent;
import android.os.Bundle;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;
import androidx.viewpager2.adapter.FragmentStateAdapter;
import androidx.viewpager2.widget.ViewPager2;

import com.google.android.material.button.MaterialButton;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {

    private ViewPager2 viewPager;
    private TcpServerManager tcpServer;
    private final String[] pageTitles = {"脑电监测", "专注度", "波形对比", "频谱对比", "训练", "方向识别"};

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
        fragments.add(new TrainingFragment());
        fragments.add(new InferenceFragment());

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

        int initialPage = getIntent().getIntExtra("initial_page", 0);
        if (initialPage >= 0 && initialPage < fragments.size()) {
            viewPager.setCurrentItem(initialPage, false);
        }

        tcpServer = TcpServerManager.getInstance();
        tcpServer.start();

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
            }
        });

        updatePageTitle(viewPager.getCurrentItem());
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
        if (tcpServer != null) {
            tcpServer.stop();
        }
    }
}
