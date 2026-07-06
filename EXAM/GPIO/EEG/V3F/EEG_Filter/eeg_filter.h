#ifndef __EEG_FILTER_H
#define __EEG_FILTER_H

#include <stdint.h>

#define MAX_SOS_SECTIONS 8
#define EEG_FILTER_FFT_SIZE 256

typedef struct {
    float b0, b1, b2, a1, a2, w1, w2;
} IIR_Biquad_t;

typedef struct {
    IIR_Biquad_t Sec[MAX_SOS_SECTIONS];
    uint8_t NumSections;
} IIR_SOS_t;

typedef struct {
    float drift_base;
    uint8_t init;
} EEG_DriftRemove_t;

float IIR_Step(float input, IIR_Biquad_t *filt);
float IIR_SOS_Step(float input, IIR_SOS_t *filt);

float EEG_RemoveRealtimeDrift(float x, EEG_DriftRemove_t *st);
float EEG_RemoveMeanAndLinearTrend(float sample, float mean, float slope, int n);
void EEG_CalcMeanAndSlope(float *src, uint16_t start, float *out_mean, float *out_slope);

void Waveform_RemoveDisplayBaseline(float in_ch0, float in_ch1, float *out_ch0, float *out_ch1);

extern IIR_SOS_t Bandpass_CH0, Bandpass_CH1, Bandpass_CH2, Bandpass_CH3;
extern IIR_SOS_t Notch_CH0, Notch_CH1, Notch_CH2, Notch_CH3;
extern EEG_DriftRemove_t AB_Drift_CH0, AB_Drift_CH1, AB_Drift_CH2, AB_Drift_CH3;

#endif