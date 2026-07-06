#ifndef __EEG_DIRECTION_INFER_H
#define __EEG_DIRECTION_INFER_H

#include <stdint.h>
#include "eeg_direction_feature.h"

#define DIR_DECISION_ROWS           4
#define DIR_RESULT_EVERY_ROWS       4
#define DIR_RESULT_DT_MS            2000

void Direction_InferAndPrint(float theta_pow[NUM_CHANNELS],
                             float alpha_pow[NUM_CHANNELS],
                             float beta_pow[NUM_CHANNELS]);
void Direction_Infer1sUpdateAndPrint(float theta_pow[NUM_CHANNELS],
                                     float alpha_pow[NUM_CHANNELS],
                                     float beta_pow[NUM_CHANNELS]);
void Direction_Infer1sReset(void);

#endif