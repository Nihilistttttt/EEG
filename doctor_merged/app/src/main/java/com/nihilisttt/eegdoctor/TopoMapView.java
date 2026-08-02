package com.nihilisttt.eegdoctor;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.view.View;

import androidx.core.content.ContextCompat;

public class TopoMapView extends View {

    private static final int NUM_CH = 8;
    private static final int GRID_SIZE = 64;
    private static final float IDW_POWER = 2.0f;
    private static final int INVALIDATE_INTERVAL_MS = 50;
    private static final float EMA_ALPHA = 0.01f;

    private static final float[] ELECTRODE_X = {
        0.00f, -0.25f, -0.37f, 0.37f, -0.37f, 0.37f, -0.50f, 0.50f
    };
    private static final float[] ELECTRODE_Y = {
        -0.70f, -0.65f, 0.55f, 0.55f, -0.30f, -0.30f, 0.00f, 0.00f
    };

    private static final int[] WAVE_COLOR_RES = {
        R.color.wave_ch0, R.color.wave_ch1, R.color.wave_ch2, R.color.wave_ch3,
        R.color.wave_ch4, R.color.wave_ch5, R.color.wave_ch6, R.color.wave_ch7
    };

    private final float[][][] idwWeights = new float[GRID_SIZE][GRID_SIZE][NUM_CH];
    private final boolean[][] insideHead = new boolean[GRID_SIZE][GRID_SIZE];

    private final float[] channelAmplitude = new float[NUM_CH];
    private final float[] emaSquared = new float[NUM_CH];

    private int[] colorBuffer;
    private Bitmap topoBitmap;
    private long lastInvalidateTime = 0;

    private float amplitudeMaxUv = 50f;

    private Paint headOutlinePaint;
    private Paint nosePaint;
    private Paint earPaint;
    private Paint electrodeFillPaint;
    private Paint electrodeStrokePaint;
    private Paint electrodeLabelPaint;
    private Paint colorBarPaint;
    private Paint colorBarLabelPaint;
    private Paint colorBarBorderPaint;

    private boolean paintsInitialized = false;

    public TopoMapView(Context context, AttributeSet attrs) {
        super(context, attrs);
        precomputeIdwWeights();
    }

    private void initPaints() {
        if (paintsInitialized) return;
        paintsInitialized = true;

        headOutlinePaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        headOutlinePaint.setStyle(Paint.Style.STROKE);
        headOutlinePaint.setStrokeWidth(2.5f);
        headOutlinePaint.setColor(ContextCompat.getColor(getContext(), R.color.text_secondary));

        nosePaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        nosePaint.setStyle(Paint.Style.STROKE);
        nosePaint.setStrokeWidth(2f);
        nosePaint.setColor(ContextCompat.getColor(getContext(), R.color.text_secondary));

        earPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        earPaint.setStyle(Paint.Style.STROKE);
        earPaint.setStrokeWidth(2f);
        earPaint.setColor(ContextCompat.getColor(getContext(), R.color.text_secondary));

        electrodeFillPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        electrodeFillPaint.setStyle(Paint.Style.FILL);

        electrodeStrokePaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        electrodeStrokePaint.setStyle(Paint.Style.STROKE);
        electrodeStrokePaint.setStrokeWidth(1.5f);
        electrodeStrokePaint.setColor(0xFFFFFFFF);

        electrodeLabelPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        electrodeLabelPaint.setTextSize(11f);
        electrodeLabelPaint.setColor(ContextCompat.getColor(getContext(), R.color.text_primary));
        electrodeLabelPaint.setTextAlign(Paint.Align.CENTER);

        colorBarPaint = new Paint(Paint.ANTI_ALIAS_FLAG);

        colorBarLabelPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        colorBarLabelPaint.setTextSize(9f);
        colorBarLabelPaint.setColor(ContextCompat.getColor(getContext(), R.color.text_secondary));

        colorBarBorderPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        colorBarBorderPaint.setStyle(Paint.Style.STROKE);
        colorBarBorderPaint.setStrokeWidth(1f);
        colorBarBorderPaint.setColor(ContextCompat.getColor(getContext(), R.color.surface_overlay));
    }

    private void precomputeIdwWeights() {
        for (int gy = 0; gy < GRID_SIZE; gy++) {
            for (int gx = 0; gx < GRID_SIZE; gx++) {
                float nx = 2.0f * gx / (GRID_SIZE - 1) - 1.0f;
                float ny = 1.0f - 2.0f * gy / (GRID_SIZE - 1);

                if (nx * nx + ny * ny > 1.0f) {
                    insideHead[gx][gy] = false;
                    continue;
                }
                insideHead[gx][gy] = true;

                float sumW = 0;
                for (int ch = 0; ch < NUM_CH; ch++) {
                    float dx = nx - ELECTRODE_X[ch];
                    float dy = ny - ELECTRODE_Y[ch];
                    float dist2 = dx * dx + dy * dy;
                    if (dist2 < 1e-6f) dist2 = 1e-6f;
                    float w = 1.0f / (float) Math.pow(dist2, IDW_POWER / 2.0f);
                    idwWeights[gx][gy][ch] = w;
                    sumW += w;
                }
                for (int ch = 0; ch < NUM_CH; ch++) {
                    idwWeights[gx][gy][ch] /= sumW;
                }
            }
        }
    }

    public void addSample(int ch, float val) {
        if (ch < 0 || ch >= NUM_CH) return;
        float valUv = Math.abs(val) * 1_000_000f;
        emaSquared[ch] = EMA_ALPHA * valUv * valUv + (1 - EMA_ALPHA) * emaSquared[ch];
        channelAmplitude[ch] = (float) Math.sqrt(emaSquared[ch]);

        long now = System.currentTimeMillis();
        if (now - lastInvalidateTime >= INVALIDATE_INTERVAL_MS) {
            lastInvalidateTime = now;
            postInvalidate();
        }
    }

    public void setAmplitudeMaxUv(float maxUv) {
        this.amplitudeMaxUv = maxUv;
        postInvalidate();
    }

    public float getAmplitudeMaxUv() {
        return amplitudeMaxUv;
    }

    public void clear() {
        for (int i = 0; i < NUM_CH; i++) {
            emaSquared[i] = 0;
            channelAmplitude[i] = 0;
        }
        postInvalidate();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        initPaints();

        int w = getWidth();
        int h = getHeight();
        if (w <= 0 || h <= 0) return;

        float density = getResources().getDisplayMetrics().density;
        int colorBarWidth = (int) (44 * density);
        int topoAreaWidth = w - colorBarWidth;
        int topoSize = Math.min(topoAreaWidth, h);
        float cx = topoAreaWidth / 2.0f;
        float cy = h / 2.0f;
        float radius = topoSize / 2.0f - 10 * density;

        drawTopoMap(canvas, cx, cy, radius);
        drawHeadOutline(canvas, cx, cy, radius);
        drawElectrodes(canvas, cx, cy, radius);
        drawColorBar(canvas, w - colorBarWidth, 0, colorBarWidth, h, density);
    }

    private void drawTopoMap(Canvas canvas, float cx, float cy, float radius) {
        int size = GRID_SIZE;
        if (colorBuffer == null || colorBuffer.length != size * size) {
            colorBuffer = new int[size * size];
            if (topoBitmap != null) topoBitmap.recycle();
            topoBitmap = Bitmap.createBitmap(size, size, Bitmap.Config.ARGB_8888);
        }

        float range = amplitudeMaxUv;
        if (range < 0.1f) range = 1f;

        for (int gy = 0; gy < size; gy++) {
            for (int gx = 0; gx < size; gx++) {
                int idx = gy * size + gx;
                if (!insideHead[gx][gy]) {
                    colorBuffer[idx] = 0x00000000;
                    continue;
                }
                float val = 0;
                for (int ch = 0; ch < NUM_CH; ch++) {
                    val += idwWeights[gx][gy][ch] * channelAmplitude[ch];
                }
                float normalized = val / range;
                colorBuffer[idx] = valueToColor(normalized);
            }
        }
        topoBitmap.setPixels(colorBuffer, 0, size, 0, 0, size, size);

        float left = cx - radius;
        float top = cy - radius;
        RectF dst = new RectF(left, top, left + 2 * radius, top + 2 * radius);
        Paint bitmapPaint = new Paint(Paint.ANTI_ALIAS_FLAG | Paint.FILTER_BITMAP_FLAG);
        canvas.drawBitmap(topoBitmap, null, dst, bitmapPaint);
    }

    private void drawHeadOutline(Canvas canvas, float cx, float cy, float radius) {
        canvas.drawCircle(cx, cy, radius, headOutlinePaint);

        Path nosePath = new Path();
        float noseW = radius * 0.12f;
        float noseH = radius * 0.10f;
        nosePath.moveTo(cx - noseW, cy - radius);
        nosePath.lineTo(cx, cy - radius - noseH);
        nosePath.lineTo(cx + noseW, cy - radius);
        canvas.drawPath(nosePath, nosePaint);

        float earW = radius * 0.06f;
        float earH = radius * 0.15f;
        canvas.drawRect(cx - radius - earW, cy - earH, cx - radius, cy + earH, earPaint);
        canvas.drawRect(cx + radius, cy - earH, cx + radius + earW, cy + earH, earPaint);
    }

    private void drawElectrodes(Canvas canvas, float cx, float cy, float radius) {
        float density = getResources().getDisplayMetrics().density;
        float dotRadius = 5 * density;
        for (int ch = 0; ch < NUM_CH; ch++) {
            float ex = cx + ELECTRODE_X[ch] * radius;
            float ey = cy - ELECTRODE_Y[ch] * radius;

            electrodeFillPaint.setColor(ContextCompat.getColor(getContext(), WAVE_COLOR_RES[ch]));
            canvas.drawCircle(ex, ey, dotRadius, electrodeFillPaint);
            canvas.drawCircle(ex, ey, dotRadius, electrodeStrokePaint);

            electrodeLabelPaint.setColor(ContextCompat.getColor(getContext(), WAVE_COLOR_RES[ch]));
            canvas.drawText(EegChannels.NAMES[ch], ex, ey - dotRadius - 3 * density, electrodeLabelPaint);
        }
    }

    private void drawColorBar(Canvas canvas, int left, int top, int width, int height, float density) {
        int barLeft = left + (int) (8 * density);
        int barWidth = (int) (14 * density);
        int barTop = top + (int) (36 * density);
        int barBottom = height - (int) (36 * density);
        int barHeight = barBottom - barTop;
        if (barHeight <= 0) return;

        for (int y = barTop; y < barBottom; y++) {
            float normalized = 1.0f - (float) (y - barTop) / barHeight;
            colorBarPaint.setColor(valueToColor(normalized) | 0xFF000000);
            canvas.drawRect(barLeft, y, barLeft + barWidth, y + 1, colorBarPaint);
        }

        canvas.drawRect(barLeft, barTop, barLeft + barWidth, barBottom, colorBarBorderPaint);

        colorBarLabelPaint.setTextAlign(Paint.Align.LEFT);
        int labelX = barLeft + barWidth + (int) (3 * density);

        String maxLabel = formatUv(amplitudeMaxUv);
        String midLabel = formatUv(amplitudeMaxUv / 2);
        String minLabel = "0";

        canvas.drawText(maxLabel, labelX, barTop + (int) (4 * density), colorBarLabelPaint);
        canvas.drawText(midLabel, labelX, (barTop + barBottom) / 2 + (int) (3 * density), colorBarLabelPaint);
        canvas.drawText(minLabel, labelX, barBottom, colorBarLabelPaint);
    }

    private static String formatUv(float val) {
        if (val >= 1000f) return String.format("%.1fmV", val / 1000f);
        if (val >= 1f) return String.format("%.0fμV", val);
        return String.format("%.1fμV", val);
    }

    private static int valueToColor(float t) {
        if (t < 0) t = 0;
        if (t > 1) t = 1;

        float r, g, b;
        if (t < 0.25f) {
            float s = t / 0.25f;
            r = 0;
            g = s;
            b = 1;
        } else if (t < 0.5f) {
            float s = (t - 0.25f) / 0.25f;
            r = 0;
            g = 1;
            b = 1 - s;
        } else if (t < 0.75f) {
            float s = (t - 0.5f) / 0.25f;
            r = s;
            g = 1;
            b = 0;
        } else {
            float s = (t - 0.75f) / 0.25f;
            r = 1;
            g = 1 - s;
            b = 0;
        }

        int alpha = 210;
        return (alpha << 24) | ((int) (r * 255) << 16) | ((int) (g * 255) << 8) | (int) (b * 255);
    }
}