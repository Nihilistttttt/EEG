package com.nihilisttt.eegdoctor;

import com.nihilisttt.eegdoctor.R;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.view.View;

import androidx.core.content.ContextCompat;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.Random;

public class PianoGameView extends View {
    private static final int LANE_COUNT = 4;
    private static final float TAP_LINE_Y_RATIO = 0.85f;
    private Paint notePaint, textPaint, lifePaint, speedPaint;
    private Paint canvasBgPaint, linePaint;
    private final ArrayList<RectF> notes = new ArrayList<>();
    private final Random random = new Random();
    private float noteSpeed = 300f;
    private float laneWidth;
    private float tapLineY;
    private int score = 0;
    private int lives = 5;
    private long lastNoteTime;
    private long spawnInterval = 800;

    public PianoGameView(Context context) {
        super(context);
        initGamePaints(context);
    }

    public PianoGameView(Context context, AttributeSet attrs) {
        super(context, attrs);
        initGamePaints(context);
    }

    public PianoGameView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        initGamePaints(context);
    }

    private void initGamePaints(Context context) {
        canvasBgPaint = new Paint();
        canvasBgPaint.setColor(ContextCompat.getColor(context, R.color.surface_base));
        canvasBgPaint.setStyle(Paint.Style.FILL);

        notePaint = new Paint();
        notePaint.setColor(ContextCompat.getColor(context, R.color.game_note));
        notePaint.setStyle(Paint.Style.FILL);
        notePaint.setAntiAlias(true);

        linePaint = new Paint();
        linePaint.setColor(ContextCompat.getColor(context, R.color.game_lane_divider));
        linePaint.setStrokeWidth(2);
        linePaint.setStyle(Paint.Style.STROKE);

        textPaint = new Paint();
        textPaint.setColor(ContextCompat.getColor(context, R.color.game_score));
        textPaint.setTextSize(50);
        textPaint.setAntiAlias(true);

        lifePaint = new Paint();
        lifePaint.setColor(ContextCompat.getColor(context, R.color.game_lives));
        lifePaint.setTextSize(40);
        lifePaint.setAntiAlias(true);

        speedPaint = new Paint();
        speedPaint.setColor(ContextCompat.getColor(context, R.color.game_speed));
        speedPaint.setTextSize(40);
        speedPaint.setAntiAlias(true);
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);
        laneWidth = (float) w / LANE_COUNT;
        tapLineY = h * TAP_LINE_Y_RATIO;
    }

    public void updateSpeed(float ema) {
        noteSpeed = mapEmaToSpeed(ema);
        spawnInterval = (long) (1200 - noteSpeed * 2);
        if (spawnInterval < 400) spawnInterval = 400;
        if (spawnInterval > 1500) spawnInterval = 1500;
    }

    public void tapLane(int lane) {
        if (lane < 0 || lane >= LANE_COUNT) return;
        boolean hit = false;
        RectF laneRect = new RectF(lane * laneWidth, tapLineY - 50,
                (lane + 1) * laneWidth, tapLineY + 50);
        Iterator<RectF> it = notes.iterator();
        while (it.hasNext()) {
            RectF note = it.next();
            if (RectF.intersects(note, laneRect)) {
                it.remove();
                score += 10;
                hit = true;
                break;
            }
        }
        if (!hit) { lives--; if (lives < 0) lives = 0; }
        invalidate();
    }

    public void gameUpdate() {
        long now = System.currentTimeMillis();
        if (now - lastNoteTime > spawnInterval) {
            lastNoteTime = now;
            int lane = random.nextInt(LANE_COUNT);
            float left = lane * laneWidth;
            notes.add(new RectF(left, -100, left + laneWidth, -100 + laneWidth));
        }
        float dt = 0.033f;
        Iterator<RectF> it = notes.iterator();
        while (it.hasNext()) {
            RectF note = it.next();
            note.top += noteSpeed * dt;
            note.bottom += noteSpeed * dt;
            if (note.top > getHeight()) { it.remove(); lives--; if (lives < 0) lives = 0; }
        }
        if (lives <= 0) { score = 0; lives = 5; notes.clear(); }
        invalidate();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        canvas.drawRect(0, 0, getWidth(), getHeight(), canvasBgPaint);

        Paint tapPaint = new Paint();
        tapPaint.setColor(ContextCompat.getColor(getContext(), R.color.game_tap_line));
        tapPaint.setStrokeWidth(3);

        for (int i = 1; i < LANE_COUNT; i++) {
            float x = i * laneWidth;
            canvas.drawLine(x, 0, x, getHeight(), linePaint);
        }
        canvas.drawLine(0, tapLineY, getWidth(), tapLineY, tapPaint);
        for (RectF note : notes) canvas.drawRoundRect(note, 4f, 4f, notePaint);
        canvas.drawText("Score: " + score, 20, 60, textPaint);
        canvas.drawText("Lives: " + lives, 20, 110, lifePaint);
        canvas.drawText("Speed: " + (int) noteSpeed, 20, 160, speedPaint);
    }

    private float mapEmaToSpeed(float ema) {
        float maxSpeed = 600f, minSpeed = 100f;
        float normalized = Math.max(0, Math.min(1, (ema - 0.05f) / 0.25f));
        return maxSpeed - normalized * (maxSpeed - minSpeed);
    }
}