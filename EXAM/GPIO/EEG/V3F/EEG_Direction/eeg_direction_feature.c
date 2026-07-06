#include "eeg_direction_feature.h"
#include "Serial.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define DIR_TEXT_PORT               SERIAL_PORT_DEBUG
#define DIR_PRINT_HEADER_ON_START   1
#define DIR_FEAT_F_MIN              4
#define DIR_FEAT_F_MAX              30

#define EEG_APP_MODE_COLLECT        1
#define EEG_APP_MODE_INFER          2
#define EEG_APP_MODE_COLLECT_CSP    3

extern uint8_t g_eeg_app_mode;

static float dir_rest_mu_ch0 = 0.0f;
static float dir_rest_mu_ch1 = 0.0f;
static float dir_rest_beta_ch0 = 0.0f;
static float dir_rest_beta_ch1 = 0.0f;
static uint16_t dir_rest_count = 0;
uint8_t dir_rest_valid = 0;

static float dir_task_zero_feature[DIR_FEAT_DIM_LOCAL] = {0.0f};
static uint16_t dir_task_zero_count = 0;
static uint8_t dir_task_zero_valid = 0;

static float dir_feature_drift_base[DIR_FEAT_DIM_LOCAL] = {0.0f};
static uint8_t dir_feature_drift_ready = 0;

float Direction_ClampFloat(float x, float min_v, float max_v)
{
    if (x < min_v) return min_v;
    if (x > max_v) return max_v;
    return x;
}

float Direction_SafePower(float p)
{
    if (p < DIR_ERD_POWER_EPS) return DIR_ERD_POWER_EPS;
    return p;
}

float Direction_LogRatio(float a, float b)
{
    float v = logf(Direction_SafePower(a) / Direction_SafePower(b));
    return Direction_ClampFloat(v, -DIR_LOG_FEATURE_CLAMP, DIR_LOG_FEATURE_CLAMP);
}

void Direction_ResetRestBaseline(void)
{
    dir_rest_mu_ch0 = 0.0f;
    dir_rest_mu_ch1 = 0.0f;
    dir_rest_beta_ch0 = 0.0f;
    dir_rest_beta_ch1 = 0.0f;
    dir_rest_count = 0;
    dir_rest_valid = 1;
}

void Direction_ResetTaskZero(void)
{
    for (int i = 0; i < DIR_FEAT_DIM_LOCAL; i++) {
        dir_task_zero_feature[i] = 0.0f;
        dir_feature_drift_base[i] = 0.0f;
    }
    dir_task_zero_count = 0;
    dir_task_zero_valid = 1;
    dir_feature_drift_ready = 0;
}

void Direction_UpdateRestBaseline(void)
{
    dir_rest_count++;
    dir_rest_valid = 1;
}

void Direction_UpdateTaskZero(void)
{
    dir_task_zero_count++;
    dir_task_zero_valid = 1;
}

void Direction_RemoveFeatureSlowDrift(float feature[DIR_FEAT_DIM_LOCAL])
{
    (void)feature;
}

void Direction_BuildRawFeature(float feature[DIR_FEAT_DIM_LOCAL],
                               float theta_pow[NUM_CHANNELS],
                               float alpha_pow[NUM_CHANNELS],
                               float beta_pow[NUM_CHANNELS])
{
    float theta[NUM_CHANNELS];
    float mu[NUM_CHANNELS];
    float beta[NUM_CHANNELS];
    float total[NUM_CHANNELS];

    for (int i = 0; i < NUM_CHANNELS; i++) {
        theta[i] = Direction_SafePower(theta_pow[i]);
        mu[i] = Direction_SafePower(alpha_pow[i]);
        beta[i] = Direction_SafePower(beta_pow[i]);
    }

    for (int i = 0; i < NUM_CHANNELS; i++) {
        total[i] = Direction_SafePower(theta[i] + mu[i] + beta[i]);
    }

    feature[0] = Direction_LogRatio(mu[0],    mu[1])    * DIR_FEATURE_SCALE;
    feature[1] = Direction_LogRatio(beta[0],  beta[1])  * DIR_FEATURE_SCALE;
    feature[2] = Direction_LogRatio(mu[2],    mu[3])    * DIR_FEATURE_SCALE;
    feature[3] = Direction_LogRatio(beta[2],  beta[3])  * DIR_FEATURE_SCALE;
    feature[4] = Direction_LogRatio(theta[0], theta[1]) * DIR_FEATURE_SCALE;
    feature[5] = Direction_LogRatio(theta[2], theta[3]) * DIR_FEATURE_SCALE;
    feature[6] = Direction_LogRatio(total[0], total[1]) * DIR_FEATURE_SCALE;
    feature[7] = Direction_LogRatio(total[2], total[3]) * DIR_FEATURE_SCALE;

    feature[8]  = Direction_LogRatio(mu[0],   total[0]) * DIR_FEATURE_SCALE;
    feature[9]  = Direction_LogRatio(mu[1],   total[1]) * DIR_FEATURE_SCALE;
    feature[10] = Direction_LogRatio(mu[2],   total[2]) * DIR_FEATURE_SCALE;
    feature[11] = Direction_LogRatio(mu[3],   total[3]) * DIR_FEATURE_SCALE;
    feature[12] = Direction_LogRatio(beta[0], total[0]) * DIR_FEATURE_SCALE;
    feature[13] = Direction_LogRatio(beta[1], total[1]) * DIR_FEATURE_SCALE;
    feature[14] = Direction_LogRatio(beta[2], total[2]) * DIR_FEATURE_SCALE;
    feature[15] = Direction_LogRatio(beta[3], total[3]) * DIR_FEATURE_SCALE;
    feature[16] = Direction_LogRatio(beta[0], mu[0])    * DIR_FEATURE_SCALE;
    feature[17] = Direction_LogRatio(beta[1], mu[1])    * DIR_FEATURE_SCALE;
    feature[18] = Direction_LogRatio(beta[2], mu[2])    * DIR_FEATURE_SCALE;
    feature[19] = Direction_LogRatio(beta[3], mu[3])    * DIR_FEATURE_SCALE;

    feature[20] = Direction_LogRatio(mu[0],   mu[2])    * DIR_FEATURE_SCALE;
    feature[21] = Direction_LogRatio(mu[1],   mu[3])    * DIR_FEATURE_SCALE;
    feature[22] = Direction_LogRatio(beta[0], beta[2])  * DIR_FEATURE_SCALE;
    feature[23] = Direction_LogRatio(beta[1], beta[3])  * DIR_FEATURE_SCALE;
}

void Direction_BuildFeature(float feature[DIR_FEAT_DIM_LOCAL],
                            float theta_pow[NUM_CHANNELS],
                            float alpha_pow[NUM_CHANNELS],
                            float beta_pow[NUM_CHANNELS])
{
    Direction_BuildRawFeature(feature, theta_pow, alpha_pow, beta_pow);
    Direction_RemoveFeatureSlowDrift(feature);
}

void Direction_PrintFeatureHeader(void)
{
    if (g_eeg_app_mode != EEG_APP_MODE_COLLECT) {
        return;
    }
#if DIR_PRINT_HEADER_ON_START
    Serial_Printf(DIR_TEXT_PORT, "DIRCSV,label,...\r\n");
    Serial_Printf(DIR_TEXT_PORT, "DIRCFG,...\r\n");
    Serial_Printf(DIR_TEXT_PORT, "TASK=REST,prepare,then left hand\r\n");
#endif
}

void Direction_PrintFeatureCSV(uint8_t label,
                               float theta_pow[NUM_CHANNELS],
                               float alpha_pow[NUM_CHANNELS],
                               float beta_pow[NUM_CHANNELS])
{
    float feature[DIR_FEAT_DIM_LOCAL];
    char line[512];
    int len = 0;

    if (!dir_rest_valid) {
        return;
    }

    Direction_BuildFeature(feature, theta_pow, alpha_pow, beta_pow);

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