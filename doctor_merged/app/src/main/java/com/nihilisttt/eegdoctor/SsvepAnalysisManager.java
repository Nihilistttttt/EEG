package com.nihilisttt.eegdoctor;

import android.os.SystemClock;
import android.util.Log;

import java.util.Locale;
import java.util.Random;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicLong;

/** Owns the FBCCA background thread and the optional no-device synthetic test. */
public final class SsvepAnalysisManager {
    private static final String TAG = "SSVEP_FBCCA";
    private static final SsvepAnalysisManager INSTANCE = new SsvepAnalysisManager();

    private final ExecutorService analysisExecutor = Executors.newSingleThreadExecutor(r -> {
        Thread t = new Thread(r, "doctor-fbcca-analysis");
        t.setDaemon(true);
        return t;
    });
    private final FbccaEngine engine = new FbccaEngine();
    private final AtomicLong sessionGeneration = new AtomicLong(0);

    private volatile boolean prepared;
    private volatile boolean running;
    private volatile boolean synthetic;
    private volatile int targetFreqIndex = -1;
    private volatile int selectedWaveCommand;
    private volatile long sessionStartedAtMs;
    private volatile int progressCounter;
    private Thread syntheticThread;

    private SsvepAnalysisManager() {}

    public static SsvepAnalysisManager getInstance() { return INSTANCE; }

    public void prepareSession(int freqIndex) {
        stopSyntheticThread();
        final long generation = sessionGeneration.incrementAndGet();
        analysisExecutor.execute(() -> {
            if (generation != sessionGeneration.get()) return;
            engine.reset();
            targetFreqIndex = freqIndex;
            selectedWaveCommand = 0;
            sessionStartedAtMs = System.currentTimeMillis();
            progressCounter = 0;
            synthetic = false;
            prepared = true;
            running = false;
            postProgress(SsvepProgress.State.PREPARED, 0, 0,
                    "已准备，等待患者端首个刺激帧");
        });
    }

    public void confirmStimulusStarted(int freqIndex, float refreshRate) {
        final long generation = sessionGeneration.get();
        analysisExecutor.execute(() -> {
            if (generation != sessionGeneration.get()) return;
            if (synthetic) {
                postProgress(SsvepProgress.State.COLLECTING, engine.getBufferedSamples(), -1,
                        String.format(Locale.US, "患者刺激已启动（屏幕 %.2f Hz）；算法自测继续运行", refreshRate));
                return;
            }
            if (!prepared || targetFreqIndex != freqIndex) {
                engine.reset();
                targetFreqIndex = freqIndex;
                prepared = true;
            }
            running = true;
            synthetic = false;
            selectedWaveCommand = 0;
            sessionStartedAtMs = System.currentTimeMillis();
            String msg = String.format(Locale.US,
                    "患者刺激已启动（屏幕 %.2f Hz），等待脑电波形", refreshRate);
            postProgress(SsvepProgress.State.WAITING_WAVE_SOURCE, 0, 0, msg);
        });
    }

    public void startSyntheticTest(int freqIndex) {
        stopSyntheticThread();
        final long generation = sessionGeneration.incrementAndGet();
        targetFreqIndex = freqIndex;
        prepared = true;
        running = true;
        synthetic = true;
        selectedWaveCommand = -1;
        sessionStartedAtMs = System.currentTimeMillis();
        analysisExecutor.execute(() -> {
            if (generation != sessionGeneration.get()) return;
            engine.reset();
            progressCounter = 0;
            postProgress(SsvepProgress.State.COLLECTING, 0, -1,
                    "算法自测：正在生成合成 SSVEP 数据");
        });

        syntheticThread = new Thread(() -> {
            Random random = new Random(20260717L + freqIndex);
            double freq = FbccaConfig.TARGET_FREQS[freqIndex];
            long next = SystemClock.elapsedRealtimeNanos();
            long periodNs = Math.round(1_000_000_000.0 / FbccaConfig.SAMPLE_RATE);
            long sampleIndex = 0;
            while (running && synthetic && generation == sessionGeneration.get()
                    && !Thread.currentThread().isInterrupted()) {
                double t = sampleIndex / FbccaConfig.SAMPLE_RATE;
                double signalMv = 0.020 * Math.sin(2.0 * Math.PI * freq * t)
                        + 0.010 * Math.sin(2.0 * Math.PI * 2.0 * freq * t + 0.25)
                        + 0.004 * Math.sin(2.0 * Math.PI * 9.0 * t)
                        + random.nextGaussian() * 0.006;
                offerSyntheticSample(signalMv, generation);
                sampleIndex++;
                next += periodNs;
                long sleepNs = next - SystemClock.elapsedRealtimeNanos();
                if (sleepNs > 0) {
                    try {
                        long ms = sleepNs / 1_000_000L;
                        int ns = (int) (sleepNs % 1_000_000L);
                        Thread.sleep(ms, ns);
                    } catch (InterruptedException e) {
                        Thread.currentThread().interrupt();
                        break;
                    }
                } else if (sleepNs < -100_000_000L) {
                    next = SystemClock.elapsedRealtimeNanos();
                }
            }
        }, "doctor-ssvep-synthetic-source");
        syntheticThread.setDaemon(true);
        syntheticThread.start();
    }

    private void offerSyntheticSample(double sampleMv, long generation) {
        analysisExecutor.execute(() -> {
            if (generation != sessionGeneration.get() || !running || !synthetic) return;
            processSample(sampleMv);
        });
    }

    /** Called directly by TcpServerManager.FrameParser before UI dispatch. */
    public void offerWaveSample(int cmd, float channel0Value) {
        if (!running || synthetic || !Float.isFinite(channel0Value)) return;
        final long generation = sessionGeneration.get();
        final long now = System.currentTimeMillis();

        if (selectedWaveCommand == 0) {
            if (cmd == FbccaConfig.PREFERRED_WAVE_CMD) {
                selectedWaveCommand = cmd;
            } else if (cmd == FbccaConfig.FALLBACK_WAVE_CMD
                    && now - sessionStartedAtMs >= FbccaConfig.RAW_FALLBACK_DELAY_MS) {
                selectedWaveCommand = cmd;
            } else {
                return;
            }
            postProgress(SsvepProgress.State.COLLECTING, engine.getBufferedSamples(), cmd,
                    String.format(Locale.US, "已选择波形源 CMD=0x%02X", cmd));
        }
        if (cmd != selectedWaveCommand) return;

        final double mv = channel0Value * FbccaConfig.INPUT_SCALE_TO_MV;
        analysisExecutor.execute(() -> {
            if (generation != sessionGeneration.get() || !running || synthetic) return;
            processSample(mv);
        });
    }

    private void processSample(double sampleMv) {
        FbccaOutput output = engine.pushSample(sampleMv);
        progressCounter++;
        if (progressCounter % 25 == 0) {
            int buffered = engine.getBufferedSamples();
            postProgress(SsvepProgress.State.COLLECTING, buffered, selectedWaveCommand,
                    buffered < FbccaConfig.WINDOW_SIZE
                            ? "正在收集 5 秒分析窗口"
                            : "窗口已满，等待下一次 2 秒步进分析");
        }
        if (output != null) {
            postProgress(SsvepProgress.State.ANALYZING,
                    engine.getBufferedSamples(), selectedWaveCommand,
                    "FBCCA 第 " + output.sequence + " 轮完成");
            SsvepResult result = SsvepResult.fromFbcca(output, synthetic);
            DataDispatcher.getInstance().postSsvepResult(result);
            TcpServerManager.getInstance().sendToPatient(result.toPatientCommand());
            Log.i(TAG, "result seq=" + result.getSeq()
                    + " raw=" + result.getRawFreq()
                    + " voted=" + result.getFreq()
                    + " ratio=" + result.getRatio());
        }
    }

    public void stopSession() {
        sessionGeneration.incrementAndGet();
        running = false;
        prepared = false;
        synthetic = false;
        targetFreqIndex = -1;
        selectedWaveCommand = 0;
        stopSyntheticThread();
        analysisExecutor.execute(() -> {
            engine.reset();
            postProgress(SsvepProgress.State.STOPPED, 0, 0, "SSVEP 分析已停止");
        });
    }

    public boolean isRunning() { return running; }
    public boolean isSynthetic() { return synthetic; }

    private void stopSyntheticThread() {
        Thread thread = syntheticThread;
        syntheticThread = null;
        if (thread != null) thread.interrupt();
    }

    private void postProgress(SsvepProgress.State state,
                              int buffered,
                              int waveCommand,
                              String message) {
        DataDispatcher.getInstance().postSsvepProgress(new SsvepProgress(
                state,
                buffered,
                FbccaConfig.WINDOW_SIZE,
                waveCommand,
                synthetic,
                message));
    }
}
