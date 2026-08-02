package com.nihilisttt.eegdoctor;

import com.nihilisttt.eegdoctor.R;

import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.os.Handler;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

public class GameFragment extends Fragment implements DataListener {

    private PianoGameView gameView;
    private Handler handler = new Handler();
    private Runnable gameRunnable;
    private volatile float currentEma0 = 0.0f;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_game, container, false);
        gameView = root.findViewById(R.id.gameView);

        root.findViewById(R.id.btn_lane0).setOnClickListener(v -> gameView.tapLane(0));
        root.findViewById(R.id.btn_lane1).setOnClickListener(v -> gameView.tapLane(1));
        root.findViewById(R.id.btn_lane2).setOnClickListener(v -> gameView.tapLane(2));
        root.findViewById(R.id.btn_lane3).setOnClickListener(v -> gameView.tapLane(3));

        root.findViewById(R.id.btn_back).setOnClickListener(v -> {
            if (getActivity() instanceof MainActivity) {
                ((MainActivity) getActivity()).switchToPage(1);
            }
        });

        return root;
    }

    @Override
    public void onResume() {
        super.onResume();
        requireActivity().setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
        DataDispatcher.getInstance().removeListener(this);
        DataDispatcher.getInstance().addListener(this);
        gameRunnable = new Runnable() {
            @Override
            public void run() {
                gameView.updateSpeed(currentEma0);
                gameView.gameUpdate();
                handler.postDelayed(this, 33);
            }
        };
        handler.post(gameRunnable);
    }

    @Override
    public void onPause() {
        super.onPause();
        DataDispatcher.getInstance().removeListener(this);
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        requireActivity().setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        DataDispatcher.getInstance().removeListener(this);
        if (handler != null && gameRunnable != null) {
            handler.removeCallbacks(gameRunnable);
        }
    }

    @Override
    public void onWaveData(int cmd, int ch, float val) {}
    @Override
    public void onSpectrumData(int cmd, float[] mags) {}
    @Override
    public void onFocusData(float attn0, float attn1, float ema0, float ema1,
                            int trend, int instant) {
        currentEma0 = ema0;
    }
}