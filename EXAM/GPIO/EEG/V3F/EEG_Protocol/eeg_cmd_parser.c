#include "eeg_cmd_parser.h"
#include "signal_analysis.h"
#include "eeg_direction_collect.h"
#include "eeg_direction_infer.h"
#include "eeg_direction_feature.h"
#include "eeg_fft.h"
#include "Message_Parser.h"
#include "Serial.h"
#include "dualcore_ipc.h"
#include "eeg_protocol.h"
#include "ADS1299.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern uint8_t g_eeg_app_mode;
extern uint8_t g_ipc_diag_enable;
extern volatile uint8_t g_ssvep_active;
#ifdef HAS_ICM42605
extern uint8_t g_posture_diag_enable;
#endif
extern volatile uint32_t g_icm42605_ms_tick;

#define RETRY_TIMEOUT_MS 300

static uint8_t s_retry_payload[256];
static uint16_t s_retry_len = 0;
static uint8_t s_retry_cmd = 0;
static uint8_t s_retry_seq = 0;
static uint8_t s_retry_count = 0;
static uint8_t s_retry_max = 10;
static uint32_t s_retry_send_tick = 0;

uint8_t Retry_GetSeq(void)
{
    return s_retry_seq + 1u;
}

static void Retry_SendNow(void)
{
    if (s_retry_len == 0) return;
    Pack_Frame(SERIAL_PORT_WIFI, s_retry_cmd, s_retry_payload, s_retry_len);
}

void Retry_Store(const uint8_t *payload, uint16_t len, uint8_t cmd)
{
    s_retry_seq++;
    s_retry_cmd = cmd;
    s_retry_len = len > sizeof(s_retry_payload) ? (uint16_t)sizeof(s_retry_payload) : len;
    memcpy(s_retry_payload, payload, s_retry_len);
    s_retry_count = 0;
    s_retry_max = 10;
    s_retry_send_tick = g_icm42605_ms_tick;
}

void Retry_StoreEx(const uint8_t *payload, uint16_t len, uint8_t cmd, uint8_t max_count)
{
    s_retry_seq++;
    s_retry_cmd = cmd;
    s_retry_len = len > sizeof(s_retry_payload) ? (uint16_t)sizeof(s_retry_payload) : len;
    memcpy(s_retry_payload, payload, s_retry_len);
    s_retry_count = 0;
    s_retry_max = max_count;
    s_retry_send_tick = g_icm42605_ms_tick;
}

void Retry_Tick(void)
{
    if (s_retry_len == 0) return;
    if (s_retry_count >= s_retry_max) {
        s_retry_len = 0;
        return;
    }
    uint32_t elapsed = g_icm42605_ms_tick - s_retry_send_tick;
    if (elapsed >= RETRY_TIMEOUT_MS) {
        s_retry_count++;
        s_retry_send_tick = g_icm42605_ms_tick;
        Retry_SendNow();
    }
}

static void Retry_Ack(uint8_t seq)
{
    if (seq == s_retry_seq && s_retry_len != 0) {
        s_retry_len = 0;
    }
}

static void Send_Resp(uint8_t cmd, const uint8_t *payload, uint16_t len)
{
    Pack_Frame(SERIAL_PORT_DEBUG, cmd, payload, len);
    Pack_Frame(SERIAL_PORT_WIFI, cmd, payload, len);
}

static void Send_RespOk(uint8_t cmd)
{
    uint8_t payload[1] = {0};
    Send_Resp(cmd, payload, 1);
}

static void Send_RespErr(uint8_t code)
{
    uint8_t payload[1] = {(uint8_t)code};
    Send_Resp(CMD_NULL, payload, 1);
    (void)code;
}

void Parse_CommandBinary(const uint8_t *payload, uint16_t len, const char *source)
{
    (void)source;
    if (len < 1) return;
    uint8_t cmd = payload[0];
    const uint8_t *data = payload + 1;
    uint16_t dlen = len - 1;

    switch (cmd) {
    case CMD_ACK:
        if (dlen >= 1) {
            Retry_Ack(data[0]);
        }
        break;

    case CMD_MODE_TRAIN:
        g_work_mode = WORK_MODE_TRAIN;
        g_trial_state = TRIAL_IDLE;
        g_paused = 0;
        g_v5f_active = V5F_ACTIVE_COLLECT;
        DualCore_IPC_RequestV5FReset();
        Direction_ResetRestBaseline();
        if (g_eeg_app_mode == EEG_APP_MODE_COLLECT_CSP) {
            Direction_CSPStreamReset();
        }
        Send_RespOk(CMD_READY_TRAIN);
        break;

    case CMD_MODE_TEST:
        g_work_mode = WORK_MODE_TEST;
        g_trial_state = TRIAL_IDLE;
        g_paused = 0;
        EEG_FFT_ResetInferState();
        Send_RespOk(CMD_READY_TEST);
        break;

    case CMD_MODEL_SET:
        if (dlen >= 1) {
            uint8_t select = data[0];
            if (select == DUALCORE_V5F_MODEL_SELECT_AUTO
                || select == DUALCORE_V5F_MODEL_SELECT_FFT24
                || select == DUALCORE_V5F_MODEL_SELECT_CSP) {
                DualCore_IPC_SetModelSelect(select);
                uint8_t resp[2] = {CMD_MODEL_SET, select};
                Send_Resp(CMD_MODE_SET_OK, resp, 2);
            } else {
                Send_RespErr(1);
            }
        }
        break;

    case CMD_MODEL_GET:
        {
            uint8_t resp[2] = {CMD_MODEL_GET, DualCore_IPC_GetModelSelect()};
            Send_Resp(CMD_MODE_SET_OK, resp, 2);
        }
        break;

    case CMD_MODE_SET:
        if (dlen >= 1) {
            uint8_t mode = data[0];
            if (mode == EEG_APP_MODE_COLLECT || mode == EEG_APP_MODE_INFER || mode == EEG_APP_MODE_COLLECT_CSP) {
                g_eeg_app_mode = mode;
                if (mode == EEG_APP_MODE_INFER) {
                    g_v5f_active = V5F_ACTIVE_INFER;
                    DualCore_IPC_RequestV5FReset();
                } else {
                    g_v5f_active = V5F_ACTIVE_IDLE;
                }
                if (mode == EEG_APP_MODE_COLLECT || mode == EEG_APP_MODE_COLLECT_CSP) {
                    g_ipc_diag_enable = 0;
                }
                uint8_t resp[2] = {CMD_MODE_SET, mode};
                Send_Resp(CMD_MODE_SET_OK, resp, 2);
                dir_phase = DIR_PHASE_REST;
                dir_phase_row_count = 0;
                dir_skip_row_count = 0;
                dir_round_count = 0;
                csp_collect_row_tick = 0;
                csp_window_id = 0;
                Direction_CSPStreamReset();
            } else {
                Send_RespErr(2);
            }
        }
        break;

    case CMD_TRIAL:
        if (g_work_mode != WORK_MODE_TRAIN) {
            Send_RespErr(3);
            break;
        }
        if (g_trial_state == TRIAL_RUNNING) {
            Send_RespErr(4);
            break;
        }
        if (dlen >= 1) {
            uint8_t side = data[0];
            if (side == 0u) {
                g_trial_label = DIR_LABEL_LEFT;
            } else if (side == 1u) {
                g_trial_label = DIR_LABEL_RIGHT;
            } else {
                Send_RespErr(5);
                break;
            }
            g_trial_state = TRIAL_RUNNING;
            g_trial_row_count = 0;
            g_paused = 0;
            g_v5f_active = V5F_ACTIVE_COLLECT;
            DualCore_IPC_RequestV5FReset();
            Direction_ResetTaskZero();
            uint8_t resp[1] = {side};
            Send_Resp(CMD_TASK_START, resp, 1);
        }
        break;

    case CMD_STOP:
        g_work_mode = WORK_MODE_IDLE;
        g_trial_state = TRIAL_IDLE;
        g_paused = 0;
        g_eeg_app_mode = EEG_APP_MODE_COLLECT;
        g_v5f_active = V5F_ACTIVE_IDLE;
        g_ssvep_active = 0;
        DualCore_IPC_SetSsvepEnable(0);
        EEG_FFT_ResetInferState();
        Direction_Infer1sReset();
        Direction_CSPStreamReset();
        Send_RespOk(CMD_TASK_STOPPED);
        break;

    case CMD_STATUS:
        {
            uint8_t resp[6];
            resp[0] = (uint8_t)g_work_mode;
            resp[1] = (uint8_t)g_trial_state;
            resp[2] = (uint8_t)g_trial_row_count;
            resp[3] = 0;
            resp[4] = DualCore_IPC_GetModelSelect();
            resp[5] = (uint8_t)(g_trial_row_count >> 8);
            Send_Resp(CMD_STATUS, resp, 6);
        }
        break;

    case CMD_IPCDIAG:
        if (dlen >= 1) {
            g_ipc_diag_enable = data[0] ? 1 : 0;
            uint8_t resp[2] = {CMD_IPCDIAG, g_ipc_diag_enable};
            Send_Resp(CMD_MODE_SET_OK, resp, 2);
        }
        break;

    case CMD_POSTURE:
#ifdef HAS_ICM42605
        if (dlen >= 1) {
            g_posture_diag_enable = data[0] ? 1 : 0;
            uint8_t resp[2] = {CMD_POSTURE, g_posture_diag_enable};
            Send_Resp(CMD_MODE_SET_OK, resp, 2);
        }
#endif
        break;

    case CMD_DISPLAY_CFG:
        {
            uint8_t n_slots = (dlen / 3u);
            uint8_t s;
            if (n_slots > DISPLAY_NUM_CH) n_slots = DISPLAY_NUM_CH;
            for (s = 0; s < n_slots; s++) {
                uint8_t wch = data[s * 3];
                if (wch < DISPLAY_MAX_CH) g_display_config.wave_ch[s] = wch;
                g_display_config.wave_type[s] = data[s * 3 + 1];
                g_display_config.spec_type[s] = data[s * 3 + 2];
            }
            EEG_FFT_ResetSendState();
            uint8_t resp[24];
            uint16_t resp_len = (uint16_t)(n_slots * 3u);
            memcpy(resp, data, resp_len);
            Send_Resp(CMD_DISPLAY_CFG, resp, resp_len);
        }
        break;

    case CMD_SSVEP_START:
        g_ssvep_active = 1;
        DualCore_IPC_SetSsvepEnable(1);
        DualCore_IPC_RequestSsvepReset();
        if (g_v5f_active == V5F_ACTIVE_IDLE) {
            g_v5f_active = V5F_ACTIVE_INFER;
            DualCore_IPC_RequestV5FReset();
        }
        Send_RespOk(CMD_SSVEP_START);
        break;

    case CMD_SSVEP_STOP:
        g_ssvep_active = 0;
        DualCore_IPC_SetSsvepEnable(0);
        DualCore_IPC_SetSsvepSelftest(0, 0);
        DualCore_IPC_RequestSsvepReset();
        if (g_v5f_active == V5F_ACTIVE_INFER
            && g_eeg_app_mode != EEG_APP_MODE_INFER) {
            g_v5f_active = V5F_ACTIVE_IDLE;
        }
        Send_RespOk(CMD_SSVEP_STOP);
        break;

    case CMD_SSVEP_SELFTEST_START:
        {
            uint8_t freq_idx = 0;
            if (dlen >= 1 && data[0] <= 3) freq_idx = data[0];
            g_ssvep_active = 1;
            DualCore_IPC_SetSsvepEnable(1);
            DualCore_IPC_SetSsvepSelftest(1, freq_idx);
            DualCore_IPC_RequestSsvepReset();
            if (g_v5f_active == V5F_ACTIVE_IDLE) {
                g_v5f_active = V5F_ACTIVE_INFER;
                DualCore_IPC_RequestV5FReset();
            }
            uint8_t resp[1] = {freq_idx};
            Send_Resp(CMD_SSVEP_SELFTEST_START, resp, 1);
        }
        break;

    case CMD_SSVEP_SELFTEST_STOP:
        DualCore_IPC_SetSsvepSelftest(0, 0);
        DualCore_IPC_RequestSsvepReset();
        Send_RespOk(CMD_SSVEP_SELFTEST_STOP);
        break;

    case CMD_IMPEDANCE_CHECK:
        {
            ADS1299_EnterImpedanceMode();
            float z_kohm[8];
            ADS1299_MeasureImpedance(z_kohm);
            ADS1299_ExitImpedanceMode();

            uint8_t resp[33];
            for (int c = 0; c < 8; c++) {
                int32_t val_i32 = (int32_t)(z_kohm[c] * 100.0f + 0.5f);
                if (val_i32 < 0) val_i32 = 0;
                resp[c * 4 + 0] = (uint8_t)(val_i32 & 0xFF);
                resp[c * 4 + 1] = (uint8_t)((val_i32 >> 8) & 0xFF);
                resp[c * 4 + 2] = (uint8_t)((val_i32 >> 16) & 0xFF);
                resp[c * 4 + 3] = (uint8_t)((val_i32 >> 24) & 0xFF);
            }
            resp[32] = ADS1299_RecheckBias();
            Send_Resp(CMD_IMPEDANCE_RESULT, resp, 33);
        }
        break;

    default:
        Send_RespErr(0xFF);
        break;
    }
}

