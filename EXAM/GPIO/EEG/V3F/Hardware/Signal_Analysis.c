#include "Signal_Analysis.h"
#include "eeg_filter.h"
#include "eeg_fft.h"
#include "eeg_direction_feature.h"
#include "eeg_direction_collect.h"

#include "eeg_cmd_parser.h"
#include "eeg_ab_extract.h"
#include "OLED.h"
#include "Serial.h"
#include "Message_Parser.h"
#include "debug.h"
#include "ADS1299.h"

#include "hardware.h"
#include "ICM42605.h"
#include "Timer_1ms.h"
#include "dualcore_ipc.h"
#include <math.h>
#include <string.h>
#include <stdio.h>


#define DUALCORE_IPC_FRAME_SHARE_ENABLE        1
#define DUALCORE_IPC_FRAME_NOTIFY_EVERY_FRAMES 1
#define DUALCORE_IPC_FRAME_PRINT_EVERY_ACKS    500
uint8_t g_ipc_diag_enable = 1;

#define DIR_1S_TEST_TEXT_ONLY       0
#define AB_REALTIME_NOTCH_ENABLE    1
#define AB_EXTRACT_PRINT_ENABLE     0
#define AB_FFT_DETREND_ENABLE       1

uint8_t g_eeg_app_mode = EEG_APP_MODE_COLLECT;

void Signal_Analysis_Start(void)
{
    Serial_Init(SERIAL_PORT_DEBUG);
    Serial_Init(SERIAL_PORT_WIFI);

    Serial_Printf(SERIAL_PORT_DEBUG, "System Start\r\n");

    ICM42605_Status icm_status = ICM42605_BCI_Init();
    Timer_1ms_Init();
    Serial_Printf(SERIAL_PORT_DEBUG, "ICM42605_INIT=%d\r\n", (int)icm_status);

    Serial_Printf(SERIAL_PORT_DEBUG, "ADS1299_Init start\r\n");
    uint8_t id = ADS1299_Init();
    Serial_Printf(SERIAL_PORT_DEBUG, "ADS1299_Init done, id=%d\r\n", id);

    EEG_FFT_Init();

#if AB_EXTRACT_PRINT_ENABLE
    Serial_Printf(DIR_TEXT_PORT, "ABCFG,fs=%d,fft=%d,step=%d,alpha=8-13Hz,beta=13-30Hz,drift_k=9960/10000,notch=%d,detrend=%d,power_scale=1e15,pct_scale=10000,db_scale=100\r\n",
                  (int)SAMPLE_RATE, FFT_SIZE, STEP_SIZE, AB_REALTIME_NOTCH_ENABLE, AB_FFT_DETREND_ENABLE);
    Serial_Printf(DIR_TEXT_PORT, "ABCSV,ch0_alpha_p15,ch1_alpha_p15,ch0_beta_p15,ch1_beta_p15,ch0_alpha_pct,ch1_alpha_pct,ch0_beta_pct,ch1_beta_pct,ch0_alpha_db100,ch1_alpha_db100,ch0_beta_db100,ch1_beta_db100,alpha_lr,beta_lr,ch0_beta_alpha,ch1_beta_alpha,total_lr\r\n");
#endif

    if (g_eeg_app_mode == EEG_APP_MODE_COLLECT) {
        Serial_Printf(DIR_TEXT_PORT, "EEG_MODE,COLLECT,rest_s=5,left_right_alternate=1,debug_output=TASK_DIRCSV\r\n");
        Direction_PrintFeatureHeader();
    } else if (g_eeg_app_mode == EEG_APP_MODE_COLLECT_CSP) {
        Serial_Printf(DIR_TEXT_PORT, "EEG_MODE,CSP,collect_window=256,step=125\r\n");
    } else if (g_eeg_app_mode == EEG_APP_MODE_INFER) {
        Serial_Printf(DIR_TEXT_PORT, "EEG_MODE,INFER,result_period_s=2\r\n");
    }

    uint8_t frame_buf[ADS1299_FRAME_BYTE_NUM];
    int32_t ch_data[ADS1299_CHANNEL_NUM];
    static uint8_t fft_active = 0;

#if DUALCORE_IPC_FRAME_SHARE_ENABLE
    static uint32_t ipc_frame_count = 0;
    static uint32_t ipc_last_print_ack = 0;
#endif

    while (1) {
#if CMD_MODE_ENABLE
        if (Serial_IsDataReady(SERIAL_PORT_DEBUG)) {
            uint8_t *cmd_buf;
            uint16_t len = Serial_GetDataPacket(SERIAL_PORT_DEBUG, &cmd_buf);
            if (len > 0 && len < 64) {
                char cmd_str[64];
                memcpy(cmd_str, cmd_buf, len);
                cmd_str[len] = '\0';
                Parse_Command(cmd_str);
            }
        }
#endif

    if (ring_buffer_get_frame(frame_buf)) {

#if DUALCORE_IPC_FRAME_SHARE_ENABLE
        ipc_frame_count++;
        if (ipc_frame_count >= DUALCORE_IPC_FRAME_NOTIFY_EVERY_FRAMES) {
            ipc_frame_count = 0;
            DualCore_IPC_SendFrameFromV3F(frame_buf, ADS1299_FRAME_BYTE_NUM);

            if (g_ipc_diag_enable) {
                static uint32_t ipc_diag_count = 0;
                ipc_diag_count++;
                if ((ipc_diag_count % 50u) == 0u) {
                    Serial_Printf(DIR_TEXT_PORT,
                        "IPCDIAG,ack=%lu,notify=%lu,ok=%lu,bad=%lu,v5fhb=%lu,ENA=%08lX,STS=%08lX,ISR=%08lX\r\n",
                        (unsigned long)DualCore_IPC_GetAckCount(),
                        (unsigned long)DualCore_IPC_GetNotifyCount(),
                        (unsigned long)DualCore_IPC_GetParseOKCount(),
                        (unsigned long)DualCore_IPC_GetParseBadCount(),
                        (unsigned long)DualCore_IPC_GetV5FHandlerCount(),
                        (unsigned long)IPC->ENA,
                        (unsigned long)IPC->STS,
                        (unsigned long)IPC->ISR);
                }
            }

            uint32_t ipc_ack = DualCore_IPC_GetAckCount();
            if ((ipc_ack - ipc_last_print_ack) >= DUALCORE_IPC_FRAME_PRINT_EVERY_ACKS) {
                ipc_last_print_ack = ipc_ack;
            }
            (void)ipc_ack;
        }
#endif

        ADS1299_ParseRawFrame(frame_buf, NULL, ch_data);

        float val_ch0 = ADS1299_CodeToVolt(ch_data[ADS1299_EEG_CH_CP3], ADS1299_VREF_DEFAULT, ADS1299_GAIN_DEFAULT);
        float val_ch1 = ADS1299_CodeToVolt(ch_data[ADS1299_EEG_CH_CP4], ADS1299_VREF_DEFAULT, ADS1299_GAIN_DEFAULT);
        float val_ch2 = ADS1299_CodeToVolt(ch_data[ADS1299_EEG_CH_C3],  ADS1299_VREF_DEFAULT, ADS1299_GAIN_DEFAULT);
        float val_ch3 = ADS1299_CodeToVolt(ch_data[ADS1299_EEG_CH_C4],  ADS1299_VREF_DEFAULT, ADS1299_GAIN_DEFAULT);
        float show_ch0 = 0.0f;
        float show_ch1 = 0.0f;

        Waveform_RemoveDisplayBaseline(val_ch2, val_ch3, &show_ch0, &show_ch1);
#if !DIR_1S_TEST_TEXT_ONLY
        Send_Waveform(CMD_RAW_WAVE, show_ch0, show_ch1);
#endif

        float drift_ch0 = EEG_RemoveRealtimeDrift(val_ch0, &AB_Drift_CH0);
        float drift_ch1 = EEG_RemoveRealtimeDrift(val_ch1, &AB_Drift_CH1);
        float drift_ch2 = EEG_RemoveRealtimeDrift(val_ch2, &AB_Drift_CH2);
        float drift_ch3 = EEG_RemoveRealtimeDrift(val_ch3, &AB_Drift_CH3);

#if AB_REALTIME_NOTCH_ENABLE
        float ab_pre_ch0 = IIR_SOS_Step(drift_ch0, &g_notch_coeff, &g_notch_state[0]);
        float ab_pre_ch1 = IIR_SOS_Step(drift_ch1, &g_notch_coeff, &g_notch_state[1]);
        float ab_pre_ch2 = IIR_SOS_Step(drift_ch2, &g_notch_coeff, &g_notch_state[2]);
        float ab_pre_ch3 = IIR_SOS_Step(drift_ch3, &g_notch_coeff, &g_notch_state[3]);
#else
        float ab_pre_ch0 = drift_ch0;
        float ab_pre_ch1 = drift_ch1;
        float ab_pre_ch2 = drift_ch2;
        float ab_pre_ch3 = drift_ch3;
#endif

        float filtered_ch0 = IIR_SOS_Step(ab_pre_ch0, &g_bandpass_coeff, &g_bandpass_state[0]);
        float filtered_ch1 = IIR_SOS_Step(ab_pre_ch1, &g_bandpass_coeff, &g_bandpass_state[1]);
        float filtered_ch2 = IIR_SOS_Step(ab_pre_ch2, &g_bandpass_coeff, &g_bandpass_state[2]);
        float filtered_ch3 = IIR_SOS_Step(ab_pre_ch3, &g_bandpass_coeff, &g_bandpass_state[3]);

        RingBufFiltered.CH0[RingBufFiltered.WriteIdx] = filtered_ch0;
        RingBufFiltered.CH1[RingBufFiltered.WriteIdx] = filtered_ch1;
        RingBufFiltered.CH2[RingBufFiltered.WriteIdx] = filtered_ch2;
        RingBufFiltered.CH3[RingBufFiltered.WriteIdx] = filtered_ch3;
        RingBufFiltered.WriteIdx = (RingBufFiltered.WriteIdx + 1) % FFT_SIZE;
        if (csp_filtered_sample_count < 0xFFFFFFFFUL) {
            csp_filtered_sample_count++;
        }

#if !DIR_1S_TEST_TEXT_ONLY
        Send_Waveform(CMD_FILT_WAVE, filtered_ch2, filtered_ch3);
#endif

        RingBuf.CH0[RingBuf.WriteIdx] = ab_pre_ch0;
        RingBuf.CH1[RingBuf.WriteIdx] = ab_pre_ch1;
        RingBuf.CH2[RingBuf.WriteIdx] = ab_pre_ch2;
        RingBuf.CH3[RingBuf.WriteIdx] = ab_pre_ch3;
        RingBuf.WriteIdx = (RingBuf.WriteIdx + 1) % FFT_SIZE;

        Update_Waveform(show_ch0);

        New_Samples_Count++;
        if (New_Samples_Count >= STEP_SIZE && !fft_active) {
            Process_FFT();
            fft_active = 1;
        }

        if (fft_active) {
            if (Process_FFT_Step()) {
                fft_active = 0;
                New_Samples_Count = 0;
            }
        }
    }

    ICM42605_Task();
    }
}
