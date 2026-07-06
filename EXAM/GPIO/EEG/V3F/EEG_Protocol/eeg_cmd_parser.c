#include "eeg_cmd_parser.h"
#include "signal_analysis.h"
#include "eeg_direction_collect.h"
#include "eeg_direction_feature.h"
#include "eeg_fft.h"
#include "Serial.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern uint8_t g_eeg_app_mode;

extern uint8_t g_ipc_diag_enable;

void Parse_Command(const char *cmd)
{
    char clean_cmd[64];
    strncpy(clean_cmd, cmd, sizeof(clean_cmd) - 1);
    clean_cmd[sizeof(clean_cmd) - 1] = '\0';
    size_t len = strlen(clean_cmd);
    while (len > 0 && (clean_cmd[len-1] == '\r' || clean_cmd[len-1] == '\n')) {
        clean_cmd[len-1] = '\0';
        len--;
    }

    Serial_Printf(SERIAL_PORT_DEBUG, "DEBUG_RX: %s\r\n", clean_cmd);

    if (strcmp(clean_cmd, "MODE,TRAIN") == 0) {
        g_work_mode = WORK_MODE_TRAIN;
        g_trial_state = TRIAL_IDLE;
        g_paused = 0;
        Direction_ResetRestBaseline();
        Serial_Printf(SERIAL_PORT_DEBUG, "READY_TRAIN\r\n");
        return;
    }
    if (strcmp(clean_cmd, "MODE,TEST") == 0) {
        g_work_mode = WORK_MODE_TEST;
        g_trial_state = TRIAL_IDLE;
        g_paused = 0;

        EEG_FFT_ResetInferState();
        Serial_Printf(SERIAL_PORT_DEBUG, "READY_TEST\r\n");
        return;
    }
    if (strncmp(clean_cmd, "MODE,SET,", 9) == 0) {
        int mode = atoi(clean_cmd + 9);
        if (mode == EEG_APP_MODE_COLLECT || mode == EEG_APP_MODE_INFER || mode == EEG_APP_MODE_COLLECT_CSP) {
            g_eeg_app_mode = (uint8_t)mode;
            if (mode == EEG_APP_MODE_COLLECT || mode == EEG_APP_MODE_COLLECT_CSP) {
                g_ipc_diag_enable = 0;
            }
            Serial_Printf(SERIAL_PORT_DEBUG, "MODE_SET_OK,%d\r\n", mode);
            dir_phase = DIR_PHASE_REST;
            dir_phase_row_count = 0;
            dir_skip_row_count = 0;
            dir_round_count = 0;
            csp_collect_row_tick = 0;
            csp_window_id = 0;
        } else {
            Serial_Printf(SERIAL_PORT_DEBUG, "ERROR,INVALID_MODE\r\n");
        }
        return;
    }
    if (strncmp(clean_cmd, "TRIAL,", 6) == 0) {
        if (g_work_mode != WORK_MODE_TRAIN) {
            Serial_Printf(SERIAL_PORT_DEBUG, "ERROR,NOT_TRAIN_MODE\r\n");
            return;
        }
        if (g_trial_state == TRIAL_RUNNING) {
            Serial_Printf(SERIAL_PORT_DEBUG, "ERROR,BUSY\r\n");
            return;
        }
        const char *side = clean_cmd + 6;
        if (strcmp(side, "LEFT") == 0) {
            g_trial_label = DIR_LABEL_LEFT;
        } else if (strcmp(side, "RIGHT") == 0) {
            g_trial_label = DIR_LABEL_RIGHT;
        } else {
            Serial_Printf(SERIAL_PORT_DEBUG, "ERROR,INVALID_SIDE\r\n");
            return;
        }
        g_trial_state = TRIAL_RUNNING;
        g_trial_row_count = 0;
        g_paused = 0;
        Direction_ResetTaskZero();
        Serial_Printf(SERIAL_PORT_DEBUG, "TASK,%s,start\r\n", (g_trial_label==DIR_LABEL_LEFT)?"LEFT":"RIGHT");
        return;
    }
    if (strcmp(clean_cmd, "STOP") == 0) {
        g_trial_state = TRIAL_DONE;
        g_paused = 1;
        EEG_FFT_ResetInferState();
        Serial_Printf(SERIAL_PORT_DEBUG, "TASK,STOPPED\r\n");
        return;
    }
    if (strcmp(clean_cmd, "STATUS") == 0) {
        Serial_Printf(SERIAL_PORT_DEBUG, "STATUS,mode=%d,trial=%d,row=%d\r\n",
                      g_work_mode, g_trial_state, g_trial_row_count);
        return;
    }
    if (strcmp(clean_cmd, "IPCDIAG,ON") == 0) {
        g_ipc_diag_enable = 1;
        Serial_Printf(SERIAL_PORT_DEBUG, "IPCDIAG,ON\r\n");
        return;
    }
    if (strcmp(clean_cmd, "IPCDIAG,OFF") == 0) {
        g_ipc_diag_enable = 0;
        Serial_Printf(SERIAL_PORT_DEBUG, "IPCDIAG,OFF\r\n");
        return;
    }
    Serial_Printf(SERIAL_PORT_DEBUG, "ERROR,UNKNOWN_CMD\r\n");
}