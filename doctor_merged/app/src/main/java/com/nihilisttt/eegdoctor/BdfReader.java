package com.nihilisttt.eegdoctor;

import java.io.RandomAccessFile;
import java.io.IOException;

public class BdfReader {

    public int nChannels;
    public int sampleRate;
    public int nRecords;
    public int samplesPerRecord;
    public int totalSamples;
    public String[] labels;
    public float[][] channels;

    private static String field(byte[] raw, int off, int len) {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < len && off + i < raw.length; i++) {
            char c = (char) raw[off + i];
            if (c == 0) break;
            if (c != ' ') sb.append(c);
        }
        return sb.toString().trim();
    }

    private static float safeFloat(String s, float def) {
        if (s.isEmpty()) return def;
        try { return Float.parseFloat(s); } catch (NumberFormatException e) { return def; }
    }

    private static int safeInt(String s, int def) {
        if (s.isEmpty()) return def;
        try { return Integer.parseInt(s); } catch (NumberFormatException e) { return def; }
    }

    public boolean parse(String path) {
        RandomAccessFile f = null;
        try {
            f = new RandomAccessFile(path, "r");
            long fileLen = f.length();
            if (fileLen < 256) return false;
            byte[] header = new byte[256];
            f.readFully(header);

            nRecords = safeInt(field(header, 236, 8), -1);
            float duration = safeFloat(field(header, 244, 8), 1.0f);
            nChannels = safeInt(field(header, 252, 4), 8);
            if (nChannels <= 0 || nChannels > 64 || nRecords <= 0) return false;

            int chHeaderSize = nChannels * 256;
            byte[] chHeader = new byte[chHeaderSize];
            f.readFully(chHeader);

            labels = new String[nChannels];
            float[] pmin = new float[nChannels];
            float[] pmax = new float[nChannels];
            float[] dmin = new float[nChannels];
            float[] dmax = new float[nChannels];
            int[] sratePerCh = new int[nChannels];

            for (int i = 0; i < nChannels; i++) {
                int base = i * 256;
                String lbl = field(chHeader, base, 16);
                labels[i] = lbl.isEmpty() ? ("Ch" + i) : lbl;
                pmin[i] = safeFloat(field(chHeader, base + 104, 8), -187500f);
                pmax[i] = safeFloat(field(chHeader, base + 112, 8), 187500f);
                dmin[i] = safeFloat(field(chHeader, base + 120, 8), -8388608f);
                dmax[i] = safeFloat(field(chHeader, base + 128, 8), 8388607f);
                sratePerCh[i] = safeInt(field(chHeader, base + 216, 8), 250);
            }

            samplesPerRecord = sratePerCh[0];
            sampleRate = samplesPerRecord;
            if (duration >= 1) sampleRate = (int) (samplesPerRecord / duration);

            int headerSize = 256 + chHeaderSize;
            int recordBytes = nChannels * samplesPerRecord * 3;
            long dataLen = fileLen - headerSize;
            int availRecords = (int) (dataLen / recordBytes);
            if (availRecords < nRecords) nRecords = availRecords;
            if (nRecords <= 0) return false;

            totalSamples = nRecords * samplesPerRecord;
            channels = new float[nChannels][totalSamples];

            byte[] recordBuf = new byte[recordBytes];
            for (int rec = 0; rec < nRecords; rec++) {
                f.seek(headerSize + (long) rec * recordBytes);
                f.readFully(recordBuf);
                for (int ch = 0; ch < nChannels; ch++) {
                    int chBase = ch * samplesPerRecord * 3;
                    float dm = dmin[ch], dx = dmax[ch], pm = pmin[ch], px = pmax[ch];
                    float scale = (dx == dm) ? 1f : (px - pm) / (dx - dm);
                    for (int s = 0; s < samplesPerRecord; s++) {
                        int p = chBase + s * 3;
                        int b0 = recordBuf[p] & 0xFF;
                        int b1 = recordBuf[p + 1] & 0xFF;
                        int b2 = recordBuf[p + 2] & 0xFF;
                        int val = b0 | (b1 << 8) | (b2 << 16);
                        if (val >= 0x800000) val -= 0x1000000;
                        channels[ch][rec * samplesPerRecord + s] = (val - dm) * scale + pm;
                    }
                }
            }
            return true;
        } catch (IOException e) {
            return false;
        } finally {
            if (f != null) try { f.close(); } catch (IOException ignored) {}
        }
    }

    public float[] getSegment(int ch, int start, int len) {
        if (ch < 0 || ch >= nChannels) return new float[0];
        int end = Math.min(start + len, totalSamples);
        if (start < 0) start = 0;
        if (end <= start) return new float[0];
        float[] out = new float[end - start];
        System.arraycopy(channels[ch], start, out, 0, end - start);
        return out;
    }
}