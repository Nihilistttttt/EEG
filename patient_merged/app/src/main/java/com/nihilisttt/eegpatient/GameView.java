package com.nihilisttt.eegpatient;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.PorterDuff;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.view.Choreographer;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Random;

public class GameView extends SurfaceView implements SurfaceHolder.Callback, Choreographer.FrameCallback {

    public static final int LANE_COUNT = 3;
    private static final float GAME_DURATION_S = 60f;
    private static final float BASE_SPEED_MPS = 4.0f;
    private static final float MAX_BOOST_MPS = 8.0f;
    private static final float COIN_HIT_RADIUS_M = 2.0f;
    private static final float VIEW_RANGE_M = 40.0f;
    private static final float COIN_SPACING_MIN = 4.0f;
    private static final float COIN_SPACING_MAX = 10.0f;
    private static final int COIN_VALUE = 10;

    public interface GameCallback {
        void onGameOver(int distance, int coins, int score);
    }

    enum State { IDLE, PLAYING, GAME_OVER }

    private State state = State.IDLE;
    private GameCallback callback;

    private float attention = 0f;
    private int miDirection = 0;

    private int playerLane = 1;
    private float worldY = 0f;
    private int coins = 0;
    private float timeLeft = GAME_DURATION_S;
    private float nextCoinY = 5f;

    private final List<Coin> coinList = new ArrayList<>();
    private final Random rng = new Random();

    private final Bitmap[] walkFrames = new Bitmap[4];
    private int walkFrameIndex = 0;
    private float walkTimer = 0f;
    private static final float WALK_FRAME_INTERVAL = 0.15f;
    private Bitmap coinBmp;

    private final Paint pLaneBg = new Paint();
    private final Paint pLaneBgActive = new Paint();
    private final Paint pLaneLine = new Paint();
    private final Paint pMile = new Paint();
    private final Paint pPlayer = new Paint();
    private final Paint pCoin = new Paint();
    private final Paint pTimeBar = new Paint();
    private final Paint pTimeBarBg = new Paint();
    private final Paint pText = new Paint(Paint.ANTI_ALIAS_FLAG);
    private final Paint pTextScore = new Paint(Paint.ANTI_ALIAS_FLAG);
    private final Paint pTextDist = new Paint(Paint.ANTI_ALIAS_FLAG);
    private final Paint pTextCoin = new Paint(Paint.ANTI_ALIAS_FLAG);
    private final Paint pOverlay = new Paint();
    private final Paint pOverText = new Paint(Paint.ANTI_ALIAS_FLAG);

    private int viewW, viewH;
    private boolean running = false;

    public GameView(Context context) { super(context); init(); }
    public GameView(Context context, AttributeSet attrs) { super(context, attrs); init(); }
    public GameView(Context context, AttributeSet attrs, int defStyle) { super(context, attrs, defStyle); init(); }

    private void init() {
        getHolder().addCallback(this);
        setZOrderOnTop(true);
        getHolder().setFormat(android.graphics.PixelFormat.RGBA_8888);

        walkFrames[0] = BitmapFactory.decodeResource(getContext().getResources(), R.drawable.player_walk_0);
        walkFrames[1] = BitmapFactory.decodeResource(getContext().getResources(), R.drawable.player_walk_1);
        walkFrames[2] = BitmapFactory.decodeResource(getContext().getResources(), R.drawable.player_walk_2);
        walkFrames[3] = BitmapFactory.decodeResource(getContext().getResources(), R.drawable.player_walk_3);
        coinBmp = BitmapFactory.decodeResource(getContext().getResources(), R.drawable.coin);

        pLaneBg.setColor(0xFF0C0E14);
        pLaneBgActive.setColor(0xFF141C28);
        pLaneLine.setColor(0xFF323C46);
        pMile.setColor(0xFF464650);
        pPlayer.setColor(0xFF1A73E8);
        pCoin.setColor(0xFFFFB400);
        pTimeBar.setColor(0xFFFF5050);
        pTimeBarBg.setColor(0xFF281414);
        pText.setColor(0xFFFFFFFF);
        pTextScore.setColor(0xFFFFC800);
        pTextDist.setColor(0xFF64FF64);
        pTextCoin.setColor(0xFFFFB400);
        pOverlay.setColor(0xCC000000);
        pOverText.setColor(0xFFFF6464);
    }

    public void setCallback(GameCallback cb) { this.callback = cb; }

    public void setAttention(float attn) {
        this.attention = Math.max(0f, Math.min(1f, attn));
    }

    public void setMiDirection(int dir) {
        this.miDirection = dir;
    }

    public void startGame() {
        state = State.PLAYING;
        worldY = 0f;
        coins = 0;
        timeLeft = GAME_DURATION_S;
        nextCoinY = 5f;
        playerLane = 1;
        walkFrameIndex = 0;
        walkTimer = 0f;
        coinList.clear();
    }

    public void stopGame() {
        if (state == State.PLAYING) {
            state = State.GAME_OVER;
            if (callback != null) {
                int score = (int)worldY + coins * COIN_VALUE;
                callback.onGameOver((int)worldY, coins, score);
            }
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        running = true;
        Choreographer.getInstance().postFrameCallback(this);
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        viewW = width;
        viewH = height;
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        running = false;
        Choreographer.getInstance().removeFrameCallback(this);
    }

    @Override
    public void doFrame(long frameTimeNanos) {
        if (running) {
            Choreographer.getInstance().postFrameCallback(this);
            update();
            draw();
        }
    }

    private float playerSy() { return viewH * 0.8f; }
    private float mpp() { return viewH * 0.72f / VIEW_RANGE_M; }

    private void update() {
        if (state != State.PLAYING) return;
        float dt = 1f / 60f;

        walkTimer += dt;
        if (walkTimer >= WALK_FRAME_INTERVAL) {
            walkTimer = 0f;
            walkFrameIndex = (walkFrameIndex + 1) % 4;
        }

        float speed = BASE_SPEED_MPS + attention * MAX_BOOST_MPS;
        worldY += speed * dt;
        timeLeft -= dt;
        if (timeLeft <= 0f) {
            timeLeft = 0f;
            stopGame();
            return;
        }

        if (miDirection == 1 && playerLane > 0) {
            playerLane--;
            miDirection = 0;
        } else if (miDirection == 2 && playerLane < LANE_COUNT - 1) {
            playerLane++;
            miDirection = 0;
        }

        while (nextCoinY < worldY + VIEW_RANGE_M) {
            int lane = (int)((nextCoinY * 37f) % LANE_COUNT);
            if (lane < 0) lane += LANE_COUNT;
            coinList.add(new Coin(nextCoinY, lane));
            nextCoinY += COIN_SPACING_MIN + rng.nextFloat() * (COIN_SPACING_MAX - COIN_SPACING_MIN);
        }

        Iterator<Coin> ci = coinList.iterator();
        while (ci.hasNext()) {
            Coin c = ci.next();
            float dy = c.worldY - worldY;
            if (dy < 0f) dy = -dy;
            if (dy < COIN_HIT_RADIUS_M && c.lane == playerLane) {
                coins++;
                ci.remove();
            } else if (c.worldY < worldY - 5f) {
                ci.remove();
            }
        }
    }

    private float laneCenterX(int lane) {
        float laneW = viewW / (float)LANE_COUNT;
        return laneW * (lane + 0.5f);
    }

    private void draw() {
        SurfaceHolder holder = getHolder();
        Canvas canvas = holder.lockCanvas();
        if (canvas == null) return;
        try {
            canvas.drawColor(0xFF000000, PorterDuff.Mode.SRC);

            if (state == State.IDLE) {
                drawIdle(canvas);
                return;
            }

            drawLanes(canvas);
            drawMileMarkers(canvas);
            drawCoins(canvas);
            drawPlayer(canvas);
            drawTimeBar(canvas);
            drawHud(canvas);

            if (state == State.GAME_OVER) drawGameOver(canvas);
        } finally {
            holder.unlockCanvasAndPost(canvas);
        }
    }

    private void drawLanes(Canvas canvas) {
        float laneW = viewW / (float)LANE_COUNT;
        for (int i = 0; i < LANE_COUNT; i++) {
            Paint p = (i == playerLane) ? pLaneBgActive : pLaneBg;
            canvas.drawRect(i * laneW, 0, (i + 1) * laneW, viewH, p);
        }
        for (int i = 1; i < LANE_COUNT; i++) {
            canvas.drawLine(i * laneW, 0, i * laneW, viewH, pLaneLine);
        }
    }

    private void drawMileMarkers(Canvas canvas) {
        float ps = playerSy();
        float m = mpp();
        int startM = ((int)worldY / 10) * 10 - 10;
        int endM = (int)worldY + 45;
        float markerW = viewW * 0.06f;
        for (int mi = startM; mi <= endM; mi += 10) {
            if (mi < 0) continue;
            float sy = ps - (mi - worldY) * m;
            if (sy < 0 || sy > viewH) continue;
            canvas.drawLine(0, sy, markerW, sy, pMile);
        }
    }

    private void drawCoins(Canvas canvas) {
        if (coinBmp == null) return;
        float ps = playerSy();
        float m = mpp();
        float dstH = viewH * 0.08f;
        float dstW = dstH * coinBmp.getWidth() / coinBmp.getHeight();
        for (Coin c : coinList) {
            float sy = ps - (c.worldY - worldY) * m;
            if (sy < -dstH || sy > viewH + dstH) continue;
            float sx = laneCenterX(c.lane);
            RectF dst = new RectF(sx - dstW * 0.5f, sy - dstH * 0.5f, sx + dstW * 0.5f, sy + dstH * 0.5f);
            canvas.drawBitmap(coinBmp, null, dst, null);
        }
    }

    private void drawPlayer(Canvas canvas) {
        float sx = laneCenterX(playerLane);
        Bitmap bmp = walkFrames[walkFrameIndex];
        if (bmp == null) return;
        float dstH = viewH * 0.36f;
        float dstW = dstH * bmp.getWidth() / bmp.getHeight();
        RectF dst = new RectF(sx - dstW * 0.5f, viewH - dstH, sx + dstW * 0.5f, viewH);
        canvas.drawBitmap(bmp, null, dst, null);
    }

    private void drawTimeBar(Canvas canvas) {
        float barH = viewH * 0.018f;
        canvas.drawRect(0, 0, viewW, barH, pTimeBarBg);
        float progress = timeLeft / GAME_DURATION_S;
        canvas.drawRect(0, 0, viewW * progress, barH, pTimeBar);
    }

    private void drawHud(Canvas canvas) {
        float pad = viewH * 0.015f;
        float textSize = viewH * 0.035f;
        float x = viewW - pad;
        float y = pad + textSize;

        pText.setTextSize(textSize);
        pText.setTextAlign(Paint.Align.RIGHT);
        pTextScore.setTextSize(textSize);
        pTextScore.setTextAlign(Paint.Align.RIGHT);
        pTextDist.setTextSize(textSize);
        pTextDist.setTextAlign(Paint.Align.RIGHT);
        pTextCoin.setTextSize(textSize);
        pTextCoin.setTextAlign(Paint.Align.RIGHT);

        int timeLeftInt = (int)Math.ceil(timeLeft);
        int score = (int)worldY + coins * COIN_VALUE;
        int dist = (int)worldY;

        canvas.drawText(timeLeftInt + "s", x, y, pText);
        y += textSize * 1.4f;
        canvas.drawText(String.valueOf(score), x, y, pTextScore);
        y += textSize * 1.4f;
        canvas.drawText(dist + "m", x, y, pTextDist);
        y += textSize * 1.4f;
        canvas.drawText(String.valueOf(coins), x, y, pTextCoin);
    }

    private void drawIdle(Canvas canvas) {
        canvas.drawRect(0, 0, viewW, viewH, pOverlay);
        pOverText.setTextSize(viewH * 0.06f);
        pOverText.setTextAlign(Paint.Align.CENTER);
        pOverText.setColor(0xFFFFFFFF);
        canvas.drawText("等待开始游戏...", viewW * 0.5f, viewH * 0.5f, pOverText);
    }

    private void drawGameOver(Canvas canvas) {
        canvas.drawRect(0, 0, viewW, viewH, pOverlay);
        pOverText.setColor(0xFFFF6464);
        pOverText.setTextSize(viewH * 0.08f);
        pOverText.setTextAlign(Paint.Align.CENTER);
        canvas.drawText("GAME OVER", viewW * 0.5f, viewH * 0.4f, pOverText);

        int score = (int)worldY + coins * COIN_VALUE;
        pTextScore.setTextSize(viewH * 0.05f);
        pTextScore.setTextAlign(Paint.Align.CENTER);
        canvas.drawText("SCORE " + score, viewW * 0.5f, viewH * 0.52f, pTextScore);

        pTextDist.setTextSize(viewH * 0.035f);
        pTextDist.setTextAlign(Paint.Align.CENTER);
        canvas.drawText((int)worldY + "m   " + coins + " coins", viewW * 0.5f, viewH * 0.6f, pTextDist);
    }

    private static class Coin {
        float worldY;
        int lane;
        Coin(float worldY, int lane) { this.worldY = worldY; this.lane = lane; }
    }
}
