package com.nihilisttt.eegdoctor;

public class IpcDiagInfo {
    private int ack;
    private int notify;
    private int ok;
    private int bad;
    private int v5fhb;
    private int ena;
    private int sts;
    private int isr;
    private long timestamp;

    public IpcDiagInfo() {
        this.timestamp = System.currentTimeMillis();
    }

    public int getAck() { return ack; }
    public void setAck(int ack) { this.ack = ack; }

    public int getNotify() { return notify; }
    public void setNotify(int notify) { this.notify = notify; }

    public int getOk() { return ok; }
    public void setOk(int ok) { this.ok = ok; }

    public int getBad() { return bad; }
    public void setBad(int bad) { this.bad = bad; }

    public int getV5fhb() { return v5fhb; }
    public void setV5fhb(int v5fhb) { this.v5fhb = v5fhb; }

    public int getEna() { return ena; }
    public void setEna(int ena) { this.ena = ena; }

    public int getSts() { return sts; }
    public void setSts(int sts) { this.sts = sts; }

    public int getIsr() { return isr; }
    public void setIsr(int isr) { this.isr = isr; }

    public long getTimestamp() { return timestamp; }

    public float getParseOkRate() {
        int total = ok + bad;
        return total > 0 ? (float) ok / total * 100f : 0f;
    }

    public static IpcDiagInfo fromLine(String line) {
        String prefix = "IPCDIAG,";
        if (!line.startsWith(prefix)) return null;
        String body = line.substring(prefix.length());
        String[] pairs = body.split(",");
        IpcDiagInfo d = new IpcDiagInfo();
        for (String pair : pairs) {
            String[] kv = pair.split("=", 2);
            if (kv.length != 2) continue;
            String key = kv[0].trim();
            String val = kv[1].trim();
            try {
                switch (key) {
                    case "ack": d.setAck(Integer.parseInt(val)); break;
                    case "notify": d.setNotify(Integer.parseInt(val)); break;
                    case "ok": d.setOk(Integer.parseInt(val)); break;
                    case "bad": d.setBad(Integer.parseInt(val)); break;
                    case "v5fhb": d.setV5fhb(Integer.parseInt(val)); break;
                    case "ENA": d.setEna(Integer.parseInt(val, 16)); break;
                    case "STS": d.setSts(Integer.parseInt(val, 16)); break;
                    case "ISR": d.setIsr(Integer.parseInt(val, 16)); break;
                }
            } catch (NumberFormatException ignored) {}
        }
        return d;
    }
}