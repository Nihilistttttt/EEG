package com.nihilisttt.eegdoctor;

import com.nihilisttt.eegdoctor.R;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.DashPathEffect;
import android.graphics.Paint;
import android.graphics.Path;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

import androidx.core.content.ContextCompat;

import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class FocusChartView extends View {
    private static final int MAX_POINTS = 5000;
    private static final int DEFAULT_X_MAX_POINTS = 500;

    private float mYRange = 1.0f;
    private int mLabelCount = 0;

    private int mXMaxPoints = DEFAULT_X_MAX_POINTS;

    private float[] buffer = new float[MAX_POINTS];
    private int writeIdx = 0;
    private int pointCount = 0;
    private Paint paint, glowPaint;
    private Lock lock = new ReentrantLock();

    private Paint axisPaint, gridPaint, textPaint;
    private Paint canvasBgPaint;

    private int focusColor;

    private static final int MODE_FIX = 0;
    private static final int MODE_TRACK = 1;
    private static final int MODE_SLOW_TRACK = 2;
    private int currentMode = MODE_FIX;

    private float fixCenterOffset = 0f;
    private float slowTargetCenter = 0f;
    private int slowOffsetCount = 0;
    private static final float SLOW_THRESHOLD_RATIO = 0.5f;
    private static final int SLOW_SETTLE_SAMPLES = 200;

    private float lastTouchY = 0;
    private boolean isDragging = false;

    public FocusChartView(Context context, AttributeSet attrs) {
        super(context, attrs);
        TypedArray a = context.obtainStyledAttributes(attrs, R.styleable.FocusChartView);
        focusColor = a.getColor(R.styleable.FocusChartView_focusColor,
                ContextCompat.getColor(context, R.color.focus_line));
        a.recycle();
        initPaints();
        setClickable(true);
        setOnClickListener(v -> {
            int newMode = (currentMode + 1) % 3;
            switchMode(newMode);
        });
    }

    private void initPaints() {
        int bgColor = ContextCompat.getColor(getContext(), R.color.surface_base);
        int gridColor = ContextCompat.getColor(getContext(), R.color.grid_line);
        int axisColor = ContextCompat.getColor(getContext(), R.color.axis_line);
        int textColor = ContextCompat.getColor(getContext(), R.color.text_secondary);

        canvasBgPaint = new Paint();
        canvasBgPaint.setColor(bgColor);
        canvasBgPaint.setStyle(Paint.Style.FILL);

        paint = new Paint();
        paint.setColor(focusColor);
        paint.setStrokeWidth(1.5f);
        paint.setStyle(Paint.Style.STROKE);
        paint.setAntiAlias(true);
        paint.setStrokeJoin(Paint.Join.ROUND);

        glowPaint = new Paint();
        glowPaint.setColor(focusColor);
        glowPaint.setStrokeWidth(5f);
        glowPaint.setStyle(Paint.Style.STROKE);
        glowPaint.setAntiAlias(true);
        glowPaint.setAlpha(35);
        glowPaint.setStrokeJoin(Paint.Join.ROUND);

        axisPaint = new Paint();
        axisPaint.setColor(axisColor);
        axisPaint.setStrokeWidth(1f);
        axisPaint.setStyle(Paint.Style.STROKE);

        gridPaint = new Paint();
        gridPaint.setColor(gridColor);
        gridPaint.setStrokeWidth(0.5f);
        gridPaint.setStyle(Paint.Style.STROKE);
        gridPaint.setPathEffect(new DashPathEffect(new float[]{4f, 4f}, 0));

        textPaint = new Paint();
        textPaint.setColor(textColor);
        textPaint.setTextSize(10f);
        textPaint.setAntiAlias(true);
    }

    private void switchMode(int newMode) {
        if (newMode == MODE_FIX) {
            fixCenterOffset = getCurrentWindowCenter();
        } else if (newMode == MODE_SLOW_TRACK) {
            slowTargetCenter = computeWindowAverage();
            slowOffsetCount = 0;
        }
        currentMode = newMode;
        invalidate();
    }

    private float getCurrentWindowCenter() {
        if (currentMode == MODE_FIX) return fixCenterOffset;
        if (currentMode == MODE_TRACK) return computeWindowAverage();
        return slowTargetCenter;
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (currentMode != MODE_FIX) return super.onTouchEvent(event);
        int width = getWidth(), height = getHeight();
        if (width <= 0 || height <= 0) return super.onTouchEvent(event);
        float left = 100f;
        float touchX = event.getX();
        if (touchX < left + 40) {
            switch (event.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    lastTouchY = event.getY();
                    isDragging = true;
                    return true;
                case MotionEvent.ACTION_MOVE:
                    if (isDragging) {
                        float deltaY = event.getY() - lastTouchY;
                        float top = 10f, bottom = height - 50f;
                        float yMin = fixCenterOffset - mYRange;
                        float yMax = fixCenterOffset + mYRange;
                        float yScale = (bottom - top) / (yMax - yMin);
                        float deltaVolt = deltaY / yScale;
                        fixCenterOffset += deltaVolt;
                        lastTouchY = event.getY();
                        invalidate();
                    }
                    return true;
                case MotionEvent.ACTION_UP:
                case MotionEvent.ACTION_CANCEL:
                    isDragging = false;
                    return true;
            }
        }
        return super.onTouchEvent(event);
    }

    public void setYRange(float range) {
        if (range > 0) {
            mYRange = range;
            slowTargetCenter = computeWindowAverage();
            slowOffsetCount = 0;
            invalidate();
        }
    }

    public float getYRange() { return mYRange; }

    public void setXMaxPoints(int points) {
        if (points > 0 && points <= MAX_POINTS) {
            mXMaxPoints = points;
            invalidate();
        }
    }

    public int getXMaxPoints() { return mXMaxPoints; }

    public void setLabelCount(int count) {
        if (count > 0) {
            mLabelCount = count;
            invalidate();
        }
    }

    public void addPoint(float value) {
        lock.lock();
        buffer[writeIdx] = value;
        writeIdx = (writeIdx + 1) % MAX_POINTS;
        if (pointCount < MAX_POINTS) pointCount++;
        lock.unlock();
        postInvalidate();
    }

    private float computeWindowAverage() {
        lock.lock();
        if (pointCount == 0) { lock.unlock(); return 0f; }
        int startIdx = (pointCount < mXMaxPoints) ? 0 : pointCount - mXMaxPoints;
        float sum = 0f;
        int count = 0;
        for (int i = startIdx; i < pointCount; i++) {
            int idx = (writeIdx - pointCount + i + MAX_POINTS) % MAX_POINTS;
            sum += buffer[idx];
            count++;
        }
        float avg = (count > 0) ? sum / count : 0f;
        lock.unlock();
        return avg;
    }

    private float niceNum(float range, boolean round) {
        float exponent = (float) Math.floor(Math.log10(range));
        float fraction = range / (float) Math.pow(10, exponent);
        float niceFraction;
        if (round) {
            if (fraction < 1.5) niceFraction = 1;
            else if (fraction < 3) niceFraction = 2;
            else if (fraction < 7) niceFraction = 5;
            else niceFraction = 10;
        } else {
            if (fraction <= 1) niceFraction = 1;
            else if (fraction <= 2) niceFraction = 2;
            else if (fraction <= 5) niceFraction = 5;
            else niceFraction = 10;
        }
        return niceFraction * (float) Math.pow(10, exponent);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        int width = getWidth(), height = getHeight();
        if (width <= 0 || height <= 0) return;

        float left = 100, top = 10, right = width - 10, bottom = height - 50;
        canvas.drawRect(0, 0, width, height, canvasBgPaint);

        float avg = computeWindowAverage();

        if (currentMode == MODE_SLOW_TRACK) {
            float threshold = mYRange * SLOW_THRESHOLD_RATIO;
            if (Math.abs(avg - slowTargetCenter) > threshold) {
                slowOffsetCount++;
                if (slowOffsetCount >= SLOW_SETTLE_SAMPLES) {
                    slowTargetCenter = avg;
                    slowOffsetCount = 0;
                }
            } else {
                slowOffsetCount = 0;
            }
        }

        final float centerVal;
        if (currentMode == MODE_FIX) centerVal = fixCenterOffset;
        else if (currentMode == MODE_TRACK) centerVal = avg;
        else centerVal = slowTargetCenter;

        final float yMin = centerVal - mYRange;
        final float yMax = centerVal + mYRange;
        float yScale = (bottom - top) / (yMax - yMin);
        float centerY = bottom - (centerVal - yMin) * yScale;

        int visibleStart = (pointCount < mXMaxPoints) ? 0 : pointCount - mXMaxPoints;
        int visibleCount = pointCount - visibleStart;
        float xScale = (right - left) / (visibleCount - 1 > 0 ? visibleCount - 1 : 1);

        float yTickSpacing;
        if (mLabelCount > 0) {
            yTickSpacing = (yMax - yMin) / (mLabelCount - 1);
        } else {
            yTickSpacing = niceNum(mYRange / 3.0f, true);
        }
        float yTick = (float) Math.floor(yMin / yTickSpacing) * yTickSpacing;
        while (yTick <= yMax + yTickSpacing * 0.5f) {
            float y = bottom - (yTick - yMin) * yScale;
            canvas.drawLine(left, y, right, y, gridPaint);
            canvas.drawLine(left - 5, y, left, y, axisPaint);
            String label = String.format("%.2f", yTick);
            float tw = textPaint.measureText(label);
            canvas.drawText(label, left - 15 - tw, y + 5, textPaint);
            yTick += yTickSpacing;
        }

        float xTickSpacing = niceNum(visibleCount / 5.0f, true);
        float xTickVal = (float) Math.floor(visibleStart / xTickSpacing) * xTickSpacing;
        while (xTickVal <= pointCount + xTickSpacing * 0.5f) {
            float x = left + (xTickVal - visibleStart) * xScale;
            if (x >= left && x <= right) {
                canvas.drawLine(x, top, x, bottom, gridPaint);
                canvas.drawLine(x, bottom, x, bottom + 5, axisPaint);
                String label = String.valueOf((int) xTickVal);
                canvas.drawText(label, x - 10, bottom + 25, textPaint);
            }
            xTickVal += xTickSpacing;
        }

        canvas.drawLine(left, top, left, bottom, axisPaint);
        canvas.drawLine(left, centerY, right, centerY, axisPaint);

        lock.lock();
        if (visibleCount > 1) {
            Path path = new Path();
            boolean first = true;
            for (int i = visibleStart; i < pointCount; i++) {
                int idx = (writeIdx - pointCount + i + MAX_POINTS) % MAX_POINTS;
                float val = buffer[idx];
                float x = left + (i - visibleStart) * xScale;
                float y = bottom - (val - yMin) * yScale;
                if (first) {
                    path.moveTo(x, y);
                    first = false;
                } else {
                    path.lineTo(x, y);
                }
            }
            canvas.drawPath(path, glowPaint);
            canvas.drawPath(path, paint);
        }
        lock.unlock();

        int modeColor = ContextCompat.getColor(getContext(), R.color.accent_info);
        textPaint.setColor(modeColor);
        textPaint.setTextSize(20f);
        String modeText;
        if (currentMode == MODE_FIX) modeText = "Fix";
        else if (currentMode == MODE_TRACK) modeText = "Track";
        else modeText = "Slow";
        float modeWidth = textPaint.measureText(modeText);
        canvas.drawText(modeText, right - modeWidth - 10, top + 25, textPaint);
    }
}
