#include "dualcore_ipc.h"
#include "dualcore_ipc_shared.h"
#include <string.h>

#if defined(Core_V3F) && DUALCORE_IPC_RUNTIME_ENABLE
#include "OLED.h"
#include "signal_analysis.h"
#include "ADS1299.h"
#include "Serial.h"
#include "ICM42605.h"

volatile uint32_t g_ipc_v3f_notify_count       = 0;
volatile uint32_t g_ipc_v3f_ack_count          = 0;
volatile uint32_t g_ipc_v3f_tx_checksum        = 0;
volatile uint32_t g_ipc_v3f_ack_checksum       = 0;
volatile uint32_t g_ipc_v3f_ack_tx_checksum    = 0;
volatile uint32_t g_ipc_v3f_checksum_ok        = 0;
volatile uint32_t g_ipc_v3f_checksum_bad       = 0;
volatile uint32_t g_ipc_v3f_parse_ok           = 0;
volatile uint32_t g_ipc_v3f_parse_bad          = 0;
volatile int32_t  g_ipc_v3f_last_v3f_ch0       = 0;
volatile int32_t  g_ipc_v3f_last_v5f_ch0       = 0;
volatile int32_t  g_ipc_v3f_last_v5f_uv0_x1000 = 0;
volatile int32_t  g_ipc_v3f_last_v5f_pre0_x1000= 0;
volatile int32_t  g_ipc_v3f_last_v5f_filt0_x1000=0;
volatile uint32_t g_ipc_v3f_last_v5f_sample_count=0;
volatile uint32_t g_ipc_v3f_last_v5f_window_count=0;
volatile uint32_t g_ipc_v3f_last_v5f_fft_count=0;
volatile uint32_t g_ipc_v3f_last_v5f_feature_valid=0;
volatile int32_t  g_ipc_v3f_last_v5f_feature_q[DUALCORE_V5F_FEATURE_DIM];
volatile uint32_t g_ipc_v3f_last_v5f_infer_valid=0;
volatile uint32_t g_ipc_v3f_last_v5f_pred=DUALCORE_V5F_PRED_UNKNOWN;
volatile uint32_t g_ipc_v3f_last_v5f_model_trained=0;
volatile uint32_t g_ipc_v3f_last_v5f_model_used=DUALCORE_V5F_MODEL_FFT24;
volatile int32_t  g_ipc_v3f_last_v5f_score_left=0;
volatile int32_t  g_ipc_v3f_last_v5f_score_right=0;
volatile int32_t  g_ipc_v3f_last_v5f_confidence=0;
volatile uint32_t g_ipc_v3f_last_v5f_infer_count=0;
volatile uint8_t  g_ipc_v3f_ready              = 0;
static volatile uint8_t g_ipc_v3f_model_select
    __attribute__((section(".bss"))) = DUALCORE_V5F_MODEL_SELECT_AUTO;

static volatile DualCore_IPC_FrameSlot_t g_ipc_v3f_frame_slot[DUALCORE_IPC_FRAME_SLOT_NUM];
static volatile uint16_t g_ipc_v3f_tx_checksum_hist[DUALCORE_IPC_TX_HISTORY_SIZE];
static volatile uint32_t g_ipc_v3f_tx_status_hist[DUALCORE_IPC_TX_HISTORY_SIZE];
static volatile int32_t  g_ipc_v3f_tx_ch_hist[DUALCORE_IPC_TX_HISTORY_SIZE][DUALCORE_ADS1299_ACTIVE_CH_NUM];

void IPC_CH0_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void DualCore_IPC_SetModelSelect(uint8_t model_select)
{
    if (model_select > DUALCORE_V5F_MODEL_SELECT_CSP) {
        model_select = DUALCORE_V5F_MODEL_SELECT_AUTO;
    }
    g_ipc_v3f_model_select = model_select;
}

uint8_t DualCore_IPC_GetModelSelect(void)
{
    return g_ipc_v3f_model_select;
}


void DualCore_IPC_Init_V3F(void)
{
    uint16_t i;
    uint16_t s;
    uint16_t c;

    g_ipc_v3f_notify_count       = 0;
    g_ipc_v3f_ack_count          = 0;
    g_ipc_v3f_tx_checksum        = 0;
    g_ipc_v3f_ack_checksum       = 0;
    g_ipc_v3f_ack_tx_checksum    = 0;
    g_ipc_v3f_checksum_ok        = 0;
    g_ipc_v3f_checksum_bad       = 0;
    g_ipc_v3f_parse_ok           = 0;
    g_ipc_v3f_parse_bad          = 0;
    g_ipc_v3f_last_v3f_ch0       = 0;
    g_ipc_v3f_last_v5f_ch0       = 0;
    g_ipc_v3f_last_v5f_uv0_x1000 = 0;
    g_ipc_v3f_last_v5f_pre0_x1000 = 0;
    g_ipc_v3f_last_v5f_filt0_x1000 = 0;
    g_ipc_v3f_last_v5f_sample_count = 0;
    g_ipc_v3f_last_v5f_window_count = 0;
    g_ipc_v3f_last_v5f_fft_count = 0;
    g_ipc_v3f_last_v5f_feature_valid = 0;
    g_ipc_v3f_last_v5f_infer_valid = 0;
    g_ipc_v3f_last_v5f_pred = DUALCORE_V5F_PRED_UNKNOWN;
    g_ipc_v3f_last_v5f_model_trained = 0;
    g_ipc_v3f_last_v5f_model_used = DUALCORE_V5F_MODEL_FFT24;
    g_ipc_v3f_last_v5f_score_left = 0;
    g_ipc_v3f_last_v5f_score_right = 0;
    g_ipc_v3f_last_v5f_confidence = 0;
    g_ipc_v3f_last_v5f_infer_count = 0;
    for (i = 0; i < DUALCORE_V5F_FEATURE_DIM; i++) {
        g_ipc_v3f_last_v5f_feature_q[i] = 0;
    }
    g_ipc_v3f_ready              = 0;
    g_ipc_v3f_model_select = DUALCORE_V5F_MODEL_SELECT_AUTO;

    for (i = 0; i < DUALCORE_IPC_TX_HISTORY_SIZE; i++) {
        g_ipc_v3f_tx_checksum_hist[i] = 0;
        g_ipc_v3f_tx_status_hist[i] = 0;
        for (c = 0; c < DUALCORE_ADS1299_ACTIVE_CH_NUM; c++) {
            g_ipc_v3f_tx_ch_hist[i][c] = 0;
        }
    }

    for (s = 0; s < DUALCORE_IPC_FRAME_SLOT_NUM; s++) {
        g_ipc_v3f_frame_slot[s].seq = 0;
        g_ipc_v3f_frame_slot[s].len = 0;
        g_ipc_v3f_frame_slot[s].checksum = 0;
        g_ipc_v3f_frame_slot[s].model_select = DUALCORE_V5F_MODEL_SELECT_AUTO;
        g_ipc_v3f_frame_slot[s].control_reserved[0] = 0u;
        g_ipc_v3f_frame_slot[s].control_reserved[1] = 0u;
        g_ipc_v3f_frame_slot[s].control_reserved[2] = 0u;
        g_ipc_v3f_frame_slot[s].v5f_parse_valid = 0;
        g_ipc_v3f_frame_slot[s].v5f_status = 0;
        g_ipc_v3f_frame_slot[s].v5f_sample_count = 0;
        g_ipc_v3f_frame_slot[s].v5f_window_count = 0;
        g_ipc_v3f_frame_slot[s].v5f_feature_valid = 0;
        g_ipc_v3f_frame_slot[s].v5f_fft_count = 0;
        g_ipc_v3f_frame_slot[s].v5f_infer_valid = 0;
        g_ipc_v3f_frame_slot[s].v5f_pred = DUALCORE_V5F_PRED_UNKNOWN;
        g_ipc_v3f_frame_slot[s].v5f_model_trained = 0;
        g_ipc_v3f_frame_slot[s].v5f_model_used = DUALCORE_V5F_MODEL_FFT24;
        g_ipc_v3f_frame_slot[s].v5f_score_left = 0;
        g_ipc_v3f_frame_slot[s].v5f_score_right = 0;
        g_ipc_v3f_frame_slot[s].v5f_confidence = 0;
        g_ipc_v3f_frame_slot[s].v5f_infer_count = 0;
        for (i = 0; i < DUALCORE_V5F_FEATURE_DIM; i++) {
            g_ipc_v3f_frame_slot[s].v5f_feature_q[i] = 0;
        }
        for (c = 0; c < DUALCORE_ADS1299_ACTIVE_CH_NUM; c++) {
            g_ipc_v3f_frame_slot[s].v5f_ch_code[c] = 0;
            g_ipc_v3f_frame_slot[s].v5f_uv_x1000[c] = 0;
            g_ipc_v3f_frame_slot[s].v5f_pre_x1000[c] = 0;
            g_ipc_v3f_frame_slot[s].v5f_filt_x1000[c] = 0;
        }
        for (i = 0; i < DUALCORE_IPC_FRAME_LEN; i++) {
            g_ipc_v3f_frame_slot[s].frame[i] = 0;
        }
    }

    IPC_DeInit();
    IPC_Config(IPC_CH0, IPC_TxCID1, IPC_RxCID0);
    IPC_CH0_Lock();

    IPC_WriteMSG(IPC_MSG0, 0);
    IPC_SetFlagStatus(IPC_CH0, IPC_CH_Sta_Bit0);
    IPC_ClearFlagStatus(IPC_CH0, IPC_CH_Sta_Bit1);

    NVIC_SetPriority(IPC_CH0_IRQn, (1 << 7) | (1 << 4));
    NVIC_EnableIRQ(IPC_CH0_IRQn);

    Delay_Ms(100);

    g_ipc_v3f_ready = 1;
}

void DualCore_IPC_NotifyFromV3F(void)
{
    if (!g_ipc_v3f_ready) {
        return;
    }

    IPC_WriteMSG(IPC_MSG0, (uint32_t)&g_ipc_v3f_frame_slot[0]);
    DUALCORE_FENCE();
    IPC_ITConfig(IPC_CH0, IPC_CH_Sta_Bit1, ENABLE);
}

void DualCore_IPC_SendFrameFromV3F(const uint8_t *frame, uint16_t len)
{
    uint16_t i;
    uint16_t c;
    uint16_t copy_len;
    uint32_t seq;
    uint32_t slot_idx;
    uint32_t hist_idx;
    volatile DualCore_IPC_FrameSlot_t *slot;
    uint16_t checksum;
    uint32_t status = 0;
    int32_t ch_data[DUALCORE_ADS1299_CHANNEL_NUM];

    if ((!g_ipc_v3f_ready) || (frame == 0)) {
        return;
    }

    seq = g_ipc_v3f_notify_count + 1u;
    slot_idx = seq % DUALCORE_IPC_FRAME_SLOT_NUM;
    hist_idx = seq % DUALCORE_IPC_TX_HISTORY_SIZE;
    slot = &g_ipc_v3f_frame_slot[slot_idx];

    copy_len = (len < DUALCORE_IPC_FRAME_LEN) ? len : DUALCORE_IPC_FRAME_LEN;

    NVIC_DisableIRQ(IPC_CH0_IRQn);

    slot->model_select = g_ipc_v3f_model_select;
    slot->v5f_parse_valid = 0;
    slot->v5f_status = 0;
    slot->v5f_sample_count = 0;
    slot->v5f_window_count = 0;
    slot->v5f_feature_valid = 0;
    slot->v5f_fft_count = 0;
    slot->v5f_infer_valid = 0;
    slot->v5f_pred = DUALCORE_V5F_PRED_UNKNOWN;
    slot->v5f_model_trained = 0;
    slot->v5f_model_used = DUALCORE_V5F_MODEL_FFT24;
    slot->v5f_score_left = 0;
    slot->v5f_score_right = 0;
    slot->v5f_confidence = 0;
    slot->v5f_infer_count = 0;
    for (i = 0; i < DUALCORE_V5F_FEATURE_DIM; i++) {
        slot->v5f_feature_q[i] = 0;
    }
    for (c = 0; c < DUALCORE_ADS1299_ACTIVE_CH_NUM; c++) {
        slot->v5f_ch_code[c] = 0;
        slot->v5f_uv_x1000[c] = 0;
        slot->v5f_pre_x1000[c] = 0;
        slot->v5f_filt_x1000[c] = 0;
    }

    for (i = 0; i < copy_len; i++) {
        slot->frame[i] = frame[i];
    }
    for (; i < DUALCORE_IPC_FRAME_LEN; i++) {
        slot->frame[i] = 0;
    }

    checksum = DualCore_IPC_Checksum16(slot->frame, DUALCORE_IPC_FRAME_LEN);
    DualCore_ADS1299_ParseRawFrame(slot->frame, &status, ch_data);

    slot->len = copy_len;
    slot->checksum = checksum;
    slot->seq = seq;

    g_ipc_v3f_tx_checksum = checksum;
    g_ipc_v3f_tx_checksum_hist[hist_idx] = checksum;
    g_ipc_v3f_tx_status_hist[hist_idx] = status;
    for (c = 0; c < DUALCORE_ADS1299_ACTIVE_CH_NUM; c++) {
        g_ipc_v3f_tx_ch_hist[hist_idx][c] = ch_data[g_v5f_ch_map[c]];
    }

    DUALCORE_FENCE();

    g_ipc_v3f_notify_count = seq;
    IPC_WriteMSG(IPC_MSG0, (uint32_t)slot);

    DUALCORE_FENCE();

    NVIC_EnableIRQ(IPC_CH0_IRQn);

    IPC_ITConfig(IPC_CH0, IPC_CH_Sta_Bit1, ENABLE);
}

void IPC_CH0_Handler(void)
{
    if (IPC_GetITStatus(IPC_CH0, IPC_CH_Sta_Bit0) != RESET) {
        uint32_t ack_pack = IPC_ReadMSG(IPC_MSG0);
        uint32_t ack_seq_low = (ack_pack & 0x0000FFFFu);
        uint32_t slot_idx = ack_seq_low % DUALCORE_IPC_FRAME_SLOT_NUM;
        uint32_t hist_idx = ack_seq_low % DUALCORE_IPC_TX_HISTORY_SIZE;

        g_ipc_v3f_ack_count++;
        g_ipc_v3f_ack_checksum = (uint16_t)(ack_pack >> 16);
        g_ipc_v3f_ack_tx_checksum = g_ipc_v3f_tx_checksum_hist[hist_idx];

        if (g_ipc_v3f_ack_checksum == g_ipc_v3f_ack_tx_checksum) {
            g_ipc_v3f_checksum_ok++;
        } else {
            g_ipc_v3f_checksum_bad++;
        }

        volatile DualCore_IPC_FrameSlot_t *slot = &g_ipc_v3f_frame_slot[slot_idx];
        uint32_t ack_seq = slot->seq;
        uint8_t parse_ok = 1u;
        uint16_t c;

        if ((ack_seq & 0x0000FFFFu) != ack_seq_low) {
            ack_seq = ack_seq_low;
        }

        DUALCORE_FENCE();

        if ((slot->seq != ack_seq) || (slot->v5f_parse_valid == 0u)) {
            parse_ok = 0u;
        } else {
            if (slot->v5f_status != g_ipc_v3f_tx_status_hist[hist_idx]) {
                parse_ok = 0u;
            }
            for (c = 0; c < DUALCORE_ADS1299_ACTIVE_CH_NUM; c++) {
                if (slot->v5f_ch_code[c] != g_ipc_v3f_tx_ch_hist[hist_idx][c]) {
                    parse_ok = 0u;
                }
            }
        }

        g_ipc_v3f_last_v3f_ch0 = g_ipc_v3f_tx_ch_hist[hist_idx][0];
        g_ipc_v3f_last_v5f_ch0 = slot->v5f_ch_code[0];
        g_ipc_v3f_last_v5f_uv0_x1000 = slot->v5f_uv_x1000[0];
        g_ipc_v3f_last_v5f_pre0_x1000 = slot->v5f_pre_x1000[0];
        g_ipc_v3f_last_v5f_filt0_x1000 = slot->v5f_filt_x1000[0];
        g_ipc_v3f_last_v5f_sample_count = slot->v5f_sample_count;
        g_ipc_v3f_last_v5f_window_count = slot->v5f_window_count;
        g_ipc_v3f_last_v5f_fft_count = slot->v5f_fft_count;
        g_ipc_v3f_last_v5f_feature_valid = slot->v5f_feature_valid;
        for (c = 0; c < DUALCORE_V5F_FEATURE_DIM; c++) {
            g_ipc_v3f_last_v5f_feature_q[c] = slot->v5f_feature_q[c];
        }
        g_ipc_v3f_last_v5f_infer_valid = slot->v5f_infer_valid;
        g_ipc_v3f_last_v5f_pred = slot->v5f_pred;
        g_ipc_v3f_last_v5f_model_trained = slot->v5f_model_trained;
        g_ipc_v3f_last_v5f_model_used = slot->v5f_model_used;
        g_ipc_v3f_last_v5f_score_left = slot->v5f_score_left;
        g_ipc_v3f_last_v5f_score_right = slot->v5f_score_right;
        g_ipc_v3f_last_v5f_confidence = slot->v5f_confidence;
        g_ipc_v3f_last_v5f_infer_count = slot->v5f_infer_count;

        if (parse_ok) {
            g_ipc_v3f_parse_ok++;
        } else {
            g_ipc_v3f_parse_bad++;
        }

        IPC_ITConfig(IPC_CH0, IPC_CH_Sta_Bit0, DISABLE);
    }
}

#endif
