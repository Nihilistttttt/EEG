#include "Signal_Analysis.h"
#include "OLED.h"
#include "malloc.h"
#include "FFT_Real.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "Serial.h"
#include "Message_Parser.h"
#include "debug.h"
#include "ADS1299.h"
#include "algo_core.h"
#include "Direction_Model.h"
#include "hardware.h"
#include "ICM42605.h"
#include "Timer_1ms.h"


/* WORKFLOW_MODE_MARKER */
/* 当前文件用途：MCU_COLLECT_DIRCSV。
 * COLLECT 模式用于单片机采集并输出 DIRCSV，给 PyCharm 离线训练。
 * TEST 模式用于加载 PyCharm 导出的 Direction_Model_4ch_Weights.h 并输出 RESULT。
 */
/*
 * 文件说明：四通道脑电信号分析与左右运动想象特征提取。
 * 本版本保留原来的 ADS1299 SPI+DMA 采集、环形缓冲区、滤波、FFT 和上位机两通道显示流程。
 * 新增内容：前 4 路脑电同时参与网络训练特征输出，电极映射如下：
 *   ADS CH1 -> CP3，数组下标 CH0；
 *   ADS CH2 -> CP4，数组下标 CH1；
 *   ADS CH3 -> C3， 数组下标 CH2；
 *   ADS CH4 -> C4， 数组下标 CH3。
 *
 * 说明：为了不破坏原上位机协议，Send_Waveform / Send_Spectrum 仍只发送两路，
 * 默认发送 C3/C4 方便观察；四通道训练数据通过 DIRCSV 输出。
 */
/* ========== 基本信号处理参数 ========== */
#define FFT_SIZE 256                                               // FFT 点数，也是环形缓冲区长度。
#define SAMPLE_RATE 250.0f                                         // ADS1299 采样率，单位 Hz。
#define NUM_CHANNELS 4                                             // 当前主要分析 CP3、CP4、C3、C4 四个脑电通道。

#define BATCH_SIZE 16                                              // 保留原有批处理参数。
#define STEP_SIZE 125                                              // 测试模式：每累计125个新样本启动一次FFT；250Hz下每行特征0.5秒。

#define STATE_RELAX 0                                              // 放松状态。
#define STATE_NEUTRAL 1                                            // 中性状态。
#define STATE_FOCUS 2                                              // 专注状态。

#define MAX_EEG_UV 50.0f                                           // 异常幅值阈值，保留原逻辑备用。
#define WAVE_FIXED_MAX 0.5f                                        // OLED 小波形显示范围。
#define SPECTRUM_Y_MAX 8.0f                                        // 频谱显示 Y 轴最大值。
#define TREND_WINDOW_SIZE 10                                       // 趋势判断窗口长度。

#define MAX_SOS_SECTIONS 8                                         // IIR 级联二阶节最大数量。

#define SPECTRUM_TOTAL_FRAGS 32                                    // 频谱分片发送数量。
#define SPECTRUM_FRAG_SIZE (128 / SPECTRUM_TOTAL_FRAGS)            // 每个频谱分片包含的 float 数量。
#define SPECTRUM_FRAG_BYTES (SPECTRUM_FRAG_SIZE * sizeof (float))  // 每个频谱分片字节数。

/* ========== 左右运动想象 ERD 特征配置 ==========
 * 本部分只修改左右想象特征输出/推理逻辑，不改变 ADS1299 采集、DMA、滤波和 FFT 主流程。
 */
/* 当前改为神经网络训练用 24 维四通道窗口相对特征，不再输出 ERD。
 * 特征来自 CP3/CP4/C3/C4 的 theta、mu、beta 能量比例和左右/前后关系。
 */
#define DIR_FEAT_F_MIN              4
#define DIR_FEAT_F_MAX              30
#define DIR_FEAT_DIM_LOCAL          24

/* ERD 是小数，通常只有 0.001~0.1。
 * 如果只放大 10000 倍，再转成整数时很容易全部变成 0。
 * 这里改为放大 1000000 倍，便于串口保存和 LDA/SVM 训练。
 * 例如 ERD = -0.0123，输出约为 -12300。
 */
#define DIR_FEATURE_SCALE           10000.0f

/* 防止电极异常或 REST 基线过小导致 ERD 数值过大。 */
#define DIR_LOG_FEATURE_CLAMP       6.0f

/* ERD 计算时的功率下限。
 * 注意：FFT 频带功率单位接近 V^2，实际数值通常是 1e-12 甚至更小。
 * 之前如果直接用 eps=1e-6 判断 REST 基线是否有效，会把正常脑电功率误判为 0，
 * 导致 DIRCSV 的 6 个 ERD 特征全部输出 0。
 */
#define DIR_ERD_POWER_EPS           1.0e-18f

/* 调试开关：置 1 后会额外输出 MU/BETA 功率，排查是否 FFT 功率本身为 0。
 * 正常采集训练数据时保持 0，否则会混入训练文本。
 */
#define DIR_DEBUG_POWER_PRINT       0

/* REST 阶段至少累计几行 FFT 结果后，才认为静息基线有效。 */
#define DIR_REST_BASELINE_MIN_ROWS  1

/* 是否启用任务起点归零。
 * 作用：每次从 REST 切换到 LEFT/RIGHT 后，先用跳过的几行建立“本阶段起点”，
 * 后续输出的特征会减去这个起点，尽量屏蔽电极慢漂移和阶段切换造成的偏移。
 */
#define DIR_TASK_ZERO_ENABLE        0

/* 是否启用特征慢漂移抑制。
 * 作用：对 24 维神经网络窗口特征再做一次慢速去基线，降低全静息时仍持续单方向漂移的问题。
 * 注意：这个处理只抑制慢变化，真正较快出现的左右差异仍会保留。
 */
#define DIR_FEATURE_DRIFT_REMOVE_ENABLE  0
#define DIR_FEATURE_DRIFT_ALPHA          0.05f

/* 上位机 RAW 波形显示去基线开关。
 * 只影响 Send_Waveform(CMD_RAW_WAVE, ...) 的显示效果，不影响 RingBuf、滤波、FFT 和训练特征。
 * 置 1 后，上位机原始曲线会围绕 0 波动，不会一直向上/向下漂。
 */
#define WAVE_DISPLAY_REMOVE_BASELINE     1
/* 显示用慢速去基线时间常数，单位：秒。
 * 数值越大，基线跟踪越慢，越不容易把真实脑电波形压小；
 * 数值越小，曲线回到 0 附近越快，但过小会削弱低频波形。
 * 建议范围：5.0f ~ 12.0f，默认 8.0f。
 */
#define WAVE_DISPLAY_BASELINE_TAU_S      0.8f
#define WAVE_DISPLAY_BASELINE_ALPHA      0.005f

/* DIRCSV、TASK 提示、INTENT 推理结果输出串口。 */
#define DIR_TEXT_PORT               SERIAL_PORT_DEBUG

/* ================= EEG 工作模式选择 =================
 * 只需要改下面 EEG_APP_MODE 这一行即可切换。
 *
 * EEG_APP_MODE_COLLECT：采集模式
 *   - DEBUG 串口只输出 TASK / DIRCSV 训练数据；
 *   - 左/右任务自动交替；
 *   - LEFT 与 RIGHT 之间插入 5 秒 REST 间隔；
 *   - V5F 仍在后台跑算法链路，但不打印 V5F 调试信息，避免干扰采集文件。
 *
 * EEG_APP_MODE_INFER：推理模式
 *   - DEBUG 串口只输出每 2 秒一次的当前判断结果、左手概率、右手概率；
 *   - 不输出 DIRCSV / V5F_DIRCSV / IPC 细节；
 *   - V3F 负责采集和外设，V5F 负责特征与 LEFT/RIGHT 推理。
 */
#define EEG_APP_MODE_COLLECT        1   /* 旧版 DIRCSV 24维频域特征采集 */
#define EEG_APP_MODE_INFER          2   /* 在线推理，只输出当前结果和左右概率 */
#define EEG_APP_MODE_COLLECT_CSP    3   /* 新增：CSP 原始窗口采集，给 PyCharm 训练 CSP+SVM */

/* 全局模式变量，默认 COLLECT（上位机可通过命令更改） */
static uint8_t g_eeg_app_mode = EEG_APP_MODE_COLLECT;

// #ifndef EEG_APP_MODE
// #define EEG_APP_MODE                3
// #endif

// #define EEG_MODE_IS_COLLECT         (EEG_APP_MODE == EEG_APP_MODE_COLLECT)
// #define EEG_MODE_IS_INFER           (EEG_APP_MODE == EEG_APP_MODE_INFER)
// #define EEG_MODE_IS_COLLECT_CSP     (EEG_APP_MODE == EEG_APP_MODE_COLLECT_CSP)

// /* V3F 旧链路输出开关：由 EEG_APP_MODE 自动决定。 */
// #define DIR_FEATURE_COLLECT_MODE    EEG_MODE_IS_COLLECT
// #define CSP_WINDOW_COLLECT_MODE     EEG_MODE_IS_COLLECT_CSP
// #define DIR_INFER_MODE              0
// #define DIR_INFER_1S_MODE           0

/* 推理窗口说明：V5F 内部仍按 STEP_SIZE=125 约 0.5s 生成一次特征；
 * 对外只每 500 帧约 2 秒打印一次简洁推理结果。
 */
#define DIR_DECISION_ROWS           4
#define DIR_RESULT_EVERY_ROWS       4
#define DIR_RESULT_DT_MS            2000
#define DIR_INFER_1S_ROWS           DIR_DECISION_ROWS   /* 兼容旧宏名 */

/* 保持上位机 RAW/FILT 波形输出走 WIFI 串口；DEBUG 串口只输出当前模式需要的文本。 */
#define DIR_1S_TEST_TEXT_ONLY       0

/* 双核 EEG 数据流一直开启：V3F 每帧写共享 ring，V5F drain ring 后完成特征/推理。 */
#define DUALCORE_IPC_FRAME_SHARE_ENABLE        1
#define DUALCORE_IPC_FRAME_NOTIFY_EVERY_FRAMES 1
#define DUALCORE_IPC_FRAME_PRINT_EVERY_ACKS    500   /* 250Hz 下约 2 秒 */


/* ========== alpha/beta 提取实验配置 ==========
 * 本实验只打印 CH0/CH1 的 alpha(8~13Hz) 和 beta(13~30Hz) 能量，
 * 不做左右手训练、不做神经网络推理、不做 ERD 基线相除。
 */
#define AB_EXTRACT_PRINT_ENABLE     0
#define AB_POWER_SCALE              1000000000000000.0f  /* 绝对功率放大 1e15，只用于观察信号强弱 */
#define AB_PCT_SCALE                10000.0f              /* 频段占比放大 10000 倍，用于观察相对功率 */
#define AB_DB_SCALE                 100.0f                /* dB 放大 100 倍，用于观察对数功率趋势 */
#define AB_RATIO_SCALE              10000.0f              /* log 比值放大 10000 倍，用于观察左右通道和 beta/alpha 比值 */
#define AB_REL_EPS                  1.0e-18f              /* 防止除 0 和 log(0) 的极小功率下限 */

/* FFT 前预处理开关：去均值 + 去线性趋势 + Hann 窗。
 * 目的：抑制窗口内部慢漂移，避免静息时 alpha/beta 绝对功率一起上飘。
 * 注意：只作用于 FFT 频谱提取，不修改原始 RingBuf 数据，也不影响上位机 RAW 波形显示。
 */
#define AB_FFT_DETREND_ENABLE       1

/* ========== 实时去电极慢漂移配置 ==========
 * 这一步放在 ADS1299 转电压之后、陷波和 FFT 之前。
 * 原理：base 慢慢跟踪直流偏置和电极漂移，输出 y = x - base。
 * k 越接近 1，基线跟踪越慢。250Hz 采样率下 k=0.996 约等效 0.16Hz 高通，
 * 基本不会影响 alpha(8~13Hz) 和 beta(13~30Hz)，主要抑制很慢的漂移。
 */
#define AB_REALTIME_DRIFT_REMOVE_ENABLE  1
#define AB_DRIFT_K                       0.996f
#define AB_REALTIME_NOTCH_ENABLE         1

/* 训练标签：保持和之前数据一致，2 表示左手想象，3 表示右手想象。 */
#define DIR_LABEL_LEFT              2
#define DIR_LABEL_RIGHT             3

/* 自动采集节奏。
 * 每输出一行 DIRCSV 大约对应 STEP_SIZE / SAMPLE_RATE 秒。
 * 当前 STEP_SIZE=125，SAMPLE_RATE=250Hz，所以一行约 0.5 秒。
 * REST_ROWS=10：LEFT 与 RIGHT 中间间隔约 5 秒。
 * COLLECT_ROWS=20：每个 LEFT/RIGHT 阶段约 10 秒；前 8 行作为切换缓冲，不保存。
 */
#define DIR_COLLECT_ROWS_PER_PHASE  20
#define DIR_REST_ROWS               10
#define DIR_SKIP_ROWS_AFTER_SWITCH  8
static uint8_t g_paused = 0;   // 0: 正常运行，1: 暂停（停止所有活动）
/* CSP 采集配置。
 * 输出的是已经经过：电压换算 -> 慢漂移去除 -> 50Hz陷波 -> 带通滤波 后的4通道时域窗口。
 * 单位：uVx1000，即微伏再乘1000，便于 Python 保存为整数并还原为 float。
 * 每个窗口 256 点，4通道，PyCharm 脚本会按 CSP_BEGIN/CSP/.../CSP_END 自动解析。
 */
#define CSP_WIN_SIZE                FFT_SIZE
#define CSP_COLLECT_EVERY_ROWS      4     /* 每 4 个 STEP_SIZE 输出一次窗口，约 2 秒一个窗口，避免串口过载 */
#define CSP_OUTPUT_SCALE_UVX1000    1000000000.0f

/* 自动采集状态机阶段。 */
#define DIR_PHASE_REST              0
#define DIR_PHASE_LEFT              1
#define DIR_PHASE_RIGHT             2

/* 是否在开始时打印 DIRCSV 表头，Python 清洗脚本会自动忽略表头。 */
#define DIR_PRINT_HEADER_ON_START   1

#define ATT_HISTORY_SIZE 300                                       // 注意力、放松度、眨眼特征的历史窗口。

/* ==================== 命令控制模式新增宏 ==================== */
#define CMD_MODE_ENABLE             1   /* 1: 启用命令模式，0: 使用原有自动模式 */

#if CMD_MODE_ENABLE
typedef enum {
    WORK_MODE_IDLE = 0,
    WORK_MODE_TRAIN,
    WORK_MODE_TEST
} WorkMode_t;

typedef enum {
    TRIAL_IDLE = 0,
    TRIAL_RUNNING,
    TRIAL_DONE
} TrialState_t;

static WorkMode_t g_work_mode = WORK_MODE_IDLE;
static TrialState_t g_trial_state = TRIAL_IDLE;
static uint8_t g_trial_label = 0;
static uint16_t g_trial_row_count = 0;
#define TRIAL_DURATION_ROWS 20             /* 10s / 0.5s = 20 行 */
static uint8_t g_skip_rows = 4;
#endif
// /* ---------- 原有历史数组结构，已由 algo_core 中的实现替代 ---------- */
// typedef struct {
//     float history[ATT_HISTORY_SIZE];
//     int idx;
//     int count;
// } AttnHistory_t;

/**
 * @brief 单个二阶 IIR 滤波器结构体，采用直接 II 型实现。
 * b0/b1/b2 为前向系数，a1/a2 为反馈系数，w1/w2 为滤波器内部状态。
 */

typedef struct {
    float b0, b1, b2, a1, a2, w1, w2;
} IIR_Biquad_t;

/** @brief 多个二阶 IIR 滤波器级联，用于构成带通或陷波滤波器。 */
typedef struct {
    IIR_Biquad_t Sec[MAX_SOS_SECTIONS]; /**< 二阶节数组 */
    uint8_t NumSections;                /**< 实际使用的二阶节数量 */
} IIR_SOS_t;

/**
 * @brief 四通道环形缓冲区。
 * CH0=CP3，CH1=CP4，CH2=C3，CH3=C4，WriteIdx 为当前写入位置。
 */
typedef struct {
    float CH0[FFT_SIZE]; /**< 通道 0：CP3 */
    float CH1[FFT_SIZE]; /**< 通道 1：CP4 */
    float CH2[FFT_SIZE]; /**< 通道 2：C3  */
    float CH3[FFT_SIZE]; /**< 通道 3：C4  */
    int WriteIdx;        /**< 写指针，范围 0~FFT_SIZE-1 */
} RingBuffer_t;

/** @brief 指标的指数滑动平均值，用于注意力/放松度/眨眼等平滑显示。 */
typedef struct {
    float Value;    /**< 当前 EMA 值 */
    uint32_t Count; /**< 已经累计的有效次数 */
    // float LastValid; /**< 保留备用 */
} AttnEma_t;

/** @brief 趋势判断窗口，用于保存最近若干次的通道指标。 */
typedef struct {
    float CH0[TREND_WINDOW_SIZE]; /**< 通道 0 趋势数据 */
    float CH1[TREND_WINDOW_SIZE]; /**< 通道 1 趋势数据 */
    int Idx;                      /**< 当前写入位置 */
    int Count;                    /**< 当前已有有效数据数量 */
} TrendWindow_t;

/**
 * @brief FFT 结果缓存。
 * CH0~CH3 分别保存 CP3、CP4、C3、C4 的频谱幅值，SpectrumHeights 用于原有频谱显示。
 */
typedef struct {
    float CH0Mags[FFT_SIZE / 2 + 1];           /**< 通道 0：CP3 频谱幅值 */
    float CH1Mags[FFT_SIZE / 2 + 1];           /**< 通道 1：CP4 频谱幅值 */
    float CH2Mags[FFT_SIZE / 2 + 1];           /**< 通道 2：C3  频谱幅值 */
    float CH3Mags[FFT_SIZE / 2 + 1];           /**< 通道 3：C4  频谱幅值 */
    uint8_t SpectrumHeights[FFT_SIZE / 2 + 1]; /**< 上位机/显示用频谱高度 */
} FFT_Data_t;

/**
 * @brief 各频段在 FFT 数组中的下标范围。
 * theta：4~8Hz；alpha/mu：8~13Hz；beta：13~30Hz。
 */
typedef struct {
    int ThetaStart, ThetaEnd;
    int AlphaStart, AlphaEnd;
    int BetaStart, BetaEnd;
} BandIndices_t;

/* ---------- FFT 分步状态机 ---------- */
typedef enum {
    FFT_STEP_START = 0,
    FFT_STEP_EXTRACT_RAW_FRAME,
    FFT_STEP_SPECTRUM_RAW,
    FFT_STEP_FREQ_FILTER,
    FFT_STEP_SEND_FREQ_SPECTRUM,
    FFT_STEP_BAND_POWER,
    FFT_STEP_ATTENTION,
    FFT_STEP_UPDATE_EMA,
    FFT_STEP_NORMALIZE,
    FFT_STEP_UPDATE_TREND,
    FFT_STEP_JUDGE_STATE,
    FFT_STEP_EXTRACT_FILT_FRAME,
    FFT_STEP_SPECTRUM_FILT,
    FFT_STEP_SEND_FILT_SPECTRUM,
    FFT_STEP_SEND_BUFFER,
    FFT_STEP_SEND_FOCUS,
    FFT_STEP_FFT_COPY_INPUT,
    FFT_STEP_FFT_TRANSFORM,
    FFT_STEP_FFT_MAGS,
    FFT_STEP_FINISH
} FFT_ProcState;

static uint8_t current_send_frag;       //                (0~7)
static uint8_t current_filt_send_frag;  //                      

static uint8_t raw_send_channel;
static uint8_t filt_send_channel;
static uint8_t proc_channel;
static uint8_t fft_pair_index;  /* 0=CP3/CP4, 1=C3/C4，每次复数 FFT 同算两路 */
/* ---------- FFT                ---------- */
static FFT_Data_t *p_fft_data;                    //      FFT      
static FFT_ProcState fft_next_state;              //             
static FFT_ProcState fft_step = FFT_STEP_FINISH;  //        

static uint8_t fft_src_type;                      // 0=  , 1=     
static uint16_t fft_src_start;                    //                         

static FFT_ProcState target_state_after_buffer;   //                       
static int8_t buffer_counter = 0;                 //               (0~5)
static float theta_pow_ch0, alpha_pow_ch0, beta_pow_ch0;
static float theta_pow_ch1, alpha_pow_ch1, beta_pow_ch1;
static float theta_pow_ch2, alpha_pow_ch2, beta_pow_ch2;
static float theta_pow_ch3, alpha_pow_ch3, beta_pow_ch3;
static float cur_attn0, cur_attn1;
static float cur_relax0, cur_relax1;
static float cur_blink0, cur_blink1;
static uint8_t instant_state, trend_state;

static AttnHistory_t attn_hist[2];
static AttnHistory_t relax_hist[2];
static AttnHistory_t blink_hist[2];
static AttnEma_t attn_ema[2];
static AttnEma_t relax_ema[2];
static AttnEma_t blink_ema[2];

float sum_win_sq = 0.0f;
float norm_factor;
/** @brief              */
static RingBuffer_t RingBuf;
/** @brief                                    */
static RingBuffer_t RingBufFiltered;
/** @brief    0      EMA */
static AttnEma_t AttnEmaCh0;
/** @brief    1      EMA */
static AttnEma_t AttnEmaCh1;
/** @brief            */
static TrendWindow_t TrendWin = {.CH0 = {0}, .CH1 = {0}, .Idx = 0, .Count = 0};
/** @brief FFT         */
static FFT_Data_t FFT_Data;
/** @brief FFT         */
static FFT_Data_t FFT_DataFiltered;

/*      FFT             */
static FFT_ConfigHandle Cplx_FFT_Cfg;
static FFT_Complex_t Cplx_Input[FFT_SIZE];
static FFT_Complex_t Cplx_Output[FFT_SIZE];

/** @brief            */
static BandIndices_t BandIdx;

/** @brief       FFT                          STEP_SIZE        FFT */
static int New_Samples_Count = 0;
/** @brief SPI OLED                    128                 (32) */
static uint8_t Wave_Buf[128] = {32};
/* 6      0.5?30?Hz (250?SPS)                     1 */

IIR_SOS_t Bandpass_CH0 = {
    .Sec = {
            {.b0 = 0.93637848f, .b1 = -1.87275696f, .b2 = 0.93637848f, .a1 = -1.95979169f, .a2 = 0.96227012f, .w1 = 0, .w2 = 0},
            {.b0 = 1.0f, .b1 = -2.0f, .b2 = 1.0f, .a1 = -1.90886498f, .a2 = 0.91127901f, .w1 = 0, .w2 = 0}},
    .NumSections = 2
};
IIR_SOS_t Bandpass_CH1 = {
    .Sec = {
            {.b0 = 0.93637848f, .b1 = -1.87275696f, .b2 = 0.93637848f, .a1 = -1.95979169f, .a2 = 0.96227012f, .w1 = 0, .w2 = 0},
            {.b0 = 1.0f, .b1 = -2.0f, .b2 = 1.0f, .a1 = -1.90886498f, .a2 = 0.91127901f, .w1 = 0, .w2 = 0}},
    .NumSections = 2
};

IIR_SOS_t Bandpass_CH2 = {
    .Sec = {
            {.b0 = 0.93637848f, .b1 = -1.87275696f, .b2 = 0.93637848f, .a1 = -1.95979169f, .a2 = 0.96227012f, .w1 = 0, .w2 = 0},
            {.b0 = 1.0f, .b1 = -2.0f, .b2 = 1.0f, .a1 = -1.90886498f, .a2 = 0.91127901f, .w1 = 0, .w2 = 0}},
    .NumSections = 2
};

IIR_SOS_t Bandpass_CH3 = {
    .Sec = {
            {.b0 = 0.93637848f, .b1 = -1.87275696f, .b2 = 0.93637848f, .a1 = -1.95979169f, .a2 = 0.96227012f, .w1 = 0, .w2 = 0},
            {.b0 = 1.0f, .b1 = -2.0f, .b2 = 1.0f, .a1 = -1.90886498f, .a2 = 0.91127901f, .w1 = 0, .w2 = 0}},
    .NumSections = 2
};
/* 50?Hz                   0.979       MATLAB        */
IIR_SOS_t Notch_CH0 = {
    .Sec = {
            {.b0 = 0.9794827610f, .b1 = -0.6053536377f, .b2 = 0.9794827610f, .a1 = -0.6053536377f, .a2 = 0.9589655220f, .w1 = 0, .w2 = 0},
            {.b0 = 0.9794827610f, .b1 = -0.6053536377f, .b2 = 0.9794827610f, .a1 = -0.6053536377f, .a2 = 0.9589655220f, .w1 = 0, .w2 = 0},
            },
    .NumSections = 2
};
IIR_SOS_t Notch_CH1 = {
    .Sec = {
            {.b0 = 0.9794827610f, .b1 = -0.6053536377f, .b2 = 0.9794827610f, .a1 = -0.6053536377f, .a2 = 0.9589655220f, .w1 = 0, .w2 = 0},
            {.b0 = 0.9794827610f, .b1 = -0.6053536377f, .b2 = 0.9794827610f, .a1 = -0.6053536377f, .a2 = 0.9589655220f, .w1 = 0, .w2 = 0},
            },
    .NumSections = 2
};

IIR_SOS_t Notch_CH2 = {
    .Sec = {
            {.b0 = 0.9794827610f, .b1 = -0.6053536377f, .b2 = 0.9794827610f, .a1 = -0.6053536377f, .a2 = 0.9589655220f, .w1 = 0, .w2 = 0},
            {.b0 = 0.9794827610f, .b1 = -0.6053536377f, .b2 = 0.9794827610f, .a1 = -0.6053536377f, .a2 = 0.9589655220f, .w1 = 0, .w2 = 0},
            },
    .NumSections = 2
};

IIR_SOS_t Notch_CH3 = {
    .Sec = {
            {.b0 = 0.9794827610f, .b1 = -0.6053536377f, .b2 = 0.9794827610f, .a1 = -0.6053536377f, .a2 = 0.9589655220f, .w1 = 0, .w2 = 0},
            {.b0 = 0.9794827610f, .b1 = -0.6053536377f, .b2 = 0.9794827610f, .a1 = -0.6053536377f, .a2 = 0.9589655220f, .w1 = 0, .w2 = 0},
            },
    .NumSections = 2
};
// 256            float  
static const float Hanning_Window[256] = {
    0.0000000f, 0.0001518f, 0.0006070f, 0.0013654f, 0.0024265f, 0.0037897f, 0.0054542f, 0.0074189f,
    0.0096826f, 0.0122440f, 0.0151015f, 0.0182534f, 0.0216978f, 0.0254325f, 0.0294554f, 0.0337639f,
    0.0383554f, 0.0432273f, 0.0483764f, 0.0537997f, 0.0594939f, 0.0654555f, 0.0716810f, 0.0781664f,
    0.0849080f, 0.0919015f, 0.0991429f, 0.1066275f, 0.1143510f, 0.1223086f, 0.1304955f, 0.1389068f,
    0.1475372f, 0.1563817f, 0.1654347f, 0.1746908f, 0.1841445f, 0.1937899f, 0.2036212f, 0.2136324f,
    0.2238175f, 0.2341703f, 0.2446844f, 0.2553535f, 0.2661712f, 0.2771308f, 0.2882257f, 0.2994492f,
    0.3107945f, 0.3222546f, 0.3338226f, 0.3454915f, 0.3572542f, 0.3691036f, 0.3810324f, 0.3930335f,
    0.4050995f, 0.4172231f, 0.4293969f, 0.4416136f, 0.4538658f, 0.4661460f, 0.4784467f, 0.4907605f,
    0.5030800f, 0.5153975f, 0.5277057f, 0.5399971f, 0.5522642f, 0.5644996f, 0.5766958f, 0.5888455f,
    0.6009412f, 0.6129756f, 0.6249415f, 0.6368315f, 0.6486384f, 0.6603551f, 0.6719745f, 0.6834894f,
    0.6948929f, 0.7061782f, 0.7173382f, 0.7283663f, 0.7392558f, 0.7500000f, 0.7605924f, 0.7710267f,
    0.7812964f, 0.7913953f, 0.8013173f, 0.8110564f, 0.8206067f, 0.8299623f, 0.8391176f, 0.8480670f,
    0.8568051f, 0.8653266f, 0.8736263f, 0.8816991f, 0.8895403f, 0.8971449f, 0.9045085f, 0.9116265f,
    0.9184946f, 0.9251086f, 0.9314645f, 0.9375585f, 0.9433868f, 0.9489460f, 0.9542326f, 0.9592435f,
    0.9639755f, 0.9684259f, 0.9725919f, 0.9764710f, 0.9800608f, 0.9833592f, 0.9863641f, 0.9890738f,
    0.9914865f, 0.9936009f, 0.9954156f, 0.9969296f, 0.9981418f, 0.9990517f, 0.9996585f, 0.9999621f,
    0.9999621f, 0.9996585f, 0.9990517f, 0.9981418f, 0.9969296f, 0.9954156f, 0.9936009f, 0.9914865f,
    0.9890738f, 0.9863641f, 0.9833592f, 0.9800608f, 0.9764710f, 0.9725919f, 0.9684259f, 0.9639755f,
    0.9592435f, 0.9542326f, 0.9489460f, 0.9433868f, 0.9375585f, 0.9314645f, 0.9251086f, 0.9184946f,
    0.9116265f, 0.9045085f, 0.8971449f, 0.8895403f, 0.8816991f, 0.8736263f, 0.8653266f, 0.8568051f,
    0.8480670f, 0.8391176f, 0.8299623f, 0.8206067f, 0.8110564f, 0.8013173f, 0.7913953f, 0.7812964f,
    0.7710267f, 0.7605924f, 0.7500000f, 0.7392558f, 0.7283663f, 0.7173382f, 0.7061782f, 0.6948929f,
    0.6834894f, 0.6719745f, 0.6603551f, 0.6486384f, 0.6368315f, 0.6249415f, 0.6129756f, 0.6009412f,
    0.5888455f, 0.5766958f, 0.5644996f, 0.5522642f, 0.5399971f, 0.5277057f, 0.5153975f, 0.5030800f,
    0.4907605f, 0.4784467f, 0.4661460f, 0.4538658f, 0.4416136f, 0.4293969f, 0.4172231f, 0.4050995f,
    0.3930335f, 0.3810324f, 0.3691036f, 0.3572542f, 0.3454915f, 0.3338226f, 0.3222546f, 0.3107945f,
    0.2994492f, 0.2882257f, 0.2771308f, 0.2661712f, 0.2553535f, 0.2446844f, 0.2341703f, 0.2238175f,
    0.2136324f, 0.2036212f, 0.1937899f, 0.1841445f, 0.1746908f, 0.1654347f, 0.1563817f, 0.1475372f,
    0.1389068f, 0.1304955f, 0.1223086f, 0.1143510f, 0.1066275f, 0.0991429f, 0.0919015f, 0.0849080f,
    0.0781664f, 0.0716810f, 0.0654555f, 0.0594939f, 0.0537997f, 0.0483764f, 0.0432273f, 0.0383554f,
    0.0337639f, 0.0294554f, 0.0254325f, 0.0216978f, 0.0182534f, 0.0151015f, 0.0122440f, 0.0096826f,
    0.0074189f, 0.0054542f, 0.0037897f, 0.0024265f, 0.0013654f, 0.0006070f, 0.0001518f, 0.0000000f};

//                0~2  
static const float DC_Window[3] = {
    0.0000000000f, 0.2500000000f, 0.7500000000f};

//                49~53  
static const float Notch_Window[5] = {
    0.7500000000f, 0.2500000000f, 0.0000000000f, 0.2500000000f, 0.7500000000f};

//                62~68  
static const float HF_Window[7] = {
    1.0000000000f, 0.9330127239f, 0.7500000000f, 0.5000000000f,
    0.2500000000f, 0.0669872984f, 0.0000000000f};


const float eps = 1e-6f;

/* ----             ---- */
float IIR_Step (float input, IIR_Biquad_t *filt);
float IIR_SOS_Step (float input, IIR_SOS_t *filt);
static void Apply_Freq_Filter (float *mags);
static inline uint8_t Map_To_Y (float value);
static void Direction_InferAndPrint(void);
// static float percentile (float *arr, int len, float p) {
//     if (len == 0)
//         return 0.0f;
//     //                    len <= 50  
//     float tmp[ATT_HISTORY_SIZE];
//     memcpy (tmp, arr, len * sizeof (float));
//     for (int i = 0; i < len - 1; i++) {
//         for (int j = i + 1; j < len; j++) {
//             if (tmp[i] > tmp[j]) {
//                 float t = tmp[i];
//                 tmp[i] = tmp[j];
//                 tmp[j] = t;
//             }
//         }
//     }
//     float idx = p * (len - 1) / 100.0f;
//     int lo = (int)idx;
//     int hi = lo + 1;
//     if (hi >= len)
//         hi = len - 1;
//     float frac = idx - lo;
//     return tmp[lo] * (1 - frac) + tmp[hi] * frac;
// }
// static float normalize_feature (float feature, AttnHistory_t *hist) {
//     //                 
//     hist->history[hist->idx] = feature;
//     hist->idx = (hist->idx + 1) % ATT_HISTORY_SIZE;
//     if (hist->count < ATT_HISTORY_SIZE)
//         hist->count++;
//     if (hist->count < 10)
//         return 50.0f;  //               
//     float low = percentile (hist->history, hist->count, 10.0f);
//     float high = percentile (hist->history, hist->count, 90.0f);
//     if (high - low < 0.1f)
//         return 50.0f;
//     float score = (feature - low) / (high - low) * 100.0f;
//     if (score < 0.0f)
//         score = 0.0f;
//     if (score > 100.0f)
//         score = 100.0f;
//     return score;
// }
/* 当前没有单独的眨眼检测输入，先使用全 0 的占位统计量。 */
static BlinkStatistics_t dummy_blink_stats = {0};

/* 左右运动想象自动采集状态。
 * dir_phase：当前阶段，REST/LEFT/RIGHT。
 * dir_next_label：下一次从 REST 结束后进入左手还是右手。
 * dir_phase_row_count：当前阶段已经经历的 FFT 行数。
 * dir_skip_row_count：刚切换阶段后需要跳过的行数，避免 FFT 窗口混入上一阶段。
 * dir_round_count：完成一轮 LEFT+RIGHT 后的计数。
 */
static uint8_t dir_phase = DIR_PHASE_REST;
static uint8_t dir_next_label = DIR_LABEL_LEFT;
static uint16_t dir_phase_row_count = 0;
static uint16_t dir_skip_row_count = 0;
static uint16_t dir_round_count = 0;

/* CSP 原始窗口采集计数。
 * csp_collect_row_tick 控制输出频率；csp_window_id 用于 PyCharm 端分组。
 */
static uint8_t csp_collect_row_tick = 0;
static uint32_t csp_window_id = 0;
static uint32_t csp_filtered_sample_count = 0;

/* REST 静息基线。
 * 这里保存最近一个 REST 阶段的 C3/C4 的 μ 波和 β 波平均功率。
 * 后续 LEFT/RIGHT 阶段用当前功率和该基线计算 ERD。
 */
static float dir_rest_mu_ch0 = 0.0f;
static float dir_rest_mu_ch1 = 0.0f;
static float dir_rest_beta_ch0 = 0.0f;
static float dir_rest_beta_ch1 = 0.0f;
static uint16_t dir_rest_count = 0;
static uint8_t dir_rest_valid = 0;

/* 每个 LEFT/RIGHT 阶段的“起点特征”。
 * 进入任务阶段后，前 DIR_SKIP_ROWS_AFTER_SWITCH 行不保存，专门用来估计本阶段初始偏移。
 * 后续真正输出训练数据时，会先减去这个起点，避免把电极慢漂移当成左右特征。
 */
static float dir_task_zero_feature[DIR_FEAT_DIM_LOCAL] = {0.0f};
static uint16_t dir_task_zero_count = 0;
static uint8_t dir_task_zero_valid = 0;

/* 6 维特征的慢漂移抑制基线。
 * 它不是 REST 基线，而是对已经计算出来的 ERD 特征再做慢速去偏移。
 */
static float dir_feature_drift_base[DIR_FEAT_DIM_LOCAL] = {0.0f};
static uint8_t dir_feature_drift_ready = 0;

/* 上位机 RAW 波形显示去基线变量。
 * 只用于显示，不参与训练和 FFT。
 */
static float wave_display_base_ch0 = 0.0f;
static float wave_display_base_ch1 = 0.0f;
static uint8_t wave_display_base_ready = 0;

/* 实时去电极慢漂移状态。
 * drift_base 用来慢慢跟踪每个通道的直流偏置和电极漂移。
 * 注意：这个状态参与 FFT 前预处理，不是上位机显示用的变量。
 */
typedef struct
{
    float drift_base;
    uint8_t init;
} EEG_DriftRemove_t;

static EEG_DriftRemove_t AB_Drift_CH0 = {0.0f, 0};
static EEG_DriftRemove_t AB_Drift_CH1 = {0.0f, 0};
static EEG_DriftRemove_t AB_Drift_CH2 = {0.0f, 0};
static EEG_DriftRemove_t AB_Drift_CH3 = {0.0f, 0};

/* 滚动判断缓存。
 * 说明：不改变原有采集、显示、滤波、FFT流程，只把每次FFT得到的24维特征写入历史缓存。
 * 当前默认最近4行特征平均，约2秒判断窗口；每新来1行特征输出一次，约0.5秒刷新。
 */
static float dir_decision_hist[DIR_DECISION_ROWS][DIR_FEAT_DIM_LOCAL] = {{0.0f}};
static uint8_t dir_decision_write_idx = 0;
static uint8_t dir_decision_count = 0;
static uint8_t dir_result_row_tick = 0;
static uint32_t dir_result_window = 0;


/* OLED 分数显示函数，当前内部显示代码已注释，仅保留接口。 */
static void Update_OLED_Scores(float attn0, float relax0, float blink0, float attn1,  float relax1, float blink1) {
    //                    
    // OLED_ShowString(I2C, 0, 0, "Att:");
    // OLED_ShowNum(I2C, 0, 4, (uint32_t)attn0, 3);
    // OLED_ShowString(I2C, 0, 9, "Rlx:");
    // OLED_ShowNum(I2C, 0, 13, (uint32_t)relax0, 3);
    // OLED_ShowString(I2C, 1, 0, "Blk:");
    // OLED_ShowNum(I2C, 1, 4, (uint32_t)blink0, 3);

    // OLED_ShowString(I2C, 2, 0, "Att:");
    // OLED_ShowNum(I2C, 2, 4, (uint32_t)attn1, 3);
    // OLED_ShowString(I2C, 2, 9, "Rlx:");
    // OLED_ShowNum(I2C, 2, 13, (uint32_t)relax1, 3);
    // OLED_ShowString(I2C, 3, 0, "Blk:");
    // OLED_ShowNum(I2C, 3, 4, (uint32_t)blink1, 3);
}

/* ---- LED 状态指示，保留原有代码备用 ---- */
// static void LED_Init (void) {
//     RCC_HB2PeriphClockCmd (RCC_HB2Periph_GPIOB, ENABLE);
//     GPIO_InitTypeDef GPIO_InitStructure;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
//     GPIO_Init (GPIOB, &GPIO_InitStructure);
//     GPIO_SetBits (GPIOB, GPIO_Pin_0);
//     GPIO_SetBits (GPIOB, GPIO_Pin_1);
//     GPIO_SetBits (GPIOB, GPIO_Pin_2);
// }

static void Show_Trend_State (uint8_t Trend_State) {
    if (Trend_State == STATE_RELAX) {
        GPIO_ResetBits (GPIOB, GPIO_Pin_0);
        GPIO_SetBits (GPIOB, GPIO_Pin_1);
        GPIO_SetBits (GPIOB, GPIO_Pin_2);
    } else if (Trend_State == STATE_NEUTRAL) {
        GPIO_ResetBits (GPIOB, GPIO_Pin_1);
        GPIO_SetBits (GPIOB, GPIO_Pin_0);
        GPIO_SetBits (GPIOB, GPIO_Pin_2);
    } else if (Trend_State == STATE_FOCUS) {
        GPIO_ResetBits (GPIOB, GPIO_Pin_2);
        GPIO_SetBits (GPIOB, GPIO_Pin_1);
        GPIO_SetBits (GPIOB, GPIO_Pin_0);
    }
}

/* ----------             ---------- */


/**
 * @brief                     FFT                  
 * @param  f_low                (Hz)
 * @param  f_high               (Hz)
 * @param  bin_width         (Hz)       SAMPLE_RATE / FFT_SIZE
 * @param  out_Start              
 * @param  out_End                 
 * @retval   
 */
static void Compute_Band_Indices (float f_low, float f_high, float bin_width,
                                  int *out_Start, int *out_End) {
    int start = (int)(f_low / bin_width + 0.5f);  //         
    int end = (int)(f_high / bin_width + 0.5f);   //         
    if (start < 0)
        start = 0;
    if (end >= (FFT_SIZE / 2 + 1))
        end = FFT_SIZE / 2;  //        N/2
    if (start > end)
        start = end;         //     
    *out_Start = start;
    *out_End = end;
}

// /**
//  * @brief             PSD       
//  * @param  mags:                     129       V  
//  * @param  start:                     
//  * @param  end:                        
//  * @param  norm_factor:           = fs *   w?
//  * @return               V?  
//  */
// static float Compute_Band_Power (float *mags, int start, int end, float norm_factor) {
//     if (start >= end)
//         return 0.0f;
//     const float freq_res = SAMPLE_RATE / FFT_SIZE;  //   f = 250/256    0.9765625 Hz
//     float integral = 0.0f;
//     //            P(f) df       (P_i + P_{i+1})/2 *   f
//     for (int i = start; i < end; i++) {
//         float psd_i = (mags[i] * mags[i]) / norm_factor;
//         float psd_ip1 = (mags[i + 1] * mags[i + 1]) / norm_factor;
//         integral += (psd_i + psd_ip1) * 0.5f * freq_res;
//     }
//     return integral;
// }
/**
 * @brief 原有注意力计算函数，当前已改用 algo_core 中的实现，保留注释备用。
 */
// static float Compute_Attention (float Theta_pow, float Alpha_pow, float Beta_pow) {
//     float ratio = Beta_pow / (Alpha_pow + Theta_pow + eps);
//     return logf (ratio + eps);
// }



/* ---------- 神经网络训练用特征辅助函数 ----------
 * 重要修改说明：
 * 1. 本版本不再用 task/rest 的 ERD 比值作为训练特征。
 *    原来的 ERD 算法非常依赖 REST 基线，只要静息基线稍微不稳，log(task/rest) 就会被放大成几十万甚至上百万。
 * 2. 当前改为“单个 FFT 窗口内部归一化特征”：只使用当前 256 点窗口的 C3/C4 频带关系，
 *    不再要求静息时 DIRCSV 必须接近 0。
 * 3. 后续交给 Python 神经网络训练脚本做标准化、异常样本过滤和 MLP 训练。
 * 4. 这样即使电极存在慢漂移，训练数据也不会被 REST 基线错误严重放大，更适合收集想象/轻握数据。
 */

/* 限幅函数：避免电极瞬间异常导致串口训练数据过大。 */
static float Direction_ClampFloat(float x, float min_v, float max_v)
{
    if (x < min_v) return min_v;
    if (x > max_v) return max_v;
    return x;
}

/* 安全功率：功率单位通常很小，低于 eps 时用 eps 代替，避免 log(0)。 */
static float Direction_SafePower(float p)
{
    if (p < DIR_ERD_POWER_EPS) return DIR_ERD_POWER_EPS;
    return p;
}

/* 实时一阶去电极慢漂移。
 * 输入 x 是当前采样点的电压值，单位保持和 ADS1299_CodeToVolt 输出一致。
 * 输出 y 是去掉慢变化基线后的信号，后面再进入 50Hz 陷波和 FFT 环形缓冲区。
 *
 * 公式：
 *   y    = x - base
 *   base = k * base + (1-k) * x
 *
 * 操作位置：Signal_Analysis_Start() 主循环中，ADS1299_CodeToVolt() 后立刻调用。
 */
static float AB_RemoveRealtimeDrift(float x, EEG_DriftRemove_t *st)
{
#if AB_REALTIME_DRIFT_REMOVE_ENABLE
    float y;

    if (!st->init) {
        st->drift_base = x;
        st->init = 1;
    }

    y = x - st->drift_base;
    st->drift_base = AB_DRIFT_K * st->drift_base + (1.0f - AB_DRIFT_K) * x;

    return y;
#else
    (void)st;
    return x;
#endif
}

/* FFT 前去均值 + 去线性趋势。
 * 输入是一段 256 点窗口，先估计线性趋势 y = mean + slope * (n - center)，
 * 再把该趋势减掉，最后再进入 Hann 加窗和 FFT。
 * 这样可以削弱电极慢漂移、参考电极慢变和窗口斜坡对 alpha/beta 功率的污染。
 */
static float AB_RemoveMeanAndLinearTrend(float sample, float mean, float slope, int n)
{
#if AB_FFT_DETREND_ENABLE
    const float center = ((float)FFT_SIZE - 1.0f) * 0.5f;
    float x = (float)n - center;
    return sample - (mean + slope * x);
#else
    (void)mean;
    (void)slope;
    (void)n;
    return sample;
#endif
}

/* 计算 256 点窗口的均值和线性趋势斜率。
 * 这里用中心化坐标 x = n - center，截距就是 mean，计算量小，适合单片机。
 */
static void AB_CalcMeanAndSlope(float *src, uint16_t start, float *out_mean, float *out_slope)
{
#if AB_FFT_DETREND_ENABLE
    float sum_y = 0.0f;
    float sum_xy = 0.0f;
    const float center = ((float)FFT_SIZE - 1.0f) * 0.5f;
    const float sum_x2 = ((float)FFT_SIZE * ((float)FFT_SIZE * (float)FFT_SIZE - 1.0f)) / 12.0f;

    for (int j = 0; j < FFT_SIZE; j++) {
        uint16_t idx = (start + j) & (FFT_SIZE - 1);
        float y = src[idx];
        float x = (float)j - center;
        sum_y += y;
        sum_xy += x * y;
    }

    *out_mean = sum_y / (float)FFT_SIZE;
    *out_slope = sum_xy / sum_x2;
#else
    (void)src;
    (void)start;
    *out_mean = 0.0f;
    *out_slope = 0.0f;
#endif
}


/* 根据通道号获取环形缓冲区指针。CH0=CP3, CH1=CP4, CH2=C3, CH3=C4。 */
static float *RingBuffer_GetChannel(RingBuffer_t *rb, uint8_t ch)
{
    switch (ch) {
    case 0: return rb->CH0;
    case 1: return rb->CH1;
    case 2: return rb->CH2;
    case 3: return rb->CH3;
    default: return rb->CH0;
    }
}

/* 根据通道号获取 FFT 幅值缓存。 */
static float *FFT_Data_GetMags(FFT_Data_t *data, uint8_t ch)
{
    switch (ch) {
    case 0: return data->CH0Mags;
    case 1: return data->CH1Mags;
    case 2: return data->CH2Mags;
    case 3: return data->CH3Mags;
    default: return data->CH0Mags;
    }
}


/* alpha/beta 提取实验：四舍五入为 long，便于串口打印。 */
static long AB_RoundToLong(float x)
{
    if (x >= 0.0f) return (long)(x + 0.5f);
    return (long)(x - 0.5f);
}

/* 安全 log 比值，输出为 log(a / b) * AB_RATIO_SCALE。
 * 用于观察相对特征：左右通道比值、beta/alpha 比值。
 */
static long AB_LogRatioScaled(float a, float b)
{
    float aa = a;
    float bb = b;
    if (aa < AB_REL_EPS) aa = AB_REL_EPS;
    if (bb < AB_REL_EPS) bb = AB_REL_EPS;
    return AB_RoundToLong(logf(aa / bb) * AB_RATIO_SCALE);
}


/* alpha/beta 提取实验：10*log10(power)，再放大 AB_DB_SCALE。
 * 使用 logf 换算，避免部分库没有 log10f。
 */
static long AB_PowerToDb100(float p)
{
    p = Direction_SafePower(p);
    return AB_RoundToLong((10.0f * 0.4342944819f * logf(p)) * AB_DB_SCALE);
}

/* 打印 alpha/beta 提取结果。
 * 输出格式：
 * ABCSV,
 *   ch0_alpha_p15,ch1_alpha_p15,ch0_beta_p15,ch1_beta_p15,        绝对功率，只看信号强弱，允许缓慢漂移
 *   ch0_alpha_pct,ch1_alpha_pct,ch0_beta_pct,ch1_beta_pct,        频段占比，重点观察，抗整体幅值漂移
 *   ch0_alpha_db100,ch1_alpha_db100,ch0_beta_db100,ch1_beta_db100,对数功率，观察趋势
 *   alpha_lr,beta_lr,ch0_beta_alpha,ch1_beta_alpha,total_lr       相对比值，后续更适合训练
 */
static void AB_PrintAlphaBetaCSV(void)
{
#if AB_EXTRACT_PRINT_ENABLE
    float a0 = Direction_SafePower(alpha_pow_ch0);
    float a1 = Direction_SafePower(alpha_pow_ch1);
    float b0 = Direction_SafePower(beta_pow_ch0);
    float b1 = Direction_SafePower(beta_pow_ch1);

    /* total 使用 theta + alpha + beta，即 4~30Hz 总功率。
     * 这样如果整体信号同时变大，占比特征会比绝对功率稳定。
     */
    float t0 = Direction_SafePower(theta_pow_ch0 + alpha_pow_ch0 + beta_pow_ch0);
    float t1 = Direction_SafePower(theta_pow_ch1 + alpha_pow_ch1 + beta_pow_ch1);

    long a0_p15 = AB_RoundToLong(a0 * AB_POWER_SCALE);
    long a1_p15 = AB_RoundToLong(a1 * AB_POWER_SCALE);
    long b0_p15 = AB_RoundToLong(b0 * AB_POWER_SCALE);
    long b1_p15 = AB_RoundToLong(b1 * AB_POWER_SCALE);

    long a0_pct = AB_RoundToLong((a0 / t0) * AB_PCT_SCALE);
    long a1_pct = AB_RoundToLong((a1 / t1) * AB_PCT_SCALE);
    long b0_pct = AB_RoundToLong((b0 / t0) * AB_PCT_SCALE);
    long b1_pct = AB_RoundToLong((b1 / t1) * AB_PCT_SCALE);

    long a0_db = AB_PowerToDb100(a0);
    long a1_db = AB_PowerToDb100(a1);
    long b0_db = AB_PowerToDb100(b0);
    long b1_db = AB_PowerToDb100(b1);

    /* 相对特征：如果 alpha/beta 绝对功率一起上飘，下面这些应该更稳定。
     * alpha_lr / beta_lr：C3 与 C4 的频段功率比。
     * beta_alpha：同一通道 beta 相对 alpha 的比例。
     * total_lr：两个通道 4~30Hz 总功率比。
     */
    long alpha_lr = AB_LogRatioScaled(a0, a1);
    long beta_lr  = AB_LogRatioScaled(b0, b1);
    long ch0_beta_alpha = AB_LogRatioScaled(b0, a0);
    long ch1_beta_alpha = AB_LogRatioScaled(b1, a1);
    long total_lr = AB_LogRatioScaled(t0, t1);

    Serial_Printf(DIR_TEXT_PORT,
                  "ABCSV,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld\r\n",
                  a0_p15, a1_p15, b0_p15, b1_p15,
                  a0_pct, a1_pct, b0_pct, b1_pct,
                  a0_db, a1_db, b0_db, b1_db,
                  alpha_lr, beta_lr, ch0_beta_alpha, ch1_beta_alpha, total_lr);
#endif
}

/* 计算 log(a/b)，并做限幅。
 * 输出不是 ERD，而是两个通道或两个频段之间的相对关系。
 */
static float Direction_LogRatio(float a, float b)
{
    float v = logf(Direction_SafePower(a) / Direction_SafePower(b));
    return Direction_ClampFloat(v, -DIR_LOG_FEATURE_CLAMP, DIR_LOG_FEATURE_CLAMP);
}

/* 切换到 REST 阶段时的保留接口。
 * 当前神经网络特征不依赖 REST 基线，所以这里只做状态标记，不再累计 REST 功率。
 */
static void Direction_ResetRestBaseline(void)
{
    dir_rest_mu_ch0 = 0.0f;
    dir_rest_mu_ch1 = 0.0f;
    dir_rest_beta_ch0 = 0.0f;
    dir_rest_beta_ch1 = 0.0f;
    dir_rest_count = 0;
    dir_rest_valid = 1;
}

/* 清空任务起点归零参数。
 * 当前版本不使用任务起点归零，避免把刚开始想象/轻握时的真实特征抵消掉。
 */
static void Direction_ResetTaskZero(void)
{
    for (int i = 0; i < DIR_FEAT_DIM_LOCAL; i++) {
        dir_task_zero_feature[i] = 0.0f;
        dir_feature_drift_base[i] = 0.0f;
    }
    dir_task_zero_count = 0;
    dir_task_zero_valid = 1;
    dir_feature_drift_ready = 0;
}

/* REST 阶段调用：当前只保持接口，不再用 REST 建立 ERD 基线。
 * 这样可以避免“静息基线一错，后面全错”的问题。
 */
static void Direction_UpdateRestBaseline(void)
{
    dir_rest_count++;
    dir_rest_valid = 1;
}

/* 构造神经网络训练用 24 维四通道特征。
 * 通道映射：CH0=CP3, CH1=CP4, CH2=C3, CH3=C4。
 * 特征全部来自当前 FFT 窗口内部的相对关系，不依赖上一段 REST。
 *
 * 0~7：左右半球比值，CP3/CP4 与 C3/C4 分别计算 mu、beta、theta、total。
 * 8~19：每个通道自身的 mu 占比、beta 占比、beta/mu 比值。
 * 20~23：同侧前后关系，CP3/C3 与 CP4/C4 的 mu、beta 比值。
 */
static void Direction_BuildRawFeature(float feature[DIR_FEAT_DIM_LOCAL])
{
    float theta[NUM_CHANNELS] = {
        Direction_SafePower(theta_pow_ch0), Direction_SafePower(theta_pow_ch1),
        Direction_SafePower(theta_pow_ch2), Direction_SafePower(theta_pow_ch3)
    };
    float mu[NUM_CHANNELS] = {
        Direction_SafePower(alpha_pow_ch0), Direction_SafePower(alpha_pow_ch1),
        Direction_SafePower(alpha_pow_ch2), Direction_SafePower(alpha_pow_ch3)
    };
    float beta[NUM_CHANNELS] = {
        Direction_SafePower(beta_pow_ch0), Direction_SafePower(beta_pow_ch1),
        Direction_SafePower(beta_pow_ch2), Direction_SafePower(beta_pow_ch3)
    };
    float total[NUM_CHANNELS];

    for (int i = 0; i < NUM_CHANNELS; i++) {
        total[i] = Direction_SafePower(theta[i] + mu[i] + beta[i]);
    }

    feature[0] = Direction_LogRatio(mu[0],    mu[1])    * DIR_FEATURE_SCALE;  /* cp_mu_lr */
    feature[1] = Direction_LogRatio(beta[0],  beta[1])  * DIR_FEATURE_SCALE;  /* cp_beta_lr */
    feature[2] = Direction_LogRatio(mu[2],    mu[3])    * DIR_FEATURE_SCALE;  /* c_mu_lr */
    feature[3] = Direction_LogRatio(beta[2],  beta[3])  * DIR_FEATURE_SCALE;  /* c_beta_lr */
    feature[4] = Direction_LogRatio(theta[0], theta[1]) * DIR_FEATURE_SCALE;  /* cp_theta_lr */
    feature[5] = Direction_LogRatio(theta[2], theta[3]) * DIR_FEATURE_SCALE;  /* c_theta_lr */
    feature[6] = Direction_LogRatio(total[0], total[1]) * DIR_FEATURE_SCALE;  /* cp_total_lr */
    feature[7] = Direction_LogRatio(total[2], total[3]) * DIR_FEATURE_SCALE;  /* c_total_lr */

    feature[8]  = Direction_LogRatio(mu[0],   total[0]) * DIR_FEATURE_SCALE;  /* cp3_mu_frac */
    feature[9]  = Direction_LogRatio(mu[1],   total[1]) * DIR_FEATURE_SCALE;  /* cp4_mu_frac */
    feature[10] = Direction_LogRatio(mu[2],   total[2]) * DIR_FEATURE_SCALE;  /* c3_mu_frac */
    feature[11] = Direction_LogRatio(mu[3],   total[3]) * DIR_FEATURE_SCALE;  /* c4_mu_frac */
    feature[12] = Direction_LogRatio(beta[0], total[0]) * DIR_FEATURE_SCALE;  /* cp3_beta_frac */
    feature[13] = Direction_LogRatio(beta[1], total[1]) * DIR_FEATURE_SCALE;  /* cp4_beta_frac */
    feature[14] = Direction_LogRatio(beta[2], total[2]) * DIR_FEATURE_SCALE;  /* c3_beta_frac */
    feature[15] = Direction_LogRatio(beta[3], total[3]) * DIR_FEATURE_SCALE;  /* c4_beta_frac */
    feature[16] = Direction_LogRatio(beta[0], mu[0])    * DIR_FEATURE_SCALE;  /* cp3_beta_mu */
    feature[17] = Direction_LogRatio(beta[1], mu[1])    * DIR_FEATURE_SCALE;  /* cp4_beta_mu */
    feature[18] = Direction_LogRatio(beta[2], mu[2])    * DIR_FEATURE_SCALE;  /* c3_beta_mu */
    feature[19] = Direction_LogRatio(beta[3], mu[3])    * DIR_FEATURE_SCALE;  /* c4_beta_mu */

    feature[20] = Direction_LogRatio(mu[0],   mu[2])    * DIR_FEATURE_SCALE;  /* left_mu_cp3_c3 */
    feature[21] = Direction_LogRatio(mu[1],   mu[3])    * DIR_FEATURE_SCALE;  /* right_mu_cp4_c4 */
    feature[22] = Direction_LogRatio(beta[0], beta[2])  * DIR_FEATURE_SCALE;  /* left_beta_cp3_c3 */
    feature[23] = Direction_LogRatio(beta[1], beta[3])  * DIR_FEATURE_SCALE;  /* right_beta_cp4_c4 */
}

/* 当前版本不再用任务阶段前几行做归零。
 * skip 仍然保留，但只是丢弃切换初期的 FFT 过渡窗口。
 */
static void Direction_UpdateTaskZero(void)
{
    dir_task_zero_count++;
    dir_task_zero_valid = 1;
}

/* 当前版本不启用在线慢漂移扣除，漂移问题交给“相对特征 + Python 标准化 + 异常样本过滤”处理。 */
static void Direction_RemoveFeatureSlowDrift(float feature[DIR_FEAT_DIM_LOCAL])
{
    (void)feature;
}

/* 构造最终输出/推理特征。 */
static void Direction_BuildFeature(float feature[DIR_FEAT_DIM_LOCAL])
{
    Direction_BuildRawFeature(feature);
    Direction_RemoveFeatureSlowDrift(feature);
}

/* 上位机 RAW 波形显示用慢速去基线。
 * 重要说明：
 * 1. 这个函数只处理上位机显示值 show_ch0/show_ch1；
 * 2. 原始 val_ch0/val_ch1 仍然直接进入滤波、FFT、环形缓冲和训练特征；
 * 3. 因此不会改变神经网络训练数据，也不会改变频谱分析结果；
 * 4. 基线跟踪采用很小的 alpha，只去掉几秒级以上的慢漂移，尽量保留真实脑电振荡。
 */
static void Waveform_RemoveDisplayBaseline(float in_ch0, float in_ch1, float *out_ch0, float *out_ch1)
{
#if WAVE_DISPLAY_REMOVE_BASELINE
    if (!wave_display_base_ready) {
        wave_display_base_ch0 = in_ch0;
        wave_display_base_ch1 = in_ch1;
        wave_display_base_ready = 1;
    }

    /* 慢速跟踪直流偏移。默认 tau=8s，250Hz 下 alpha≈0.0005，
     * 比之前 0.010 慢很多，不会明显压小脑电波形。
     */
    wave_display_base_ch0 += WAVE_DISPLAY_BASELINE_ALPHA * (in_ch0 - wave_display_base_ch0);
    wave_display_base_ch1 += WAVE_DISPLAY_BASELINE_ALPHA * (in_ch1 - wave_display_base_ch1);

    *out_ch0 = in_ch0 - wave_display_base_ch0;
    *out_ch1 = in_ch1 - wave_display_base_ch1;
#else
    *out_ch0 = in_ch0;
    *out_ch1 = in_ch1;
#endif
}

/* 打印 DIRCSV 表头，方便 Python 脚本识别当前特征格式。 */
static void Direction_PrintFeatureHeader(void)
{
    if (g_eeg_app_mode != EEG_APP_MODE_COLLECT) {
        return;   // 仅采集模式打印表头
    }
#if DIR_PRINT_HEADER_ON_START
    Serial_Printf(DIR_TEXT_PORT, "DIRCSV,label,...\r\n");
    Serial_Printf(DIR_TEXT_PORT, "DIRCFG,...\r\n");
    Serial_Printf(DIR_TEXT_PORT, "TASK=REST,prepare,then left hand\r\n");
#endif
}

/* 清空滚动判断缓存。 */
static void Direction_Infer1sReset(void)
{
    for (int r = 0; r < DIR_DECISION_ROWS; r++) {
        for (int i = 0; i < DIR_FEAT_DIM_LOCAL; i++) {
            dir_decision_hist[r][i] = 0.0f;
        }
    }
    dir_decision_write_idx = 0;
    dir_decision_count = 0;
    dir_result_row_tick = 0;
    dir_result_window = 0;
}
/* ==================== 命令解析函数（新增） ==================== */
#if CMD_MODE_ENABLE
static void Parse_Command(const char *cmd)
{
    /* ---- 去除命令末尾的 '\r' 或 '\n' ---- */
    char clean_cmd[64];
    strncpy(clean_cmd, cmd, sizeof(clean_cmd) - 1);
    clean_cmd[sizeof(clean_cmd) - 1] = '\0';
    size_t len = strlen(clean_cmd);
    while (len > 0 && (clean_cmd[len-1] == '\r' || clean_cmd[len-1] == '\n')) {
        clean_cmd[len-1] = '\0';
        len--;
    }

    Serial_Printf(SERIAL_PORT_DEBUG, "DEBUG_RX: %s\r\n", clean_cmd); 

    if (strcmp(clean_cmd, "MODE,TRAIN") == 0) {
        g_work_mode = WORK_MODE_TRAIN;
        g_trial_state = TRIAL_IDLE;
        g_paused = 0; 
        Direction_ResetRestBaseline();
        Serial_Printf(SERIAL_PORT_DEBUG, "READY_TRAIN\r\n");
        return;
    }
    if (strcmp(clean_cmd, "MODE,TEST") == 0) {
        g_work_mode = WORK_MODE_TEST;
        g_trial_state = TRIAL_IDLE;
        g_paused = 0; 
        Direction_Infer1sReset();
        Serial_Printf(SERIAL_PORT_DEBUG, "READY_TEST\r\n");
        return;
    }
     /* ---- 新增：动态设置 EEG 工作模式 ---- */
    if (strncmp(clean_cmd, "MODE,SET,", 9) == 0) {
        int mode = atoi(clean_cmd + 9);
        if (mode == EEG_APP_MODE_COLLECT || mode == EEG_APP_MODE_INFER || mode == EEG_APP_MODE_COLLECT_CSP) {
            g_eeg_app_mode = (uint8_t)mode;
            Serial_Printf(SERIAL_PORT_DEBUG, "MODE_SET_OK,%d\r\n", mode);
            /* 重置自动采集状态机 */
            dir_phase = DIR_PHASE_REST;
            dir_phase_row_count = 0;
            dir_skip_row_count = 0;
            dir_round_count = 0;
            csp_collect_row_tick = 0;
            csp_window_id = 0;
        } else {
            Serial_Printf(SERIAL_PORT_DEBUG, "ERROR,INVALID_MODE\r\n");
        }
        return;
    }
    if (strncmp(clean_cmd, "TRIAL,", 6) == 0) {
        if (g_work_mode != WORK_MODE_TRAIN) {
            Serial_Printf(SERIAL_PORT_DEBUG, "ERROR,NOT_TRAIN_MODE\r\n");
            return;
        }
        if (g_trial_state == TRIAL_RUNNING) {
            Serial_Printf(SERIAL_PORT_DEBUG, "ERROR,BUSY\r\n");
            return;
        }
        const char *side = clean_cmd + 6;
        if (strcmp(side, "LEFT") == 0) {
            g_trial_label = DIR_LABEL_LEFT;
        } else if (strcmp(side, "RIGHT") == 0) {
            g_trial_label = DIR_LABEL_RIGHT;
        } else {
            Serial_Printf(SERIAL_PORT_DEBUG, "ERROR,INVALID_SIDE\r\n");
            return;
        }
        g_trial_state = TRIAL_RUNNING;
        g_trial_row_count = 0;
        g_paused = 0; 
        Direction_ResetTaskZero();
        Serial_Printf(SERIAL_PORT_DEBUG, "TASK,%s,start\r\n", (g_trial_label==DIR_LABEL_LEFT)?"LEFT":"RIGHT");
        return;
    }
    if (strcmp(clean_cmd, "STOP") == 0) {
        g_trial_state = TRIAL_DONE;
        g_paused = 1;                     // 设置暂停标志
        Serial_Printf(SERIAL_PORT_DEBUG, "TASK,STOPPED\r\n");
        return;
    }
    if (strcmp(clean_cmd, "STATUS") == 0) {
        Serial_Printf(SERIAL_PORT_DEBUG, "STATUS,mode=%d,trial=%d,row=%d\r\n",
                      g_work_mode, g_trial_state, g_trial_row_count);
        return;
    }
    Serial_Printf(SERIAL_PORT_DEBUG, "ERROR,UNKNOWN_CMD\r\n");
}
#endif


/* 输出一行训练数据。
 * 格式：DIRCSV,label,24个四通道神经网络窗口特征。
 * label=2 表示左手想象，label=3 表示右手想象。
 */
static void Direction_PrintFeatureCSV(uint8_t label)
{
    float feature[DIR_FEAT_DIM_LOCAL];
    char line[512];
    int len = 0;

    if (!dir_rest_valid) {
        return;
    }

    Direction_BuildFeature(feature);

    len += snprintf(line + len, sizeof(line) - len, "DIRCSV,%d", label);
    for (int i = 0; i < DIR_FEAT_DIM_LOCAL; i++) {
        int32_t v = (feature[i] >= 0.0f) ? (int32_t)(feature[i] + 0.5f) : (int32_t)(feature[i] - 0.5f);
        if (len < (int)sizeof(line) - 24) {
            len += snprintf(line + len, sizeof(line) - len, ",%ld", (long)v);
        }
    }

    if (len < (int)sizeof(line) - 3) {
        len += snprintf(line + len, sizeof(line) - len, "\r\n");
        Serial_SendArray_DMA(DIR_TEXT_PORT, (const uint8_t *)line, (uint16_t)len);
    }
}


/* CSP 训练窗口输出。
 * 输出格式适合 PyCharm 端 parser 直接读取：
 *   CSP_BEGIN,label=2,win=1,fs=250,n=256,ch=4,unit=uVx1000,order=CP3_CP4_C3_C4
 *   CSP,1,0,ch0,ch1,ch2,ch3
 *   ... 共 256 行 ...
 *   CSP_END,label=2,win=1
 *
 * 注意：这里使用 RingBufFiltered，也就是 V3F 已经完成带通滤波后的 CP3/CP4/C3/C4。
 * 这样 Python 端训练的 CSP，与 V5F 实时推理链路中的预处理目标保持一致。
 */
static int32_t CSP_VoltToUVX1000(float v)
{
    float x = v * CSP_OUTPUT_SCALE_UVX1000;
    if (x > 2147483000.0f) return 2147483000L;
    if (x < -2147483000.0f) return -2147483000L;
    return (x >= 0.0f) ? (int32_t)(x + 0.5f) : (int32_t)(x - 0.5f);
}

static void CSP_PrintWindowCSV(uint8_t label)
{
    char line[160];
    uint16_t j;
    uint32_t win_id;
    int start;

    if (csp_filtered_sample_count < CSP_WIN_SIZE) {
        return;
    }

    win_id = ++csp_window_id;
    start = RingBufFiltered.WriteIdx;

    snprintf(line, sizeof(line),
             "CSP_BEGIN,label=%u,win=%lu,fs=250,n=%u,ch=4,unit=uVx1000,order=CP3_CP4_C3_C4\r\n",
             (unsigned int)label,
             (unsigned long)win_id,
             (unsigned int)CSP_WIN_SIZE);
    Serial_SendString(DIR_TEXT_PORT, line);

    for (j = 0; j < CSP_WIN_SIZE; j++) {
        uint16_t idx = (uint16_t)((start + j) & (CSP_WIN_SIZE - 1u));
        int32_t ch0 = CSP_VoltToUVX1000(RingBufFiltered.CH0[idx]);
        int32_t ch1 = CSP_VoltToUVX1000(RingBufFiltered.CH1[idx]);
        int32_t ch2 = CSP_VoltToUVX1000(RingBufFiltered.CH2[idx]);
        int32_t ch3 = CSP_VoltToUVX1000(RingBufFiltered.CH3[idx]);

        snprintf(line, sizeof(line),
                 "CSP,%lu,%u,%ld,%ld,%ld,%ld\r\n",
                 (unsigned long)win_id,
                 (unsigned int)j,
                 (long)ch0,
                 (long)ch1,
                 (long)ch2,
                 (long)ch3);
        Serial_SendString(DIR_TEXT_PORT, line);
    }

    snprintf(line, sizeof(line),
             "CSP_END,label=%u,win=%lu\r\n",
             (unsigned int)label,
             (unsigned long)win_id);
    Serial_SendString(DIR_TEXT_PORT, line);
}

/* CSP 原始窗口采集状态机。
 * 保留原来的 LEFT/REST/RIGHT/REST 节奏，但输出的是时域窗口而不是 DIRCSV 频域特征。
 */
static void Direction_AutoCollectCSPProcess(void)
{
#if CMD_MODE_ENABLE
    if (g_paused) return;
    if (g_work_mode == WORK_MODE_TRAIN && g_trial_state == TRIAL_RUNNING) {
        g_trial_row_count++;
        if (g_trial_row_count > g_skip_rows) {
            csp_collect_row_tick++;
            if (csp_collect_row_tick >= CSP_COLLECT_EVERY_ROWS) {
                csp_collect_row_tick = 0;
                CSP_PrintWindowCSV(g_trial_label);
            }
        }
        if (g_trial_row_count >= TRIAL_DURATION_ROWS) {
            g_trial_state = TRIAL_DONE;
            Serial_Printf(SERIAL_PORT_DEBUG, "TASK,DONE\r\n");
        }
        return;
    }
    if (g_work_mode == WORK_MODE_TEST) {
        return;
    }
    return;
#else
    /* 原有自动 CSP 采集代码（保持不变）—— 此处省略 */
    uint8_t label_to_print = 0;
    if (dir_phase == DIR_PHASE_REST) {
        Direction_UpdateRestBaseline();
    } else if (dir_phase == DIR_PHASE_LEFT || dir_phase == DIR_PHASE_RIGHT) {
        if (dir_skip_row_count > 0) {
            dir_skip_row_count--;
            csp_collect_row_tick = 0;
        } else {
            csp_collect_row_tick++;
            if (csp_collect_row_tick >= CSP_COLLECT_EVERY_ROWS) {
                csp_collect_row_tick = 0;
                label_to_print = (dir_phase == DIR_PHASE_LEFT) ? DIR_LABEL_LEFT : DIR_LABEL_RIGHT;
            }
        }
    }
    if (label_to_print != 0) {
        CSP_PrintWindowCSV(label_to_print);
    }
    dir_phase_row_count++;
    if (dir_phase == DIR_PHASE_REST) {
        if (dir_phase_row_count >= DIR_REST_ROWS) {
            dir_phase_row_count = 0;
            dir_skip_row_count = DIR_SKIP_ROWS_AFTER_SWITCH;
            csp_collect_row_tick = 0;
            if (dir_next_label == DIR_LABEL_LEFT) {
                dir_phase = DIR_PHASE_LEFT;
                Serial_Printf(DIR_TEXT_PORT, "TASK=LEFT,start_light_grip_or_imagine,skip=%d,feature=CSPWIN\r\n", DIR_SKIP_ROWS_AFTER_SWITCH);
            } else {
                dir_phase = DIR_PHASE_RIGHT;
                Serial_Printf(DIR_TEXT_PORT, "TASK=RIGHT,start_light_grip_or_imagine,skip=%d,feature=CSPWIN\r\n", DIR_SKIP_ROWS_AFTER_SWITCH);
            }
        }
    } else if (dir_phase == DIR_PHASE_LEFT) {
        if (dir_phase_row_count >= DIR_COLLECT_ROWS_PER_PHASE) {
            dir_phase_row_count = 0;
            dir_phase = DIR_PHASE_REST;
            dir_next_label = DIR_LABEL_RIGHT;
            csp_collect_row_tick = 0;
            Direction_ResetRestBaseline();
            Serial_Printf(DIR_TEXT_PORT, "TASK=REST,next right\r\n");
        }
    } else if (dir_phase == DIR_PHASE_RIGHT) {
        if (dir_phase_row_count >= DIR_COLLECT_ROWS_PER_PHASE) {
            dir_phase_row_count = 0;
            dir_phase = DIR_PHASE_REST;
            dir_next_label = DIR_LABEL_LEFT;
            csp_collect_row_tick = 0;
            dir_round_count++;
            Direction_ResetRestBaseline();
            Serial_Printf(DIR_TEXT_PORT, "TASK=REST,next left,round=%d\r\n", dir_round_count);
        }
    }
#endif
}

/* 自动采集状态机。
 * 流程：REST 提示放松 -> LEFT/RIGHT 阶段输出神经网络训练特征 -> 循环。
 * 注意：REST 阶段不再作为 ERD 基线，只用于让使用者放松和分隔左右任务。
 */
static void Direction_AutoCollectProcess(void)
{
#if CMD_MODE_ENABLE
    if (g_paused) return;
    if (g_work_mode == WORK_MODE_TRAIN && g_trial_state == TRIAL_RUNNING) {
        g_trial_row_count++;
        if (g_trial_row_count > g_skip_rows) {
            Direction_PrintFeatureCSV(g_trial_label);
        }
        if (g_trial_row_count >= TRIAL_DURATION_ROWS) {
            g_trial_state = TRIAL_DONE;
            Serial_Printf(SERIAL_PORT_DEBUG, "TASK,DONE\r\n");
        }
        return;
    }
    if (g_work_mode == WORK_MODE_TEST) {
        Direction_InferAndPrint();
        return;
    }
    return;
#else
    /* 原有自动模式代码（保持不变）—— 这里省略，实际应保留原代码 */
    uint8_t label_to_print = 0;
    if (dir_phase == DIR_PHASE_REST) {
        Direction_UpdateRestBaseline();
    } else if (dir_phase == DIR_PHASE_LEFT || dir_phase == DIR_PHASE_RIGHT) {
        if (dir_skip_row_count > 0) {
            Direction_UpdateTaskZero();
            dir_skip_row_count--;
        } else {
            label_to_print = (dir_phase == DIR_PHASE_LEFT) ? DIR_LABEL_LEFT : DIR_LABEL_RIGHT;
        }
    }
    if (label_to_print != 0) {
        Direction_PrintFeatureCSV(label_to_print);
    }
    dir_phase_row_count++;
    if (dir_phase == DIR_PHASE_REST) {
        if (dir_phase_row_count >= DIR_REST_ROWS) {
            dir_phase_row_count = 0;
            dir_skip_row_count = DIR_SKIP_ROWS_AFTER_SWITCH;
            Direction_ResetTaskZero();
            if (dir_next_label == DIR_LABEL_LEFT) {
                dir_phase = DIR_PHASE_LEFT;
                Serial_Printf(DIR_TEXT_PORT, "TASK=LEFT,start_light_grip_or_imagine,skip=%d,feature=nn_window\r\n", DIR_SKIP_ROWS_AFTER_SWITCH);
            } else {
                dir_phase = DIR_PHASE_RIGHT;
                Serial_Printf(DIR_TEXT_PORT, "TASK=RIGHT,start_light_grip_or_imagine,skip=%d,feature=nn_window\r\n", DIR_SKIP_ROWS_AFTER_SWITCH);
            }
        }
    } else if (dir_phase == DIR_PHASE_LEFT) {
        if (dir_phase_row_count >= DIR_COLLECT_ROWS_PER_PHASE) {
            dir_phase_row_count = 0;
            dir_phase = DIR_PHASE_REST;
            dir_next_label = DIR_LABEL_RIGHT;
            Direction_ResetRestBaseline();
            Serial_Printf(DIR_TEXT_PORT, "TASK=REST,next right\r\n");
        }
    } else if (dir_phase == DIR_PHASE_RIGHT) {
        if (dir_phase_row_count >= DIR_COLLECT_ROWS_PER_PHASE) {
            dir_phase_row_count = 0;
            dir_phase = DIR_PHASE_REST;
            dir_next_label = DIR_LABEL_LEFT;
            dir_round_count++;
            Direction_ResetRestBaseline();
            Serial_Printf(DIR_TEXT_PORT, "TASK=REST,next left,round=%d\r\n", dir_round_count);
        }
    }
#endif
}


/* 神经网络推理输出。
 * 当前特征不依赖 REST 基线，开机后每次 FFT 都可以直接推理。
 */
static void Direction_InferAndPrint(void)
{
    if (g_eeg_app_mode != EEG_APP_MODE_INFER) {
        return;   // 非推理模式不输出
    }
    float feature[DIR_FEAT_DIM_LOCAL];
    int32_t score[DIR_CLASS_NUM];
    Direction_t dir;

    Direction_BuildFeature(feature);
    dir = Direction_Model_Infer(feature, score);

    Serial_Printf(DIR_TEXT_PORT, "INTENT=%s,S_LEFT=%ld,S_RIGHT=%ld,trained=%d\r\n",
                  Direction_ToString(dir), (long)score[0], (long)score[1], Direction_Model_IsTrained());
}




/* 滚动判断：每收到一行FFT窗口特征就写入历史缓存。
 * 默认每0.5秒来一行特征，使用最近4行平均，即约2秒决策窗口；每0.5秒输出一次 RESULT。
 * 注意：这里复用原来的 Direction_BuildFeature() 和 Direction_Model_Infer()，
 * 因此不会改变前面的 ADS1299 采集、上位机显示、滤波和 FFT 结构。
 */
static void Direction_Infer1sUpdateAndPrint(void)
{
    if (g_paused) return;
    if (g_eeg_app_mode != EEG_APP_MODE_INFER) {
        return;   // 非推理模式不输出
    }

    float feature[DIR_FEAT_DIM_LOCAL];
    float avg_feature[DIR_FEAT_DIM_LOCAL];
    int32_t score[DIR_CLASS_NUM];
    Direction_t dir;
    int32_t conf;

    Direction_BuildFeature(feature);

    for (int i = 0; i < DIR_FEAT_DIM_LOCAL; i++) {
        dir_decision_hist[dir_decision_write_idx][i] = feature[i];
    }
    dir_decision_write_idx = (uint8_t)((dir_decision_write_idx + 1) % DIR_DECISION_ROWS);

    if (dir_decision_count < DIR_DECISION_ROWS) {
        dir_decision_count++;
    }

    dir_result_row_tick++;
    if (dir_decision_count < DIR_DECISION_ROWS) {
        return;   /* 缓存还没有达到设定决策窗口，先不输出 */
    }
    if (dir_result_row_tick < DIR_RESULT_EVERY_ROWS) {
        return;
    }
    dir_result_row_tick = 0;

    for (int i = 0; i < DIR_FEAT_DIM_LOCAL; i++) {
        float sum = 0.0f;
        for (int r = 0; r < DIR_DECISION_ROWS; r++) {
            sum += dir_decision_hist[r][i];
        }
        avg_feature[i] = sum / (float)DIR_DECISION_ROWS;
    }

    dir = Direction_Model_Infer(avg_feature, score);
    conf = score[0] - score[1];
    if (conf < 0) conf = -conf;

    Serial_Printf(DIR_TEXT_PORT,
                  "RESULT,window=%lu,dt_ms=%u,win_rows=%u,INTENT=%s,S_LEFT=%ld,S_RIGHT=%ld,CONF=%ld,trained=%d\r\n",
                  (unsigned long)dir_result_window,
                  (unsigned int)DIR_RESULT_DT_MS,
                  (unsigned int)DIR_DECISION_ROWS,
                  Direction_ToString(dir),
                  (long)score[0],
                  (long)score[1],
                  (long)conf,
                  Direction_Model_IsTrained());

    dir_result_window++;

}


/**
 * @brief 根据当前值和 EMA 均值判断状态。
 * @return 0 表示放松，1 表示中性，2 表示专注。
 */
static uint8_t Judge_Attn_State (float v0, float v1, float ema0, float ema1, float eps) {
    if (ema0 < eps)
        ema0 = eps;
    if (ema1 < eps)
        ema1 = eps;

    int invalid0 = (v0 < eps) ? 1 : 0;
    int invalid1 = (v1 < eps) ? 1 : 0;

    if (invalid0 && invalid1)
        return STATE_NEUTRAL;
    if (invalid0) {
        if (v1 > ema1)
            return STATE_FOCUS;
        if (v1 < ema1)
            return STATE_RELAX;
        return STATE_NEUTRAL;
    }
    if (invalid1) {
        if (v0 > ema0)
            return STATE_FOCUS;
        if (v0 < ema0)
            return STATE_RELAX;
        return STATE_NEUTRAL;
    }

    if (v0 > ema0 && v1 > ema1)
        return STATE_FOCUS;
    if (v0 < ema0 && v1 < ema1)
        return STATE_RELAX;

    int ch0_up = (v0 > ema0 * 1.05f) ? 1 : 0;
    int ch1_up = (v1 > ema1 * 1.05f) ? 1 : 0;
    int ch0_down = (v0 < ema0 * 0.95f) ? 1 : 0;
    int ch1_down = (v1 < ema1 * 0.95f) ? 1 : 0;

    if (ch0_up && !ch1_down)
        return STATE_FOCUS;
    if (ch1_up && !ch0_down)
        return STATE_FOCUS;
    if (ch0_down && !ch1_up)
        return STATE_RELAX;
    if (ch1_down && !ch0_up)
        return STATE_RELAX;

    return STATE_NEUTRAL;
}

// /**
//  * @brief       I2C OLED                           Y        
//  * @param  attn:       0          
//  * @param  mags:       0        (           )
//  * @retval   
//  * @note             SPECTRUM_Y_MAX                                 
//  *                                                  
//  */
// static void Update_Spectrum_Display (float attn, float *mags) {
//     for (int j = 0; j < 128; j++) {
//         float value = mags[j];
//         if (value > SPECTRUM_Y_MAX)
//             value = SPECTRUM_Y_MAX;                  //    
//         float norm = value / SPECTRUM_Y_MAX;         //         [0, 1]
//         uint8_t h = (uint8_t)(norm * 64.0f + 0.5f);  //     0~64     
//         if (h > 64)
//             h = 64;
//         FFT_Data.SpectrumHeights[j] = h;  //               FFT_Data
//     }
//  OLED_Clear (I2C);
//  OLED_DrawSpectrum (I2C, 0, FFT_Data.SpectrumHeights, 128, 64);
//  OLED_ShowString (I2C, 0, 8, "Focus:");
//  uint32_t ei_scaled = (uint32_t)(attn * 1000.0f + 0.5f);
//  if (ei_scaled > 9999)
//      ei_scaled = 9999;
//  uint32_t int_part = ei_scaled / 1000;
//  uint32_t frac_part = ei_scaled % 1000;
//  OLED_ShowNum (I2C, 1, 11, int_part, 1);
//  OLED_ShowChar (I2C, 1, 12, '.');
//  OLED_ShowNum (I2C, 1, 13, frac_part, 3);
//  OLED_ShowString (I2C, 2, 8, "Avg:");
//  uint32_t avg_scaled = (uint32_t)(AttnEmaCh0.Ema * 1000.0f + 0.5f);
//  if (avg_scaled > 99999)
//      avg_scaled = 99999;
//  uint32_t avg_int = avg_scaled / 1000;
//  uint32_t avg_frac = avg_scaled % 1000;
//  OLED_ShowNum (I2C, 3, 11, avg_int, 1);
//  OLED_ShowChar (I2C, 3, 12, '.');
//  OLED_ShowNum (I2C, 3, 13, avg_frac, 3);
// }

/**
 * @brief  FFT                           
 * @retval 1                0           
 */
/**
 * @brief  FFT                           
 * @retval 1                0           
 */
/**
 * @brief  FFT                           
 * @retval 1                0           
 */
static uint8_t Process_FFT_Step (void) {
    switch (fft_step) {
    case FFT_STEP_START:
        fft_step = FFT_STEP_EXTRACT_RAW_FRAME;
        break;

    case FFT_STEP_EXTRACT_RAW_FRAME:
        fft_src_type = 0;
        fft_src_start = (RingBuf.WriteIdx - FFT_SIZE) & (FFT_SIZE - 1);
        fft_step = FFT_STEP_SPECTRUM_RAW;
        break;

    case FFT_STEP_SPECTRUM_RAW:
        p_fft_data = &FFT_Data;
        fft_next_state = FFT_STEP_FREQ_FILTER;
        proc_channel = 0;
        fft_pair_index = 0;
        fft_step = FFT_STEP_FFT_COPY_INPUT;
        break;

        /* ---------- 四通道 FFT：每次复数 FFT 同时计算一对真实通道 ---------- */
    case FFT_STEP_FFT_COPY_INPUT: {
        RingBuffer_t *src_buf = (fft_src_type == 0) ? &RingBuf : &RingBufFiltered;
        uint8_t ch_a = (uint8_t)(fft_pair_index * 2U);
        uint8_t ch_b = (uint8_t)(ch_a + 1U);
        float *ch_a_src = RingBuffer_GetChannel(src_buf, ch_a);
        float *ch_b_src = RingBuffer_GetChannel(src_buf, ch_b);
        uint16_t start = fft_src_start;

        float ch_a_mean, ch_a_slope;
        float ch_b_mean, ch_b_slope;
        AB_CalcMeanAndSlope(ch_a_src, start, &ch_a_mean, &ch_a_slope);
        AB_CalcMeanAndSlope(ch_b_src, start, &ch_b_mean, &ch_b_slope);

        for (int j = 0; j < FFT_SIZE; j++) {
            uint16_t idx = (start + j) & (FFT_SIZE - 1);
            float w = Hanning_Window[j];
            float xa = AB_RemoveMeanAndLinearTrend(ch_a_src[idx], ch_a_mean, ch_a_slope, j);
            float xb = AB_RemoveMeanAndLinearTrend(ch_b_src[idx], ch_b_mean, ch_b_slope, j);
            Cplx_Input[j].r = xa * w;
            Cplx_Input[j].i = xb * w;
        }
        fft_step = FFT_STEP_FFT_TRANSFORM;
        break;
    }

    case FFT_STEP_FFT_TRANSFORM:
        FFT_Transform (Cplx_FFT_Cfg, Cplx_Input, Cplx_Output);
        fft_step = FFT_STEP_FFT_MAGS;
        break;

    case FFT_STEP_FFT_MAGS: {
        uint8_t ch_a = (uint8_t)(fft_pair_index * 2U);
        uint8_t ch_b = (uint8_t)(ch_a + 1U);
        float *ch_a_mags = FFT_Data_GetMags(p_fft_data, ch_a);
        float *ch_b_mags = FFT_Data_GetMags(p_fft_data, ch_b);

        ch_a_mags[0] = fabsf (Cplx_Output[0].r);
        ch_b_mags[0] = fabsf (Cplx_Output[0].i);
        for (int k = 1; k < 128; k++) {
            float Zkr = Cplx_Output[k].r;
            float Zki = Cplx_Output[k].i;
            int nk = FFT_SIZE - k;
            float ZNkr = Cplx_Output[nk].r;
            float ZNki = Cplx_Output[nk].i;

            float Xr = 0.5f * (Zkr + ZNkr);
            float Xi = 0.5f * (Zki - ZNki);
            float Yr = 0.5f * (Zki + ZNki);
            float Yi = 0.5f * (ZNkr - Zkr);

            ch_a_mags[k] = sqrtf (Xr * Xr + Xi * Xi);
            ch_b_mags[k] = sqrtf (Yr * Yr + Yi * Yi);
        }
        ch_a_mags[128] = fabsf (Cplx_Output[128].r);
        ch_b_mags[128] = fabsf (Cplx_Output[128].i);

        fft_pair_index++;
        if (fft_pair_index < (NUM_CHANNELS / 2U)) {
            fft_step = FFT_STEP_FFT_COPY_INPUT;
        } else {
            fft_step = fft_next_state;
        }
        break;
    }

    /* ---------- 频谱显示滤波：四路都处理，但原协议只发送 C3/C4 两路 ---------- */
    case FFT_STEP_FREQ_FILTER:
        for (uint8_t ch = 0; ch < NUM_CHANNELS; ch++) {
            Apply_Freq_Filter (FFT_Data_GetMags(&FFT_Data, ch));
        }
        proc_channel = 0;
        target_state_after_buffer = FFT_STEP_SEND_FREQ_SPECTRUM;
        raw_send_channel = 0;
        current_send_frag = 0;
        buffer_counter = 0;
        fft_step = FFT_STEP_SEND_BUFFER;
        break;

    /* ----------               +        ---------- */
    case FFT_STEP_SEND_FREQ_SPECTRUM: {
        float *mags = (raw_send_channel == 0) ? FFT_Data.CH2Mags : FFT_Data.CH3Mags;  /* 上位机 CH0/CH1 显示 C3/C4 */
        CmdType cmd = (raw_send_channel == 0) ? CMD_FREQ_SPECTRUM_CH0 : CMD_FREQ_SPECTRUM_CH1;
        Send_Spectrum (cmd, mags, current_send_frag);
        current_send_frag++;
        if (current_send_frag >= SPECTRUM_TOTAL_FRAGS) {
            current_send_frag = 0;
            if (raw_send_channel == 0) {
                raw_send_channel = 1;
            } else {
                raw_send_channel = 0;
                // fft_step = FFT_STEP_BAND_POWER;
                fft_step = FFT_STEP_EXTRACT_FILT_FRAME;
                proc_channel = 0;
            }
        }
        break;
    }

    /* ----------                ---------- */
    case FFT_STEP_EXTRACT_FILT_FRAME:
        fft_src_type = 1;
        fft_src_start = (RingBufFiltered.WriteIdx - FFT_SIZE) & (FFT_SIZE - 1);
        fft_step = FFT_STEP_SPECTRUM_FILT;
        break;

    case FFT_STEP_SPECTRUM_FILT:
        p_fft_data = &FFT_DataFiltered;
        fft_next_state = FFT_STEP_BAND_POWER;
        filt_send_channel = 0;
        current_filt_send_frag = 0;
        fft_pair_index = 0;
        fft_step = FFT_STEP_FFT_COPY_INPUT;
        break;

        /* ----------                    ---------- */
        // case FFT_STEP_BAND_POWER:
        //     if (proc_channel == 0) {
        //         theta_pow_ch0 = Compute_Band_Power (FFT_Data.CH0Mags, BandIdx.ThetaStart, BandIdx.ThetaEnd, norm_factor);
        //         alpha_pow_ch0 = Compute_Band_Power (FFT_Data.CH0Mags, BandIdx.AlphaStart, BandIdx.AlphaEnd, norm_factor);
        //         beta_pow_ch0 = Compute_Band_Power (FFT_Data.CH0Mags, BandIdx.BetaStart, BandIdx.BetaEnd, norm_factor);
        //         // Serial_Printf(SERIAL_PORT_DEBUG, "  :%d   :%d   :%d\r\n",
        //         //   (int)(theta_pow_ch0 * 1e9f),
        //         //   (int)(alpha_pow_ch0 * 1e9f),
        //         //   (int)(beta_pow_ch0 * 1e9f));
        //         proc_channel = 1;
        //     } else {
        //         theta_pow_ch1 = Compute_Band_Power (FFT_Data.CH1Mags, BandIdx.ThetaStart, BandIdx.ThetaEnd, norm_factor);
        //         alpha_pow_ch1 = Compute_Band_Power (FFT_Data.CH1Mags, BandIdx.AlphaStart, BandIdx.AlphaEnd, norm_factor);
        //         beta_pow_ch1 = Compute_Band_Power (FFT_Data.CH1Mags, BandIdx.BetaStart, BandIdx.BetaEnd, norm_factor);
        //         proc_channel = 0;
        //         fft_step = FFT_STEP_ATTENTION;
        //     }
        //     break;
    case FFT_STEP_BAND_POWER: {
        /* 计算 CP3、CP4、C3、C4 四通道 theta、alpha/mu、beta 频段功率。 */
        BandPowers_t bp0 = compute_band_powers (FFT_Data.CH0Mags, FFT_SIZE,
                                                BandIdx.ThetaStart, BandIdx.ThetaEnd,
                                                BandIdx.AlphaStart, BandIdx.AlphaEnd,
                                                BandIdx.BetaStart, BandIdx.BetaEnd,
                                                norm_factor, SAMPLE_RATE);
        BandPowers_t bp1 = compute_band_powers (FFT_Data.CH1Mags, FFT_SIZE,
                                                BandIdx.ThetaStart, BandIdx.ThetaEnd,
                                                BandIdx.AlphaStart, BandIdx.AlphaEnd,
                                                BandIdx.BetaStart, BandIdx.BetaEnd,
                                                norm_factor, SAMPLE_RATE);
        BandPowers_t bp2 = compute_band_powers (FFT_Data.CH2Mags, FFT_SIZE,
                                                BandIdx.ThetaStart, BandIdx.ThetaEnd,
                                                BandIdx.AlphaStart, BandIdx.AlphaEnd,
                                                BandIdx.BetaStart, BandIdx.BetaEnd,
                                                norm_factor, SAMPLE_RATE);
        BandPowers_t bp3 = compute_band_powers (FFT_Data.CH3Mags, FFT_SIZE,
                                                BandIdx.ThetaStart, BandIdx.ThetaEnd,
                                                BandIdx.AlphaStart, BandIdx.AlphaEnd,
                                                BandIdx.BetaStart, BandIdx.BetaEnd,
                                                norm_factor, SAMPLE_RATE);
        theta_pow_ch0 = bp0.theta_power; alpha_pow_ch0 = bp0.alpha_power; beta_pow_ch0 = bp0.beta_power;
        theta_pow_ch1 = bp1.theta_power; alpha_pow_ch1 = bp1.alpha_power; beta_pow_ch1 = bp1.beta_power;
        theta_pow_ch2 = bp2.theta_power; alpha_pow_ch2 = bp2.alpha_power; beta_pow_ch2 = bp2.beta_power;
        theta_pow_ch3 = bp3.theta_power; alpha_pow_ch3 = bp3.alpha_power; beta_pow_ch3 = bp3.beta_power;

        fft_step = FFT_STEP_ATTENTION;
        break;
    }
    /* ----------                   ---------- */
    // case FFT_STEP_ATTENTION:
    //     if (proc_channel == 0) {
    //         cur_attn0 = Compute_Attention (theta_pow_ch0, alpha_pow_ch0, beta_pow_ch0);
    //         proc_channel = 1;
    //     } else {
    //         cur_attn1 = Compute_Attention (theta_pow_ch1, alpha_pow_ch1, beta_pow_ch1);
    //         proc_channel = 0;
    //         fft_step = FFT_STEP_NORMALIZE;
    //     }
    //     break;
    /* ----------           ---------- */
    case FFT_STEP_ATTENTION:
        /* 原有注意力、放松度、眨眼特征仍然保留。
         * 新增的左右想象 ERD 特征也放在这里处理，因为此时频段功率已经计算完成。
         */
        cur_attn0  = compute_attention_feature(theta_pow_ch2, alpha_pow_ch2, beta_pow_ch2);  /* 显示/注意力沿用 C3 */
        cur_attn1  = compute_attention_feature(theta_pow_ch3, alpha_pow_ch3, beta_pow_ch3);  /* 显示/注意力沿用 C4 */
        cur_relax0 = compute_relaxation_feature(theta_pow_ch2, alpha_pow_ch2, beta_pow_ch2);
        cur_relax1 = compute_relaxation_feature(theta_pow_ch3, alpha_pow_ch3, beta_pow_ch3);
        cur_blink0 = compute_blink_feature(dummy_blink_stats.blink_rate, dummy_blink_stats.slow_blink_ratio);
        cur_blink1 = cur_blink0;

#if AB_EXTRACT_PRINT_ENABLE
        AB_PrintAlphaBetaCSV();
#endif
        /* 根据当前运行时模式执行不同处理 */
        if (g_eeg_app_mode == EEG_APP_MODE_COLLECT) {
            Direction_AutoCollectProcess();
        } else if (g_eeg_app_mode == EEG_APP_MODE_COLLECT_CSP) {
            Direction_AutoCollectCSPProcess();
        } else if (g_eeg_app_mode == EEG_APP_MODE_INFER) {
            Direction_Infer1sUpdateAndPrint();   // 推理模式使用滚动判断
        }

#if DIR_DEBUG_POWER_PRINT
        Serial_Printf(DIR_TEXT_PORT,
                      "DBGPWR_P15,cp3_mu=%ld,cp4_mu=%ld,c3_mu=%ld,c4_mu=%ld,cp3_beta=%ld,cp4_beta=%ld,c3_beta=%ld,c4_beta=%ld,rest_valid=%d\r\n",
                      (long)(alpha_pow_ch0 * 1000000000000000.0f),
                      (long)(alpha_pow_ch1 * 1000000000000000.0f),
                      (long)(alpha_pow_ch2 * 1000000000000000.0f),
                      (long)(alpha_pow_ch3 * 1000000000000000.0f),
                      (long)(beta_pow_ch0 * 1000000000000000.0f),
                      (long)(beta_pow_ch1 * 1000000000000000.0f),
                      (long)(beta_pow_ch2 * 1000000000000000.0f),
                      (long)(beta_pow_ch3 * 1000000000000000.0f),
                      dir_rest_valid);
#endif

        fft_step = FFT_STEP_NORMALIZE;
        break;
        // case FFT_STEP_NORMALIZE:
        //     norm_score0 = normalize_feature (cur_attn0, &attn_hist[0]);
        //     norm_score1 = normalize_feature (cur_attn1, &attn_hist[1]);
        //     fft_step = FFT_STEP_UPDATE_EMA;  //          
        //     break;
        /* ----------                 algo_core normalize_attention   ---------- */

    /* ----------       + EMA     ---------- */
    case FFT_STEP_NORMALIZE: {
        float a0 = normalize_attention(cur_attn0, &attn_hist[0]);
        float a1 = normalize_attention(cur_attn1, &attn_hist[1]);
        float r0 = normalize_attention(cur_relax0, &relax_hist[0]);
        float r1 = normalize_attention(cur_relax1, &relax_hist[1]);
        float b0 = 100.0f - normalize_attention(cur_blink0, &blink_hist[0]);  //      
        float b1 = 100.0f - normalize_attention(cur_blink1, &blink_hist[1]);

        const float alpha = 0.25f;
        #define EMA_UPDATE(ema, val) do { \
            if ((ema).Count == 0) (ema).Value = (val); \
            else (ema).Value = ema_smooth((val), (ema).Value, alpha); \
            (ema).Count++; } while(0)

        EMA_UPDATE(attn_ema[0], a0);  EMA_UPDATE(attn_ema[1], a1);
        EMA_UPDATE(relax_ema[0], r0); EMA_UPDATE(relax_ema[1], r1);
        EMA_UPDATE(blink_ema[0], b0); EMA_UPDATE(blink_ema[1], b1);

        fft_step = FFT_STEP_UPDATE_TREND;
        break;
    }

case FFT_STEP_UPDATE_TREND:
        if (attn_ema[0].Value > 1e-6f && attn_ema[1].Value > 1e-6f) {
            TrendWin.CH0[TrendWin.Idx] = attn_ema[0].Value;
            TrendWin.CH1[TrendWin.Idx] = attn_ema[1].Value;
            TrendWin.Idx = (TrendWin.Idx + 1) % TREND_WINDOW_SIZE;
            if (TrendWin.Count < TREND_WINDOW_SIZE) TrendWin.Count++;
        }
        fft_step = FFT_STEP_JUDGE_STATE;
        break;

    case FFT_STEP_JUDGE_STATE: {
        const float epsilon = 1e-6f;
        instant_state = Judge_Attn_State(attn_ema[0].Value, attn_ema[1].Value, attn_ema[0].Value, attn_ema[1].Value, epsilon);
        trend_state = STATE_NEUTRAL;
        if (TrendWin.Count > 0) {
            float sum0=0,sum1=0;
            for (int i=0;i<TrendWin.Count;i++) { sum0 += TrendWin.CH0[i]; sum1 += TrendWin.CH1[i]; }
            trend_state = Judge_Attn_State(sum0/TrendWin.Count, sum1/TrendWin.Count, attn_ema[0].Value, attn_ema[1].Value, epsilon);
        }
        Show_Trend_State(trend_state);
        fft_step = FFT_STEP_SEND_FOCUS;
        break;
    }

    case FFT_STEP_SEND_FILT_SPECTRUM: {
        float *mags = (filt_send_channel == 0) ? FFT_DataFiltered.CH2Mags : FFT_DataFiltered.CH3Mags;  /* 上位机 CH0/CH1 显示 C3/C4 */
        CmdType cmd = (filt_send_channel == 0) ? CMD_FILT_SPECTRUM_CH0 : CMD_FILT_SPECTRUM_CH1;
        Send_Spectrum (cmd, mags, current_filt_send_frag);
        current_filt_send_frag++;
        if (current_filt_send_frag >= SPECTRUM_TOTAL_FRAGS) {
            current_filt_send_frag = 0;
            if (filt_send_channel == 0) {
                filt_send_channel = 1;
            } else {
                filt_send_channel = 0;
                fft_step = FFT_STEP_FINISH;
            }
        }
        break;
    }

    /* ----------            ---------- */
    case FFT_STEP_SEND_BUFFER:
        buffer_counter++;
        if (buffer_counter >= 10) {
            fft_step = target_state_after_buffer;
        } else {
            fft_step = FFT_STEP_SEND_BUFFER;
        }
        break;

    /* ----------             ---------- */
    case FFT_STEP_SEND_FOCUS:
        Send_Focus (attn_ema[0].Value, attn_ema[1].Value, AttnEmaCh0.Value, AttnEmaCh1.Value, trend_state, instant_state);
        Update_OLED_Scores(attn_ema[0].Value, relax_ema[0].Value, blink_ema[0].Value, attn_ema[1].Value, relax_ema[1].Value, blink_ema[1].Value);
        fft_step = FFT_STEP_SEND_FILT_SPECTRUM;
        break;

    case FFT_STEP_FINISH:
        fft_step = FFT_STEP_START;
        return 1;

    default:
        fft_step = FFT_STEP_FINISH;
        break;
    }
    return 0;
}

/**
 * @brief                FFT                 
 */
static void Process_FFT (void) {
    fft_step = FFT_STEP_START;
}

/**
 * @brief                 SPI OLED  
 * @param  new_val:           0  
 * @retval   
 */
static void Update_Waveform (float new_val) {
    uint8_t y = Map_To_Y (new_val);
    for (int j = 0; j < 127; j++) Wave_Buf[j] = Wave_Buf[j + 1];
    Wave_Buf[127] = y;
    // OLED_DrawWaveformFast (SPI, Wave_Buf);
}

/* ----            ---- */
/**
 * @brief              
 * @param input         
 * @param filt             
 * @return          
 */
float IIR_SOS_Step (float input, IIR_SOS_t *filt) {
    float tmp = input;
#pragma GCC unroll 4
    for (uint8_t i = 0; i < filt->NumSections; i++) {
        tmp = IIR_Step (tmp, &filt->Sec[i]);
    }
    return tmp;
}

/**
 * @brief            IIR                        
 * @param  input:             
 * @param  filt:                                   
 * @return             
 * @note             
 *         1.             wn = input - a1*w1 - a2*w2
 *         2.          out = b0*wn + b1*w1 + b2*w2
 *         3.         w2 = w1  w1 = wn
 *                                        
 *                                
 */
float IIR_Step (float input, IIR_Biquad_t *filt) {
    //                       wn                       
    float wn = input - filt->a1 * filt->w1 - filt->a2 * filt->w2;
    //             wn                
    float out = filt->b0 * wn + filt->b1 * filt->w1 + filt->b2 * filt->w2;
    //                                   
    filt->w2 = filt->w1;  //        w1     w2
    filt->w1 = wn;        //           wn      w1
    return out;
}

/**
 * @brief 对频谱做显示层面的修正：削弱直流、50Hz 工频和高频部分。
 */
static void Apply_Freq_Filter (float *mags) {
    // 抑制 0~2Hz 的直流和极低频漂移
    for (int i = 0; i < 3; i++) {
        mags[i] *= DC_Window[i];
    }

    // 抑制 49~53Hz 工频附近频率
    for (int i = 49, j = 0; i <= 53; i++, j++) {
        mags[i] *= Notch_Window[j];
    }

    // 高频逐渐衰减
    for (int i = 62, j = 0; i <= 68; i++, j++) {
        mags[i] *= HF_Window[j];
    }
    // 68Hz 以上直接置零
    for (int i = 69; i < (FFT_SIZE / 2 + 1); i++) {
        mags[i] = 0.0f;
    }
}

/**
 * @brief 将波形电压值映射到 OLED 纵坐标 0~63。
 */
static inline uint8_t Map_To_Y (float value) {
    float norm = value / WAVE_FIXED_MAX;
    int16_t y = 32 - (int16_t)(norm * 31.0f);
    if (y < 0)
        y = 0;
    if (y > 63)
        y = 63;
    return (uint8_t)y;
}

/**
 * @brief 信号分析主任务。
 * @note 初始化串口、ADS1299、FFT，然后循环读取环形缓冲区中的脑电帧。
 */
void Signal_Analysis_Start (void) {
    /* ---------- 1. 初始化串口 ---------- */
    Serial_Init (SERIAL_PORT_DEBUG);
    Serial_Init (SERIAL_PORT_WIFI);

    Serial_Printf (SERIAL_PORT_DEBUG, "System Start\r\n");

    /* 初始化 ICM-42605 姿态传感器。
     * 放在 ADS1299_Init() 前面：ICM42605_BCI_Init() 内部会做约 2 秒静止零偏校准，
     * 这样不会打断 ADS1299 正式连续采集。
     * 后续 ICM42605_Task() 只在主循环中做短任务片段，不阻塞脑电 DMA/DRDY。
     */
    ICM42605_Status icm_status = ICM42605_BCI_Init();
    Timer_1ms_Init();
    Serial_Printf(SERIAL_PORT_DEBUG, "ICM42605_INIT=%d\r\n", (int)icm_status);

    /* 初始化 ADS1299。ADS1299_Init() 内部会配置 GPIO、SPI、DMA、寄存器并启动连续采样。 */
    Serial_Printf(SERIAL_PORT_DEBUG, "ADS1299_Init start\r\n");
    uint8_t id = ADS1299_Init();
    Serial_Printf(SERIAL_PORT_DEBUG, "ADS1299_Init done, id=%d\r\n", id);

    // Serial_Printf (SERIAL_PORT_DEBUG, "ADS1299 id = %d\r\n", id);
    // OLED_ShowHexNum (SPI, 0, 6, id, 6);

    /* ---------- 2. 初始化 256 点复数 FFT ---------- */
    /* 使用复数 FFT 两次完成四通道计算：CP3/CP4 一次，C3/C4 一次。 */
    Cplx_FFT_Cfg = FFT_Alloc (FFT_SIZE, FFT_FORWARD, NULL, NULL);
    if (Cplx_FFT_Cfg == NULL)
        while (1);

    /* ---------- 3. 计算 theta、mu、beta 频段对应的 FFT 下标 ---------- */
    // ADS1299 当前采样率为 250 SPS，所以 SAMPLE_RATE 设置为 250.0f。
    const float bin_width = SAMPLE_RATE / FFT_SIZE;
    Compute_Band_Indices (4.0f, 8.0f, bin_width, &BandIdx.ThetaStart, &BandIdx.ThetaEnd);
    Compute_Band_Indices (8.0f, 13.0f, bin_width, &BandIdx.AlphaStart, &BandIdx.AlphaEnd);
    Compute_Band_Indices (13.0f, 30.0f, bin_width, &BandIdx.BetaStart, &BandIdx.BetaEnd);

#if AB_EXTRACT_PRINT_ENABLE
    Serial_Printf(DIR_TEXT_PORT, "ABCFG,fs=%d,fft=%d,step=%d,alpha=8-13Hz,beta=13-30Hz,drift_k=9960/10000,notch=%d,detrend=%d,power_scale=1e15,pct_scale=10000,db_scale=100\r\n",
                  (int)SAMPLE_RATE, FFT_SIZE, STEP_SIZE, AB_REALTIME_NOTCH_ENABLE, AB_FFT_DETREND_ENABLE);
    Serial_Printf(DIR_TEXT_PORT, "ABCSV,ch0_alpha_p15,ch1_alpha_p15,ch0_beta_p15,ch1_beta_p15,ch0_alpha_pct,ch1_alpha_pct,ch0_beta_pct,ch1_beta_pct,ch0_alpha_db100,ch1_alpha_db100,ch0_beta_db100,ch1_beta_db100,alpha_lr,beta_lr,ch0_beta_alpha,ch1_beta_alpha,total_lr\r\n");
#endif

    /* 根据运行时模式打印启动信息 */
    if (g_eeg_app_mode == EEG_APP_MODE_COLLECT) {
        Serial_Printf(DIR_TEXT_PORT, "EEG_MODE,COLLECT,rest_s=5,left_right_alternate=1,debug_output=TASK_DIRCSV\r\n");
        Direction_PrintFeatureHeader();
    } else if (g_eeg_app_mode == EEG_APP_MODE_COLLECT_CSP) {
        Serial_Printf(DIR_TEXT_PORT, "EEG_MODE,CSP,collect_window=256,step=125\r\n");
    } else if (g_eeg_app_mode == EEG_APP_MODE_INFER) {
        Serial_Printf(DIR_TEXT_PORT, "EEG_MODE,INFER,result_period_s=2\r\n");
    }

#if EEG_MODE_IS_INFER
    Serial_Printf(DIR_TEXT_PORT, "EEG_MODE,INFER,result_period_s=2,debug_output=CURRENT_LEFT_RIGHT_PROB\r\n");
#endif

#if DIR_INFER_1S_MODE
    Direction_Infer1sReset();
    Serial_Printf(DIR_TEXT_PORT,
                  "ROLLING_READY,CH0=CP3,CH1=CP4,CH2=C3,CH3=C4,step_ms=%u,win_rows=%u,result_every_rows=%u,model=%s\r\n",
                  (unsigned int)DIR_RESULT_DT_MS,
                  (unsigned int)DIR_DECISION_ROWS,
                  (unsigned int)DIR_RESULT_EVERY_ROWS,
                  Direction_Model_Name());
#endif

    /* ---------- 4. 主循环变量：每帧 ADS1299 数据为 27 字节 ---------- */
    uint8_t frame_buf[ADS1299_FRAME_BYTE_NUM];
    int32_t ch_data[ADS1299_CHANNEL_NUM];
    static uint8_t fft_active = 0;  // 标记当前是否正在执行 FFT 分步处理。

#if DUALCORE_IPC_FRAME_SHARE_ENABLE
    static uint32_t ipc_frame_count = 0;
    static uint32_t ipc_last_print_ack = 0;
#endif

    // 计算窗函数归一化因子，后面频段功率计算会用到。
    for (int i = 0; i < FFT_SIZE; i++) {
        sum_win_sq += Hanning_Window[i] * Hanning_Window[i];
    }
    norm_factor = SAMPLE_RATE * sum_win_sq;

    memset(attn_hist, 0, sizeof(attn_hist));
    memset(relax_hist, 0, sizeof(relax_hist));
    memset(blink_hist, 0, sizeof(blink_hist));
    memset(attn_ema, 0, sizeof(attn_ema));
    memset(relax_ema, 0, sizeof(relax_ema));
    memset(blink_ema, 0, sizeof(blink_ema));
    while (1) {
#if CMD_MODE_ENABLE
        if (Serial_IsDataReady(SERIAL_PORT_DEBUG)) {
            uint8_t *cmd_buf;
            uint16_t len = Serial_GetDataPacket(SERIAL_PORT_DEBUG, &cmd_buf);
            if (len > 0 && len < 64) {
                char cmd_str[64];
                memcpy(cmd_str, cmd_buf, len);
                cmd_str[len] = '\0';
                Parse_Command(cmd_str);
            }
        }
#endif

    // Serial_Printf (SERIAL_PORT_WIFI, "hello\r\n");  // 调试用，正常采集时关闭。
    if (ring_buffer_get_frame (frame_buf)) {

#if DUALCORE_IPC_FRAME_SHARE_ENABLE
        ipc_frame_count++;
        if (ipc_frame_count >= DUALCORE_IPC_FRAME_NOTIFY_EVERY_FRAMES) {
            ipc_frame_count = 0;
            DualCore_IPC_SendFrameFromV3F(frame_buf, ADS1299_FRAME_BYTE_NUM);

            uint32_t ipc_ack = DualCore_IPC_GetAckCount();      /* ACKed frame sequence */
            uint32_t ipc_tx  = DualCore_IPC_GetNotifyCount();   /* V3F latest sent sequence */
            uint32_t tx_cs   = DualCore_IPC_GetAckTxChecksum(); /* TX checksum of ACKed sequence */
            uint32_t v5_cs   = DualCore_IPC_GetAckChecksum();
            uint32_t cs_ok_cnt    = DualCore_IPC_GetChecksumOKCount();
            uint32_t cs_bad_cnt   = DualCore_IPC_GetChecksumBadCount();
            uint32_t parse_ok_cnt = DualCore_IPC_GetParseOKCount();
            uint32_t parse_bad_cnt= DualCore_IPC_GetParseBadCount();
            int32_t  v3f_ch0      = DualCore_IPC_GetLastV3FCh0();
            int32_t  v5f_ch0      = DualCore_IPC_GetLastV5FCh0();
            int32_t  v5f_ch0_uvx1000 = DualCore_IPC_GetLastV5FCh0uVX1000();
            int32_t  v5f_pre0_uvx1000 = DualCore_IPC_GetLastV5FPre0uVX1000();
            int32_t  v5f_filt0_uvx1000 = DualCore_IPC_GetLastV5FFilt0uVX1000();
            uint32_t v5f_samples = DualCore_IPC_GetLastV5FSampleCount();
            uint32_t v5f_windows = DualCore_IPC_GetLastV5FWindowCount();
            uint32_t v5f_fft = DualCore_IPC_GetLastV5FFFTCount();
            uint32_t v5f_feat_valid = DualCore_IPC_GetLastV5FFeatureValid();
            uint32_t v5f_infer_valid = DualCore_IPC_GetLastV5FInferValid();
            uint32_t v5f_pred = DualCore_IPC_GetLastV5FPred();
            uint32_t v5f_trained = DualCore_IPC_GetLastV5FModelTrained();
            int32_t  v5f_score_l = DualCore_IPC_GetLastV5FScoreLeft();
            int32_t  v5f_score_r = DualCore_IPC_GetLastV5FScoreRight();
            int32_t  v5f_conf = DualCore_IPC_GetLastV5FConfidence();
            uint32_t v5f_infer_cnt = DualCore_IPC_GetLastV5FInferCount();
            const char *v5f_pred_str = (v5f_pred == 0u) ? "LEFT" : ((v5f_pred == 1u) ? "RIGHT" : "UNKNOWN");
            if ((ipc_ack - ipc_last_print_ack) >= DUALCORE_IPC_FRAME_PRINT_EVERY_ACKS) {
                char v5f_line[256];
                int v5f_len;

                ipc_last_print_ack = ipc_ack;

#if EEG_MODE_IS_INFER
                (void)ipc_tx;
                (void)tx_cs;
                (void)v5_cs;
                (void)cs_ok_cnt;
                (void)cs_bad_cnt;
                (void)parse_ok_cnt;
                (void)parse_bad_cnt;
                (void)v3f_ch0;
                (void)v5f_ch0;
                (void)v5f_ch0_uvx1000;
                (void)v5f_pre0_uvx1000;
                (void)v5f_filt0_uvx1000;
                (void)v5f_samples;
                (void)v5f_windows;
                (void)v5f_feat_valid;
                (void)v5f_conf;
                (void)v5f_trained;
                (void)v5f_infer_cnt;

                /* 推理模式：只输出每 2 秒一次的最终判断结果和左右手概率。
                 * 分数范围为 0~10000，这里转换成百分比显示，避免输出 IPC/FFT/调试细节。
                 */
                if (v5f_infer_valid) {
                    int32_t left_score = v5f_score_l;
                    int32_t right_score = v5f_score_r;

                    if (left_score < 0) left_score = 0;
                    if (left_score > 10000) left_score = 10000;
                    if (right_score < 0) right_score = 0;
                    if (right_score > 10000) right_score = 10000;

                    v5f_len = snprintf(v5f_line, sizeof(v5f_line),
                                       "--------------------------------\r\n"
                                       "Current : %s\r\n"
                                       "Left    : %ld.%02ld %%\r\n"
                                       "Right   : %ld.%02ld %%\r\n"
                                       "--------------------------------\r\n",
                                       v5f_pred_str,
                                       (long)(left_score / 100),
                                       (long)(left_score % 100),
                                       (long)(right_score / 100),
                                       (long)(right_score % 100));
                    if (v5f_len > 0) {
                        Serial_SendString(DIR_TEXT_PORT, v5f_line);
                    }
                }
#else
                /* 采集模式：不打印 V5F 推理/调试信息，只保留 TASK 与 DIRCSV。 */
                (void)v5f_line;
                (void)v5f_len;
                (void)ipc_tx;
                (void)tx_cs;
                (void)v5_cs;
                (void)cs_ok_cnt;
                (void)cs_bad_cnt;
                (void)parse_ok_cnt;
                (void)parse_bad_cnt;
                (void)v3f_ch0;
                (void)v5f_ch0;
                (void)v5f_ch0_uvx1000;
                (void)v5f_pre0_uvx1000;
                (void)v5f_filt0_uvx1000;
                (void)v5f_samples;
                (void)v5f_windows;
                (void)v5f_fft;
                (void)v5f_feat_valid;
                (void)v5f_infer_valid;
                (void)v5f_pred;
                (void)v5f_trained;
                (void)v5f_score_l;
                (void)v5f_score_r;
                (void)v5f_conf;
                (void)v5f_infer_cnt;
                (void)v5f_pred_str;
#endif
            }
        }
#endif

        /* 解析 ADS1299 完整 8 通道帧，本工程只取前 4 路：CH0=CP3, CH1=CP4, CH2=C3, CH3=C4。 */
        ADS1299_ParseRawFrame (frame_buf, NULL, ch_data);

        float val_ch0 = ADS1299_CodeToVolt (ch_data[ADS1299_EEG_CH_CP3], 4.5f, 24.0f);  /* CP3 */
        float val_ch1 = ADS1299_CodeToVolt (ch_data[ADS1299_EEG_CH_CP4], 4.5f, 24.0f);  /* CP4 */
        float val_ch2 = ADS1299_CodeToVolt (ch_data[ADS1299_EEG_CH_C3],  4.5f, 24.0f);  /* C3  */
        float val_ch3 = ADS1299_CodeToVolt (ch_data[ADS1299_EEG_CH_C4],  4.5f, 24.0f);  /* C4  */
        float show_ch0 = 0.0f;
        float show_ch1 = 0.0f;

        /* Training/inference output uses DIRCSV/INTENT.
         * 不逐点打印原始电压，避免串口被无效文本占满。 */

        /* 为了保留原上位机二通道显示协议，这里仍发送两路 RAW，但内容改为 C3/C4。 */
        Waveform_RemoveDisplayBaseline(val_ch2, val_ch3, &show_ch0, &show_ch1);
        #if !DIR_1S_TEST_TEXT_ONLY
        Send_Waveform (CMD_RAW_WAVE, show_ch0, show_ch1);
        #endif

        /* ========== alpha/beta 训练前端预处理位置 1：四通道实时去电极慢漂移 ========== */
        float drift_ch0 = AB_RemoveRealtimeDrift(val_ch0, &AB_Drift_CH0);
        float drift_ch1 = AB_RemoveRealtimeDrift(val_ch1, &AB_Drift_CH1);
        float drift_ch2 = AB_RemoveRealtimeDrift(val_ch2, &AB_Drift_CH2);
        float drift_ch3 = AB_RemoveRealtimeDrift(val_ch3, &AB_Drift_CH3);

        /* ========== alpha/beta 训练前端预处理位置 2：四通道 50Hz 陷波 ========== */
#if AB_REALTIME_NOTCH_ENABLE
        float ab_pre_ch0 = IIR_SOS_Step(drift_ch0, &Notch_CH0);
        float ab_pre_ch1 = IIR_SOS_Step(drift_ch1, &Notch_CH1);
        float ab_pre_ch2 = IIR_SOS_Step(drift_ch2, &Notch_CH2);
        float ab_pre_ch3 = IIR_SOS_Step(drift_ch3, &Notch_CH3);
#else
        float ab_pre_ch0 = drift_ch0;
        float ab_pre_ch1 = drift_ch1;
        float ab_pre_ch2 = drift_ch2;
        float ab_pre_ch3 = drift_ch3;
#endif

        /* 滤波波形仅用于上位机观察，四通道均写入 RingBufFiltered，显示仍取 C3/C4。 */
        float filtered_ch0 = IIR_SOS_Step (ab_pre_ch0, &Bandpass_CH0);
        float filtered_ch1 = IIR_SOS_Step (ab_pre_ch1, &Bandpass_CH1);
        float filtered_ch2 = IIR_SOS_Step (ab_pre_ch2, &Bandpass_CH2);
        float filtered_ch3 = IIR_SOS_Step (ab_pre_ch3, &Bandpass_CH3);

        RingBufFiltered.CH0[RingBufFiltered.WriteIdx] = filtered_ch0;
        RingBufFiltered.CH1[RingBufFiltered.WriteIdx] = filtered_ch1;
        RingBufFiltered.CH2[RingBufFiltered.WriteIdx] = filtered_ch2;
        RingBufFiltered.CH3[RingBufFiltered.WriteIdx] = filtered_ch3;
        RingBufFiltered.WriteIdx = (RingBufFiltered.WriteIdx + 1) % FFT_SIZE;
        if (csp_filtered_sample_count < 0xFFFFFFFFUL) {
            csp_filtered_sample_count++;
        }

        #if !DIR_1S_TEST_TEXT_ONLY
        Send_Waveform (CMD_FILT_WAVE, filtered_ch2, filtered_ch3);
        #endif

        /* ========== alpha/beta 训练前端预处理位置 3：四通道写入 FFT 环形缓冲区 ==========
         * RingBuf 保存：原始电压 -> 实时去漂移 -> 50Hz 陷波 后的数据。
         * 后续 Process_FFT_Step() 会从 RingBuf 中取 256 点，再做去均值、去线性趋势、Hann 窗和 FFT。
         */
        RingBuf.CH0[RingBuf.WriteIdx] = ab_pre_ch0;
        RingBuf.CH1[RingBuf.WriteIdx] = ab_pre_ch1;
        RingBuf.CH2[RingBuf.WriteIdx] = ab_pre_ch2;
        RingBuf.CH3[RingBuf.WriteIdx] = ab_pre_ch3;
        RingBuf.WriteIdx = (RingBuf.WriteIdx + 1) % FFT_SIZE;

        //      OLED     
        Update_Waveform (show_ch0);

        //                 STEP_SIZE          FFT     
        New_Samples_Count++;
        if (New_Samples_Count >= STEP_SIZE && !fft_active) {
            Process_FFT();  //      FFT     
            fft_active = 1;
        }

        //      FFT                       
        if (fft_active) {
            if (Process_FFT_Step()) {   //      1           
                fft_active = 0;
                New_Samples_Count = 0;  //          
            }
        }
    }

    /* ICM-42605 姿态任务：
     * 主循环低优先级执行，10ms 触发一次 SPI4 DMA 读数，200ms 刷新 SPI OLED。
     * 不放入 ADS1299 中断/DMA 中断，不影响脑电实时采集。
     */
    ICM42605_Task();
    }
}