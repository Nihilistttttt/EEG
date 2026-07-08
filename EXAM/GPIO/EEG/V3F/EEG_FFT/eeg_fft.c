#include "eeg_fft.h"
#include "signal_analysis.h"
#include "eeg_direction_collect.h"
#include "eeg_direction_infer.h"
#include "eeg_ab_extract.h"
#include "eeg_cmd_parser.h"
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
extern uint8_t g_paused;
extern WorkMode_t g_work_mode;

#define DIR_DEBUG_POWER_PRINT       0
#define DIR_1S_TEST_TEXT_ONLY       0
#define DIR_DECISION_ROWS           4
#define DIR_RESULT_DT_MS            2000

#define ARTIFACT_THRESHOLD_UV       200.0f
#define BAD_WINDOW_THRESHOLD        0.3f
#define BAD_WINDOW_LIMIT            5

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

float delta_pow_ch0, theta_pow_ch0, alpha_pow_ch0, beta_pow_ch0;
float delta_pow_ch1, theta_pow_ch1, alpha_pow_ch1, beta_pow_ch1;
float delta_pow_ch2, theta_pow_ch2, alpha_pow_ch2, beta_pow_ch2;
float delta_pow_ch3, theta_pow_ch3, alpha_pow_ch3, beta_pow_ch3;

static AttentionEngine_t g_attn_engine;
static AttentionOutput_t g_attn_output;
static float g_raw_attn_scores[2];
static float g_raw_relax_scores[2];

uint8_t instant_state, trend_state;

AttnEma_t attn_ema[2];
AttnEma_t relax_ema[2];

float sum_win_sq = 0.0f;
float norm_factor;

RingBuffer_t RingBuf;
RingBuffer_t RingBufFiltered;

static TrendWindow_t TrendWin = {.CH0 = {0}, .CH1 = {0}, .Idx = 0, .Count = 0};
FFT_Data_t FFT_Data;
FFT_Data_t FFT_DataFiltered;

static FFT_ConfigHandle Cplx_FFT_Cfg;
static uint8_t s_fft_mem_pool[4096] __attribute__((aligned(4)));
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
    int i;
    for (i = 0; i < 3; i++) {
        mags[i] *= DC_Window[i];
    }
    for (i = 49; i <= 53; i++) {
        mags[i] *= Notch_Window[i - 49];
    }
    for (i = 62; i <= 68; i++) {
        mags[i] *= HF_Window[i - 62];
    }
    for (i = 69; i < (FFT_SIZE / 2 + 1); i++) {
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

static uint8_t Judge_Attn_State(float attn_score, float relax_score, float confidence)
{
    if (confidence < 0.15f)
        return STATE_NEUTRAL;

    if (attn_score > 60.0f && attn_score > relax_score)
        return STATE_FOCUS;
    if (relax_score > 60.0f && relax_score > attn_score)
        return STATE_RELAX;

    if (attn_score > 55.0f && attn_score > relax_score + 5.0f)
        return STATE_FOCUS;
    if (relax_score > 55.0f && relax_score > attn_score + 5.0f)
        return STATE_RELAX;

    return STATE_NEUTRAL;
}

void EEG_FFT_Init(void)
{
    size_t fft_mem_needed = 0;
    FFT_Alloc(FFT_SIZE, FFT_FORWARD, NULL, &fft_mem_needed);
    if (fft_mem_needed > sizeof(s_fft_mem_pool))
        while (1);
    Cplx_FFT_Cfg = FFT_Alloc(FFT_SIZE, FFT_FORWARD, s_fft_mem_pool, &fft_mem_needed);
    if (Cplx_FFT_Cfg == NULL)
        while (1);

    const float bin_width = SAMPLE_RATE / FFT_SIZE;
    Compute_Band_Indices(1.0f, 4.0f, bin_width, &BandIdx.DeltaStart, &BandIdx.DeltaEnd);
    Compute_Band_Indices(4.0f, 8.0f, bin_width, &BandIdx.ThetaStart, &BandIdx.ThetaEnd);
    Compute_Band_Indices(8.0f, 13.0f, bin_width, &BandIdx.AlphaStart, &BandIdx.AlphaEnd);
    Compute_Band_Indices(13.0f, 30.0f, bin_width, &BandIdx.BetaStart, &BandIdx.BetaEnd);

    int i;
    for (i = 0; i < FFT_SIZE; i++) {
        sum_win_sq += Hanning_Window[i] * Hanning_Window[i];
    }
    norm_factor = SAMPLE_RATE * sum_win_sq;

    memset(attn_ema, 0, sizeof(attn_ema));
    memset(relax_ema, 0, sizeof(relax_ema));

    attention_engine_init(&g_attn_engine, NULL);
    memset(&g_attn_output, 0, sizeof(g_attn_output));
    g_raw_attn_scores[0] = 50.0f;
    g_raw_attn_scores[1] = 50.0f;
    g_raw_relax_scores[0] = 50.0f;
    g_raw_relax_scores[1] = 50.0f;
}

static float compute_artifact_from_ringbuf(RingBuffer_t *rb, uint16_t start_idx)
{
    int count = 0;
    int i;
    for (i = 0; i < FFT_SIZE; i++) {
        uint16_t idx = (start_idx + i) & (FFT_SIZE - 1);
        float v0 = fabsf(rb->CH0[idx]);
        float v1 = fabsf(rb->CH1[idx]);
        if (v0 > ARTIFACT_THRESHOLD_UV || v1 > ARTIFACT_THRESHOLD_UV) {
            count++;
        }
    }
    return (float)count / (float)FFT_SIZE;
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

        int j;
        for (j = 0; j < FFT_SIZE; j++) {
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
        int k;
        for (k = 1; k < 128; k++) {
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

    case FFT_STEP_FREQ_FILTER: {
        uint8_t ch;
        for (ch = 0; ch < NUM_CHANNELS; ch++) {
            Apply_Freq_Filter(FFT_Data_GetMags(&FFT_Data, ch));
        }
        proc_channel = 0;
        target_state_after_buffer = FFT_STEP_SEND_FREQ_SPECTRUM;
        raw_send_channel = 0;
        current_send_frag = 0;
        buffer_counter = 0;
        fft_step = FFT_STEP_SEND_BUFFER;
        break;
    }

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
                                                BandIdx.DeltaStart, BandIdx.DeltaEnd,
                                                BandIdx.ThetaStart, BandIdx.ThetaEnd,
                                                BandIdx.AlphaStart, BandIdx.AlphaEnd,
                                                BandIdx.BetaStart, BandIdx.BetaEnd,
                                                norm_factor, SAMPLE_RATE);
        BandPowers_t bp1 = compute_band_powers(FFT_Data.CH1Mags, FFT_SIZE,
                                                BandIdx.DeltaStart, BandIdx.DeltaEnd,
                                                BandIdx.ThetaStart, BandIdx.ThetaEnd,
                                                BandIdx.AlphaStart, BandIdx.AlphaEnd,
                                                BandIdx.BetaStart, BandIdx.BetaEnd,
                                                norm_factor, SAMPLE_RATE);
        BandPowers_t bp2 = compute_band_powers(FFT_Data.CH2Mags, FFT_SIZE,
                                                BandIdx.DeltaStart, BandIdx.DeltaEnd,
                                                BandIdx.ThetaStart, BandIdx.ThetaEnd,
                                                BandIdx.AlphaStart, BandIdx.AlphaEnd,
                                                BandIdx.BetaStart, BandIdx.BetaEnd,
                                                norm_factor, SAMPLE_RATE);
        BandPowers_t bp3 = compute_band_powers(FFT_Data.CH3Mags, FFT_SIZE,
                                                BandIdx.DeltaStart, BandIdx.DeltaEnd,
                                                BandIdx.ThetaStart, BandIdx.ThetaEnd,
                                                BandIdx.AlphaStart, BandIdx.AlphaEnd,
                                                BandIdx.BetaStart, BandIdx.BetaEnd,
                                                norm_factor, SAMPLE_RATE);

        delta_pow_ch0 = bp0.delta_power; theta_pow_ch0 = bp0.theta_power; alpha_pow_ch0 = bp0.alpha_power; beta_pow_ch0 = bp0.beta_power;
        delta_pow_ch1 = bp1.delta_power; theta_pow_ch1 = bp1.theta_power; alpha_pow_ch1 = bp1.alpha_power; beta_pow_ch1 = bp1.beta_power;
        delta_pow_ch2 = bp2.delta_power; theta_pow_ch2 = bp2.theta_power; alpha_pow_ch2 = bp2.alpha_power; beta_pow_ch2 = bp2.beta_power;
        delta_pow_ch3 = bp3.delta_power; theta_pow_ch3 = bp3.theta_power; alpha_pow_ch3 = bp3.alpha_power; beta_pow_ch3 = bp3.beta_power;

        fft_step = FFT_STEP_ATTENTION;
        break;
    }

    case FFT_STEP_ATTENTION: {
        float artifact_ratio = compute_artifact_from_ringbuf(&RingBuf, fft_src_start);

        {
            static uint32_t s_mode_diag_count = 0;
            s_mode_diag_count++;
            if (s_mode_diag_count >= 500u) {
                s_mode_diag_count = 0;
                Serial_Printf(SERIAL_PORT_DEBUG,
                    "MODE_DIAG,app=%u,work=%u,paused=%u\r\n",
                    (unsigned)g_eeg_app_mode,
                    (unsigned)g_work_mode,
                    (unsigned)g_paused);
                Serial_Printf(SERIAL_PORT_WIFI,
                    "MODE_DIAG,app=%u,work=%u,paused=%u\r\n",
                    (unsigned)g_eeg_app_mode,
                    (unsigned)g_work_mode,
                    (unsigned)g_paused);
            }
        }

        BandPowers_t fused_powers;
        fused_powers.delta_power  = 0.4f * (delta_pow_ch0 + delta_pow_ch1) +
                                    0.6f * (delta_pow_ch2 + delta_pow_ch3);
        fused_powers.theta_power = 0.4f * (theta_pow_ch0 + theta_pow_ch1) +
                                   0.6f * (theta_pow_ch2 + theta_pow_ch3);
        fused_powers.alpha_power = 0.4f * (alpha_pow_ch0 + alpha_pow_ch1) +
                                   0.6f * (alpha_pow_ch2 + alpha_pow_ch3);
        fused_powers.beta_power  = 0.4f * (beta_pow_ch0 + beta_pow_ch1) +
                                   0.6f * (beta_pow_ch2 + beta_pow_ch3);

        g_attn_output = attention_engine_process(&g_attn_engine,
                                                  &fused_powers,
                                                  artifact_ratio,
                                                  &dummy_blink_stats,
                                                  BAD_WINDOW_THRESHOLD,
                                                  BAD_WINDOW_LIMIT,
                                                  0.0f);

        g_raw_attn_scores[0] = g_raw_attn_scores[1] = g_attn_output.attention_score;
        g_raw_relax_scores[0] = g_raw_relax_scores[1] = g_attn_output.relaxation_score;

        attn_ema[0].Value = attn_ema[1].Value = g_attn_output.attention_score;
        relax_ema[0].Value = relax_ema[1].Value = g_attn_output.relaxation_score;

        float theta_arr[NUM_CHANNELS] = {theta_pow_ch0, theta_pow_ch1, theta_pow_ch2, theta_pow_ch3};
        float alpha_arr[NUM_CHANNELS] = {alpha_pow_ch0, alpha_pow_ch1, alpha_pow_ch2, alpha_pow_ch3};
        float beta_arr[NUM_CHANNELS]  = {beta_pow_ch0,  beta_pow_ch1,  beta_pow_ch2,  beta_pow_ch3};

        AB_PrintAlphaBetaCSV(theta_arr, alpha_arr, beta_arr);

        {
            static uint32_t s_v5f_raw_diag_count = 0;
            s_v5f_raw_diag_count++;
            if (s_v5f_raw_diag_count >= 500u) {
                s_v5f_raw_diag_count = 0;
                uint32_t raw_valid = DualCore_IPC_GetLastV5FInferValid();
                uint32_t raw_cnt = DualCore_IPC_GetLastV5FInferCount();
                uint32_t raw_fft = DualCore_IPC_GetLastV5FFFTCount();
                uint32_t raw_fv = DualCore_IPC_GetLastV5FFeatureValid();
                Serial_Printf(SERIAL_PORT_DEBUG,
                    "V5F_RAW,app=%u,valid=%lu,cnt=%lu,fft=%lu,fv=%lu,pred=%lu\r\n",
                    (unsigned)g_eeg_app_mode,
                    (unsigned long)raw_valid,
                    (unsigned long)raw_cnt,
                    (unsigned long)raw_fft,
                    (unsigned long)raw_fv,
                    (unsigned long)DualCore_IPC_GetLastV5FPred());
                Serial_Printf(SERIAL_PORT_WIFI,
                    "V5F_RAW,app=%u,valid=%lu,cnt=%lu,fft=%lu,fv=%lu,pred=%lu\r\n",
                    (unsigned)g_eeg_app_mode,
                    (unsigned long)raw_valid,
                    (unsigned long)raw_cnt,
                    (unsigned long)raw_fft,
                    (unsigned long)raw_fv,
                    (unsigned long)DualCore_IPC_GetLastV5FPred());
            }
        }

        if (g_eeg_app_mode == EEG_APP_MODE_COLLECT) {
            Direction_AutoCollectProcess(theta_arr, alpha_arr, beta_arr);
        } else if (g_eeg_app_mode == EEG_APP_MODE_COLLECT_CSP) {
            Direction_AutoCollectCSPProcess(&RingBufFiltered);
        } else if (g_eeg_app_mode == EEG_APP_MODE_INFER) {
            if (!g_paused) {
                uint32_t v5f_pred = DualCore_IPC_GetLastV5FPred();
                uint32_t v5f_infer_valid = DualCore_IPC_GetLastV5FInferValid();
                int32_t v5f_score_l = DualCore_IPC_GetLastV5FScoreLeft();
                int32_t v5f_score_r = DualCore_IPC_GetLastV5FScoreRight();
                int32_t v5f_conf = DualCore_IPC_GetLastV5FConfidence();
                uint32_t v5f_trained = DualCore_IPC_GetLastV5FModelTrained();
                uint32_t v5f_infer_cnt = DualCore_IPC_GetLastV5FInferCount();

                {
                    static uint32_t s_v5f_diag_count = 0;
                    s_v5f_diag_count++;
                    if (s_v5f_diag_count >= 250u) {
                        s_v5f_diag_count = 0;
                        Serial_Printf(SERIAL_PORT_DEBUG,
                            "V5F_DIAG,valid=%lu,cnt=%lu,last_cnt=%lu,trained=%lu,pred=%lu\r\n",
                            (unsigned long)v5f_infer_valid,
                            (unsigned long)v5f_infer_cnt,
                            (unsigned long)s_last_infer_cnt,
                            (unsigned long)v5f_trained,
                            (unsigned long)v5f_pred);
                        Serial_Printf(SERIAL_PORT_WIFI,
                            "V5F_DIAG,valid=%lu,cnt=%lu,last_cnt=%lu,trained=%lu,pred=%lu\r\n",
                            (unsigned long)v5f_infer_valid,
                            (unsigned long)v5f_infer_cnt,
                            (unsigned long)s_last_infer_cnt,
                            (unsigned long)v5f_trained,
                            (unsigned long)v5f_pred);
                    }
                }

                if (v5f_infer_valid && v5f_infer_cnt != s_last_infer_cnt) {
                    s_last_infer_cnt = v5f_infer_cnt;
                    s_infer_row_tick++;
                    if (s_infer_row_tick >= DIR_DECISION_ROWS) {
                        s_infer_row_tick = 0;
                        const char *pred_str;
                        if (v5f_pred == 0u) {
                            pred_str = "LEFT";
                        } else if (v5f_pred == 1u) {
                            pred_str = "RIGHT";
                        } else {
                            pred_str = (v5f_score_l >= v5f_score_r) ? "LEFT" : "RIGHT";
                        }
                        uint8_t seq = Retry_GetSeq();
                        char result_buf[128];
                        snprintf(result_buf, sizeof(result_buf),
                                 "RESULT,seq=%u,src=V5F,window=%lu,dt_ms=%u,win_rows=%u,INTENT=%s,S_LEFT=%ld,S_RIGHT=%ld,CONF=%ld,trained=%d\r\n",
                                 (unsigned)seq,
                                 (unsigned long)s_result_window,
                                 (unsigned int)DIR_RESULT_DT_MS,
                                 (unsigned int)DIR_DECISION_ROWS,
                                 pred_str,
                                 (long)v5f_score_l,
                                 (long)v5f_score_r,
                                 (long)v5f_conf,
                                 (int)v5f_trained);
                        Serial_Printf(SERIAL_PORT_DEBUG, "%s", result_buf);
                        Serial_Printf(SERIAL_PORT_WIFI, "%s", result_buf);
                        Retry_Store(result_buf);
                        s_result_window++;
                    }
                }
            }
        }

        fft_step = FFT_STEP_UPDATE_TREND;
        break;
    }

    case FFT_STEP_UPDATE_TREND:
        if (g_attn_output.attention_score > 1e-6f) {
            TrendWin.CH0[TrendWin.Idx] = g_attn_output.attention_score;
            TrendWin.CH1[TrendWin.Idx] = g_attn_output.relaxation_score;
            TrendWin.Idx = (TrendWin.Idx + 1) % TREND_WINDOW_SIZE;
            if (TrendWin.Count < TREND_WINDOW_SIZE) TrendWin.Count++;
        }
        fft_step = FFT_STEP_JUDGE_STATE;
        break;

    case FFT_STEP_JUDGE_STATE: {
        instant_state = Judge_Attn_State(g_attn_output.attention_score,
                                          g_attn_output.relaxation_score,
                                          g_attn_output.attention_confidence);
        trend_state = STATE_NEUTRAL;
        if (TrendWin.Count > 0) {
            float sum_attn = 0.0f, sum_relax = 0.0f;
            int i;
            for (i = 0; i < TrendWin.Count; i++) {
                sum_attn += TrendWin.CH0[i];
                sum_relax += TrendWin.CH1[i];
            }
            float avg_attn = sum_attn / (float)TrendWin.Count;
            float avg_relax = sum_relax / (float)TrendWin.Count;
            float diff = avg_attn - avg_relax;
            float strength = fabsf(diff);
            if (strength < 8.0f) {
                trend_state = STATE_NEUTRAL;
            } else if (diff > 0.0f) {
                trend_state = STATE_FOCUS;
            } else {
                trend_state = STATE_RELAX;
            }
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
        Send_Focus(g_attn_output.attention_score, g_attn_output.relaxation_score,
                   g_attn_output.attention_confidence, g_attn_output.relaxation_confidence,
                   trend_state, instant_state);
        Update_OLED_Scores(g_attn_output.attention_score, g_attn_output.relaxation_score, g_attn_output.blink_score,
                           g_attn_output.attention_confidence, g_attn_output.relaxation_confidence, g_attn_output.blink_score);
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
    int j;
    for (j = 0; j < 127; j++) Wave_Buf[j] = Wave_Buf[j + 1];
    Wave_Buf[127] = y;
}

void EEG_FFT_ResetInferState(void)
{
    s_last_infer_cnt = 0;
    s_infer_row_tick = 0;
    s_result_window = 0;
}
