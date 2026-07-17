package com.nihilisttt.eegdoctor;

import java.util.ArrayDeque;
import java.util.Arrays;
import java.util.Deque;

/**
 * Pure-Java, single-channel FBCCA implementation for mode 1.
 *
 * The Python code uses FBCCA_USE_CHANNELS=[0], so canonical correlation can be
 * calculated exactly as a weighted multiple-correlation coefficient. This
 * avoids a large SVD dependency while preserving the one-channel math.
 */
public final class FbccaEngine {
    private final double[] ring = new double[FbccaConfig.WINDOW_SIZE];
    private int ringWrite;
    private int ringCount;
    private int sinceLastUpdate;
    private int sequence;

    private final NotchFilter notch = new NotchFilter(
            FbccaConfig.SAMPLE_RATE,
            FbccaConfig.NOTCH_FREQ,
            FbccaConfig.NOTCH_RHO);

    private final Deque<Integer> voteHistory = new ArrayDeque<>();
    private final FourierTable fourierTable = new FourierTable(
            FbccaConfig.WINDOW_SIZE,
            FbccaConfig.SAMPLE_RATE,
            6.0,
            45.0);
    private final double[] timeWeights = makeTukeyWeights(FbccaConfig.WINDOW_SIZE, 0.25);

    public void reset() {
        Arrays.fill(ring, 0.0);
        ringWrite = 0;
        ringCount = 0;
        sinceLastUpdate = 0;
        sequence = 0;
        notch.reset();
        voteHistory.clear();
    }

    public int getBufferedSamples() {
        return ringCount;
    }

    public int getSamplesUntilNextUpdate() {
        if (ringCount < FbccaConfig.WINDOW_SIZE) {
            return FbccaConfig.WINDOW_SIZE - ringCount;
        }
        return Math.max(0, FbccaConfig.STEP_SIZE - sinceLastUpdate);
    }

    public FbccaOutput pushSample(double sampleMv) {
        if (!Double.isFinite(sampleMv)) return null;
        double filtered = notch.process(sampleMv);
        ring[ringWrite] = filtered;
        ringWrite = (ringWrite + 1) % ring.length;
        if (ringCount < ring.length) ringCount++;
        sinceLastUpdate++;

        if (ringCount < FbccaConfig.WINDOW_SIZE) return null;
        if (sinceLastUpdate < FbccaConfig.STEP_SIZE) return null;
        sinceLastUpdate = 0;
        return analyze(copyWindowChronological());
    }

    private double[] copyWindowChronological() {
        double[] out = new double[ring.length];
        int start = ringCount < ring.length ? 0 : ringWrite;
        for (int i = 0; i < ring.length; i++) {
            out[i] = ring[(start + i) % ring.length];
        }
        return out;
    }

    private FbccaOutput analyze(double[] x) {
        sequence++;
        if (!isChannelUsable(x)) {
            voteHistory.clear();
            return new FbccaOutput(sequence, -1, -1, 0.0, 0.0, 0.0,
                    new double[4], new int[4], false);
        }

        removeMeanInPlace(x);
        FourierTable.Spectrum spectrum = fourierTable.forward(x);
        double[][] banks = new double[FbccaConfig.FILTER_BANKS.length][];
        for (int b = 0; b < banks.length; b++) {
            banks[b] = fourierTable.inverseBand(
                    spectrum,
                    FbccaConfig.FILTER_BANKS[b][0],
                    FbccaConfig.FILTER_BANKS[b][1]);
            normalizeInPlace(banks[b]);
        }

        double[] scores = new double[FbccaConfig.TARGET_FREQS.length];
        for (int targetIndex = 0; targetIndex < FbccaConfig.TARGET_FREQS.length; targetIndex++) {
            double target = FbccaConfig.TARGET_FREQS[targetIndex];
            double bestTotal = -1.0;
            for (double offset : FbccaConfig.FREQ_SEARCH_OFFSETS) {
                double refFreq = target + offset;
                double total = 0.0;
                for (int bankIndex = 0; bankIndex < FbccaConfig.FILTER_BANKS.length; bankIndex++) {
                    double low = FbccaConfig.FILTER_BANKS[bankIndex][0];
                    double high = FbccaConfig.FILTER_BANKS[bankIndex][1];
                    double[][] refs = generateReferences(
                            refFreq,
                            FbccaConfig.WINDOW_SIZE,
                            FbccaConfig.SAMPLE_RATE,
                            FbccaConfig.HARMONICS,
                            low,
                            high);
                    double rho = refs.length == 0 ? 0.0
                            : canonicalCorrSingleX(banks[bankIndex], refs, timeWeights, 1e-6);
                    total += FbccaConfig.filterBankWeight(bankIndex) * rho * rho;
                }
                if (total > bestTotal) bestTotal = total;
            }
            scores[targetIndex] = Math.max(0.0, bestTotal);
        }

        Decision decision = choose(scores);
        int votedIndex = vote(decision.rawIndex);
        int[] counts = voteCounts();
        return new FbccaOutput(sequence,
                decision.rawIndex,
                votedIndex,
                decision.ratio,
                decision.bestScore,
                decision.margin,
                scores,
                counts,
                true);
    }

    private int vote(int rawIndex) {
        if (rawIndex < 0) {
            voteHistory.clear();
            return -1;
        }
        voteHistory.addLast(rawIndex);
        while (voteHistory.size() > FbccaConfig.VOTE_HISTORY_LEN) voteHistory.removeFirst();
        if (voteHistory.size() < FbccaConfig.VOTE_MIN_COUNT) return -1;
        int[] counts = voteCounts();
        int best = -1;
        int bestCount = 0;
        for (int i = 0; i < counts.length; i++) {
            if (counts[i] > bestCount) {
                bestCount = counts[i];
                best = i;
            }
        }
        return bestCount >= FbccaConfig.VOTE_MIN_COUNT ? best : -1;
    }

    private int[] voteCounts() {
        int[] counts = new int[FbccaConfig.TARGET_FREQS.length];
        for (Integer index : voteHistory) {
            if (index != null && index >= 0 && index < counts.length) counts[index]++;
        }
        return counts;
    }

    private static Decision choose(double[] scores) {
        int best = -1;
        int second = -1;
        for (int i = 0; i < scores.length; i++) {
            if (best < 0 || scores[i] > scores[best]) {
                second = best;
                best = i;
            } else if (second < 0 || scores[i] > scores[second]) {
                second = i;
            }
        }
        if (best < 0) return new Decision(-1, 0.0, 0.0, 0.0);
        double bestScore = scores[best];
        double secondScore = second < 0 ? 0.0 : scores[second];
        double ratio = second < 0 ? 999.0 : bestScore / (secondScore + 1e-12);
        double margin = bestScore - secondScore;
        int raw = bestScore >= FbccaConfig.MIN_SCORE
                && ratio >= FbccaConfig.RATIO_THRESHOLD
                && margin >= FbccaConfig.MARGIN_THRESHOLD ? best : -1;
        return new Decision(raw, ratio, bestScore, margin);
    }

    private static boolean isChannelUsable(double[] x) {
        if (x == null || x.length < 10) return false;
        double sum = 0.0;
        double min = Double.POSITIVE_INFINITY;
        double max = Double.NEGATIVE_INFINITY;
        for (double v : x) {
            if (!Double.isFinite(v)) return false;
            sum += v;
            min = Math.min(min, v);
            max = Math.max(max, v);
        }
        double mean = sum / x.length;
        double var = 0.0;
        for (double v : x) {
            double d = v - mean;
            var += d * d;
        }
        double std = Math.sqrt(var / x.length);
        double ptp = max - min;
        return std >= FbccaConfig.CHANNEL_MIN_STD_MV
                && std <= FbccaConfig.CHANNEL_MAX_STD_MV
                && ptp <= FbccaConfig.CHANNEL_MAX_PTP_MV;
    }

    private static void removeMeanInPlace(double[] x) {
        double sum = 0.0;
        for (double v : x) sum += v;
        double mean = sum / x.length;
        for (int i = 0; i < x.length; i++) x[i] -= mean;
    }

    private static void normalizeInPlace(double[] x) {
        removeMeanInPlace(x);
        double sum = 0.0;
        for (double v : x) sum += v * v;
        double std = Math.sqrt(sum / Math.max(1, x.length));
        double scale = std + 1e-8;
        for (int i = 0; i < x.length; i++) x[i] /= scale;
    }

    private static double[][] generateReferences(double freq,
                                                 int n,
                                                 double sampleRate,
                                                 int harmonics,
                                                 double low,
                                                 double high) {
        int rows = 0;
        double nyquist = sampleRate / 2.0;
        for (int h = 1; h <= harmonics; h++) {
            double hf = h * freq;
            if (hf >= low && hf <= high && hf < nyquist) rows += 2;
        }
        double[][] refs = new double[rows][n];
        int row = 0;
        for (int h = 1; h <= harmonics; h++) {
            double hf = h * freq;
            if (hf < low || hf > high || hf >= nyquist) continue;
            for (int i = 0; i < n; i++) {
                double angle = 2.0 * Math.PI * hf * i / sampleRate;
                refs[row][i] = Math.sin(angle);
                refs[row + 1][i] = Math.cos(angle);
            }
            row += 2;
        }
        return refs;
    }

    /** Exact CCA for a one-row X and a multi-row Y. */
    private static double canonicalCorrSingleX(double[] x,
                                                double[][] y,
                                                double[] weights,
                                                double reg) {
        int n = x.length;
        int q = y.length;
        if (q == 0 || n < 5) return 0.0;
        double weightSum = 0.0;
        for (double w : weights) weightSum += w;

        double meanX = 0.0;
        double[] meanY = new double[q];
        for (int i = 0; i < n; i++) {
            double w = weights[i] / weightSum;
            meanX += w * x[i];
            for (int r = 0; r < q; r++) meanY[r] += w * y[r][i];
        }

        double cxx = 0.0;
        double[] cxy = new double[q];
        double[][] cyy = new double[q][q];
        for (int i = 0; i < n; i++) {
            double w = weights[i] / weightSum;
            double dx = x[i] - meanX;
            cxx += w * dx * dx;
            for (int r = 0; r < q; r++) {
                double dyr = y[r][i] - meanY[r];
                cxy[r] += w * dx * dyr;
                for (int c = r; c < q; c++) {
                    double dyc = y[c][i] - meanY[c];
                    cyy[r][c] += w * dyr * dyc;
                }
            }
        }
        for (int r = 0; r < q; r++) {
            for (int c = 0; c < r; c++) cyy[r][c] = cyy[c][r];
            cyy[r][r] += reg;
        }
        cxx += reg;
        double[][] inv = invert(cyy);
        if (inv == null || cxx <= 0.0) return 0.0;
        double value = 0.0;
        for (int r = 0; r < q; r++) {
            double tmp = 0.0;
            for (int c = 0; c < q; c++) tmp += inv[r][c] * cxy[c];
            value += cxy[r] * tmp;
        }
        value /= cxx;
        return Math.sqrt(Math.max(0.0, Math.min(1.0, value)));
    }

    private static double[][] invert(double[][] source) {
        int n = source.length;
        double[][] a = new double[n][2 * n];
        for (int r = 0; r < n; r++) {
            System.arraycopy(source[r], 0, a[r], 0, n);
            a[r][n + r] = 1.0;
        }
        for (int col = 0; col < n; col++) {
            int pivot = col;
            for (int r = col + 1; r < n; r++) {
                if (Math.abs(a[r][col]) > Math.abs(a[pivot][col])) pivot = r;
            }
            if (Math.abs(a[pivot][col]) < 1e-12) return null;
            if (pivot != col) {
                double[] tmp = a[pivot];
                a[pivot] = a[col];
                a[col] = tmp;
            }
            double divisor = a[col][col];
            for (int c = 0; c < 2 * n; c++) a[col][c] /= divisor;
            for (int r = 0; r < n; r++) {
                if (r == col) continue;
                double factor = a[r][col];
                if (factor == 0.0) continue;
                for (int c = 0; c < 2 * n; c++) a[r][c] -= factor * a[col][c];
            }
        }
        double[][] inv = new double[n][n];
        for (int r = 0; r < n; r++) System.arraycopy(a[r], n, inv[r], 0, n);
        return inv;
    }

    private static double[] makeTukeyWeights(int n, double alpha) {
        double[] w = new double[n];
        if (n == 1) {
            w[0] = 1.0;
            return w;
        }
        double edge = alpha * (n - 1) / 2.0;
        double sum = 0.0;
        for (int i = 0; i < n; i++) {
            double value = 1.0;
            if (i < edge) {
                value = 0.5 * (1.0 + Math.cos(Math.PI * (2.0 * i / (alpha * (n - 1)) - 1.0)));
            } else if (i >= (n - 1) * (1.0 - alpha / 2.0)) {
                value = 0.5 * (1.0 + Math.cos(Math.PI * (2.0 * i / (alpha * (n - 1)) - 2.0 / alpha + 1.0)));
            }
            w[i] = Math.max(value, 1e-8);
            sum += w[i];
        }
        double mean = sum / n;
        for (int i = 0; i < n; i++) w[i] /= mean;
        return w;
    }

    private static final class Decision {
        final int rawIndex;
        final double ratio;
        final double bestScore;
        final double margin;
        Decision(int rawIndex, double ratio, double bestScore, double margin) {
            this.rawIndex = rawIndex;
            this.ratio = ratio;
            this.bestScore = bestScore;
            this.margin = margin;
        }
    }

    /** Cached Fourier basis for the 6-45 Hz region. */
    private static final class FourierTable {
        private final int n;
        private final int[] bins;
        private final double[] binFreqs;
        private final double[][] cos;
        private final double[][] sin;

        FourierTable(int n, double sampleRate, double low, double high) {
            this.n = n;
            int first = (int) Math.ceil(low * n / sampleRate);
            int last = (int) Math.floor(high * n / sampleRate);
            bins = new int[last - first + 1];
            binFreqs = new double[bins.length];
            cos = new double[bins.length][n];
            sin = new double[bins.length][n];
            for (int j = 0; j < bins.length; j++) {
                int k = first + j;
                bins[j] = k;
                binFreqs[j] = k * sampleRate / n;
                for (int i = 0; i < n; i++) {
                    double angle = 2.0 * Math.PI * k * i / n;
                    cos[j][i] = Math.cos(angle);
                    sin[j][i] = Math.sin(angle);
                }
            }
        }

        Spectrum forward(double[] x) {
            double[] re = new double[bins.length];
            double[] im = new double[bins.length];
            for (int j = 0; j < bins.length; j++) {
                double sumRe = 0.0;
                double sumIm = 0.0;
                double[] cj = cos[j];
                double[] sj = sin[j];
                for (int i = 0; i < n; i++) {
                    sumRe += x[i] * cj[i];
                    sumIm -= x[i] * sj[i];
                }
                re[j] = sumRe;
                im[j] = sumIm;
            }
            return new Spectrum(re, im);
        }

        double[] inverseBand(Spectrum spectrum, double low, double high) {
            double[] out = new double[n];
            double factor = 2.0 / n;
            for (int j = 0; j < bins.length; j++) {
                if (binFreqs[j] < low || binFreqs[j] > high) continue;
                double re = spectrum.re[j];
                double im = spectrum.im[j];
                double[] cj = cos[j];
                double[] sj = sin[j];
                for (int i = 0; i < n; i++) {
                    out[i] += factor * (re * cj[i] - im * sj[i]);
                }
            }
            return out;
        }

        static final class Spectrum {
            final double[] re;
            final double[] im;
            Spectrum(double[] re, double[] im) {
                this.re = re;
                this.im = im;
            }
        }
    }
}
