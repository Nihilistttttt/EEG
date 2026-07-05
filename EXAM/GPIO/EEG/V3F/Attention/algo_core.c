#include "algo_core.h"
#include <string.h>
#include <stdlib.h>

/* ---------- 内部常数 ---------- */
static const real_t kSmoothAlpha   = 0.25f;  /* 常规 EMA 系数 */
static const real_t kJumpAlpha     = 0.1f;   /* 跳变时快速平滑系数 */
static const real_t kJumpThreshold = 40.0f;  /* 分数跳变阈值 */
static const real_t kEpsilon       = 1e-10f;

/* ---------- 辅助：排序并求百分位数 ---------- */
static real_t percentile(const real_t *arr, int n, real_t p) {
    if (n <= 0) return 0.0f;
    /* 复制并排序 */
    real_t *tmp = (real_t*)malloc(n * sizeof(real_t));
    if (!tmp) return 0.0f;
    memcpy(tmp, arr, n * sizeof(real_t));
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (tmp[j] > tmp[j+1]) {
                real_t t = tmp[j];
                tmp[j] = tmp[j+1];
                tmp[j+1] = t;
            }
        }
    }
    real_t idx = p / 100.0f * (n - 1);
    int lo = (int)idx;
    int hi = lo + 1;
    if (hi >= n) hi = n - 1;
    real_t frac = idx - lo;
    real_t res = tmp[lo] * (1.0f - frac) + tmp[hi] * frac;
    free(tmp);
    return res;
}

/* ---------- 公共：频段功率（梯形积分，基于幅度谱） ---------- */
static real_t integrate_band(const real_t *mags, int lo, int hi, real_t df, real_t norm) {
    real_t sum = 0.0f;
    for (int i = lo; i < hi; i++) {
        real_t psd_i   = (mags[i] * mags[i]) / norm;
        real_t psd_ip1 = (mags[i+1] * mags[i+1]) / norm;
        sum += (psd_i + psd_ip1) * 0.5f * df;
    }
    return sum;
}

BandPowers_t compute_band_powers(const real_t *mags, int fft_size,
                                 int theta_lo, int theta_hi,
                                 int alpha_lo, int alpha_hi,
                                 int beta_lo,  int beta_hi,
                                 real_t norm_factor, real_t fs)
{
    BandPowers_t bp = {0};
    if (theta_lo >= theta_hi || alpha_lo >= alpha_hi || beta_lo >= beta_hi)
        return bp;

    real_t df = fs / fft_size;
    bp.theta_power = integrate_band(mags, theta_lo, theta_hi, df, norm_factor);
    bp.alpha_power = integrate_band(mags, alpha_lo, alpha_hi, df, norm_factor);
    bp.beta_power  = integrate_band(mags, beta_lo,  beta_hi,  df, norm_factor);
    return bp;
}

/* ---------- 特征函数 ---------- */
real_t compute_attention_feature(real_t theta, real_t alpha, real_t beta) {
    real_t ratio = beta / (alpha + theta + kEpsilon);
    return logf(ratio + kEpsilon);
}

real_t compute_relaxation_feature(real_t theta, real_t alpha, real_t beta) {
    real_t ratio = alpha / (beta + theta + kEpsilon);
    return logf(ratio + kEpsilon);
}

real_t compute_blink_feature(real_t blink_rate, real_t slow_blink_ratio) {
    /* 与 C++ 一致：log(rate / baseline) + 2*slow_ratio */
    const real_t baseline_rate = 17.5f;
    real_t rate_feature = logf((blink_rate + kEpsilon) / baseline_rate);
    return rate_feature + slow_blink_ratio * 2.0f;
}
real_t clampf(real_t v, real_t lo, real_t hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}
/* ---------- 归一化 ---------- */
real_t normalize_attention(real_t feature, AttnHistory_t *hist) {
    /* 加入历史 */
    hist->history[hist->idx] = feature;
    hist->idx = (hist->idx + 1) % ATT_HISTORY_SIZE;
    if (hist->count < ATT_HISTORY_SIZE) hist->count++;

    if (hist->count < 10) return 50.0f;

    real_t low  = percentile(hist->history, hist->count, 10.0f);
    real_t high = percentile(hist->history, hist->count, 90.0f);
    if (high - low < 0.1f) return 50.0f;

    real_t score = (feature - low) / (high - low) * 100.0f;
    return clampf(score, 0.0f, 100.0f);
}

real_t normalize_feature_calibrated(real_t feature,
                                    real_t relax_baseline,
                                    real_t focus_baseline) {
    real_t low  = relax_baseline;
    real_t high = focus_baseline;
    if (high - low < 0.1f) return 50.0f;
    real_t score = (feature - low) / (high - low) * 100.0f;
    return clampf(score, 0.0f, 100.0f);
}

/* ---------- EMA 平滑 ---------- */
real_t ema_smooth(real_t value, real_t smoothed, real_t alpha) {
    return alpha * value + (1.0f - alpha) * smoothed;
}

/* 带跳变检测的平滑（内部使用） */
static real_t smooth_with_jump(real_t new_val, real_t *smoothed_ptr) {
    real_t diff = fabsf(new_val - *smoothed_ptr);
    real_t alpha = (diff > kJumpThreshold) ? kJumpAlpha : kSmoothAlpha;
    *smoothed_ptr = ema_smooth(new_val, *smoothed_ptr, alpha);
    return *smoothed_ptr;
}

/* ---------- 引擎初始化 ---------- */
void attention_engine_init(AttentionEngine_t *eng, const CalibrationConfig_t *calib) {
    memset(eng, 0, sizeof(AttentionEngine_t));
    eng->smooth_alpha = kSmoothAlpha;
    eng->smoothed_attention = 50.0f;
    eng->smoothed_relaxation = 50.0f;
    eng->smoothed_blink = 50.0f;
    eng->last_valid_attention = 50.0f;
    eng->last_valid_relaxation = 50.0f;
    eng->last_valid_blink = 50.0f;
    if (calib) {
        eng->calibration = *calib;
    }
}

void attention_engine_reset(AttentionEngine_t *eng) {
    attention_engine_init(eng, &eng->calibration);
}

void attention_engine_set_calibration(AttentionEngine_t *eng,
                                      const CalibrationConfig_t *calib) {
    if (calib) {
        eng->calibration = *calib;
    } else {
        eng->calibration.has_value = 0;
    }
}

/* ---------- 主处理函数 ---------- */
AttentionOutput_t attention_engine_process(AttentionEngine_t *eng,
                                           const BandPowers_t *powers,
                                           real_t artifact_ratio,
                                           const BlinkStatistics_t *blink_stats,
                                           real_t bad_window_threshold,
                                           int bad_window_limit,
                                           real_t timestamp)
{
    AttentionOutput_t out;
    memset(&out, 0, sizeof(out));
    out.timestamp = timestamp;
    out.artifact_ratio = artifact_ratio;

    /* 复制眨眼统计（如果有） */
    if (blink_stats) {
        out.blink_rate        = blink_stats->blink_rate;
        out.avg_blink_duration = blink_stats->avg_duration;
        out.slow_blink_ratio  = blink_stats->slow_blink_ratio;
        out.total_blink_count = blink_stats->total_blinks;
        out.recent_blink_count = blink_stats->recent_count;
    }

    out.theta_power = powers->theta_power;
    out.alpha_power = powers->alpha_power;
    out.beta_power  = powers->beta_power;

    /* ---- 坏窗口检测 ---- */
    if (artifact_ratio > bad_window_threshold) {
        eng->consecutive_bad_windows++;
        if (eng->consecutive_bad_windows >= bad_window_limit) {
            /* 连续坏窗口过多 → 输出 Invalid */
            out.quality = 2; // Invalid
            out.attention_score = NAN;
            out.relaxation_score = NAN;
            out.blink_score = NAN;
            return out;
        }
        /* 降级输出：使用上次有效分数 */
        out.quality = 1; // Degraded
        out.attention_score = eng->last_valid_attention;
        out.relaxation_score = eng->last_valid_relaxation;
        out.blink_score = eng->last_valid_blink;
        return out;
    }
    /* 正常窗口：复位坏窗口计数 */
    eng->consecutive_bad_windows = 0;

    /* ---- 特征计算 ---- */
    real_t attn_feat = compute_attention_feature(powers->theta_power,
                                                 powers->alpha_power,
                                                 powers->beta_power);
    real_t relax_feat = compute_relaxation_feature(powers->theta_power,
                                                   powers->alpha_power,
                                                   powers->beta_power);

    /* ---- 眨眼特征（若无统计则设为0，对应分数为50） ---- */
    real_t blink_feat = 0.0f;
    if (blink_stats) {
        blink_feat = compute_blink_feature(blink_stats->blink_rate,
                                           blink_stats->slow_blink_ratio);
    }

    /* ---- 归一化（校准/自适应） ---- */
    real_t attn_score, relax_score, blink_score;

    if (eng->calibration.has_value) {
        attn_score = normalize_feature_calibrated(attn_feat,
                                                  eng->calibration.attention_relax_baseline,
                                                  eng->calibration.attention_focus_baseline);
        relax_score = normalize_feature_calibrated(relax_feat,
                                                   eng->calibration.relaxation_relax_baseline,
                                                   eng->calibration.relaxation_focus_baseline);
        /* 眨眼无校准，仍用自适应（但反转映射） —— 注意 C++ 中眨眼是反转映射，我们保持原样 */
        blink_score = normalize_attention(blink_feat, &eng->blink_history);
        /* 但我们需要反转：低特征→高分，C++ 使用 normalise_feature_inverted，
           这里简单实现：100 - score */
        blink_score = 100.0f - blink_score;  /* 近似反转 */
    } else {
        attn_score = normalize_attention(attn_feat, &eng->attn_history);
        relax_score = normalize_attention(relax_feat, &eng->relax_history);
        blink_score = normalize_attention(blink_feat, &eng->blink_history);
        blink_score = 100.0f - blink_score;  /* 反转映射 */
    }

    /* ---- EMA 平滑（双系数） ---- */
    attn_score = smooth_with_jump(attn_score, &eng->smoothed_attention);
    relax_score = smooth_with_jump(relax_score, &eng->smoothed_relaxation);
    blink_score = smooth_with_jump(blink_score, &eng->smoothed_blink);

    /* ---- 保存有效分数 ---- */
    eng->last_valid_attention = attn_score;
    eng->last_valid_relaxation = relax_score;
    eng->last_valid_blink = blink_score;

    out.quality = 0; // Normal
    out.attention_score = attn_score;
    out.relaxation_score = relax_score;
    out.blink_score = blink_score;

    return out;
}