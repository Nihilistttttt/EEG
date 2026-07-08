#include "eeg_cmd_parser.h"
#include "signal_analysis.h"
#include "eeg_direction_collect.h"
#include "eeg_direction_infer.h"
#include "eeg_direction_feature.h"
#include "eeg_fft.h"
#include "Serial.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern uint8_t g_eeg_app_mode;

extern uint8_t g_ipc_diag_enable;

extern uint8_t g_posture_diag_enable;

#define RESP(fmt, ...) do { \
    Serial_Printf(SERIAL_PORT_DEBUG, fmt, ##__VA_ARGS__); \
    Serial_Printf(SERIAL_PORT_WIFI, fmt, ##__VA_ARGS__); \
} while(0)

void Parse_CommandEx(const char *cmd, const char *source)
{
    char clean_cmd[64];
    strncpy(clean_cmd, cmd, sizeof(clean_cmd) - 1);
    clean_cmd[sizeof(clean_cmd) - 1] = '\0';
    size_t len = strlen(clean_cmd);
    while (len > 0 && (clean_cmd[len-1] == '\r' || clean_cmd[len-1] == '\n')) {
        clean_cmd[len-1] = '\0';
        len--;
    }

    Serial_Printf(SERIAL_PORT_DEBUG, "RX[%s]: %s\r\n", source, clean_cmd);
    Serial_Printf(SERIAL_PORT_WIFI, "RX[%s]: %s\r\n", source, clean_cmd);


    if (strcmp(clean_cmd, "MODE,TRAIN") == 0) {
        g_work_mode = WORK_MODE_TRAIN;
        g_trial_state = TRIAL_IDLE;
        g_paused = 0;
        Direction_ResetRestBaseline();
        RESP("READY_TRAIN\r\n");
        return;
    }
    if (strcmp(clean_cmd, "MODE,TEST") == 0) {
        g_work_mode = WORK_MODE_TEST;
        g_trial_state = TRIAL_IDLE;
        g_paused = 0;

        EEG_FFT_ResetInferState();
        RESP("READY_TEST\r\n");
        return;
    }
    if (strncmp(clean_cmd, "MODE,SET,", 9) == 0) {
        int mode = atoi(clean_cmd + 9);
        if (mode == EEG_APP_MODE_COLLECT || mode == EEG_APP_MODE_INFER || mode == EEG_APP_MODE_COLLECT_CSP) {
            g_eeg_app_mode = (uint8_t)mode;
            if (mode == EEG_APP_MODE_COLLECT || mode == EEG_APP_MODE_COLLECT_CSP) {
                g_ipc_diag_enable = 0;
            }
            RESP("MODE_SET_OK,%d\r\n", mode);
            dir_phase = DIR_PHASE_REST;
            dir_phase_row_count = 0;
            dir_skip_row_count = 0;
            dir_round_count = 0;
            csp_collect_row_tick = 0;
            csp_window_id = 0;
        } else {
            RESP("ERROR,INVALID_MODE\r\n");
        }
        return;
    }
    if (strncmp(clean_cmd, "TRIAL,", 6) == 0) {
        if (g_work_mode != WORK_MODE_TRAIN) {
            RESP("ERROR,NOT_TRAIN_MODE\r\n");
            return;
        }
        if (g_trial_state == TRIAL_RUNNING) {
            RESP("ERROR,BUSY\r\n");
            return;
        }
        const char *side = clean_cmd + 6;
        if (strcmp(side, "LEFT") == 0) {
            g_trial_label = DIR_LABEL_LEFT;
        } else if (strcmp(side, "RIGHT") == 0) {
            g_trial_label = DIR_LABEL_RIGHT;
        } else {
            RESP("ERROR,INVALID_SIDE\r\n");
            return;
        }
        g_trial_state = TRIAL_RUNNING;
        g_trial_row_count = 0;
        g_paused = 0;
        Direction_ResetTaskZero();
        RESP("TASK,%s,start\r\n", (g_trial_label==DIR_LABEL_LEFT)?"LEFT":"RIGHT");
        return;
    }
    if (strcmp(clean_cmd, "STOP") == 0) {
        g_work_mode = WORK_MODE_IDLE;
        g_trial_state = TRIAL_IDLE;
        g_paused = 0;
        g_eeg_app_mode = EEG_APP_MODE_COLLECT;
        EEG_FFT_ResetInferState();
        Direction_Infer1sReset();
        RESP("TASK,STOPPED\r\n");
        return;
    }
    if (strcmp(clean_cmd, "STATUS") == 0) {
        RESP("STATUS,mode=%d,trial=%d,row=%d\r\n",
                      g_work_mode, g_trial_state, g_trial_row_count);
        return;
    }
    if (strcmp(clean_cmd, "IPCDIAG,ON") == 0) {
        g_ipc_diag_enable = 1;
        RESP("IPCDIAG,ON\r\n");
        return;
    }
    if (strcmp(clean_cmd, "IPCDIAG,OFF") == 0) {
        g_ipc_diag_enable = 0;
        RESP("IPCDIAG,OFF\r\n");
        return;
    }
    if (strcmp(clean_cmd, "POSTURE,ON") == 0) {
        g_posture_diag_enable = 1;
        RESP("POSTURE,ON\r\n");
        return;
    }
    if (strcmp(clean_cmd, "POSTURE,OFF") == 0) {
        g_posture_diag_enable = 0;
        RESP("POSTURE,OFF\r\n");
        return;
    }
    RESP("ERROR,UNKNOWN_CMD\r\n");
}