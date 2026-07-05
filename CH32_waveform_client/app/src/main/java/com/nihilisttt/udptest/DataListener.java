// DataListener.java
package com.nihilisttt.udptest;

public interface DataListener {
    void onWaveData(int cmd, float ch0, float ch1);
    void onSpectrumData(int cmd, float[] mags);
    void onFocusData(float attn0, float attn1, float ema0, float ema1, int trend, int instant);
}