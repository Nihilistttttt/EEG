#include "dualcore_ipc.h"
#include "dualcore_ipc_shared.h"
#include "dualcore_v5f_dsp.h"
#include <string.h>

#if defined(Core_V5F) && DUALCORE_IPC_RUNTIME_ENABLE

static volatile uint32_t g_ipc_v5f_recv_count      = 0;
static volatile uint32_t g_ipc_v5f_last_seq        = 0;
static volatile uint32_t g_ipc_v5f_last_checksum   = 0;
volatile uint32_t g_ipc_v5f_window_count    = 0;
volatile uint16_t g_ipc_v5f_window_samples  = 0;
static volatile uint8_t  g_ipc_v5f_ready           = 0;
static volatile uint8_t  g_ipc_v5f_pending_flag    = 0;
static volatile uint32_t g_ipc_v5f_pending_share_addr = 0;
static volatile uint8_t  g_ipc_v5f_last_frame[DUALCORE_IPC_FRAME_LEN];
static volatile uint32_t g_ipc_v5f_handler_count = 0;
static volatile uint8_t  g_ipc_v5f_ack_ready = 0;
static volatile uint32_t g_ipc_v5f_ack_data = 0;

void IPC_CH0_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

static void DualCore_V5F_SendAckNow(void)
{
    if (!g_ipc_v5f_ack_ready) {
        return;
    }

    IPC_WriteMSG(IPC_MSG0, g_ipc_v5f_ack_data);
    DUALCORE_FENCE();
    IPC_ITConfig(IPC_CH0, IPC_CH_Sta_Bit0, DISABLE);
    IPC_ITConfig(IPC_CH0, IPC_CH_Sta_Bit0, ENABLE);
    IPC_SetFlagStatus(IPC_CH0, IPC_CH_Sta_Bit0);
    g_ipc_v5f_ack_ready = 0u;
}

static int32_t DualCore_FloatVoltToMicroVoltX1000(float volt)
{
    float v = volt * 1000000000.0f;
    if (v >= 0.0f) {
        v += 0.5f;
    } else {
        v -= 0.5f;
    }
    return (int32_t)v;
}

static void DualCore_V5F_ProcessPreprocess(volatile DualCore_IPC_FrameSlot_t *slot,
                                           int32_t ch_data[DUALCORE_ADS1299_CHANNEL_NUM])
{
    uint8_t c;
    uint8_t i;

    g_ipc_v5f_window_samples++;
    g_v5f_step_count++;
    if (g_ipc_v5f_window_samples >= DUALCORE_V5F_WINDOW_SIZE) {
        g_ipc_v5f_window_samples = 0;
        g_ipc_v5f_window_count++;
    }

    if (slot->model_select <= DUALCORE_V5F_MODEL_SELECT_CSP) {
        g_ipc_v5f_model_select = slot->model_select;
    } else {
        g_ipc_v5f_model_select = DUALCORE_V5F_MODEL_SELECT_AUTO;
    }

    slot->v5f_sample_count = g_ipc_v5f_recv_count;
    slot->v5f_window_count = g_ipc_v5f_window_count;

    for (c = 0; c < DUALCORE_ADS1299_ACTIVE_CH_NUM; c++) {
        uint8_t src_ch = c + 4u;
        int32_t uv_x1000 = DualCore_ADS1299_CodeToMicroVoltX1000(ch_data[src_ch]);
        float volt = (float)uv_x1000 / 1000000000.0f;
        float drift = DualCore_RemoveRealtimeDrift(volt, &g_v5f_drift[c]);
        float pre = DualCore_IIR_SOS_Step(drift, &g_v5f_notch_coeff, &g_v5f_notch_state[c]);
        float filt = DualCore_IIR_SOS_Step(pre, &g_v5f_bandpass_coeff, &g_v5f_bandpass_state[c]);
        float csp_filt = DualCore_IIR_SOS_Step(pre,
                                               &g_v5f_csp_bandpass_coeff,
                                               &g_v5f_csp_bandpass_state[c]);

        slot->v5f_uv_x1000[c] = uv_x1000;
        slot->v5f_pre_x1000[c] = DualCore_FloatVoltToMicroVoltX1000(pre);
        slot->v5f_filt_x1000[c] = DualCore_FloatVoltToMicroVoltX1000(filt);

        g_v5f_ring[c][g_v5f_ring_write_idx] = filt;
        g_v5f_csp_ring[c][g_v5f_ring_write_idx] = csp_filt;
    }

    g_v5f_ring_write_idx = (uint16_t)((g_v5f_ring_write_idx + 1u) & (DUALCORE_V5F_FFT_SIZE - 1u));
    if (g_v5f_ring_valid_count < DUALCORE_V5F_FFT_SIZE) {
        g_v5f_ring_valid_count++;
    }

    if ((g_v5f_ring_valid_count >= DUALCORE_V5F_FFT_SIZE) &&
        (g_v5f_step_count >= DUALCORE_V5F_STEP_SIZE)) {
        g_v5f_step_count = 0;
        DualCore_V5F_ComputeFFTFeature();
    }

    slot->v5f_feature_valid = g_ipc_v5f_feature_valid;
    slot->v5f_fft_count = g_ipc_v5f_fft_count;
    for (i = 0; i < DUALCORE_V5F_FEATURE_DIM; i++) {
        slot->v5f_feature_q[i] = g_ipc_v5f_feature_q[i];
    }
    slot->v5f_infer_valid = g_ipc_v5f_infer_valid;
    slot->v5f_pred = g_ipc_v5f_pred;
    slot->v5f_model_trained = g_ipc_v5f_model_trained;
    slot->v5f_model_used = g_ipc_v5f_model_used;
    slot->v5f_score_left = g_ipc_v5f_score_left;
    slot->v5f_score_right = g_ipc_v5f_score_right;
    slot->v5f_confidence = g_ipc_v5f_confidence;
    slot->v5f_infer_count = g_ipc_v5f_infer_count;
}

void DualCore_V5F_MainLoopProcess(void)
{
    uint32_t share_addr;
    uint16_t checksum = 0;
    uint32_t seq = 0;
    uint32_t latest_seq = 0;
    uint32_t start_seq = 0;
    uint32_t latest_idx = 0;
    uint32_t status = 0;
    int32_t ch_data[DUALCORE_ADS1299_CHANNEL_NUM];
    uint16_t i;
    uint16_t c;
    uint8_t processed_any = 0u;

    if (!g_ipc_v5f_pending_flag) {
        return;
    }

    g_ipc_v5f_pending_flag = 0u;
    share_addr = g_ipc_v5f_pending_share_addr;

    if (share_addr < 0x20110000u || share_addr > 0x2017FFFFu) {
        share_addr = 0u;
    }

    if (g_ipc_v5f_ready && share_addr != 0u) {
        volatile DualCore_IPC_FrameSlot_t *latest_slot = (volatile DualCore_IPC_FrameSlot_t *)share_addr;
        volatile DualCore_IPC_FrameSlot_t *base_slot;

        DUALCORE_FENCE();

        latest_seq = latest_slot->seq;
        latest_idx = latest_seq % DUALCORE_IPC_FRAME_SLOT_NUM;
        base_slot = latest_slot - latest_idx;

        start_seq = g_ipc_v5f_last_seq + 1u;
        if (latest_seq >= DUALCORE_IPC_FRAME_SLOT_NUM) {
            uint32_t oldest_valid_seq = latest_seq - DUALCORE_IPC_FRAME_SLOT_NUM + 1u;
            if (start_seq < oldest_valid_seq) {
                start_seq = oldest_valid_seq;
            }
        }

        for (seq = start_seq; seq <= latest_seq; seq++) {
            volatile DualCore_IPC_FrameSlot_t *slot = &base_slot[seq % DUALCORE_IPC_FRAME_SLOT_NUM];

            DUALCORE_FENCE();
            if (slot->seq != seq) {
                continue;
            }

            for (i = 0; i < DUALCORE_IPC_FRAME_LEN; i++) {
                g_ipc_v5f_last_frame[i] = slot->frame[i];
            }

            checksum = DualCore_IPC_Checksum16(g_ipc_v5f_last_frame, DUALCORE_IPC_FRAME_LEN);
            DualCore_ADS1299_ParseRawFrame(g_ipc_v5f_last_frame, &status, ch_data);

            g_ipc_v5f_recv_count++;

            slot->v5f_status = status;
            for (c = 0; c < DUALCORE_ADS1299_ACTIVE_CH_NUM; c++) {
                slot->v5f_ch_code[c] = ch_data[c + 4u];
            }

            DualCore_V5F_ProcessPreprocess(slot, ch_data);

            DUALCORE_FENCE();
            slot->v5f_parse_valid = 1u;

            g_ipc_v5f_last_seq = seq;
            g_ipc_v5f_last_checksum = checksum;
            processed_any = 1u;
        }

        if (!processed_any) {
            seq = latest_seq;
            checksum = 0xDDDDu;
        } else {
            seq = g_ipc_v5f_last_seq;
            checksum = (uint16_t)g_ipc_v5f_last_checksum;
        }
    } else {
        checksum = 0xEEEEu;
        seq = 0u;
    }

    g_ipc_v5f_ack_data = (((uint32_t)checksum) << 16) | (seq & 0x0000FFFFu);
    g_ipc_v5f_ack_ready = 1u;
    DualCore_V5F_SendAckNow();
}

void DualCore_IPC_Init_V5F(void)
{
    uint16_t i;

    g_ipc_v5f_recv_count = 0;
    g_ipc_v5f_last_seq = 0;
    g_ipc_v5f_last_checksum = 0;
    g_ipc_v5f_ready = 0;
    g_ipc_v5f_window_count = 0;
    g_ipc_v5f_window_samples = 0;
    g_ipc_v5f_pending_flag = 0;
    g_ipc_v5f_pending_share_addr = 0;
    g_ipc_v5f_ack_ready = 0u;
    g_ipc_v5f_ack_data = 0u;

    for (i = 0; i < DUALCORE_IPC_FRAME_LEN; i++) {
        g_ipc_v5f_last_frame[i] = 0;
    }

    DualCore_V5F_DSP_Reset();

    IPC_ClearFlagStatus(IPC_CH0, IPC_CH_Sta_Bit1);
    NVIC_ClearPendingIRQ(IPC_CH0_IRQn);

    NVIC_SetPriority(IPC_CH0_IRQn, (2 << 5) | (0 << 4));
    NVIC_EnableIRQ(IPC_CH0_IRQn);

    g_ipc_v5f_ready = 1;
}

void IPC_CH0_Handler(void)
{
    if (IPC_GetITStatus(IPC_CH0, IPC_CH_Sta_Bit1) != RESET) {
        g_ipc_v5f_handler_count++;
        IPC_WriteMSG(IPC_MSG2, g_ipc_v5f_handler_count);

        uint32_t addr = IPC_ReadMSG(IPC_MSG0);
        if (addr >= 0x20110000u && addr <= 0x2017FFFFu) {
            g_ipc_v5f_pending_share_addr = addr;
            g_ipc_v5f_pending_flag = 1u;
        }
    }

    IPC_ITConfig(IPC_CH0, IPC_CH_Sta_Bit1, DISABLE);

    DualCore_V5F_SendAckNow();
}

#endif
