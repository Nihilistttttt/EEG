#include "eeg_infer_glxss.h"
#include "Message_Parser.h"
#include "Serial.h"
#include "dualcore_ipc.h"
#include "eeg_protocol.h"
#include "eeg_cmd_parser.h"
#include "signal_analysis.h"
#include <string.h>
#include <stdlib.h>

#ifdef GLXSS_ENABLED
#include "ipc_log.h"
#endif

extern uint8_t g_eeg_app_mode;
extern volatile uint8_t g_v5f_active;

#define GLXSS_INFER_STATE_IDLE    0
#define GLXSS_INFER_STATE_TARGET  1
#define GLXSS_INFER_STATE_COLLECT 2

static uint8_t s_state = GLXSS_INFER_STATE_IDLE;
static uint8_t s_mode = GLXSS_INFER_MODE_CYCLE;
static uint8_t s_rounds = 3;
static uint8_t s_cur_dir = 0;          /* 0=LEFT, 1=RIGHT */
static uint32_t s_frame_count = 0;
static uint32_t s_cycle_rounds_left = 0;
static uint32_t s_cycle_rounds_right = 0;
static uint32_t s_total_targets = 0;
static uint32_t s_infer_base_cnt = 0;  /* infer count at window start */
static uint8_t s_window_pred = 0;
static int32_t s_window_sl = 0;
static int32_t s_window_sr = 0;
static int32_t s_window_conf = 0;
static uint8_t s_window_valid = 0;

void GLXSS_Infer_Config(uint8_t mode, uint8_t rounds)
{
    s_mode = (mode == GLXSS_INFER_MODE_RANDOM) ? GLXSS_INFER_MODE_RANDOM
                                              : GLXSS_INFER_MODE_CYCLE;
    if (rounds == 0) rounds = 1;
    s_rounds = rounds;
}

void GLXSS_Infer_Start(void)
{
    if (s_state != GLXSS_INFER_STATE_IDLE) return;
    s_cycle_rounds_left = s_rounds;
    s_cycle_rounds_right = s_rounds;
    s_total_targets = 0;
    s_cur_dir = 0;
    s_state = GLXSS_INFER_STATE_TARGET;
    s_frame_count = 0;
#ifdef GLXSS_ENABLED
    Serial_Printf(SERIAL_PORT_DEBUG, "[GLXSS] INFER start mode=%u rounds=%u\r\n",
                  (unsigned)s_mode, (unsigned)s_rounds);
    IPC_Cmd_Send_V3F(IPC_CMD_ARROW, 0);
#endif
}

void GLXSS_Infer_Stop(void)
{
    if (s_state == GLXSS_INFER_STATE_IDLE) return;
    s_state = GLXSS_INFER_STATE_IDLE;
#ifdef GLXSS_ENABLED
    IPC_Cmd_Send_V3F(IPC_CMD_RESET, 0);
#endif
}

uint8_t GLXSS_Infer_IsActive(void)
{
    return (s_state != GLXSS_INFER_STATE_IDLE) ? 1u : 0u;
}

static uint8_t next_direction(void)
{
    if (s_mode == GLXSS_INFER_MODE_RANDOM) {
        uint32_t r = (s_total_targets * 2654435761u) + 0x9E3779B9u;
        r = (r >> 13) ^ r;
        return (uint8_t)(r & 1u);
    }
    /* cycle: LEFT x rounds then RIGHT x rounds, repeat */
    if (s_cycle_rounds_left > 0) {
        s_cycle_rounds_left--;
        return 0;
    }
    if (s_cycle_rounds_right > 0) {
        s_cycle_rounds_right--;
        return 1;
    }
    /* flip back */
    s_cycle_rounds_left = s_rounds;
    s_cycle_rounds_right = s_rounds;
    return next_direction();
}

static void report_window_result(void)
{
    uint8_t result_buf[17];
    uint8_t seq = Retry_GetSeq();
    uint8_t pred = s_window_valid
        ? ((s_window_pred == 0u) ? 0u : 1u)
        : ((s_window_sl >= s_window_sr) ? 0u : 1u);
    result_buf[0] = seq;
    result_buf[1] = pred;
    memcpy(result_buf + 2, &s_window_sl, 4);
    memcpy(result_buf + 6, &s_window_sr, 4);
    memcpy(result_buf + 10, &s_window_conf, 4);
    result_buf[14] = (uint8_t)(DualCore_IPC_GetLastV5FModelTrained() ? 1u : 0u);
    result_buf[15] = (uint8_t)DualCore_IPC_GetLastV5FModelUsed();
    result_buf[16] = s_cur_dir;       /* ground-truth target label */
    Pack_Frame(SERIAL_PORT_DEBUG, CMD_RESULT_MI, result_buf, sizeof(result_buf));
    Pack_Frame(SERIAL_PORT_WIFI, CMD_RESULT_MI, result_buf, sizeof(result_buf));
    Retry_Store(result_buf, sizeof(result_buf), CMD_RESULT_MI);
    Serial_Printf(SERIAL_PORT_DEBUG, "[GLXSS] INFER win tgt=%s pred=%s sl=%ld sr=%ld\r\n",
                  s_cur_dir ? "R" : "L", pred ? "R" : "L",
                  (long)s_window_sl, (long)s_window_sr);
}

void GLXSS_Infer_Poll(void)
{
    if (s_state == GLXSS_INFER_STATE_IDLE) return;
    if (g_eeg_app_mode != EEG_APP_MODE_INFER) return;

    s_frame_count++;

    if (s_state == GLXSS_INFER_STATE_TARGET) {
        if (s_frame_count >= GLXSS_INFER_TARGET_FRAMES) {
            /* begin collect window */
            s_state = GLXSS_INFER_STATE_COLLECT;
            s_frame_count = 0;
            s_infer_base_cnt = DualCore_IPC_GetLastV5FInferCount();
            s_window_valid = 0;
        }
        return;
    }

    /* COLLECT: capture latest V5F inference */
    uint32_t infer_cnt = DualCore_IPC_GetLastV5FInferCount();
    uint32_t infer_valid = DualCore_IPC_GetLastV5FInferValid();
    if (infer_valid && infer_cnt != s_infer_base_cnt) {
        s_infer_base_cnt = infer_cnt;
        s_window_pred = (uint8_t)DualCore_IPC_GetLastV5FPred();
        s_window_sl = DualCore_IPC_GetLastV5FScoreLeft();
        s_window_sr = DualCore_IPC_GetLastV5FScoreRight();
        s_window_conf = DualCore_IPC_GetLastV5FConfidence();
        s_window_valid = 1;
    }

    if (s_frame_count >= GLXSS_INFER_COLLECT_FRAMES) {
        report_window_result();
        s_total_targets++;
        /* next target */
        s_cur_dir = next_direction();
        s_state = GLXSS_INFER_STATE_TARGET;
        s_frame_count = 0;
#ifdef GLXSS_ENABLED
        IPC_Cmd_Send_V3F(IPC_CMD_ARROW, s_cur_dir);
#endif
    }
}