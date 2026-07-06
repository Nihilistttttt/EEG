#include "eeg_fft.h"
#include "signal_analysis.h"
#include "eeg_direction_collect.h"
#include "eeg_ab_extract.h"
#include "FFT_Real.h"
#include "malloc.h"
#include "Message_Parser.h"
#include "OLED.h"
#include "Serial.h"
#include "dualcore_ipc.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

extern uint8_t g_eeg_app_mode;

#define DIR_DEBUG_POWER_PRINT       0
#define DIR_1S_TEST_TEXT_ONLY       0
#define DIR_DECISION_ROWS           4
#define DIR_RESULT_DT_MS            2000

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

static const float DC_Window[3] = {
    0.0000000000f, 0.2500000000f, 0.7500000000f};

static const float Notch_Window[5] = {
    0.7500000000f, 0.2500000000f, 0.0000000000f, 0.2500000000f, 0.7500000000f};

static const float HF_Window[7] = {
    1.0000000000f, 0.9330127239f, 0.7500000000f, 0.5000000000f,
    0.2500000000f, 0.0669872984f, 0.0000000000f};

const float eps = 1e-6f;

static BlinkStatistics_t dummy_blink_stats = {0};

static uint8_t current_send_frag;
static uint8_t current_filt_send_frag;
static uint8_t raw_send_channel;
static uint8_t filt_send_channel;
static uint8_t proc_channel;
static uint8_t fft_pair_index;

static FFT_Data_t *p_fft_data;
static FFT_ProcState fft_next_state;
static FFT_ProcState fft_step = FFT_STEP_FINISH;
static uint8_t fft_src_type;
static uint16_t fft_src_start;
static FFT_ProcState target_state_after_buffer;
static int8_t buffer_counter = 0;

float theta_pow_ch0, alpha_pow_ch0, beta_pow_ch0;
float theta_pow_ch1, alpha_pow_ch1, beta_pow_ch1;
float theta_pow_ch2, alpha_pow_ch2, beta_pow_ch2;
float theta_pow_ch3, alpha_pow_ch3, beta_pow_ch3;
float cur_attn0, cur_attn1;
float cur_relax0, cur_relax1;
float cur_blink0, cur_blink1;
uint8_t instant_state, trend_state;

static AttnHistory_t attn_hist[2];
static AttnHistory_t relax_hist[2];
static AttnHistory_t blink_hist[2];
AttnEma_t attn_ema[2];
AttnEma_t relax_ema[2];
AttnEma_t blink_ema[2];

float sum_win_sq = 0.0f;
float norm_factor;

RingBuffer_t RingBuf;
RingBuffer_t RingBufFiltered;

static TrendWindow_t TrendWin = {.CH0 = {0}, .CH1 = {0}, .Idx = 0, .Count = 0};
FFT_Data_t FFT_Data;
FFT_Data_t FFT_DataFiltered;

static FFT_ConfigHandle Cplx_FFT_Cfg;
static FFT_Complex_t Cplx_Input[FFT_SIZE];
static FFT_Complex_t Cplx_Output[FFT_SIZE];

BandIndices_t BandIdx;

int New_Samples_Count = 0;
static uint8_t Wave_Buf[128] = {32};

static uint32_t s_last_infer_cnt = 0;
static uint8_t s_infer_row_tick = 0;
static uint32_t s_result_window = 0;

float *RingBuffer_GetChannel(RingBuffer_t *rb, uint8_t ch)
{
    switch (ch) {
    case 0: return rb->CH0;
    case 1: return rb->CH1;
    case 2: return rb->CH2;
    case 3: return rb->CH3;
    default: return rb->CH0;
    }
}

float *FFT_Data_GetMags(FFT_Data_t *data, uint8_t ch)
{
    switch (ch) {
    case 0: return data->CH0Mags;
    case 1: return data->CH1Mags;
    case 2: return data->CH2Mags;
    case 3: return data->CH3Mags;
    default: return data->CH0Mags;
    }
}

static void Compute_Band_Indices(float f_low, float f_high, float bin_width,
                                  int *out_Start, int *out_End)
{
    int start = (int)(f_low / bin_width + 0.5f);
    int end = (int)(f_high / bin_width + 0.5f);
    if (start < 0)
        start = 0;
    if (end >= (FFT_SIZE / 2 + 1))
        end = FFT_SIZE / 2;
    if (start > end)
        start = end;
    *out_Start = start;
    *out_End = end;
}

static void Apply_Freq_Filter(float *mags)
{
    for (int i = 0; i < 3; i++) {
        mags[i] *= DC_Window[i];
    }
    for (int i = 49, j = 0; i <= 53; i++, j++) {
        mags[i] *= Notch_Window[j];
    }
    for (int i = 62, j = 0; i <= 68; i++, j++) {
        mags[i] *= HF_Window[j];
    }
    for (int i = 69; i < (FFT_SIZE / 2 + 1); i++) {
        mags[i] = 0.0f;
    }
}

static inline uint8_t Map_To_Y(float value)
{
    float norm = value / WAVE_FIXED_MAX;
    int16_t y = 32 - (int16_t)(norm * 31.0f);
    if (y < 0)
        y = 0;
    if (y > 63)
        y = 63;
    return (uint8_t)y;
}

static void Update_OLED_Scores(float attn0, float relax0, float blink0, float attn1, float relax1, float blink1)
{
    (void)attn0; (void)relax0; (void)blink0;
    (void)attn1; (void)relax1; (void)blink1;
}

static void Show_Trend_State(uint8_t Trend_State)
{
    if (Trend_State == STATE_RELAX) {
        GPIO_ResetBits(GPIOB, GPIO_Pin_0);
        GPIO_SetBits(GPIOB, GPIO_Pin_1);
        GPIO_SetBits(GPIOB, GPIO_Pin_2);
    } else if (Trend_State == STATE_NEUTRAL) {
        GPIO_ResetBits(GPIOB, GPIO_Pin_1);
        GPIO_SetBits(GPIOB, GPIO_Pin_0);
        GPIO_SetBits(GPIOB, GPIO_Pin_2);
    } else if (Trend_State == STATE_FOCUS) {
        GPIO_ResetBits(GPIOB, GPIO_Pin_2);
        GPIO_SetBits(GPIOB, GPIO_Pin_1);
        GPIO_SetBits(GPIOB, GPIO_Pin_0);
    }
}

static uint8_t Judge_Attn_State(float v0, float v1, float ema0, float ema1, float eps)
{
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

void EEG_FFT_Init(void)
{
    Cplx_FFT_Cfg = FFT_Alloc(FFT_SIZE, FFT_FORWARD, NULL, NULL);
    if (Cplx_FFT_Cfg == NULL)
        while (1);

    const float bin_width = SAMPLE_RATE / FFT_SIZE;
    Compute_Band_Indices(4.0f, 8.0f, bin_width, &BandIdx.ThetaStart, &BandIdx.ThetaEnd);
    Compute_Band_Indices(8.0f, 13.0f, bin_width, &BandIdx.AlphaStart, &BandIdx.AlphaEnd);
    Compute_Band_Indices(13.0f, 30.0f, bin_width, &BandIdx.BetaStart, &BandIdx.BetaEnd);

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
}

uint8_t Process_FFT_Step(void)
{
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

    case FFT_STEP_FFT_COPY_INPUT: {
        RingBuffer_t *src_buf = (fft_src_type == 0) ? &RingBuf : &RingBufFiltered;
        uint8_t ch_a = (uint8_t)(fft_pair_index * 2U);
        uint8_t ch_b = (uint8_t)(ch_a + 1U);
        float *ch_a_src = RingBuffer_GetChannel(src_buf, ch_a);
        float *ch_b_src = RingBuffer_GetChannel(src_buf, ch_b);
        uint16_t start = fft_src_start;

        float ch_a_mean, ch_a_slope;
        float ch_b_mean, ch_b_slope;
        EEG_CalcMeanAndSlope(ch_a_src, start, &ch_a_mean, &ch_a_slope);
        EEG_CalcMeanAndSlope(ch_b_src, start, &ch_b_mean, &ch_b_slope);

        for (int j = 0; j < FFT_SIZE; j++) {
            uint16_t idx = (start + j) & (FFT_SIZE - 1);
            float w = Hanning_Window[j];
            float xa = EEG_RemoveMeanAndLinearTrend(ch_a_src[idx], ch_a_mean, ch_a_slope, j);
            float xb = EEG_RemoveMeanAndLinearTrend(ch_b_src[idx], ch_b_mean, ch_b_slope, j);
            Cplx_Input[j].r = xa * w;
            Cplx_Input[j].i = xb * w;
        }
        fft_step = FFT_STEP_FFT_TRANSFORM;
        break;
    }

    case FFT_STEP_FFT_TRANSFORM:
        FFT_Transform(Cplx_FFT_Cfg, Cplx_Input, Cplx_Output);
        fft_step = FFT_STEP_FFT_MAGS;
        break;

    case FFT_STEP_FFT_MAGS: {
        uint8_t ch_a = (uint8_t)(fft_pair_index * 2U);
        uint8_t ch_b = (uint8_t)(ch_a + 1U);
        float *ch_a_mags = FFT_Data_GetMags(p_fft_data, ch_a);
        float *ch_b_mags = FFT_Data_GetMags(p_fft_data, ch_b);

        ch_a_mags[0] = fabsf(Cplx_Output[0].r);
        ch_b_mags[0] = fabsf(Cplx_Output[0].i);
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

            ch_a_mags[k] = sqrtf(Xr * Xr + Xi * Xi);
            ch_b_mags[k] = sqrtf(Yr * Yr + Yi * Yi);
        }
        ch_a_mags[128] = fabsf(Cplx_Output[128].r);
        ch_b_mags[128] = fabsf(Cplx_Output[128].i);

        fft_pair_index++;
        if (fft_pair_index < (NUM_CHANNELS / 2U)) {
            fft_step = FFT_STEP_FFT_COPY_INPUT;
        } else {
            fft_step = fft_next_state;
        }
        break;
    }

    case FFT_STEP_FREQ_FILTER:
        for (uint8_t ch = 0; ch < NUM_CHANNELS; ch++) {
            Apply_Freq_Filter(FFT_Data_GetMags(&FFT_Data, ch));
        }
        proc_channel = 0;
        target_state_after_buffer = FFT_STEP_SEND_FREQ_SPECTRUM;
        raw_send_channel = 0;
        current_send_frag = 0;
        buffer_counter = 0;
        fft_step = FFT_STEP_SEND_BUFFER;
        break;

    case FFT_STEP_SEND_FREQ_SPECTRUM: {
        float *mags = (raw_send_channel == 0) ? FFT_Data.CH2Mags : FFT_Data.CH3Mags;
        CmdType cmd = (raw_send_channel == 0) ? CMD_FREQ_SPECTRUM_CH0 : CMD_FREQ_SPECTRUM_CH1;
        Send_Spectrum(cmd, mags, current_send_frag);
        current_send_frag++;
        if (current_send_frag >= SPECTRUM_TOTAL_FRAGS) {
            current_send_frag = 0;
            if (raw_send_channel == 0) {
                raw_send_channel = 1;
            } else {
                raw_send_channel = 0;
                fft_step = FFT_STEP_EXTRACT_FILT_FRAME;
                proc_channel = 0;
            }
        }
        break;
    }

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

    case FFT_STEP_BAND_POWER: {
        BandPowers_t bp0 = compute_band_powers(FFT_Data.CH0Mags, FFT_SIZE,
                                                BandIdx.ThetaStart, BandIdx.ThetaEnd,
                                                BandIdx.AlphaStart, BandIdx.AlphaEnd,
                                                BandIdx.BetaStart, BandIdx.BetaEnd,
                                                norm_factor, SAMPLE_RATE);
        BandPowers_t bp1 = compute_band_powers(FFT_Data.CH1Mags, FFT_SIZE,
                                                BandIdx.ThetaStart, BandIdx.ThetaEnd,
                                                BandIdx.AlphaStart, BandIdx.AlphaEnd,
                                                BandIdx.BetaStart, BandIdx.BetaEnd,
                                                norm_factor, SAMPLE_RATE);
        BandPowers_t bp2 = compute_band_powers(FFT_Data.CH2Mags, FFT_SIZE,
                                                BandIdx.ThetaStart, BandIdx.ThetaEnd,
                                                BandIdx.AlphaStart, BandIdx.AlphaEnd,
                                                BandIdx.BetaStart, BandIdx.BetaEnd,
                                                norm_factor, SAMPLE_RATE);
        BandPowers_t bp3 = compute_band_powers(FFT_Data.CH3Mags, FFT_SIZE,
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

    case FFT_STEP_ATTENTION: {
        cur_attn0  = compute_attention_feature(theta_pow_ch2, alpha_pow_ch2, beta_pow_ch2);
        cur_attn1  = compute_attention_feature(theta_pow_ch3, alpha_pow_ch3, beta_pow_ch3);
        cur_relax0 = compute_relaxation_feature(theta_pow_ch2, alpha_pow_ch2, beta_pow_ch2);
        cur_relax1 = compute_relaxation_feature(theta_pow_ch3, alpha_pow_ch3, beta_pow_ch3);
        cur_blink0 = compute_blink_feature(dummy_blink_stats.blink_rate, dummy_blink_stats.slow_blink_ratio);
        cur_blink1 = cur_blink0;

        float theta_arr[NUM_CHANNELS] = {theta_pow_ch0, theta_pow_ch1, theta_pow_ch2, theta_pow_ch3};
        float alpha_arr[NUM_CHANNELS] = {alpha_pow_ch0, alpha_pow_ch1, alpha_pow_ch2, alpha_pow_ch3};
        float beta_arr[NUM_CHANNELS]  = {beta_pow_ch0,  beta_pow_ch1,  beta_pow_ch2,  beta_pow_ch3};

        AB_PrintAlphaBetaCSV(theta_arr, alpha_arr, beta_arr);

        if (g_eeg_app_mode == EEG_APP_MODE_COLLECT) {
            Direction_AutoCollectProcess(theta_arr, alpha_arr, beta_arr);
        } else if (g_eeg_app_mode == EEG_APP_MODE_COLLECT_CSP) {
            Direction_AutoCollectCSPProcess(&RingBufFiltered);
        } else if (g_eeg_app_mode == EEG_APP_MODE_INFER) {
            uint32_t v5f_pred = DualCore_IPC_GetLastV5FPred();
            uint32_t v5f_infer_valid = DualCore_IPC_GetLastV5FInferValid();
            int32_t v5f_score_l = DualCore_IPC_GetLastV5FScoreLeft();
            int32_t v5f_score_r = DualCore_IPC_GetLastV5FScoreRight();
            int32_t v5f_conf = DualCore_IPC_GetLastV5FConfidence();
            uint32_t v5f_trained = DualCore_IPC_GetLastV5FModelTrained();
            uint32_t v5f_infer_cnt = DualCore_IPC_GetLastV5FInferCount();

            if (v5f_infer_valid && v5f_infer_cnt != s_last_infer_cnt) {
                s_last_infer_cnt = v5f_infer_cnt;
                s_infer_row_tick++;
                if (s_infer_row_tick >= DIR_DECISION_ROWS) {
                    s_infer_row_tick = 0;
                    const char *pred_str = (v5f_pred == 0u) ? "LEFT" : ((v5f_pred == 1u) ? "RIGHT" : "UNKNOWN");
                    Serial_Printf(DIR_TEXT_PORT,
                                  "RESULT,src=V5F,window=%lu,dt_ms=%u,win_rows=%u,INTENT=%s,S_LEFT=%ld,S_RIGHT=%ld,CONF=%ld,trained=%d\r\n",
                                  (unsigned long)s_result_window,
                                  (unsigned int)DIR_RESULT_DT_MS,
                                  (unsigned int)DIR_DECISION_ROWS,
                                  pred_str,
                                  (long)v5f_score_l,
                                  (long)v5f_score_r,
                                  (long)v5f_conf,
                                  (int)v5f_trained);
                    s_result_window++;
                }
            }
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
    }

    case FFT_STEP_NORMALIZE: {
        float a0 = normalize_attention(cur_attn0, &attn_hist[0]);
        float a1 = normalize_attention(cur_attn1, &attn_hist[1]);
        float r0 = normalize_attention(cur_relax0, &relax_hist[0]);
        float r1 = normalize_attention(cur_relax1, &relax_hist[1]);
        float b0 = 100.0f - normalize_attention(cur_blink0, &blink_hist[0]);
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
        float *mags = (filt_send_channel == 0) ? FFT_DataFiltered.CH2Mags : FFT_DataFiltered.CH3Mags;
        CmdType cmd = (filt_send_channel == 0) ? CMD_FILT_SPECTRUM_CH0 : CMD_FILT_SPECTRUM_CH1;
        Send_Spectrum(cmd, mags, current_filt_send_frag);
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

    case FFT_STEP_SEND_BUFFER:
        buffer_counter++;
        if (buffer_counter >= 10) {
            fft_step = target_state_after_buffer;
        } else {
            fft_step = FFT_STEP_SEND_BUFFER;
        }
        break;

    case FFT_STEP_SEND_FOCUS:
        Send_Focus(attn_ema[0].Value, attn_ema[1].Value, attn_ema[0].Value, attn_ema[1].Value, trend_state, instant_state);
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

void Process_FFT(void)
{
    fft_step = FFT_STEP_START;
}

void Update_Waveform(float new_val)
{
    uint8_t y = Map_To_Y(new_val);
    for (int j = 0; j < 127; j++) Wave_Buf[j] = Wave_Buf[j + 1];
    Wave_Buf[127] = y;
}

void EEG_FFT_ResetInferState(void)
{
    s_last_infer_cnt = 0;
    s_infer_row_tick = 0;
    s_result_window = 0;
}