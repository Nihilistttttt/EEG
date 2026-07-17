package com.nihilisttt.eegpatient;

/** Result message sent by the doctor-side FBCCA engine. */
public final class PatientSsvepResult {
    private final int seq;
    private final String freq;
    private final String rawFreq;
    private final float ratio;
    private final float margin;
    private final float[] scores;
    private final int[] votes;
    private final boolean synthetic;
    private final boolean usable;

    public PatientSsvepResult(int seq, String freq, String rawFreq,
                              float ratio, float margin,
                              float[] scores, int[] votes,
                              boolean synthetic, boolean usable) {
        this.seq = seq;
        this.freq = freq;
        this.rawFreq = rawFreq;
        this.ratio = ratio;
        this.margin = margin;
        this.scores = scores;
        this.votes = votes;
        this.synthetic = synthetic;
        this.usable = usable;
    }

    public static PatientSsvepResult fromCommand(String line) {
        try {
            String[] p = line.split(",");
            if (p.length < 17 || !"SSVEP".equals(p[0]) || !"RESULT".equals(p[1])) return null;
            int seq = Integer.parseInt(p[2]);
            String freq = p[3];
            String raw = p[4];
            float ratio = Float.parseFloat(p[5]);
            // p[6] is best score, retained in wire protocol for doctor diagnostics.
            float margin = Float.parseFloat(p[7]);
            float[] scores = {
                    Float.parseFloat(p[8]), Float.parseFloat(p[9]),
                    Float.parseFloat(p[10]), Float.parseFloat(p[11])
            };
            int[] votes = {
                    Integer.parseInt(p[12]), Integer.parseInt(p[13]),
                    Integer.parseInt(p[14]), Integer.parseInt(p[15])
            };
            boolean synthetic = Integer.parseInt(p[16]) != 0;
            boolean usable = p.length < 18 || Integer.parseInt(p[17]) != 0;
            return new PatientSsvepResult(seq, freq, raw, ratio, margin,
                    scores, votes, synthetic, usable);
        } catch (Exception ignored) {
            return null;
        }
    }

    public int getSeq() { return seq; }
    public String getFreq() { return freq; }
    public String getRawFreq() { return rawFreq; }
    public float getRatio() { return ratio; }
    public float getMargin() { return margin; }
    public float[] getScores() { return scores; }
    public int[] getVotes() { return votes; }
    public boolean isSynthetic() { return synthetic; }
    public boolean isUsable() { return usable; }
    public boolean isUncertain() { return "UNCERTAIN".equals(freq); }

    public int getFreqIndex() {
        try {
            double value = Double.parseDouble(freq);
            double[] freqs = {11.0, 13.0, 15.0, 17.0};
            for (int i = 0; i < freqs.length; i++) {
                if (Math.abs(value - freqs[i]) < 0.2) return i;
            }
        } catch (Exception ignored) {}
        return -1;
    }
}
