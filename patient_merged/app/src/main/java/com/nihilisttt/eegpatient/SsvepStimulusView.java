package com.nihilisttt.eegpatient;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.Choreographer;
import android.view.View;

/** Single-target stimulus driven by display vsync rather than Handler millisecond timers. */
public class SsvepStimulusView extends View implements Choreographer.FrameCallback {

    public interface OnFirstFrameListener {
        void onFirstStimulusFrame(long frameTimeNanos);
    }

    private boolean bright = true;
    private boolean blinking;
    private String arrowText = "";
    private int arrowColor = Color.WHITE;
    private float freqHz;
    private long startFrameNanos;
    private boolean firstFrameReported;
    private OnFirstFrameListener firstFrameListener;

    private final Paint bgPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
    private final Paint arrowPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
    private final Paint freqPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
    private final Paint hintPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
    private final Rect textBounds = new Rect();

    public SsvepStimulusView(Context context) { super(context); init(); }
    public SsvepStimulusView(Context context, AttributeSet attrs) { super(context, attrs); init(); }
    public SsvepStimulusView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr); init();
    }

    private void init() {
        setKeepScreenOn(true);
        arrowPaint.setTextAlign(Paint.Align.CENTER);
        arrowPaint.setFakeBoldText(true);
        freqPaint.setTextAlign(Paint.Align.CENTER);
        freqPaint.setColor(0xFFB0B0B0);
        hintPaint.setTextAlign(Paint.Align.CENTER);
        hintPaint.setColor(0xFFB0B0B0);
    }

    public void setArrow(String arrow, int color) {
        arrowText = arrow == null ? "" : arrow;
        arrowColor = color;
        invalidate();
    }

    public void setFreqHz(float freq) {
        freqHz = Math.max(0f, freq);
        invalidate();
    }

    public void setOnFirstFrameListener(OnFirstFrameListener listener) {
        firstFrameListener = listener;
    }

    public void startStimulus() {
        if (blinking) return;
        blinking = true;
        bright = true;
        startFrameNanos = 0L;
        firstFrameReported = false;
        Choreographer.getInstance().postFrameCallback(this);
        invalidate();
    }

    public void stopStimulus() {
        blinking = false;
        startFrameNanos = 0L;
        firstFrameReported = false;
        Choreographer.getInstance().removeFrameCallback(this);
        bright = false;
        invalidate();
    }

    public boolean isBlinking() { return blinking; }

    @Override
    public void doFrame(long frameTimeNanos) {
        if (!blinking) return;
        if (startFrameNanos == 0L) startFrameNanos = frameTimeNanos;
        if (!firstFrameReported) {
            firstFrameReported = true;
            if (firstFrameListener != null) firstFrameListener.onFirstStimulusFrame(frameTimeNanos);
        }
        if (freqHz > 0f) {
            double elapsedSeconds = (frameTimeNanos - startFrameNanos) / 1_000_000_000.0;
            boolean newBright = (((long) Math.floor(elapsedSeconds * freqHz * 2.0)) & 1L) == 0L;
            if (newBright != bright) {
                bright = newBright;
                invalidate();
            }
        }
        Choreographer.getInstance().postFrameCallback(this);
    }

    @Override
    protected void onDetachedFromWindow() {
        stopStimulus();
        super.onDetachedFromWindow();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        int w = getWidth();
        int h = getHeight();
        bgPaint.setColor(blinking && bright ? Color.WHITE : Color.rgb(45, 45, 45));
        canvas.drawRect(0, 0, w, h, bgPaint);

        if (!blinking || arrowText.isEmpty()) return;
        float arrowSize = Math.min(w, h) * 0.42f;
        arrowPaint.setTextSize(arrowSize);
        arrowPaint.setColor(bright ? Color.BLACK : arrowColor);
        arrowPaint.getTextBounds(arrowText, 0, arrowText.length(), textBounds);
        float cx = w / 2f;
        float cy = h / 2f - (arrowPaint.ascent() + arrowPaint.descent()) / 2f;
        canvas.drawText(arrowText, cx, cy, arrowPaint);

        freqPaint.setTextSize(Math.max(30f, Math.min(w, h) * 0.055f));
        freqPaint.setColor(bright ? Color.DKGRAY : Color.LTGRAY);
        canvas.drawText(String.format(java.util.Locale.US, "%.0f Hz", freqHz),
                cx, cy + arrowSize * 0.55f, freqPaint);
        hintPaint.setTextSize(Math.max(24f, Math.min(w, h) * 0.038f));
        hintPaint.setColor(bright ? Color.DKGRAY : Color.LTGRAY);
        canvas.drawText("请持续注视闪烁箭头", cx,
                cy + arrowSize * 0.55f + hintPaint.getTextSize() * 1.4f, hintPaint);
    }
}
