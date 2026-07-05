#include "dualcore_ipc.h"
#include <math.h>
#include <string.h>

#if defined(Core_V5F)
#include "DualCore_Direction_Model_4ch_Weights.h"
#include "DualCore_CSP_Model_4ch_Weights.h"
#endif

#if defined(Core_V3F)
#include "OLED.h"
#include "signal_analysis.h"
#include "ADS1299.h"
#include "Serial.h"

#include "ICM42605.h"
#endif

/* ================= Dual-core IPC frame-share + V5F FFT feature + inference verification =================
 * v2.4 purpose:
 *   - Keep the complete EEG algorithm chain on V3F for safety and comparison.
 *   - V3F writes every ADS1299 raw frame into a 64-slot shared ring and uses IPC as a doorbell.
 *   - V5F drains all pending ring frames on each IPC event, then parses all four active channels,
 *     converts code to voltage, performs the same
 *     front-end preprocessing blocks used by V3F: drift removal -> 50 Hz notch -> bandpass.
 *   - V5F accumulates 256-point windows with 125-sample hop and extracts the same
 *     24-dimension theta/mu/beta ratio feature family used by the direction classifier exposes all feature_q values for V5F_DIRCSV diagnostics, and runs the embedded 4-channel LEFT/RIGHT classifier on V5F.
 *   - V3F only prints slow diagnostic information. V5F does not touch USART/OLED/SPI/DMA.
 */

#define DUALCORE_IPC_RUNTIME_ENABLE     1
#define DUALCORE_IPC_FRAME_LEN          27u
#define DUALCORE_IPC_FRAME_SLOT_NUM     64u
#define DUALCORE_IPC_TX_HISTORY_SIZE    256u

#define DUALCORE_ADS1299_STATUS_BYTES   3u
#define DUALCORE_ADS1299_CHANNEL_NUM    8u
#define DUALCORE_ADS1299_ACTIVE_CH_NUM  4u
#define DUALCORE_ADS1299_CH_BYTES       3u
#define DUALCORE_ADS1299_FULL_SCALE     8388608LL
#define DUALCORE_ADS1299_VREF_UV        4500000LL
#define DUALCORE_ADS1299_GAIN           24LL

#define DUALCORE_V5F_WINDOW_SIZE        125u
#define DUALCORE_V5F_FFT_SIZE           256u
#define DUALCORE_V5F_STEP_SIZE          125u
#define DUALCORE_V5F_FS                 250.0f
#define DUALCORE_V5F_MAX_BIN            31u
#define DUALCORE_V5F_FEATURE_DIM        24u
#define DUALCORE_V5F_CLASS_NUM          2u
#define DUALCORE_V5F_PRED_LEFT          0u
#define DUALCORE_V5F_PRED_RIGHT         1u
#define DUALCORE_V5F_PRED_UNKNOWN       2u
#define DUALCORE_V5F_CLASSIFIER_GAIN    1.0f
#define DUALCORE_V5F_CSP_ENABLE          1u
#define DUALCORE_V5F_USE_CSP_WHEN_TRAINED 1u
#define DUALCORE_V5F_UNKNOWN_CONF        1200  /* 低于 12% 差值输出 UNKNOWN */
/* v2.9 keeps the original 24-dimensional FFT feature classifier, then adds a
 * decision-layer optimizer for online use: 4-window probability voting + EMA
 * smoothing + low-confidence anti-jitter. This does not delete or replace the
 * previous Signal_Analysis / Attention / FFT algorithms; it only stabilizes the
 * V5F inference result after the existing feature vector has been produced.
 */
#define DUALCORE_V5F_RESULT_FUSION_ENABLE 1u
#define DUALCORE_V5F_FUSION_ROWS          4u
#define DUALCORE_V5F_FUSION_EMA_ALPHA     0.60f
#define DUALCORE_V5F_KEEP_LAST_CONF       650
#define DUALCORE_V5F_FEATURE_SCALE      10000.0f
#define DUALCORE_V5F_LOG_CLAMP          6.0f
#define DUALCORE_V5F_POWER_EPS          1.0e-18f
#define DUALCORE_V5F_PI                 3.14159265358979323846f
#define DUALCORE_AB_DRIFT_K             0.996f
#define DUALCORE_FENCE()                __asm volatile ("fence iorw, iorw" ::: "memory")

typedef struct
{
    float b0, b1, b2, a1, a2, w1, w2;
} DualCore_IIR_Biquad_t;

typedef struct
{
    DualCore_IIR_Biquad_t Sec[4];
    uint8_t NumSections;
} DualCore_IIR_SOS_t;

typedef struct
{
    float drift_base;
    uint8_t init;
} DualCore_DriftRemove_t;

typedef struct
{
    volatile uint32_t seq;
    volatile uint16_t len;
    volatile uint16_t checksum;
    volatile uint8_t  frame[DUALCORE_IPC_FRAME_LEN];

    volatile uint8_t  v5f_parse_valid;
    volatile uint32_t v5f_status;
    volatile uint32_t v5f_sample_count;
    volatile uint32_t v5f_window_count;
    volatile int32_t  v5f_ch_code[DUALCORE_ADS1299_ACTIVE_CH_NUM];
    volatile int32_t  v5f_uv_x1000[DUALCORE_ADS1299_ACTIVE_CH_NUM];
    volatile int32_t  v5f_pre_x1000[DUALCORE_ADS1299_ACTIVE_CH_NUM];
    volatile int32_t  v5f_filt_x1000[DUALCORE_ADS1299_ACTIVE_CH_NUM];
    volatile uint8_t  v5f_feature_valid;
    volatile uint32_t v5f_fft_count;
    volatile int32_t  v5f_feature_q[DUALCORE_V5F_FEATURE_DIM];
    volatile uint8_t  v5f_infer_valid;
    volatile uint8_t  v5f_pred;
    volatile uint8_t  v5f_model_trained;
    volatile uint8_t  v5f_reserved;
    volatile int32_t  v5f_score_left;
    volatile int32_t  v5f_score_right;
    volatile int32_t  v5f_confidence;
    volatile uint32_t v5f_infer_count;
} DualCore_IPC_FrameSlot_t;

static volatile uint32_t g_ipc_v3f_notify_count       = 0;
static volatile uint32_t g_ipc_v3f_ack_count          = 0;
static volatile uint32_t g_ipc_v3f_tx_checksum        = 0;
static volatile uint32_t g_ipc_v3f_ack_checksum       = 0;
static volatile uint32_t g_ipc_v3f_ack_tx_checksum    = 0;
static volatile uint32_t g_ipc_v3f_checksum_ok        = 0;
static volatile uint32_t g_ipc_v3f_checksum_bad       = 0;
static volatile uint32_t g_ipc_v3f_parse_ok           = 0;
static volatile uint32_t g_ipc_v3f_parse_bad          = 0;
static volatile int32_t  g_ipc_v3f_last_v3f_ch0       = 0;
static volatile int32_t  g_ipc_v3f_last_v5f_ch0       = 0;
static volatile int32_t  g_ipc_v3f_last_v5f_uv0_x1000 = 0;
static volatile int32_t  g_ipc_v3f_last_v5f_pre0_x1000= 0;
static volatile int32_t  g_ipc_v3f_last_v5f_filt0_x1000=0;
static volatile uint32_t g_ipc_v3f_last_v5f_sample_count=0;
static volatile uint32_t g_ipc_v3f_last_v5f_window_count=0;
static volatile uint32_t g_ipc_v3f_last_v5f_fft_count=0;
static volatile uint32_t g_ipc_v3f_last_v5f_feature_valid=0;
static volatile int32_t  g_ipc_v3f_last_v5f_feature_q[DUALCORE_V5F_FEATURE_DIM];
static volatile uint32_t g_ipc_v3f_last_v5f_infer_valid=0;
static volatile uint32_t g_ipc_v3f_last_v5f_pred=DUALCORE_V5F_PRED_UNKNOWN;
static volatile uint32_t g_ipc_v3f_last_v5f_model_trained=0;
static volatile int32_t  g_ipc_v3f_last_v5f_score_left=0;
static volatile int32_t  g_ipc_v3f_last_v5f_score_right=0;
static volatile int32_t  g_ipc_v3f_last_v5f_confidence=0;
static volatile uint32_t g_ipc_v3f_last_v5f_infer_count=0;
static volatile uint8_t  g_ipc_v3f_ready              = 0;

#if defined(Core_V3F)
static volatile DualCore_IPC_FrameSlot_t g_ipc_v3f_frame_slot[DUALCORE_IPC_FRAME_SLOT_NUM];
static volatile uint16_t g_ipc_v3f_tx_checksum_hist[DUALCORE_IPC_TX_HISTORY_SIZE];
static volatile uint32_t g_ipc_v3f_tx_status_hist[DUALCORE_IPC_TX_HISTORY_SIZE];
static volatile int32_t  g_ipc_v3f_tx_ch_hist[DUALCORE_IPC_TX_HISTORY_SIZE][DUALCORE_ADS1299_ACTIVE_CH_NUM];
#endif

#if defined(Core_V5F)
static volatile uint32_t g_ipc_v5f_recv_count      = 0;
static volatile uint32_t g_ipc_v5f_last_seq        = 0;
static volatile uint32_t g_ipc_v5f_last_checksum   = 0;
static volatile uint32_t g_ipc_v5f_window_count    = 0;
static volatile uint16_t g_ipc_v5f_window_samples  = 0;
static volatile uint8_t  g_ipc_v5f_ready           = 0;
static volatile uint8_t  g_ipc_v5f_pending_flag    = 0;
static volatile uint32_t g_ipc_v5f_pending_share_addr = 0;
static volatile uint8_t  g_ipc_v5f_last_frame[DUALCORE_IPC_FRAME_LEN];
static volatile uint32_t g_ipc_v5f_fft_count = 0;
static volatile uint8_t  g_ipc_v5f_feature_valid = 0;
static volatile int32_t  g_ipc_v5f_feature_q[DUALCORE_V5F_FEATURE_DIM];
static volatile uint8_t  g_ipc_v5f_infer_valid = 0;
static volatile uint8_t  g_ipc_v5f_pred = DUALCORE_V5F_PRED_UNKNOWN;
static volatile uint8_t  g_ipc_v5f_model_trained = 0;
static volatile int32_t  g_ipc_v5f_score_left = 0;
static volatile int32_t  g_ipc_v5f_score_right = 0;
static volatile int32_t  g_ipc_v5f_confidence = 0;
static volatile uint32_t g_ipc_v5f_infer_count = 0;

#if DUALCORE_V5F_RESULT_FUSION_ENABLE
static int32_t  g_v5f_fusion_right_hist[DUALCORE_V5F_FUSION_ROWS];
static uint8_t  g_v5f_fusion_write_idx = 0;
static uint8_t  g_v5f_fusion_count = 0;
static uint8_t  g_v5f_fusion_ema_ready = 0;
static float    g_v5f_fusion_ema_right = 0.5f;
static uint8_t  g_v5f_fusion_last_pred = DUALCORE_V5F_PRED_UNKNOWN;
#endif

static float g_v5f_ring[DUALCORE_ADS1299_ACTIVE_CH_NUM][DUALCORE_V5F_FFT_SIZE];
static float g_v5f_hann[DUALCORE_V5F_FFT_SIZE];
static float g_v5f_goertzel_coeff[DUALCORE_V5F_MAX_BIN + 1u];
static float g_v5f_norm_factor = 1.0f;
static uint16_t g_v5f_ring_write_idx = 0;
static uint16_t g_v5f_ring_valid_count = 0;
static uint16_t g_v5f_step_count = 0;
static uint8_t  g_v5f_fft_cfg_ready = 0;

static DualCore_DriftRemove_t g_v5f_drift[DUALCORE_ADS1299_ACTIVE_CH_NUM];
static DualCore_IIR_SOS_t     g_v5f_notch[DUALCORE_ADS1299_ACTIVE_CH_NUM];
static DualCore_IIR_SOS_t     g_v5f_bandpass[DUALCORE_ADS1299_ACTIVE_CH_NUM];
#endif

void IPC_CH0_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

static uint16_t DualCore_IPC_Checksum16(const volatile uint8_t *buf, uint16_t len)
{
    uint32_t sum = 0;
    uint16_t i;

    for (i = 0; i < len; i++) {
        sum += buf[i];
    }

    return (uint16_t)(sum & 0xFFFFu);
}

static int32_t DualCore_ADS1299_SignExtend24(uint32_t raw24)
{
    raw24 &= 0x00FFFFFFUL;
    if ((raw24 & 0x00800000UL) != 0U) {
        raw24 |= 0xFF000000UL;
    }
    return (int32_t)raw24;
}

static void DualCore_ADS1299_ParseRawFrame(const volatile uint8_t *frame,
                                           uint32_t *status,
                                           int32_t ch_data[DUALCORE_ADS1299_CHANNEL_NUM])
{
    uint8_t i;
    uint8_t index;
    uint32_t raw24;

    if ((frame == 0) || (ch_data == 0)) {
        return;
    }

    if (status != 0) {
        *status = ((uint32_t)frame[0] << 16) |
                  ((uint32_t)frame[1] << 8)  |
                  ((uint32_t)frame[2]);
    }

    for (i = 0; i < DUALCORE_ADS1299_CHANNEL_NUM; i++) {
        index = (uint8_t)(DUALCORE_ADS1299_STATUS_BYTES + i * DUALCORE_ADS1299_CH_BYTES);
        raw24 = ((uint32_t)frame[index] << 16) |
                ((uint32_t)frame[index + 1] << 8) |
                ((uint32_t)frame[index + 2]);
        ch_data[i] = DualCore_ADS1299_SignExtend24(raw24);
    }
}

static int32_t DualCore_ADS1299_CodeToMicroVoltX1000(int32_t code)
{
    int64_t numerator;
    int64_t denominator;

    numerator = (int64_t)code * DUALCORE_ADS1299_VREF_UV * 1000LL;
    denominator = DUALCORE_ADS1299_GAIN * DUALCORE_ADS1299_FULL_SCALE;

    if (numerator >= 0) {
        numerator += denominator / 2;
    } else {
        numerator -= denominator / 2;
    }

    return (int32_t)(numerator / denominator);
}

#if defined(Core_V5F)
static int32_t DualCore_FloatVoltToMicroVoltX1000(float volt)
{
    float v = volt * 1000000000.0f;
    if (v >= 0.0f) {
        v += 0.5f;
    } else {
        v -= 0.5f;
    }
    return (int32_t)v;
}

static float DualCore_IIR_Step(float input, DualCore_IIR_Biquad_t *filt)
{
    float wn = input - filt->a1 * filt->w1 - filt->a2 * filt->w2;
    float out = filt->b0 * wn + filt->b1 * filt->w1 + filt->b2 * filt->w2;

    filt->w2 = filt->w1;
    filt->w1 = wn;

    return out;
}

static float DualCore_IIR_SOS_Step(float input, DualCore_IIR_SOS_t *filt)
{
    float tmp = input;
    uint8_t i;

    for (i = 0; i < filt->NumSections; i++) {
        tmp = DualCore_IIR_Step(tmp, &filt->Sec[i]);
    }

    return tmp;
}

static float DualCore_RemoveRealtimeDrift(float x, DualCore_DriftRemove_t *st)
{
    float y;

    if (!st->init) {
        st->drift_base = x;
        st->init = 1;
    }

    y = x - st->drift_base;
    st->drift_base = DUALCORE_AB_DRIFT_K * st->drift_base + (1.0f - DUALCORE_AB_DRIFT_K) * x;

    return y;
}

static void DualCore_InitOneNotch(DualCore_IIR_SOS_t *f)
{
    f->NumSections = 2;
    f->Sec[0].b0 = 0.9794827610f; f->Sec[0].b1 = -0.6053536377f; f->Sec[0].b2 = 0.9794827610f;
    f->Sec[0].a1 = -0.6053536377f; f->Sec[0].a2 = 0.9589655220f;  f->Sec[0].w1 = 0.0f; f->Sec[0].w2 = 0.0f;
    f->Sec[1].b0 = 0.9794827610f; f->Sec[1].b1 = -0.6053536377f; f->Sec[1].b2 = 0.9794827610f;
    f->Sec[1].a1 = -0.6053536377f; f->Sec[1].a2 = 0.9589655220f;  f->Sec[1].w1 = 0.0f; f->Sec[1].w2 = 0.0f;
}

static void DualCore_InitOneBandpass(DualCore_IIR_SOS_t *f)
{
    f->NumSections = 2;
    f->Sec[0].b0 = 0.93637848f; f->Sec[0].b1 = -1.87275696f; f->Sec[0].b2 = 0.93637848f;
    f->Sec[0].a1 = -1.95979169f; f->Sec[0].a2 = 0.96227012f;  f->Sec[0].w1 = 0.0f; f->Sec[0].w2 = 0.0f;
    f->Sec[1].b0 = 1.0f;        f->Sec[1].b1 = -2.0f;        f->Sec[1].b2 = 1.0f;
    f->Sec[1].a1 = -1.90886498f; f->Sec[1].a2 = 0.91127901f; f->Sec[1].w1 = 0.0f; f->Sec[1].w2 = 0.0f;
}

static float DualCore_V5F_SafePower(float p)
{
    return (p < DUALCORE_V5F_POWER_EPS) ? DUALCORE_V5F_POWER_EPS : p;
}

static float DualCore_V5F_ClampFloat(float x, float lo, float hi)
{
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

static float DualCore_V5F_LogRatio(float a, float b)
{
    float v = logf(DualCore_V5F_SafePower(a) / DualCore_V5F_SafePower(b));
    return DualCore_V5F_ClampFloat(v, -DUALCORE_V5F_LOG_CLAMP, DUALCORE_V5F_LOG_CLAMP);
}

static int32_t DualCore_V5F_FeatureToQ(float x)
{
    if (x >= 0.0f) return (int32_t)(x + 0.5f);
    return (int32_t)(x - 0.5f);
}

static float DualCore_V5F_Sigmoid(float x)
{
    if (x > 30.0f) return 1.0f;
    if (x < -30.0f) return 0.0f;
    return 1.0f / (1.0f + expf(-x));
}

static float DualCore_V5F_StdFeature(const volatile int32_t feature_q[DUALCORE_V5F_FEATURE_DIM], uint8_t i)
{
    float scale = dir4ch_scale[i];
    if ((scale < 1.0e-6f) && (scale > -1.0e-6f)) {
        scale = 1.0f;
    }
    return (((float)feature_q[i]) - dir4ch_mean[i]) / scale;
}

static float DualCore_V5F_FFTClassifierRightProbability(const volatile int32_t feature_q[DUALCORE_V5F_FEATURE_DIM])
{
    float margin = dir4ch_svm_bias;
    uint8_t i;

    for (i = 0; i < DUALCORE_V5F_FEATURE_DIM; i++) {
        margin += dir4ch_svm_weight[i] * DualCore_V5F_StdFeature(feature_q, i);
    }

    return DualCore_V5F_Sigmoid(margin * DUALCORE_V5F_CLASSIFIER_GAIN);
}

#if DUALCORE_V5F_CSP_ENABLE
static float DualCore_V5F_CSPStdFeature(float x, uint8_t i)
{
    float scale = csp4ch_scale[i];
    if ((scale < 1.0e-6f) && (scale > -1.0e-6f)) {
        scale = 1.0f;
    }
    return (x - csp4ch_mean[i]) / scale;
}

static float DualCore_V5F_CSPRightProbability(void)
{
    float sum[CSP4CH_FEATURE_DIM] = {0.0f, 0.0f, 0.0f, 0.0f};
    float sumsq[CSP4CH_FEATURE_DIM] = {0.0f, 0.0f, 0.0f, 0.0f};
    float var[CSP4CH_FEATURE_DIM];
    float feat[CSP4CH_FEATURE_DIM];
    float total_var = 0.0f;
    float margin = csp4ch_svm_bias;
    uint16_t start = g_v5f_ring_write_idx;
    uint16_t j;
    uint8_t k;
    uint8_t ch;

    for (j = 0; j < DUALCORE_V5F_FFT_SIZE; j++) {
        uint16_t idx = (uint16_t)((start + j) & (DUALCORE_V5F_FFT_SIZE - 1u));
        float x[DUALCORE_ADS1299_ACTIVE_CH_NUM];
        float y[CSP4CH_FEATURE_DIM];

        for (ch = 0; ch < DUALCORE_ADS1299_ACTIVE_CH_NUM; ch++) {
            x[ch] = g_v5f_ring[ch][idx];
        }

        for (k = 0; k < CSP4CH_FEATURE_DIM; k++) {
            y[k] = 0.0f;
            for (ch = 0; ch < DUALCORE_ADS1299_ACTIVE_CH_NUM; ch++) {
                y[k] += csp4ch_matrix[k][ch] * x[ch];
            }
            sum[k] += y[k];
            sumsq[k] += y[k] * y[k];
        }
    }

    for (k = 0; k < CSP4CH_FEATURE_DIM; k++) {
        float mean = sum[k] / (float)DUALCORE_V5F_FFT_SIZE;
        var[k] = sumsq[k] / (float)DUALCORE_V5F_FFT_SIZE - mean * mean;
        if (var[k] < DUALCORE_V5F_POWER_EPS) {
            var[k] = DUALCORE_V5F_POWER_EPS;
        }
        total_var += var[k];
    }
    if (total_var < DUALCORE_V5F_POWER_EPS) {
        total_var = DUALCORE_V5F_POWER_EPS;
    }

    for (k = 0; k < CSP4CH_FEATURE_DIM; k++) {
        feat[k] = logf(var[k] / total_var);
        margin += csp4ch_svm_weight[k] * DualCore_V5F_CSPStdFeature(feat[k], k);
    }

    return DualCore_V5F_Sigmoid(margin);
}
#endif

static void DualCore_V5F_RunClassifier(const volatile int32_t feature_q[DUALCORE_V5F_FEATURE_DIM])
{
    float p_right;
    int32_t sr;
    int32_t sl;
    int32_t conf;

#if DUALCORE_V5F_CSP_ENABLE && DUALCORE_V5F_USE_CSP_WHEN_TRAINED
    if (CSP4CH_WEIGHTS_TRAINED) {
        p_right = DualCore_V5F_CSPRightProbability();
    } else {
        p_right = DualCore_V5F_FFTClassifierRightProbability(feature_q);
    }
#else
    p_right = DualCore_V5F_FFTClassifierRightProbability(feature_q);
#endif

#if DUALCORE_V5F_RESULT_FUSION_ENABLE
    {
        float ema_right;
        int32_t right_raw;
        int32_t right_sum = 0;
        uint8_t n;

        if (!g_v5f_fusion_ema_ready) {
            g_v5f_fusion_ema_right = p_right;
            g_v5f_fusion_ema_ready = 1u;
        } else {
            g_v5f_fusion_ema_right = DUALCORE_V5F_FUSION_EMA_ALPHA * g_v5f_fusion_ema_right +
                                     (1.0f - DUALCORE_V5F_FUSION_EMA_ALPHA) * p_right;
        }

        ema_right = g_v5f_fusion_ema_right;
        if (ema_right < 0.0f) ema_right = 0.0f;
        if (ema_right > 1.0f) ema_right = 1.0f;

        right_raw = (int32_t)(ema_right * 10000.0f + 0.5f);
        if (right_raw < 0) right_raw = 0;
        if (right_raw > 10000) right_raw = 10000;

        g_v5f_fusion_right_hist[g_v5f_fusion_write_idx] = right_raw;
        g_v5f_fusion_write_idx = (uint8_t)((g_v5f_fusion_write_idx + 1u) % DUALCORE_V5F_FUSION_ROWS);
        if (g_v5f_fusion_count < DUALCORE_V5F_FUSION_ROWS) {
            g_v5f_fusion_count++;
        }

        for (n = 0; n < g_v5f_fusion_count; n++) {
            right_sum += g_v5f_fusion_right_hist[n];
        }
        sr = (g_v5f_fusion_count > 0u) ? (right_sum / (int32_t)g_v5f_fusion_count) : right_raw;
    }
#else
    sr = (int32_t)(p_right * 10000.0f + 0.5f);
#endif

    if (sr < 0) sr = 0;
    if (sr > 10000) sr = 10000;
    sl = 10000 - sr;
    conf = sl - sr;
    if (conf < 0) conf = -conf;

    if (conf < DUALCORE_V5F_UNKNOWN_CONF) {
        g_ipc_v5f_pred = DUALCORE_V5F_PRED_UNKNOWN;
    } else {
        g_ipc_v5f_pred = (sr >= sl) ? DUALCORE_V5F_PRED_RIGHT : DUALCORE_V5F_PRED_LEFT;
#if DUALCORE_V5F_RESULT_FUSION_ENABLE
        g_v5f_fusion_last_pred = g_ipc_v5f_pred;
#endif
    }

    g_ipc_v5f_score_left = sl;
    g_ipc_v5f_score_right = sr;
    g_ipc_v5f_confidence = conf;
#if DUALCORE_V5F_CSP_ENABLE && DUALCORE_V5F_USE_CSP_WHEN_TRAINED
    g_ipc_v5f_model_trained = CSP4CH_WEIGHTS_TRAINED ? 1u : 0u;
#else
#ifdef DIR4CH_WEIGHTS_TRAINED
    g_ipc_v5f_model_trained = DIR4CH_WEIGHTS_TRAINED ? 1u : 0u;
#else
    g_ipc_v5f_model_trained = 1u;
#endif
#endif
    g_ipc_v5f_infer_count++;
    g_ipc_v5f_infer_valid = 1u;
}

static void DualCore_V5F_InitFFTConfig(void)
{
    uint16_t i;
    uint16_t k;
    float sum = 0.0f;

    for (i = 0; i < DUALCORE_V5F_FFT_SIZE; i++) {
        float phase = (2.0f * DUALCORE_V5F_PI * (float)i) / ((float)DUALCORE_V5F_FFT_SIZE - 1.0f);
        float w = 0.5f - 0.5f * cosf(phase);
        g_v5f_hann[i] = w;
        sum += w * w;
    }

    for (k = 0; k <= DUALCORE_V5F_MAX_BIN; k++) {
        float omega = (2.0f * DUALCORE_V5F_PI * (float)k) / (float)DUALCORE_V5F_FFT_SIZE;
        g_v5f_goertzel_coeff[k] = 2.0f * cosf(omega);
    }

    g_v5f_norm_factor = DUALCORE_V5F_FS * sum;
    if (g_v5f_norm_factor <= 0.0f) {
        g_v5f_norm_factor = 1.0f;
    }

    g_v5f_fft_cfg_ready = 1;
}

static void DualCore_V5F_CalcMeanSlope(const float *src, uint16_t start,
                                       float *out_mean, float *out_slope)
{
    float sum_y = 0.0f;
    float sum_xy = 0.0f;
    const float center = ((float)DUALCORE_V5F_FFT_SIZE - 1.0f) * 0.5f;
    const float sum_x2 = ((float)DUALCORE_V5F_FFT_SIZE *
                         ((float)DUALCORE_V5F_FFT_SIZE * (float)DUALCORE_V5F_FFT_SIZE - 1.0f)) / 12.0f;
    uint16_t j;

    for (j = 0; j < DUALCORE_V5F_FFT_SIZE; j++) {
        uint16_t idx = (uint16_t)((start + j) & (DUALCORE_V5F_FFT_SIZE - 1u));
        float y = src[idx];
        float x = (float)j - center;
        sum_y += y;
        sum_xy += x * y;
    }

    *out_mean = sum_y / (float)DUALCORE_V5F_FFT_SIZE;
    *out_slope = sum_xy / sum_x2;
}

static float DualCore_V5F_GoertzelMag(const float *src, uint16_t start,
                                      uint8_t bin, float mean, float slope)
{
    float q0 = 0.0f;
    float q1 = 0.0f;
    float q2 = 0.0f;
    float coeff = g_v5f_goertzel_coeff[bin];
    const float center = ((float)DUALCORE_V5F_FFT_SIZE - 1.0f) * 0.5f;
    uint16_t j;
    float power;

    for (j = 0; j < DUALCORE_V5F_FFT_SIZE; j++) {
        uint16_t idx = (uint16_t)((start + j) & (DUALCORE_V5F_FFT_SIZE - 1u));
        float x = (float)j - center;
        float sample = src[idx] - (mean + slope * x);
        sample *= g_v5f_hann[j];
        q0 = sample + coeff * q1 - q2;
        q2 = q1;
        q1 = q0;
    }

    power = q1 * q1 + q2 * q2 - coeff * q1 * q2;
    if (power < 0.0f) {
        power = 0.0f;
    }
    return sqrtf(power);
}

static float DualCore_V5F_IntegrateBand(float *mags, int lo, int hi)
{
    float df = DUALCORE_V5F_FS / (float)DUALCORE_V5F_FFT_SIZE;
    float sum = 0.0f;
    int i;

    for (i = lo; i < hi; i++) {
        float psd_i = (mags[i] * mags[i]) / g_v5f_norm_factor;
        float psd_ip1 = (mags[i + 1] * mags[i + 1]) / g_v5f_norm_factor;
        sum += (psd_i + psd_ip1) * 0.5f * df;
    }

    return sum;
}

static void DualCore_V5F_BuildFeature(const float theta[DUALCORE_ADS1299_ACTIVE_CH_NUM],
                                      const float mu[DUALCORE_ADS1299_ACTIVE_CH_NUM],
                                      const float beta[DUALCORE_ADS1299_ACTIVE_CH_NUM],
                                      volatile int32_t feature_q[DUALCORE_V5F_FEATURE_DIM])
{
    float t[DUALCORE_ADS1299_ACTIVE_CH_NUM];
    float m[DUALCORE_ADS1299_ACTIVE_CH_NUM];
    float b[DUALCORE_ADS1299_ACTIVE_CH_NUM];
    float total[DUALCORE_ADS1299_ACTIVE_CH_NUM];
    float feature[DUALCORE_V5F_FEATURE_DIM];
    uint8_t i;

    for (i = 0; i < DUALCORE_ADS1299_ACTIVE_CH_NUM; i++) {
        t[i] = DualCore_V5F_SafePower(theta[i]);
        m[i] = DualCore_V5F_SafePower(mu[i]);
        b[i] = DualCore_V5F_SafePower(beta[i]);
        total[i] = DualCore_V5F_SafePower(t[i] + m[i] + b[i]);
    }

    feature[0]  = DualCore_V5F_LogRatio(m[0],     m[1])     * DUALCORE_V5F_FEATURE_SCALE;
    feature[1]  = DualCore_V5F_LogRatio(b[0],     b[1])     * DUALCORE_V5F_FEATURE_SCALE;
    feature[2]  = DualCore_V5F_LogRatio(m[2],     m[3])     * DUALCORE_V5F_FEATURE_SCALE;
    feature[3]  = DualCore_V5F_LogRatio(b[2],     b[3])     * DUALCORE_V5F_FEATURE_SCALE;
    feature[4]  = DualCore_V5F_LogRatio(t[0],     t[1])     * DUALCORE_V5F_FEATURE_SCALE;
    feature[5]  = DualCore_V5F_LogRatio(t[2],     t[3])     * DUALCORE_V5F_FEATURE_SCALE;
    feature[6]  = DualCore_V5F_LogRatio(total[0], total[1]) * DUALCORE_V5F_FEATURE_SCALE;
    feature[7]  = DualCore_V5F_LogRatio(total[2], total[3]) * DUALCORE_V5F_FEATURE_SCALE;
    feature[8]  = DualCore_V5F_LogRatio(m[0],     total[0]) * DUALCORE_V5F_FEATURE_SCALE;
    feature[9]  = DualCore_V5F_LogRatio(m[1],     total[1]) * DUALCORE_V5F_FEATURE_SCALE;
    feature[10] = DualCore_V5F_LogRatio(m[2],     total[2]) * DUALCORE_V5F_FEATURE_SCALE;
    feature[11] = DualCore_V5F_LogRatio(m[3],     total[3]) * DUALCORE_V5F_FEATURE_SCALE;
    feature[12] = DualCore_V5F_LogRatio(b[0],     total[0]) * DUALCORE_V5F_FEATURE_SCALE;
    feature[13] = DualCore_V5F_LogRatio(b[1],     total[1]) * DUALCORE_V5F_FEATURE_SCALE;
    feature[14] = DualCore_V5F_LogRatio(b[2],     total[2]) * DUALCORE_V5F_FEATURE_SCALE;
    feature[15] = DualCore_V5F_LogRatio(b[3],     total[3]) * DUALCORE_V5F_FEATURE_SCALE;
    feature[16] = DualCore_V5F_LogRatio(b[0],     m[0])     * DUALCORE_V5F_FEATURE_SCALE;
    feature[17] = DualCore_V5F_LogRatio(b[1],     m[1])     * DUALCORE_V5F_FEATURE_SCALE;
    feature[18] = DualCore_V5F_LogRatio(b[2],     m[2])     * DUALCORE_V5F_FEATURE_SCALE;
    feature[19] = DualCore_V5F_LogRatio(b[3],     m[3])     * DUALCORE_V5F_FEATURE_SCALE;
    feature[20] = DualCore_V5F_LogRatio(m[0],     m[2])     * DUALCORE_V5F_FEATURE_SCALE;
    feature[21] = DualCore_V5F_LogRatio(m[1],     m[3])     * DUALCORE_V5F_FEATURE_SCALE;
    feature[22] = DualCore_V5F_LogRatio(b[0],     b[2])     * DUALCORE_V5F_FEATURE_SCALE;
    feature[23] = DualCore_V5F_LogRatio(b[1],     b[3])     * DUALCORE_V5F_FEATURE_SCALE;

    for (i = 0; i < DUALCORE_V5F_FEATURE_DIM; i++) {
        feature_q[i] = DualCore_V5F_FeatureToQ(feature[i]);
    }
}

static void DualCore_V5F_ComputeFFTFeature(void)
{
    uint8_t ch;
    uint8_t bin;
    uint8_t i;
    uint16_t start = g_v5f_ring_write_idx;
    float theta[DUALCORE_ADS1299_ACTIVE_CH_NUM];
    float mu[DUALCORE_ADS1299_ACTIVE_CH_NUM];
    float beta[DUALCORE_ADS1299_ACTIVE_CH_NUM];
    float mags[DUALCORE_V5F_MAX_BIN + 1u];

    if (!g_v5f_fft_cfg_ready) {
        DualCore_V5F_InitFFTConfig();
    }

    for (ch = 0; ch < DUALCORE_ADS1299_ACTIVE_CH_NUM; ch++) {
        float mean = 0.0f;
        float slope = 0.0f;
        DualCore_V5F_CalcMeanSlope(g_v5f_ring[ch], start, &mean, &slope);

        for (bin = 0; bin <= DUALCORE_V5F_MAX_BIN; bin++) {
            mags[bin] = 0.0f;
        }
        for (bin = 4; bin <= DUALCORE_V5F_MAX_BIN; bin++) {
            mags[bin] = DualCore_V5F_GoertzelMag(g_v5f_ring[ch], start, bin, mean, slope);
        }

        theta[ch] = DualCore_V5F_IntegrateBand(mags, 4, 8);
        mu[ch]    = DualCore_V5F_IntegrateBand(mags, 8, 13);
        beta[ch]  = DualCore_V5F_IntegrateBand(mags, 13, 31);
    }

    DualCore_V5F_BuildFeature(theta, mu, beta, g_ipc_v5f_feature_q);
    DualCore_V5F_RunClassifier(g_ipc_v5f_feature_q);

    (void)i;

    g_ipc_v5f_fft_count++;
    g_ipc_v5f_feature_valid = 1;
}

static void DualCore_V5F_ResetDSP(void)
{
    uint8_t c;
    uint16_t i;

    if (!g_v5f_fft_cfg_ready) {
        DualCore_V5F_InitFFTConfig();
    }

    for (c = 0; c < DUALCORE_ADS1299_ACTIVE_CH_NUM; c++) {
        g_v5f_drift[c].drift_base = 0.0f;
        g_v5f_drift[c].init = 0;
        DualCore_InitOneNotch(&g_v5f_notch[c]);
        DualCore_InitOneBandpass(&g_v5f_bandpass[c]);
        for (i = 0; i < DUALCORE_V5F_FFT_SIZE; i++) {
            g_v5f_ring[c][i] = 0.0f;
        }
    }

    for (i = 0; i < DUALCORE_V5F_FEATURE_DIM; i++) {
        g_ipc_v5f_feature_q[i] = 0;
    }

    g_v5f_ring_write_idx = 0;
    g_v5f_ring_valid_count = 0;
    g_v5f_step_count = 0;
    g_ipc_v5f_window_count = 0;
    g_ipc_v5f_window_samples = 0;
    g_ipc_v5f_fft_count = 0;
    g_ipc_v5f_feature_valid = 0;
    g_ipc_v5f_infer_valid = 0;
    g_ipc_v5f_pred = DUALCORE_V5F_PRED_UNKNOWN;
    g_ipc_v5f_model_trained = 0;
    g_ipc_v5f_score_left = 0;
    g_ipc_v5f_score_right = 0;
    g_ipc_v5f_confidence = 0;
    g_ipc_v5f_infer_count = 0;

#if DUALCORE_V5F_RESULT_FUSION_ENABLE
    for (i = 0; i < DUALCORE_V5F_FUSION_ROWS; i++) {
        g_v5f_fusion_right_hist[i] = 5000;
    }
    g_v5f_fusion_write_idx = 0;
    g_v5f_fusion_count = 0;
    g_v5f_fusion_ema_ready = 0;
    g_v5f_fusion_ema_right = 0.5f;
    g_v5f_fusion_last_pred = DUALCORE_V5F_PRED_UNKNOWN;
#endif
}

static void DualCore_V5F_ProcessPreprocess(volatile DualCore_IPC_FrameSlot_t *slot,
                                           int32_t ch_data[DUALCORE_ADS1299_CHANNEL_NUM])
{
    uint8_t c;
    uint8_t i;

    g_ipc_v5f_window_samples++;
    g_v5f_step_count++;
    if (g_ipc_v5f_window_samples >= DUALCORE_V5F_WINDOW_SIZE) {
        g_ipc_v5f_window_samples = 0;
        g_ipc_v5f_window_count++;
    }

    slot->v5f_sample_count = g_ipc_v5f_recv_count;
    slot->v5f_window_count = g_ipc_v5f_window_count;

    for (c = 0; c < DUALCORE_ADS1299_ACTIVE_CH_NUM; c++) {
        int32_t uv_x1000 = DualCore_ADS1299_CodeToMicroVoltX1000(ch_data[c]);
        float volt = (float)uv_x1000 / 1000000000.0f;
        float drift = DualCore_RemoveRealtimeDrift(volt, &g_v5f_drift[c]);
        float pre = DualCore_IIR_SOS_Step(drift, &g_v5f_notch[c]);
        float filt = DualCore_IIR_SOS_Step(pre, &g_v5f_bandpass[c]);

        slot->v5f_uv_x1000[c] = uv_x1000;
        slot->v5f_pre_x1000[c] = DualCore_FloatVoltToMicroVoltX1000(pre);
        slot->v5f_filt_x1000[c] = DualCore_FloatVoltToMicroVoltX1000(filt);

        g_v5f_ring[c][g_v5f_ring_write_idx] = filt;
    }

    g_v5f_ring_write_idx = (uint16_t)((g_v5f_ring_write_idx + 1u) & (DUALCORE_V5F_FFT_SIZE - 1u));
    if (g_v5f_ring_valid_count < DUALCORE_V5F_FFT_SIZE) {
        g_v5f_ring_valid_count++;
    }

    if ((g_v5f_ring_valid_count >= DUALCORE_V5F_FFT_SIZE) &&
        (g_v5f_step_count >= DUALCORE_V5F_STEP_SIZE)) {
        g_v5f_step_count = 0;
        DualCore_V5F_ComputeFFTFeature();
    }

    slot->v5f_feature_valid = g_ipc_v5f_feature_valid;
    slot->v5f_fft_count = g_ipc_v5f_fft_count;
    for (i = 0; i < DUALCORE_V5F_FEATURE_DIM; i++) {
        slot->v5f_feature_q[i] = g_ipc_v5f_feature_q[i];
    }
    slot->v5f_infer_valid = g_ipc_v5f_infer_valid;
    slot->v5f_pred = g_ipc_v5f_pred;
    slot->v5f_model_trained = g_ipc_v5f_model_trained;
    slot->v5f_score_left = g_ipc_v5f_score_left;
    slot->v5f_score_right = g_ipc_v5f_score_right;
    slot->v5f_confidence = g_ipc_v5f_confidence;
    slot->v5f_infer_count = g_ipc_v5f_infer_count;
}

void DualCore_V5F_MainLoopProcess(void)
{
    uint32_t share_addr;
    uint16_t checksum = 0;
    uint32_t seq = 0;
    uint32_t latest_seq = 0;
    uint32_t start_seq = 0;
    uint32_t latest_idx = 0;
    uint32_t status = 0;
    int32_t ch_data[DUALCORE_ADS1299_CHANNEL_NUM];
    uint16_t i;
    uint16_t c;
    uint8_t processed_any = 0u;

    if (!g_ipc_v5f_pending_flag) {
        return;
    }

    g_ipc_v5f_pending_flag = 0u;
    share_addr = g_ipc_v5f_pending_share_addr;

    if (share_addr < 0x20110000u || share_addr > 0x2017FFFFu) {
        share_addr = 0u;
    }

    if (g_ipc_v5f_ready && share_addr != 0u) {
        volatile DualCore_IPC_FrameSlot_t *latest_slot = (volatile DualCore_IPC_FrameSlot_t *)share_addr;
        volatile DualCore_IPC_FrameSlot_t *base_slot;

        DUALCORE_FENCE();

        latest_seq = latest_slot->seq;
        latest_idx = latest_seq % DUALCORE_IPC_FRAME_SLOT_NUM;
        base_slot = latest_slot - latest_idx;

        start_seq = g_ipc_v5f_last_seq + 1u;
        if (latest_seq >= DUALCORE_IPC_FRAME_SLOT_NUM) {
            uint32_t oldest_valid_seq = latest_seq - DUALCORE_IPC_FRAME_SLOT_NUM + 1u;
            if (start_seq < oldest_valid_seq) {
                start_seq = oldest_valid_seq;
            }
        }

        for (seq = start_seq; seq <= latest_seq; seq++) {
            volatile DualCore_IPC_FrameSlot_t *slot = &base_slot[seq % DUALCORE_IPC_FRAME_SLOT_NUM];

            DUALCORE_FENCE();
            if (slot->seq != seq) {
                continue;
            }

            for (i = 0; i < DUALCORE_IPC_FRAME_LEN; i++) {
                g_ipc_v5f_last_frame[i] = slot->frame[i];
            }

            checksum = DualCore_IPC_Checksum16(g_ipc_v5f_last_frame, DUALCORE_IPC_FRAME_LEN);
            DualCore_ADS1299_ParseRawFrame(g_ipc_v5f_last_frame, &status, ch_data);

            g_ipc_v5f_recv_count++;

            slot->v5f_status = status;
            for (c = 0; c < DUALCORE_ADS1299_ACTIVE_CH_NUM; c++) {
                slot->v5f_ch_code[c] = ch_data[c];
            }

            DualCore_V5F_ProcessPreprocess(slot, ch_data);

            DUALCORE_FENCE();
            slot->v5f_parse_valid = 1u;

            g_ipc_v5f_last_seq = seq;
            g_ipc_v5f_last_checksum = checksum;
            processed_any = 1u;
        }

        if (!processed_any) {
            seq = latest_seq;
            checksum = 0xDDDDu;
        } else {
            seq = g_ipc_v5f_last_seq;
            checksum = (uint16_t)g_ipc_v5f_last_checksum;
        }
    } else {
        checksum = 0xEEEEu;
        seq = 0u;
    }

    IPC_WriteMSG(IPC_MSG1, (((uint32_t)checksum) << 16) | (seq & 0x0000FFFFu));

    DUALCORE_FENCE();

    IPC_ITConfig(IPC_CH1, IPC_CH_Sta_Bit0, ENABLE);
}
#endif

void IPC_Config(IPC_Channel_TypeDef IPC_CHx, IPC_TxCID_TypeDef IPC_TxCIDx, IPC_RxCID_TypeDef IPC_RxCIDx)
{
    IPC_InitTypeDef IPC_InitStructure = {0};

    IPC_InitStructure.IPC_CH = IPC_CHx;
    IPC_InitStructure.TxCID  = IPC_TxCIDx;
    IPC_InitStructure.RxCID  = IPC_RxCIDx;
    IPC_InitStructure.TxIER  = ENABLE;
    IPC_InitStructure.RxIER  = ENABLE;
    IPC_InitStructure.AutoEN = ENABLE;

    IPC_Init(&IPC_InitStructure);
}

void DualCore_IPC_Init_V3F(void)
{
#if defined(Core_V3F) && DUALCORE_IPC_RUNTIME_ENABLE
    uint16_t i;
    uint16_t s;
    uint16_t c;

    g_ipc_v3f_notify_count       = 0;
    g_ipc_v3f_ack_count          = 0;
    g_ipc_v3f_tx_checksum        = 0;
    g_ipc_v3f_ack_checksum       = 0;
    g_ipc_v3f_ack_tx_checksum    = 0;
    g_ipc_v3f_checksum_ok        = 0;
    g_ipc_v3f_checksum_bad       = 0;
    g_ipc_v3f_parse_ok           = 0;
    g_ipc_v3f_parse_bad          = 0;
    g_ipc_v3f_last_v3f_ch0       = 0;
    g_ipc_v3f_last_v5f_ch0       = 0;
    g_ipc_v3f_last_v5f_uv0_x1000 = 0;
    g_ipc_v3f_last_v5f_pre0_x1000 = 0;
    g_ipc_v3f_last_v5f_filt0_x1000 = 0;
    g_ipc_v3f_last_v5f_sample_count = 0;
    g_ipc_v3f_last_v5f_window_count = 0;
    g_ipc_v3f_last_v5f_fft_count = 0;
    g_ipc_v3f_last_v5f_feature_valid = 0;
    g_ipc_v3f_last_v5f_infer_valid = 0;
    g_ipc_v3f_last_v5f_pred = DUALCORE_V5F_PRED_UNKNOWN;
    g_ipc_v3f_last_v5f_model_trained = 0;
    g_ipc_v3f_last_v5f_score_left = 0;
    g_ipc_v3f_last_v5f_score_right = 0;
    g_ipc_v3f_last_v5f_confidence = 0;
    g_ipc_v3f_last_v5f_infer_count = 0;
    for (i = 0; i < DUALCORE_V5F_FEATURE_DIM; i++) {
        g_ipc_v3f_last_v5f_feature_q[i] = 0;
    }
    g_ipc_v3f_ready              = 0;

    for (i = 0; i < DUALCORE_IPC_TX_HISTORY_SIZE; i++) {
        g_ipc_v3f_tx_checksum_hist[i] = 0;
        g_ipc_v3f_tx_status_hist[i] = 0;
        for (c = 0; c < DUALCORE_ADS1299_ACTIVE_CH_NUM; c++) {
            g_ipc_v3f_tx_ch_hist[i][c] = 0;
        }
    }

    for (s = 0; s < DUALCORE_IPC_FRAME_SLOT_NUM; s++) {
        g_ipc_v3f_frame_slot[s].seq = 0;
        g_ipc_v3f_frame_slot[s].len = 0;
        g_ipc_v3f_frame_slot[s].checksum = 0;
        g_ipc_v3f_frame_slot[s].v5f_parse_valid = 0;
        g_ipc_v3f_frame_slot[s].v5f_status = 0;
        g_ipc_v3f_frame_slot[s].v5f_sample_count = 0;
        g_ipc_v3f_frame_slot[s].v5f_window_count = 0;
        g_ipc_v3f_frame_slot[s].v5f_feature_valid = 0;
        g_ipc_v3f_frame_slot[s].v5f_fft_count = 0;
        g_ipc_v3f_frame_slot[s].v5f_infer_valid = 0;
        g_ipc_v3f_frame_slot[s].v5f_pred = DUALCORE_V5F_PRED_UNKNOWN;
        g_ipc_v3f_frame_slot[s].v5f_model_trained = 0;
        g_ipc_v3f_frame_slot[s].v5f_score_left = 0;
        g_ipc_v3f_frame_slot[s].v5f_score_right = 0;
        g_ipc_v3f_frame_slot[s].v5f_confidence = 0;
        g_ipc_v3f_frame_slot[s].v5f_infer_count = 0;
        for (i = 0; i < DUALCORE_V5F_FEATURE_DIM; i++) {
            g_ipc_v3f_frame_slot[s].v5f_feature_q[i] = 0;
        }
        for (c = 0; c < DUALCORE_ADS1299_ACTIVE_CH_NUM; c++) {
            g_ipc_v3f_frame_slot[s].v5f_ch_code[c] = 0;
            g_ipc_v3f_frame_slot[s].v5f_uv_x1000[c] = 0;
            g_ipc_v3f_frame_slot[s].v5f_pre_x1000[c] = 0;
            g_ipc_v3f_frame_slot[s].v5f_filt_x1000[c] = 0;
        }
        for (i = 0; i < DUALCORE_IPC_FRAME_LEN; i++) {
            g_ipc_v3f_frame_slot[s].frame[i] = 0;
        }
    }

    IPC_DeInit();
    IPC_Config(IPC_CH0, IPC_TxCID1, IPC_RxCID0);
    IPC_CH0_Lock();

    IPC_Config(IPC_CH1, IPC_TxCID1, IPC_RxCID0);
    IPC_CH1_Lock();

    IPC_WriteMSG(IPC_MSG0, 0);
    IPC_SetFlagStatus(IPC_CH0, IPC_CH_Sta_Bit0);
    IPC_ClearFlagStatus(IPC_CH0, IPC_CH_Sta_Bit1);

    IPC_ClearFlagStatus(IPC_CH1, IPC_CH_Sta_Bit0);
    IPC_ClearFlagStatus(IPC_CH1, IPC_CH_Sta_Bit1);

    NVIC_SetPriority(IPC_CH0_IRQn, (1 << 7) | (1 << 4));
    NVIC_EnableIRQ(IPC_CH0_IRQn);

    NVIC_SetPriority(IPC_CH1_IRQn, (1 << 7) | (2 << 4));
    NVIC_EnableIRQ(IPC_CH1_IRQn);

    g_ipc_v3f_ready = 1;
#endif
}

void DualCore_IPC_Init_V5F(void)
{
#if defined(Core_V5F) && DUALCORE_IPC_RUNTIME_ENABLE
    uint16_t i;

    g_ipc_v5f_recv_count = 0;
    g_ipc_v5f_last_seq = 0;
    g_ipc_v5f_last_checksum = 0;
    g_ipc_v5f_ready = 0;
    g_ipc_v5f_window_count = 0;
    g_ipc_v5f_window_samples = 0;
    g_ipc_v5f_pending_flag = 0;
    g_ipc_v5f_pending_share_addr = 0;

    for (i = 0; i < DUALCORE_IPC_FRAME_LEN; i++) {
        g_ipc_v5f_last_frame[i] = 0;
    }

    DualCore_V5F_ResetDSP();

    IPC_ITConfig(IPC_CH0, IPC_CH_Sta_Bit0, DISABLE);
    IPC_ITConfig(IPC_CH0, IPC_CH_Sta_Bit1, DISABLE);
    IPC_ClearFlagStatus(IPC_CH0, IPC_CH_Sta_Bit0);
    IPC_ClearFlagStatus(IPC_CH0, IPC_CH_Sta_Bit1);
    NVIC_ClearPendingIRQ(IPC_CH0_IRQn);

    IPC_ITConfig(IPC_CH1, IPC_CH_Sta_Bit0, DISABLE);
    IPC_ITConfig(IPC_CH1, IPC_CH_Sta_Bit1, DISABLE);
    IPC_ClearFlagStatus(IPC_CH1, IPC_CH_Sta_Bit0);
    IPC_ClearFlagStatus(IPC_CH1, IPC_CH_Sta_Bit1);
    NVIC_ClearPendingIRQ(IPC_CH1_IRQn);

    NVIC_SetPriority(IPC_CH0_IRQn, (2 << 5) | (0 << 4));
    NVIC_EnableIRQ(IPC_CH0_IRQn);

    NVIC_SetPriority(IPC_CH1_IRQn, (2 << 5) | (1 << 4));
    NVIC_EnableIRQ(IPC_CH1_IRQn);

    g_ipc_v5f_ready = 1;
#endif
}

void DualCore_IPC_NotifyFromV3F(void)
{
#if defined(Core_V3F) && DUALCORE_IPC_RUNTIME_ENABLE
    if (!g_ipc_v3f_ready) {
        return;
    }

    IPC_WriteMSG(IPC_MSG0, (uint32_t)&g_ipc_v3f_frame_slot[0]);
    DUALCORE_FENCE();
    IPC_ITConfig(IPC_CH0, IPC_CH_Sta_Bit1, ENABLE);
#else
    (void)0;
#endif
}

void DualCore_IPC_SendFrameFromV3F(const uint8_t *frame, uint16_t len)
{
#if defined(Core_V3F) && DUALCORE_IPC_RUNTIME_ENABLE
    uint16_t i;
    uint16_t c;
    uint16_t copy_len;
    uint32_t seq;
    uint32_t slot_idx;
    uint32_t hist_idx;
    volatile DualCore_IPC_FrameSlot_t *slot;
    uint16_t checksum;
    uint32_t status = 0;
    int32_t ch_data[DUALCORE_ADS1299_CHANNEL_NUM];

    if ((!g_ipc_v3f_ready) || (frame == 0)) {
        return;
    }

    seq = g_ipc_v3f_notify_count + 1u;
    slot_idx = seq % DUALCORE_IPC_FRAME_SLOT_NUM;
    hist_idx = seq % DUALCORE_IPC_TX_HISTORY_SIZE;
    slot = &g_ipc_v3f_frame_slot[slot_idx];

    copy_len = (len < DUALCORE_IPC_FRAME_LEN) ? len : DUALCORE_IPC_FRAME_LEN;

    slot->v5f_parse_valid = 0;
    slot->v5f_status = 0;
    slot->v5f_sample_count = 0;
    slot->v5f_window_count = 0;
    slot->v5f_feature_valid = 0;
    slot->v5f_fft_count = 0;
    slot->v5f_infer_valid = 0;
    slot->v5f_pred = DUALCORE_V5F_PRED_UNKNOWN;
    slot->v5f_model_trained = 0;
    slot->v5f_score_left = 0;
    slot->v5f_score_right = 0;
    slot->v5f_confidence = 0;
    slot->v5f_infer_count = 0;
    for (i = 0; i < DUALCORE_V5F_FEATURE_DIM; i++) {
        slot->v5f_feature_q[i] = 0;
    }
    for (c = 0; c < DUALCORE_ADS1299_ACTIVE_CH_NUM; c++) {
        slot->v5f_ch_code[c] = 0;
        slot->v5f_uv_x1000[c] = 0;
        slot->v5f_pre_x1000[c] = 0;
        slot->v5f_filt_x1000[c] = 0;
    }

    for (i = 0; i < copy_len; i++) {
        slot->frame[i] = frame[i];
    }
    for (; i < DUALCORE_IPC_FRAME_LEN; i++) {
        slot->frame[i] = 0;
    }

    checksum = DualCore_IPC_Checksum16(slot->frame, DUALCORE_IPC_FRAME_LEN);
    DualCore_ADS1299_ParseRawFrame(slot->frame, &status, ch_data);

    slot->len = copy_len;
    slot->checksum = checksum;
    slot->seq = seq;

    g_ipc_v3f_tx_checksum = checksum;
    g_ipc_v3f_tx_checksum_hist[hist_idx] = checksum;
    g_ipc_v3f_tx_status_hist[hist_idx] = status;
    for (c = 0; c < DUALCORE_ADS1299_ACTIVE_CH_NUM; c++) {
        g_ipc_v3f_tx_ch_hist[hist_idx][c] = ch_data[c];
    }

    DUALCORE_FENCE();

    g_ipc_v3f_notify_count = seq;
    IPC_WriteMSG(IPC_MSG0, (uint32_t)slot);

    DUALCORE_FENCE();

    IPC_ITConfig(IPC_CH0, IPC_CH_Sta_Bit1, ENABLE);
#else
    (void)frame;
    (void)len;
#endif
}

uint32_t DualCore_IPC_GetNotifyCount(void)       { return g_ipc_v3f_notify_count; }
uint32_t DualCore_IPC_GetAckCount(void)          { return g_ipc_v3f_ack_count; }
uint32_t DualCore_IPC_GetTxChecksum(void)        { return g_ipc_v3f_tx_checksum; }
uint32_t DualCore_IPC_GetAckChecksum(void)       { return g_ipc_v3f_ack_checksum; }
uint32_t DualCore_IPC_GetAckTxChecksum(void)     { return g_ipc_v3f_ack_tx_checksum; }
uint32_t DualCore_IPC_GetChecksumOKCount(void)   { return g_ipc_v3f_checksum_ok; }
uint32_t DualCore_IPC_GetChecksumBadCount(void)  { return g_ipc_v3f_checksum_bad; }
uint32_t DualCore_IPC_GetParseOKCount(void)      { return g_ipc_v3f_parse_ok; }
uint32_t DualCore_IPC_GetParseBadCount(void)     { return g_ipc_v3f_parse_bad; }
int32_t  DualCore_IPC_GetLastV3FCh0(void)        { return g_ipc_v3f_last_v3f_ch0; }
int32_t  DualCore_IPC_GetLastV5FCh0(void)        { return g_ipc_v3f_last_v5f_ch0; }
int32_t  DualCore_IPC_GetLastV5FCh0uVX1000(void) { return g_ipc_v3f_last_v5f_uv0_x1000; }
int32_t  DualCore_IPC_GetLastV5FPre0uVX1000(void){ return g_ipc_v3f_last_v5f_pre0_x1000; }
int32_t  DualCore_IPC_GetLastV5FFilt0uVX1000(void){ return g_ipc_v3f_last_v5f_filt0_x1000; }
uint32_t DualCore_IPC_GetLastV5FSampleCount(void){ return g_ipc_v3f_last_v5f_sample_count; }
uint32_t DualCore_IPC_GetLastV5FWindowCount(void){ return g_ipc_v3f_last_v5f_window_count; }
uint32_t DualCore_IPC_GetLastV5FFFTCount(void)   { return g_ipc_v3f_last_v5f_fft_count; }
uint32_t DualCore_IPC_GetLastV5FFeatureValid(void){ return g_ipc_v3f_last_v5f_feature_valid; }
int32_t  DualCore_IPC_GetLastV5FFeature(uint8_t idx)
{
    if (idx >= DUALCORE_V5F_FEATURE_DIM) {
        return 0;
    }
    return g_ipc_v3f_last_v5f_feature_q[idx];
}
uint32_t DualCore_IPC_GetLastV5FInferValid(void){ return g_ipc_v3f_last_v5f_infer_valid; }
uint32_t DualCore_IPC_GetLastV5FPred(void){ return g_ipc_v3f_last_v5f_pred; }
uint32_t DualCore_IPC_GetLastV5FModelTrained(void){ return g_ipc_v3f_last_v5f_model_trained; }
int32_t  DualCore_IPC_GetLastV5FScoreLeft(void){ return g_ipc_v3f_last_v5f_score_left; }
int32_t  DualCore_IPC_GetLastV5FScoreRight(void){ return g_ipc_v3f_last_v5f_score_right; }
int32_t  DualCore_IPC_GetLastV5FConfidence(void){ return g_ipc_v3f_last_v5f_confidence; }
uint32_t DualCore_IPC_GetLastV5FInferCount(void){ return g_ipc_v3f_last_v5f_infer_count; }

void IPC_CH0_Handler(void)
{
#if defined(Core_V3F)
    if (IPC_GetITStatus(IPC_CH0, IPC_CH_Sta_Bit1) != RESET) {
        IPC_ITConfig(IPC_CH0, IPC_CH_Sta_Bit1, DISABLE);
    }

#elif defined(Core_V5F)
    if (IPC_GetITStatus(IPC_CH0, IPC_CH_Sta_Bit0) != RESET) {
        IPC_ITConfig(IPC_CH0, IPC_CH_Sta_Bit0, DISABLE);
    }
    if (IPC_GetITStatus(IPC_CH0, IPC_CH_Sta_Bit1) != RESET) {
        uint32_t addr = IPC_ReadMSG(IPC_MSG0);
        if (addr >= 0x20110000u && addr <= 0x2017FFFFu) {
            g_ipc_v5f_pending_share_addr = addr;
            g_ipc_v5f_pending_flag = 1u;
        }
    }
    IPC_ITConfig(IPC_CH0, IPC_CH_Sta_Bit1, DISABLE);
#endif
}

void IPC_CH1_Handler(void)
{
#if defined(Core_V3F)
    if (IPC_GetITStatus(IPC_CH1, IPC_CH_Sta_Bit0) != RESET) {
        uint32_t ack_pack = IPC_ReadMSG(IPC_MSG1);
        uint32_t ack_seq_low = (ack_pack & 0x0000FFFFu);
        uint32_t ack_cs   = ((ack_pack >> 16) & 0x0000FFFFu);
        uint32_t slot_idx = ack_seq_low % DUALCORE_IPC_FRAME_SLOT_NUM;
        volatile DualCore_IPC_FrameSlot_t *slot = &g_ipc_v3f_frame_slot[slot_idx];
        uint32_t ack_seq = slot->seq;
        uint32_t hist_idx;
        uint32_t tx_cs;
        uint8_t parse_ok = 1u;
        uint16_t c;

        if ((ack_seq & 0x0000FFFFu) != ack_seq_low) {
            ack_seq = ack_seq_low;
}


        DUALCORE_FENCE();

        if ((slot->seq != ack_seq) || (slot->v5f_parse_valid == 0u)) {
            parse_ok = 0u;
        } else {
            if (slot->v5f_status != g_ipc_v3f_tx_status_hist[hist_idx]) {
                parse_ok = 0u;
            }
            for (c = 0; c < DUALCORE_ADS1299_ACTIVE_CH_NUM; c++) {
                if (slot->v5f_ch_code[c] != g_ipc_v3f_tx_ch_hist[hist_idx][c]) {
                    parse_ok = 0u;
                }
            }
        }

        g_ipc_v3f_last_v3f_ch0 = g_ipc_v3f_tx_ch_hist[hist_idx][0];
        g_ipc_v3f_last_v5f_ch0 = slot->v5f_ch_code[0];
        g_ipc_v3f_last_v5f_uv0_x1000 = slot->v5f_uv_x1000[0];
        g_ipc_v3f_last_v5f_pre0_x1000 = slot->v5f_pre0_x1000[0];
        g_ipc_v3f_last_v5f_filt0_x1000 = slot->v5f_filt_x1000[0];
        g_ipc_v3f_last_v5f_sample_count = slot->v5f_sample_count;
        g_ipc_v3f_last_v5f_window_count = slot->v5f_window_count;
        g_ipc_v3f_last_v5f_fft_count = slot->v5f_fft_count;
        g_ipc_v3f_last_v5f_feature_valid = slot->v5f_feature_valid;
        for (c = 0; c < DUALCORE_V5F_FEATURE_DIM; c++) {
            g_ipc_v3f_last_v5f_feature_q[c] = slot->v5f_feature_q[c];
        }
        g_ipc_v3f_last_v5f_infer_valid = slot->v5f_infer_valid;
        g_ipc_v3f_last_v5f_pred = slot->v5f_pred;
        g_ipc_v3f_last_v5f_model_trained = slot->v5f_model_trained;
        g_ipc_v3f_last_v5f_score_left = slot->v5f_score_left;
        g_ipc_v3f_last_v5f_score_right = slot->v5f_score_right;
        g_ipc_v3f_last_v5f_confidence = slot->v5f_confidence;
        g_ipc_v3f_last_v5f_infer_count = slot->v5f_infer_count;

        if (parse_ok) {
            g_ipc_v3f_parse_ok++;
        } else {
            g_ipc_v3f_parse_bad++;
        }

        IPC_ITConfig(IPC_CH1, IPC_CH_Sta_Bit0, DISABLE);
    }

#elif defined(Core_V5F)
    if (IPC_GetITStatus(IPC_CH1, IPC_CH_Sta_Bit0) != RESET) {
        IPC_ITConfig(IPC_CH1, IPC_CH_Sta_Bit0, DISABLE);
    }
    if (IPC_GetITStatus(IPC_CH1, IPC_CH_Sta_Bit1) != RESET) {
        IPC_ITConfig(IPC_CH1, IPC_CH_Sta_Bit1, DISABLE);
    }
#endif
}


