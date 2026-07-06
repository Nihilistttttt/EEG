package com.nihilisttt.eegdoctor;

import android.os.Handler;
import android.os.Looper;
import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;

public class DataDispatcher {
    private static final DataDispatcher INSTANCE = new DataDispatcher();
    private final Handler mainHandler = new Handler(Looper.getMainLooper());
    private final List<DataListener> listeners = new CopyOnWriteArrayList<>();

    public static DataDispatcher getInstance() { return INSTANCE; }

    public void addListener(DataListener listener) { listeners.add(listener); }
    public void removeListener(DataListener listener) { listeners.remove(listener); }

    public void postWaveData(int cmd, float ch0, float ch1) {
        mainHandler.post(() -> {
            for (DataListener l : listeners) l.onWaveData(cmd, ch0, ch1);
        });
    }

    public void postSpectrumData(int cmd, float[] mags) {
        mainHandler.post(() -> {
            for (DataListener l : listeners) l.onSpectrumData(cmd, mags);
        });
    }

    public void postFocusData(float attn0, float attn1, float ema0, float ema1, int trend, int instant) {
        mainHandler.post(() -> {
            for (DataListener l : listeners) l.onFocusData(attn0, attn1, ema0, ema1, trend, instant);
        });
    }

    public void postEegFrame(EegFrame frame) {
        mainHandler.post(() -> {
            for (DataListener l : listeners) l.onEegFrame(frame);
        });
    }

    public void postInferenceResult(InferenceResult result) {
        mainHandler.post(() -> {
            for (DataListener l : listeners) l.onInferenceResult(result);
        });
    }

    public void postIpcDiag(IpcDiagInfo diag) {
        mainHandler.post(() -> {
            for (DataListener l : listeners) l.onIpcDiag(diag);
        });
    }

    public void postDirConfig(String configJson) {
        mainHandler.post(() -> {
            for (DataListener l : listeners) l.onDirConfig(configJson);
        });
    }
}