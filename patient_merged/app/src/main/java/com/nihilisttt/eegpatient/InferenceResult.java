package com.nihilisttt.eegpatient;

public class InferenceResult {
    private int window;
    private int dtMs;
    private int winRows;
    private String intent;
    private float scoreLeft;
    private float scoreRight;
    private float confidence;
    private boolean trained;
    private long timestamp;

    public InferenceResult() {
        this.timestamp = System.currentTimeMillis();
    }

    public int getWindow() { return window; }
    public void setWindow(int window) { this.window = window; }

    public int getDtMs() { return dtMs; }
    public void setDtMs(int dtMs) { this.dtMs = dtMs; }

    public int getWinRows() { return winRows; }
    public void setWinRows(int winRows) { this.winRows = winRows; }

    public String getIntent() { return intent; }
    public void setIntent(String intent) { this.intent = intent; }

    public float getScoreLeft() { return scoreLeft; }
    public void setScoreLeft(float scoreLeft) { this.scoreLeft = scoreLeft; }

    public float getScoreRight() { return scoreRight; }
    public void setScoreRight(float scoreRight) { this.scoreRight = scoreRight; }

    public float getConfidence() { return confidence; }
    public void setConfidence(float confidence) { this.confidence = confidence; }

    public boolean isTrained() { return trained; }
    public void setTrained(boolean trained) { this.trained = trained; }

    public long getTimestamp() { return timestamp; }
    public void setTimestamp(long timestamp) { this.timestamp = timestamp; }

    public static InferenceResult fromResultLine(String line) {
        String prefix = "RESULT,";
        if (!line.startsWith(prefix)) return null;
        String body = line.substring(prefix.length());
        String[] pairs = body.split(",");
        InferenceResult r = new InferenceResult();
        for (String pair : pairs) {
            String[] kv = pair.split("=", 2);
            if (kv.length != 2) continue;
            String key = kv[0].trim();
            String val = kv[1].trim();
            try {
                switch (key) {
                    case "window": r.setWindow(Integer.parseInt(val)); break;
                    case "dt_ms": r.setDtMs(Integer.parseInt(val)); break;
                    case "win_rows": r.setWinRows(Integer.parseInt(val)); break;
                    case "INTENT": r.setIntent(val); break;
                    case "S_LEFT": r.setScoreLeft(Float.parseFloat(val)); break;
                    case "S_RIGHT": r.setScoreRight(Float.parseFloat(val)); break;
                    case "CONF": r.setConfidence(Float.parseFloat(val)); break;
                    case "trained": r.setTrained(!val.equals("0")); break;
                }
            } catch (NumberFormatException ignored) {}
        }
        return r;
    }
}