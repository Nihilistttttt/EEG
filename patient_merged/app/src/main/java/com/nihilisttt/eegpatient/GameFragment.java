package com.nihilisttt.eegpatient;

import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

public class GameFragment extends Fragment implements DoctorConnector.DataListener {

    private static final String TAG = "GameFragment";
    private static final long START_DELAY_MS = 300L;

    private GameView gameView;
    private final Handler handler = new Handler(Looper.getMainLooper());
    private boolean gameStarted = false;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_game, container, false);
        gameView = root.findViewById(R.id.game_view);
        gameView.setCallback((distance, coins, score) ->
                Log.i(TAG, "game over: dist=" + distance + " coins=" + coins + " score=" + score));
        return root;
    }

    @Override
    public void onResume() {
        super.onResume();
        DoctorConnector.getInstance().removeListener(this);
        DoctorConnector.getInstance().addListener(this);
        startGameDelayed();
    }

    @Override
    public void onPause() {
        super.onPause();
        stopGame();
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        DoctorConnector.getInstance().removeListener(this);
        handler.removeCallbacksAndMessages(null);
        if (gameView != null) gameView.stopGame();
    }

    private void startGameDelayed() {
        handler.removeCallbacksAndMessages(null);
        handler.postDelayed(() -> {
            if (gameView != null && gameView.getHolder() != null && gameView.getHolder().getSurface() != null && gameView.getHolder().getSurface().isValid()) {
                gameView.startGame();
                gameStarted = true;
                Log.i(TAG, "game auto-started");
            } else {
                startGameDelayed();
            }
        }, START_DELAY_MS);
    }

    private void stopGame() {
        handler.removeCallbacksAndMessages(null);
        if (gameView != null && gameStarted) {
            gameView.stopGame();
            gameStarted = false;
        }
    }

    @Override
    public void onFocusData(float attn0, float attn1, float ema0, float ema1, int trend, int instant) {
        if (gameView != null) gameView.setAttention(attn0);
    }

    @Override
    public void onInferenceResult(InferenceResult result) {
        if (gameView == null || !gameStarted) return;
        String intent = result.getIntent();
        int dir;
        if ("LEFT".equalsIgnoreCase(intent)) dir = 1;
        else if ("RIGHT".equalsIgnoreCase(intent)) dir = 2;
        else dir = 0;
        if (dir != 0) gameView.setMiDirection(dir);
    }

    @Override public void onWaveData(int cmd, float ch0, float ch1) {}
    @Override public void onEegFrame(EegFrame frame) {}
    @Override public void onIpcDiag(IpcDiagInfo diag) {}
    @Override public void onConnectionChanged(boolean connected) {}
    @Override public void onTaskStart(String side) {}
    @Override public void onTaskDone() {}
    @Override public void onReadyTrain() {}
    @Override public void onReadyTest() {}
    @Override public void onModeSetOk(int mode) {}
    @Override public void onPageSwitch(int page) {}
    @Override public void onTargetDirection(String direction) {}
    @Override public void onTrainStop() {}
}