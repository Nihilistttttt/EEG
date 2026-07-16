#ifndef __EEG_DIRECTION_FEATURE_H
#define __EEG_DIRECTION_FEATURE_H

#include <stdint.h>

#define DIR_FEAT_DIM_LOCAL          24
#define DIR_ERD_POWER_EPS           1.0e-18f
#define DIR_LOG_FEATURE_CLAMP       6.0f
#define DIR_FEATURE_SCALE           10000.0f

#ifndef NUM_CHANNELS
#define NUM_CHANNELS                8
#endif

float Direction_ClampFloat(float x, float min_v, float max_v);
float Direction_SafePower(float p);
float Direction_LogRatio(float a, float b);

void Direction_BuildRawFeature(float feature[DIR_FEAT_DIM_LOCAL],
                               float theta_pow[NUM_CHANNELS],
                               float alpha_pow[NUM_CHANNELS],
                               float beta_pow[NUM_CHANNELS]);
void Direction_BuildFeature(float feature[DIR_FEAT_DIM_LOCAL],
                            float theta_pow[NUM_CHANNELS],
                            float alpha_pow[NUM_CHANNELS],
                            float beta_pow[NUM_CHANNELS]);

void Direction_ResetRestBaseline(void);
void Direction_UpdateRestBaseline(void);
void Direction_ResetTaskZero(void);
void Direction_UpdateTaskZero(void);


void Direction_PrintFeatureHeader(void);
void Direction_PrintFeatureCSV(uint8_t label,
                               float theta_pow[NUM_CHANNELS],
                               float alpha_pow[NUM_CHANNELS],
                               float beta_pow[NUM_CHANNELS]);

extern uint8_t dir_rest_valid;

#endif