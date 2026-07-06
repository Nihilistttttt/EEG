package com.nihilisttt.eegdoctor;

import android.content.Context;
import android.content.SharedPreferences;

public class SettingsStore {
    private static final String PREFS_NAME = "eeg_doctor_settings";

    private static final String KEY_WAVE_STEP = "wave_step";
    private static final String KEY_WAVE_STEP_UNIT = "wave_step_unit";
    private static final String KEY_WAVE_LABEL_COUNT = "wave_label_count";
    private static final String KEY_WAVE_X_MAX = "wave_x_max";
    private static final String KEY_SPEC_RANGE = "spec_range";
    private static final String KEY_SPEC_UNIT = "spec_unit";
    private static final String KEY_FOCUS_LABEL_COUNT = "focus_label_count";
    private static final String KEY_FOCUS_Y_RANGE = "focus_y_range";
    private static final String KEY_FOCUS_X_POINTS = "focus_x_points";

    private static SharedPreferences getPrefs(Context ctx) {
        return ctx.getApplicationContext().getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE);
    }

    public static int getWaveStep(Context ctx, int def) {
        return getPrefs(ctx).getInt(KEY_WAVE_STEP, def);
    }
    public static void setWaveStep(Context ctx, int val) {
        getPrefs(ctx).edit().putInt(KEY_WAVE_STEP, val).apply();
    }

    public static String getWaveStepUnit(Context ctx, String def) {
        return getPrefs(ctx).getString(KEY_WAVE_STEP_UNIT, def);
    }
    public static void setWaveStepUnit(Context ctx, String val) {
        getPrefs(ctx).edit().putString(KEY_WAVE_STEP_UNIT, val).apply();
    }

    public static int getWaveLabelCount(Context ctx, int def) {
        return getPrefs(ctx).getInt(KEY_WAVE_LABEL_COUNT, def);
    }
    public static void setWaveLabelCount(Context ctx, int val) {
        getPrefs(ctx).edit().putInt(KEY_WAVE_LABEL_COUNT, val).apply();
    }

    public static float getWaveXMax(Context ctx, float def) {
        return getPrefs(ctx).getFloat(KEY_WAVE_X_MAX, def);
    }
    public static void setWaveXMax(Context ctx, float val) {
        getPrefs(ctx).edit().putFloat(KEY_WAVE_X_MAX, val).apply();
    }

    public static float getSpecRange(Context ctx, float def) {
        return getPrefs(ctx).getFloat(KEY_SPEC_RANGE, def);
    }
    public static void setSpecRange(Context ctx, float val) {
        getPrefs(ctx).edit().putFloat(KEY_SPEC_RANGE, val).apply();
    }

    public static String getSpecUnit(Context ctx, String def) {
        return getPrefs(ctx).getString(KEY_SPEC_UNIT, def);
    }
    public static void setSpecUnit(Context ctx, String val) {
        getPrefs(ctx).edit().putString(KEY_SPEC_UNIT, val).apply();
    }

    public static int getFocusLabelCount(Context ctx, int def) {
        return getPrefs(ctx).getInt(KEY_FOCUS_LABEL_COUNT, def);
    }
    public static void setFocusLabelCount(Context ctx, int val) {
        getPrefs(ctx).edit().putInt(KEY_FOCUS_LABEL_COUNT, val).apply();
    }

    public static float getFocusYRange(Context ctx, float def) {
        return getPrefs(ctx).getFloat(KEY_FOCUS_Y_RANGE, def);
    }
    public static void setFocusYRange(Context ctx, float val) {
        getPrefs(ctx).edit().putFloat(KEY_FOCUS_Y_RANGE, val).apply();
    }

    public static int getFocusXPoints(Context ctx, int def) {
        return getPrefs(ctx).getInt(KEY_FOCUS_X_POINTS, def);
    }
    public static void setFocusXPoints(Context ctx, int val) {
        getPrefs(ctx).edit().putInt(KEY_FOCUS_X_POINTS, val).apply();
    }
}