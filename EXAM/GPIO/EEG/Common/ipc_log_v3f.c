#include "ipc_log.h"
#include <string.h>

#if defined(Core_V3F)
#include "Serial.h"

static uint8_t s_v3f_print_buf[IPC_LOG_BUF_SIZE];
static uint32_t s_v3f_cmd_seq = 0;

void IPC_Log_Init_V3F(void)
{
    volatile IPC_Log_Shared_t *shared = IPC_LOG_SHARED;
    memset((void *)shared, 0, sizeof(IPC_Log_Shared_t));
    shared->v5f_status = IPC_LOG_STATUS_IDLE;

    IPC_DeInit();
    {
        IPC_InitTypeDef ipc_init;
        IPC_StructInit(&ipc_init);
        ipc_init.IPC_CH = IPC_CH0;
        ipc_init.TxCID = IPC_TxCID1;
        ipc_init.RxCID = IPC_RxCID0;
        IPC_Init(&ipc_init);
    }
    IPC_CH0_Lock();
    IPC_WriteMSG(IPC_MSG0, 0);
    IPC_SetFlagStatus(IPC_CH0, IPC_CH_Sta_Bit0);
    IPC_ClearFlagStatus(IPC_CH0, IPC_CH_Sta_Bit1);
    NVIC_SetPriority(IPC_CH0_IRQn, (1 << 7) | (1 << 4));
    NVIC_EnableIRQ(IPC_CH0_IRQn);

    s_v3f_cmd_seq = 0;
}

void IPC_Log_Poll_V3F(void)
{
    volatile IPC_Log_Shared_t *shared = IPC_LOG_SHARED;
    uint32_t wp = shared->write_pos;
    uint32_t rp = shared->read_pos;

    if (wp == rp) return;

    uint32_t avail;
    if (wp > rp) {
        avail = wp - rp;
    } else {
        avail = IPC_LOG_BUF_SIZE - rp + wp;
    }

    uint32_t to_read = avail;
    if (to_read > sizeof(s_v3f_print_buf) - 1)
        to_read = sizeof(s_v3f_print_buf) - 1;

    uint32_t i;
    for (i = 0; i < to_read; i++) {
        s_v3f_print_buf[i] = shared->buf[(rp + i) % IPC_LOG_BUF_SIZE];
    }
    s_v3f_print_buf[to_read] = '\0';

    shared->read_pos = (rp + to_read) % IPC_LOG_BUF_SIZE;

    Serial_Printf(SERIAL_PORT_DEBUG, "%s", s_v3f_print_buf);
}

void IPC_Cmd_Send_V3F(uint32_t cmd, uint32_t param)
{
    volatile IPC_Log_Shared_t *shared = IPC_LOG_SHARED;
    s_v3f_cmd_seq++;
    shared->cmd = cmd;
    shared->cmd_param = param;
    __asm volatile ("fence iorw, iorw" ::: "memory");
    shared->cmd_seq = s_v3f_cmd_seq;

    IPC_WriteMSG(IPC_MSG0, (uint32_t)shared);
    __asm volatile ("fence iorw, iorw" ::: "memory");
    IPC_ITConfig(IPC_CH0, IPC_CH_Sta_Bit1, ENABLE);
}

void IPC_Ctrl_SetFlags_V3F(uint32_t flags)
{
    IPC_LOG_SHARED->ctrl_flags = flags;
    __asm volatile ("fence iorw, iorw" ::: "memory");
}

uint8_t IPC_GetV5FPred_V3F(void)
{
    return (uint8_t)IPC_LOG_SHARED->v5f_pred;
}

int32_t IPC_GetV5FConfidence_V3F(void)
{
    return IPC_LOG_SHARED->v5f_confidence;
}

int32_t IPC_GetV5FScoreLeft_V3F(void)
{
    return IPC_LOG_SHARED->v5f_score_left;
}

int32_t IPC_GetV5FScoreRight_V3F(void)
{
    return IPC_LOG_SHARED->v5f_score_right;
}

int8_t IPC_GetSSVEPIndex_V3F(void)
{
    return (int8_t)IPC_LOG_SHARED->ssvep_raw_index;
}

uint8_t IPC_GetFeatureValid_V3F(void)
{
    return (uint8_t)IPC_LOG_SHARED->feature_valid;
}

const volatile int32_t *IPC_GetFeatureV3F(void)
{
    return IPC_LOG_SHARED->feature_q;
}

uint32_t IPC_GetFeatureSeq_V3F(void)
{
    return IPC_LOG_SHARED->feature_seq;
}

uint32_t IPC_GetSampleCount_V3F(void)
{
    return IPC_LOG_SHARED->sample_count;
}

#ifdef GLXSS_ENABLED
void IPC_CH0_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void IPC_CH0_Handler(void)
{
    if (IPC_GetITStatus(IPC_CH0, IPC_CH_Sta_Bit0) != RESET) {
        IPC_ITConfig(IPC_CH0, IPC_CH_Sta_Bit0, DISABLE);
    }
}
#endif

#endif
