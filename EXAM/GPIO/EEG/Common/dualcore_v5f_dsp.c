#include "dualcore_v5f_dsp.h"
#include <math.h>
#include <string.h>

#if defined(Core_V5F)
#include "DualCore_Direction_Model_4ch_Weights.h"
#include "DualCore_CSP_Model_4ch_Weights.h"

volatile uint32_t g_ipc_v5f_fft_count = 0;
volatile uint8_t  g_ipc_v5f_feature_valid = 0;
volatile int32_t  g_ipc_v5f_feature_q[DUALCORE_V5F_FEATURE_DIM];
volatile uint8_t  g_ipc_v5f_infer_valid = 0;
volatile uint8_t  g_ipc_v5f_pred = DUALCORE_V5F_PRED_UNKNOWN;
volatile uint8_t  g_ipc_v5f_model_trained = 0;
volatile int32_t  g_ipc_v5f_score_left = 0;
volatile int32_t  g_ipc_v5f_score_right = 0;
volatile int32_t  g_ipc_v5f_confidence = 0;
volatile uint32_t g_ipc_v5f_infer_count = 0;

#if DUALCORE_V5F_RESULT_FUSION_ENABLE
static int32_t  g_v5f_fusion_right_hist[DUALCORE_V5F_FUSION_ROWS];
static uint8_t  g_v5f_fusion_write_idx = 0;
static uint8_t  g_v5f_fusion_count = 0;
static uint8_t  g_v5f_fusion_ema_ready = 0;
static float    g_v5f_fusion_ema_right = 0.5f;
static uint8_t  g_v5f_fusion_last_pred = DUALCORE_V5F_PRED_UNKNOWN;
#endif

float g_v5f_ring[DUALCORE_ADS1299_ACTIVE_CH_NUM][DUALCORE_V5F_FFT_SIZE];
static float g_v5f_hann[DUALCORE_V5F_FFT_SIZE];
static float g_v5f_goertzel_coeff[DUALCORE_V5F_MAX_BIN + 1u];
static float g_v5f_norm_factor = 1.0f;
uint16_t g_v5f_ring_write_idx = 0;
uint16_t g_v5f_ring_valid_count = 0;
uint16_t g_v5f_step_count = 0;
static uint8_t  g_v5f_fft_cfg_ready = 0;

DualCore_DriftRemove_t g_v5f_drift[DUALCORE_ADS1299_ACTIVE_CH_NUM];
DualCore_IIR_SOS_t     g_v5f_notch[DUALCORE_ADS1299_ACTIVE_CH_NUM];
DualCore_IIR_SOS_t     g_v5f_bandpass[DUALCORE_ADS1299_ACTIVE_CH_NUM];

static float DualCore_IIR_Step(float input, DualCore_IIR_Biquad_t *filt)
{
    float wn = input - filt->a1 * filt->w1 - filt->a2 * filt->w2;
    float out = filt->b0 * wn + filt->b1 * filt->w1 + filt->b2 * filt->w2;

    filt->w2 = filt->w1;
    filt->w1 = wn;

    return out;
}

float DualCore_IIR_SOS_Step(float input, DualCore_IIR_SOS_t *filt)
{
    float tmp = input;
    uint8_t i;

    for (i = 0; i < filt->NumSections; i++) {
        tmp = DualCore_IIR_Step(tmp, &filt->Sec[i]);
    }

    return tmp;
}

float DualCore_RemoveRealtimeDrift(float x, DualCore_DriftRemove_t *st)
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

void DualCore_InitOneNotch(DualCore_IIR_SOS_t *f)
{
    f->NumSections = 2;
    f->Sec[0].b0 = 0.9794827610f; f->Sec[0].b1 = -0.6053536377f; f->Sec[0].b2 = 0.9794827610f;
    f->Sec[0].a1 = -0.6053536377f; f->Sec[0].a2 = 0.9589655220f;  f->Sec[0].w1 = 0.0f; f->Sec[0].w2 = 0.0f;
    f->Sec[1].b0 = 0.9794827610f; f->Sec[1].b1 = -0.6053536377f; f->Sec[1].b2 = 0.9794827610f;
    f->Sec[1].a1 = -0.6053536377f; f->Sec[1].a2 = 0.9589655220f;  f->Sec[1].w1 = 0.0f; f->Sec[1].w2 = 0.0f;
}

void DualCore_InitOneBandpass(DualCore_IIR_SOS_t *f)
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

void DualCore_V5F_ComputeFFTFeature(void)
{
    uint8_t ch;
    uint8_t bin;
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

    g_ipc_v5f_fft_count++;
    g_ipc_v5f_feature_valid = 1;
}

void DualCore_V5F_DSP_Init(void)
{
    DualCore_V5F_InitFFTConfig();
}

void DualCore_V5F_DSP_Reset(void)
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

#endif