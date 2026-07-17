package com.nihilisttt.eegdoctor;

public class EegFrame {
    public static final int CHANNEL_COUNT = 8;
    public static final int FEATURE_COUNT = 24;

    private String label;
    private final float[] channels = new float[CHANNEL_COUNT];
    private final float[] features = new float[FEATURE_COUNT];
    private long timestamp;

    public EegFrame() {
        this.timestamp = System.currentTimeMillis();
    }

    public String getLabel() { return label; }
    public void setLabel(String label) { this.label = label; }

    public float getChannel(int index) {
        if (index < 0 || index >= CHANNEL_COUNT) return 0f;
        return channels[index];
    }

    public void setChannel(int index, float value) {
        if (index >= 0 && index < CHANNEL_COUNT) channels[index] = value;
    }

    public float[] getChannels() { return channels; }

    public float getFeature(int index) {
        if (index < 0 || index >= FEATURE_COUNT) return 0f;
        return features[index];
    }

    public void setFeature(int index, float value) {
        if (index >= 0 && index < FEATURE_COUNT) features[index] = value;
    }

    public float[] getFeatures() { return features; }

    public long getTimestamp() { return timestamp; }
    public void setTimestamp(long timestamp) { this.timestamp = timestamp; }

    public static EegFrame fromDirCsv(String line) {
        String[] parts = line.split(",");
        if (parts.length < 1) return null;
        EegFrame frame = new EegFrame();
        int idx = 0;
        if (parts[0].equals("DIRCSV")) idx = 1;
        if (idx < parts.length) frame.setLabel(parts[idx++]);
        for (int i = 0; i < CHANNEL_COUNT && idx < parts.length; i++) {
            try { frame.setChannel(i, Float.parseFloat(parts[idx++])); }
            catch (NumberFormatException e) { frame.setChannel(i, 0f); }
        }
        for (int i = 0; i < FEATURE_COUNT && idx < parts.length; i++) {
            try { frame.setFeature(i, Float.parseFloat(parts[idx++])); }
            catch (NumberFormatException e) { frame.setFeature(i, 0f); }
        }
        return frame;
    }
}