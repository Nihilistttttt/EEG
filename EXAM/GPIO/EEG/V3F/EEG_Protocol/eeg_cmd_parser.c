#include "eeg_cmd_parser.h"
#include "signal_analysis.h"
#include "eeg_direction_collect.h"
#include "eeg_direction_infer.h"
#include "eeg_direction_feature.h"
#include "eeg_fft.h"
#include "Message_Parser.h"
#include "Serial.h"
#include "dualcore_ipc.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern uint8_t g_eeg_app_mode;
extern uint8_t g_ipc_diag_enable;
extern uint8_t g_posture_diag_enable;
extern volatile uint32_t g_icm42605_ms_tick;

#define RESP(fmt, ...) do { \
    Serial_Printf(SERIAL_PORT_DEBUG, fmt, ##__VA_ARGS__); \
    Serial_Printf(SERIAL_PORT_WIFI, fmt, ##__VA_ARGS__); \
} while(0)

#define RETRY_BUF_SIZE 128
#define RETRY_TIMEOUT_MS 300

static char s_retry_buf[RETRY_BUF_SIZE];
static uint8_t s_retry_seq = 0;
static uint8_t s_retry_count = 0;
static uint8_t s_retry_max = 10;
static uint32_t s_retry_send_tick = 0;

uint8_t Retry_GetSeq(void)
{
    return s_retry_seq + 1u;
}

void Retry_Store(const char *msg)
{
    s_retry_seq++;
    strncpy(s_retry_buf, msg, RETRY_BUF_SIZE - 1);
    s_retry_buf[RETRY_BUF_SIZE - 1] = '\0';
    s_retry_count = 0;
    s_retry_max = 10;
    s_retry_send_tick = g_icm42605_ms_tick;
}

void Retry_StoreEx(const char *msg, uint8_t max_count)
{
    s_retry_seq++;
    strncpy(s_retry_buf, msg, RETRY_BUF_SIZE - 1);
    s_retry_buf[RETRY_BUF_SIZE - 1] = '\0';
    s_retry_count = 0;
    s_retry_max = max_count;
    s_retry_send_tick = g_icm42605_ms_tick;
}

void Retry_Tick(void)
{
    if (s_retry_buf[0] == '\0') return;
    if (s_retry_count >= s_retry_max) {
        s_retry_buf[0] = '\0';
        return;
    }
    uint32_t elapsed = g_icm42605_ms_tick - s_retry_send_tick;
    if (elapsed >= RETRY_TIMEOUT_MS) {
        s_retry_count++;
        s_retry_send_tick = g_icm42605_ms_tick;
        Serial_Printf(SERIAL_PORT_WIFI, "%s", s_retry_buf);
        Serial_Printf(SERIAL_PORT_DEBUG, "[RETRY:%u] %s", s_retry_count, s_retry_buf);
    }
}

static void Retry_Ack(uint8_t seq)
{
    if (seq == s_retry_seq && s_retry_buf[0] != '\0') {
        s_retry_buf[0] = '\0';
    }
}

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

    if (strncmp(clean_cmd, "ACK,", 4) == 0) {
        uint8_t ack_seq = (uint8_t)atoi(clean_cmd + 4);
        Retry_Ack(ack_seq);
        return;
    }

    if (strcmp(clean_cmd, "MODE,TRAIN") == 0) {
        g_work_mode = WORK_MODE_TRAIN;
        g_trial_state = TRIAL_IDLE;
        g_paused = 0;
        Direction_ResetRestBaseline();
        if (g_eeg_app_mode == EEG_APP_MODE_COLLECT_CSP) {
            Direction_CSPStreamReset();
        }
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

    if (strncmp(clean_cmd, "MODEL,SET,", 10) == 0) {
        const char *name = clean_cmd + 10;
        uint8_t select;
        if (strcmp(name, "AUTO") == 0) {
            select = DUALCORE_V5F_MODEL_SELECT_AUTO;
        } else if ((strcmp(name, "FFT") == 0) || (strcmp(name, "FFT24") == 0)) {
            select = DUALCORE_V5F_MODEL_SELECT_FFT24;
        } else if (strcmp(name, "CSP") == 0) {
            select = DUALCORE_V5F_MODEL_SELECT_CSP;
        } else {
            RESP("ERROR,INVALID_MODEL\r\n");
            return;
        }
        DualCore_IPC_SetModelSelect(select);
        RESP("MODEL_SET_OK,%s\r\n",
             (select == DUALCORE_V5F_MODEL_SELECT_CSP) ? "CSP" :
             (select == DUALCORE_V5F_MODEL_SELECT_FFT24) ? "FFT24" : "AUTO");
        return;
    }
    if (strcmp(clean_cmd, "MODEL,GET") == 0) {
        uint8_t select = DualCore_IPC_GetModelSelect();
        RESP("MODEL,%s\r\n",
             (select == DUALCORE_V5F_MODEL_SELECT_CSP) ? "CSP" :
             (select == DUALCORE_V5F_MODEL_SELECT_FFT24) ? "FFT24" : "AUTO");
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
            Direction_CSPStreamReset();
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
        Direction_CSPStreamReset();
        RESP("TASK,STOPPED\r\n");
        return;
    }
    if (strcmp(clean_cmd, "STATUS") == 0) {
        RESP("STATUS,mode=%d,trial=%d,row=%d,model=%u\r\n",
                      g_work_mode, g_trial_state, g_trial_row_count,
                      (unsigned)DualCore_IPC_GetModelSelect());
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
    if (strncmp(clean_cmd, "DISPLAY_CFG,", 12) == 0) {
        const char *p = clean_cmd + 12;
        int vals[12];
        int count = 0;
        while (*p && count < 12) {
            vals[count++] = atoi(p);
            const char *next = strchr(p, ',');
            if (next == NULL) break;
            p = next + 1;
        }
        if (count >= 6) {
            if (vals[0] >= 0 && vals[0] < DISPLAY_MAX_CH) g_display_config.wave_ch[0] = (uint8_t)vals[0];
            if (vals[1] >= 0 && vals[1] <= 2 || vals[1] == WAVE_TYPE_NONE) g_display_config.wave_type[0] = (uint8_t)vals[1];
            if (vals[2] >= 0 && vals[2] < DISPLAY_MAX_CH) g_display_config.wave_ch[1] = (uint8_t)vals[2];
            if (vals[3] >= 0 && vals[3] <= 2 || vals[3] == WAVE_TYPE_NONE) g_display_config.wave_type[1] = (uint8_t)vals[3];
            if (vals[4] >= 0 && vals[4] <= 2 || vals[4] == SPEC_TYPE_NONE) g_display_config.spec_type[0] = (uint8_t)vals[4];
            if (vals[5] >= 0 && vals[5] <= 2 || vals[5] == SPEC_TYPE_NONE) g_display_config.spec_type[1] = (uint8_t)vals[5];
        }
        if (count >= 12) {
            if (vals[6] >= 0 && vals[6] < DISPLAY_MAX_CH) g_display_config.wave_ch[2] = (uint8_t)vals[6];
            if (vals[7] >= 0 && vals[7] <= 2 || vals[7] == WAVE_TYPE_NONE) g_display_config.wave_type[2] = (uint8_t)vals[7];
            if (vals[8] >= 0 && vals[8] < DISPLAY_MAX_CH) g_display_config.wave_ch[3] = (uint8_t)vals[8];
            if (vals[9] >= 0 && vals[9] <= 2 || vals[9] == WAVE_TYPE_NONE) g_display_config.wave_type[3] = (uint8_t)vals[9];
            if (vals[10] >= 0 && vals[10] <= 2 || vals[10] == SPEC_TYPE_NONE) g_display_config.spec_type[2] = (uint8_t)vals[10];
            if (vals[11] >= 0 && vals[11] <= 2 || vals[11] == SPEC_TYPE_NONE) g_display_config.spec_type[3] = (uint8_t)vals[11];
        }

        RESP("DISPLAY_CFG_OK,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u\r\n",
             (unsigned)g_display_config.wave_ch[0],
             (unsigned)g_display_config.wave_type[0],
             (unsigned)g_display_config.wave_ch[1],
             (unsigned)g_display_config.wave_type[1],
             (unsigned)g_display_config.spec_type[0],
             (unsigned)g_display_config.spec_type[1],
             (unsigned)g_display_config.wave_ch[2],
             (unsigned)g_display_config.wave_type[2],
             (unsigned)g_display_config.wave_ch[3],
             (unsigned)g_display_config.wave_type[3],
             (unsigned)g_display_config.spec_type[2],
             (unsigned)g_display_config.spec_type[3]);
        return;
    }
    RESP("ERROR,UNKNOWN_CMD\r\n");
}
