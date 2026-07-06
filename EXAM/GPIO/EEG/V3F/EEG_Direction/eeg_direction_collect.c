#include "eeg_direction_collect.h"
#include "signal_analysis.h"
#include "eeg_direction_feature.h"
#include "Serial.h"
#include <stdio.h>
#include <string.h>

extern uint8_t g_eeg_app_mode;

uint8_t dir_phase = DIR_PHASE_REST;
uint8_t dir_next_label = DIR_LABEL_LEFT;
uint16_t dir_phase_row_count = 0;
uint16_t dir_skip_row_count = 0;
uint16_t dir_round_count = 0;

uint8_t csp_collect_row_tick = 0;
uint32_t csp_window_id = 0;
uint32_t csp_filtered_sample_count = 0;

#if CMD_MODE_ENABLE
WorkMode_t g_work_mode = WORK_MODE_IDLE;
TrialState_t g_trial_state = TRIAL_IDLE;
uint8_t g_trial_label = 0;
uint16_t g_trial_row_count = 0;
uint8_t g_paused = 0;
uint8_t g_skip_rows = 4;
#endif

static int32_t CSP_VoltToUVX1000(float v)
{
    float x = v * CSP_OUTPUT_SCALE_UVX1000;
    if (x > 2147483000.0f) return 2147483000L;
    if (x < -2147483000.0f) return -2147483000L;
    return (x >= 0.0f) ? (int32_t)(x + 0.5f) : (int32_t)(x - 0.5f);
}

static void CSP_PrintWindowCSV(uint8_t label, RingBuffer_t *filt_buf)
{
    char line[160];
    uint16_t j;
    uint32_t win_id;
    int start;

    if (csp_filtered_sample_count < CSP_WIN_SIZE) {
        return;
    }

    win_id = ++csp_window_id;
    start = filt_buf->WriteIdx;

    snprintf(line, sizeof(line),
             "CSP_BEGIN,label=%u,win=%lu,fs=250,n=%u,ch=4,unit=uVx1000,order=CP3_CP4_C3_C4\r\n",
             (unsigned int)label,
             (unsigned long)win_id,
             (unsigned int)CSP_WIN_SIZE);
    Serial_SendString(DIR_TEXT_PORT, line);

    for (j = 0; j < CSP_WIN_SIZE; j++) {
        uint16_t idx = (uint16_t)((start + j) & (CSP_WIN_SIZE - 1u));
        int32_t ch0 = CSP_VoltToUVX1000(filt_buf->CH0[idx]);
        int32_t ch1 = CSP_VoltToUVX1000(filt_buf->CH1[idx]);
        int32_t ch2 = CSP_VoltToUVX1000(filt_buf->CH2[idx]);
        int32_t ch3 = CSP_VoltToUVX1000(filt_buf->CH3[idx]);

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

void Direction_AutoCollectCSPProcess(RingBuffer_t *filt_buf)
{
#if CMD_MODE_ENABLE
    if (g_paused) return;
    if (g_work_mode == WORK_MODE_TRAIN && g_trial_state == TRIAL_RUNNING) {
        g_trial_row_count++;
        if (g_trial_row_count > g_skip_rows) {
            csp_collect_row_tick++;
            if (csp_collect_row_tick >= CSP_COLLECT_EVERY_ROWS) {
                csp_collect_row_tick = 0;
                CSP_PrintWindowCSV(g_trial_label, filt_buf);
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
        CSP_PrintWindowCSV(label_to_print, filt_buf);
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

void Direction_AutoCollectProcess(float theta_pow[NUM_CHANNELS],
                                  float alpha_pow[NUM_CHANNELS],
                                  float beta_pow[NUM_CHANNELS])
{
#if CMD_MODE_ENABLE
    if (g_paused) return;
    if (g_work_mode == WORK_MODE_TRAIN && g_trial_state == TRIAL_RUNNING) {
        g_trial_row_count++;
        if (g_trial_row_count > g_skip_rows) {
            Direction_PrintFeatureCSV(g_trial_label, theta_pow, alpha_pow, beta_pow);
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
        Direction_PrintFeatureCSV(label_to_print, theta_pow, alpha_pow, beta_pow);
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