#ifndef ALGO_CORE_H
#define ALGO_CORE_H

#include <stdint.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ATT_HISTORY_SIZE 300

/* ---------- 通用类型 ---------- */
typedef float real_t;

/* 注意力历史（自适应归一化） */
typedef struct {
    real_t history[ATT_HISTORY_SIZE];
    int    idx;
    int    count;
} AttnHistory_t;

/* 眨眼统计（由外部眨眼检测器提供，对应 C++ BlinkStatistics） */
typedef struct {
    real_t blink_rate;         /* 最近窗口眨眼率（次/分钟） */
    real_t avg_duration;       /* 平均眨眼时长（ms） */
    real_t slow_blink_ratio;   /* 慢眨眼比例 (>200ms) */
    uint32_t total_blinks;     /* 累计总数 */
    uint32_t recent_count;     /* 最近窗口次数 */
} BlinkStatistics_t;

/* 校准配置（对应 C++ CalibrationConfig） */
typedef struct {
    real_t attention_relax_baseline;   /* 注意力-放松基线（特征值） */
    real_t attention_focus_baseline;   /* 注意力-专注基线 */
    real_t relaxation_relax_baseline;  /* 放松度-放松基线 */
    real_t relaxation_focus_baseline;  /* 放松度-专注基线 */
    real_t calibration_time;           /* 校准时间戳 */
    real_t quality_score;              /* 校准质量 0-1 */
    int    has_value;                  /* 是否已校准（0/1） */
} CalibrationConfig_t;

/* 注意力引擎内部状态（对应 C++ AttentionEngine 成员变量） */
typedef struct {
    /* 平滑值（三指标独立） */
    real_t smoothed_attention;
    real_t smoothed_relaxation;
    real_t smoothed_blink;
    real_t smooth_alpha;             /* 常规平滑系数 (0.25) */
    /* 上次有效评分（用于坏窗口降级） */
    real_t last_valid_attention;
    real_t last_valid_relaxation;
    real_t last_valid_blink;
    /* 坏窗口连续计数 */
    int consecutive_bad_windows;
    /* 历史缓冲（三指标独立自适应归一化） */
    AttnHistory_t attn_history;
    AttnHistory_t relax_history;
    AttnHistory_t blink_history;
    /* 校准配置（可动态更新） */
    CalibrationConfig_t calibration;
} AttentionEngine_t;

/* 窗口分析结果（对应 C++ AttentionFrame） */
typedef struct {
    real_t timestamp;              /* 窗口时间戳 ms */
    real_t attention_score;        /* 0-100 */
    real_t relaxation_score;       /* 0-100 */
    real_t blink_score;            /* 0-100 */
    int    quality;                /* 0=Normal, 1=Degraded, 2=Invalid */
    real_t theta_power;
    real_t alpha_power;
    real_t beta_power;
    real_t artifact_ratio;         /* 当前窗伪迹比例 (0~1) */
    /* 眨眼相关（从 BlinkStatistics 复制） */
    real_t blink_rate;
    real_t avg_blink_duration;
    real_t slow_blink_ratio;
    uint32_t total_blink_count;
    uint32_t recent_blink_count;
} AttentionOutput_t;

/* ---------- 频段功率 ---------- */
typedef struct {
    real_t theta_power;
    real_t alpha_power;
    real_t beta_power;
} BandPowers_t;

/* 计算频段功率（幅度谱，已加窗，norm = fs * Σw?） */
BandPowers_t compute_band_powers(const real_t *mags, int fft_size,
                                 int theta_lo, int theta_hi,
                                 int alpha_lo, int alpha_hi,
                                 int beta_lo,  int beta_hi,
                                 real_t norm_factor, real_t fs);

/* ---------- 特征计算 ---------- */
real_t compute_attention_feature(real_t theta, real_t alpha, real_t beta);
real_t compute_relaxation_feature(real_t theta, real_t alpha, real_t beta);
real_t compute_blink_feature(real_t blink_rate, real_t slow_blink_ratio);

/* ---------- 归一化 ---------- */
real_t normalize_attention(real_t feature, AttnHistory_t *hist);
real_t normalize_feature_calibrated(real_t feature,
                                    real_t relax_baseline,
                                    real_t focus_baseline);

/* ---------- EMA 平滑 ---------- */
real_t ema_smooth(real_t value, real_t smoothed, real_t alpha);

/* ---------- 钳位 ---------- */
real_t clampf(real_t v, real_t lo, real_t hi);

/* ---------- 引擎初始化/处理 ---------- */
void attention_engine_init(AttentionEngine_t *eng,
                           const CalibrationConfig_t *calib);
void attention_engine_reset(AttentionEngine_t *eng);
void attention_engine_set_calibration(AttentionEngine_t *eng,
                                      const CalibrationConfig_t *calib);

/* 处理一个窗口，返回结果。
 * artifact_ratio: 该窗伪迹比例
 * blink_stats: 眨眼统计（可传入 NULL，则眨眼特征无效）
 * bad_window_threshold: 伪迹阈值，超过则视为坏窗口
 * bad_window_limit: 连续坏窗口数上限，超过则输出 Invalid
 */
AttentionOutput_t attention_engine_process(AttentionEngine_t *eng,
                                           const BandPowers_t *powers,
                                           real_t artifact_ratio,
                                           const BlinkStatistics_t *blink_stats,
                                           real_t bad_window_threshold,
                                           int bad_window_limit,
                                           real_t timestamp);

#ifdef __cplusplus
}
#endif

#endif /* ALGO_CORE_H */