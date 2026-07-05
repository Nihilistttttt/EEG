package com.nihilisttt.udptest;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;

public class HomeActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getSupportActionBar() != null) getSupportActionBar().hide();
        setContentView(R.layout.activity_home);

        findViewById(R.id.card_realtime).setOnClickListener(v -> launchMonitor(0));
        findViewById(R.id.card_focus).setOnClickListener(v -> launchMonitor(1));
        findViewById(R.id.card_wave_compare).setOnClickListener(v -> launchMonitor(2));
        findViewById(R.id.card_spectrum_compare).setOnClickListener(v -> launchMonitor(3));
        findViewById(R.id.card_training).setOnClickListener(v -> launchMonitor(4));
    }

    private void launchMonitor(int page) {
        Intent intent = new Intent(this, MainActivity.class);
        intent.putExtra("initial_page", page);
        startActivity(intent);
    }
}