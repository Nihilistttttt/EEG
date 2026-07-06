package com.nihilisttt.eegdoctor;

public interface DataListener {
    void onWaveData(int cmd, float ch0, float ch1);
    void onSpectrumData(int cmd, float[] mags);
    void onFocusData(float attn0, float attn1, float ema0, float ema1, int trend, int instant);
    default void onEegFrame(EegFrame frame) {}
    default void onInferenceResult(InferenceResult result) {}
    default void onIpcDiag(IpcDiagInfo diag) {}
    default void onDirConfig(String configJson) {}
}