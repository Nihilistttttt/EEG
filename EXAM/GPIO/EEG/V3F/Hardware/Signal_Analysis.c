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
#ifdef HAS_ICM42605
#include "ICM42605.h"
#include "Timer_1ms.h"
#include "posture_detect.h"
#include "patient_monitor.h"
#endif
#include "dualcore_ipc.h"
#include <math.h>
#include <string.h>
#include <stdio.h>


#define DUALCORE_IPC_FRAME_SHARE_ENABLE 1
#define DUALCORE_IPC_FRAME_NOTIFY_EVERY_FRAMES 1
#define DUALCORE_IPC_FRAME_PRINT_EVERY_ACKS 500
uint8_t g_ipc_diag_enable = 0;
#ifdef HAS_ICM42605
uint8_t g_posture_diag_enable = 0;
#else
volatile uint32_t g_icm42605_ms_tick = 0;
#endif

#define DIR_1S_TEST_TEXT_ONLY 0
#define AB_REALTIME_NOTCH_ENABLE 1
#define AB_EXTRACT_PRINT_ENABLE 0
#define AB_FFT_DETREND_ENABLE 1

uint8_t g_eeg_app_mode = EEG_APP_MODE_COLLECT;

void Signal_Analysis_Start (void) {
    Serial_Init (SERIAL_PORT_DEBUG);
    Serial_Init (SERIAL_PORT_WIFI);


#ifdef HAS_ICM42605
    ICM42605_Status icm_status = ICM42605_BCI_Init();
    Timer_1ms_Init();
    Serial_Printf (SERIAL_PORT_DEBUG, "ICM42605_INIT=%d\r\n", (int)icm_status);
    OLED_ShowString(SPI,0,0,"ICM42605 = ");
    OLED_ShowNum(SPI,1,0,(int)icm_status,4);
    EEG_FFT_Init();
    DisplayConfig_SetDefaults(&g_display_config);
#endif

    // Serial_Printf (SERIAL_PORT_DEBUG, "ADS1299_Init start\r\n");
    uint8_t id = ADS1299_Init();
    Serial_Printf (SERIAL_PORT_DEBUG, "ADS1299_Init done, id=%d\r\n", id);
    OLED_ShowString(SPI,2,0,"ADS1299 id = ");
    OLED_ShowNum(SPI,2,13,id,2);
#if AB_EXTRACT_PRINT_ENABLE
    Serial_Printf (DIR_TEXT_PORT, "ABCFG,fs=%d,fft=%d,step=%d,alpha=8-13Hz,beta=13-30Hz,drift_k=9960/10000,notch=%d,detrend=%d,power_scale=1e15,pct_scale=10000,db_scale=100\r\n",
                   (int)SAMPLE_RATE, FFT_SIZE, STEP_SIZE, AB_REALTIME_NOTCH_ENABLE, AB_FFT_DETREND_ENABLE);
    Serial_Printf (DIR_TEXT_PORT, "ABCSV,ch0_alpha_p15,ch1_alpha_p15,ch0_beta_p15,ch1_beta_p15,ch0_alpha_pct,ch1_alpha_pct,ch0_beta_pct,ch1_beta_pct,ch0_alpha_db100,ch1_alpha_db100,ch0_beta_db100,ch1_beta_db100,alpha_lr,beta_lr,ch0_beta_alpha,ch1_beta_alpha,total_lr\r\n");
#endif

    if (g_eeg_app_mode == EEG_APP_MODE_COLLECT) {
        Serial_Printf (DIR_TEXT_PORT, "EEG_MODE,COLLECT,rest_s=5,left_right_alternate=1,debug_output=TASK_DIRCSV\r\n");
        Direction_PrintFeatureHeader();
    } else if (g_eeg_app_mode == EEG_APP_MODE_COLLECT_CSP) {
        Serial_Printf (DIR_TEXT_PORT, "EEG_MODE,CSP,collect_window=256,step=125\r\n");
    } else if (g_eeg_app_mode == EEG_APP_MODE_INFER) {
        Serial_Printf (DIR_TEXT_PORT, "EEG_MODE,INFER,result_period_s=2\r\n");
    }

    uint8_t frame_buf[ADS1299_FRAME_BYTE_NUM];
    int32_t ch_data[ADS1299_CHANNEL_NUM];
    static uint8_t fft_active = 0;

#if DUALCORE_IPC_FRAME_SHARE_ENABLE
    static uint32_t ipc_frame_count = 0;
    static uint32_t ipc_last_print_ack = 0;
#endif

    while (1) {
#ifndef HAS_ICM42605

        g_icm42605_ms_tick++;
#endif
#if CMD_MODE_ENABLE
        Retry_Tick();
        if (Serial_IsDataReady (SERIAL_PORT_DEBUG)) {
            uint8_t *cmd_buf;
            uint16_t len = Serial_GetDataPacket (SERIAL_PORT_DEBUG, &cmd_buf);
            if (len > 0 && len < 256) {
                char cmd_str[256];
                memcpy (cmd_str, cmd_buf, len);
                cmd_str[len] = '\0';
                char *line = cmd_str;
                while (line && *line) {
                    char *nl = strchr (line, '\n');
                    if (nl) {
                        *nl = '\0';
                    }
                    char *cr = strchr (line, '\r');
                    if (cr) {
                        *cr = '\0';
                    }
                    if (strlen (line) > 0)
                        Parse_CommandEx (line, "PY");
                    line = nl ? nl + 1 : NULL;
                }
            }
        }
        if (Serial_IsDataReady (SERIAL_PORT_WIFI)) {
            uint8_t *cmd_buf;
            uint16_t len = Serial_GetDataPacket (SERIAL_PORT_WIFI, &cmd_buf);
            if (len > 0 && len < 256) {
                char cmd_str[256];
                memcpy (cmd_str, cmd_buf, len);
                cmd_str[len] = '\0';
                char *line = cmd_str;
                while (line && *line) {
                    char *nl = strchr (line, '\n');
                    if (nl) {
                        *nl = '\0';
                    }
                    char *cr = strchr (line, '\r');
                    if (cr) {
                        *cr = '\0';
                    }
                    if (strlen (line) > 0)
                        Parse_CommandEx (line, "AND");
                    line = nl ? nl + 1 : NULL;
                }
            }
        }
#endif

        if (ring_buffer_get_frame (frame_buf)) {

#if DUALCORE_IPC_FRAME_SHARE_ENABLE
            ipc_frame_count++;
            if (ipc_frame_count >= DUALCORE_IPC_FRAME_NOTIFY_EVERY_FRAMES) {
                ipc_frame_count = 0;
                DualCore_IPC_SendFrameFromV3F (frame_buf, ADS1299_FRAME_BYTE_NUM);

                if (g_ipc_diag_enable) {
                    static uint32_t ipc_diag_count = 0;
                    ipc_diag_count++;
                    if ((ipc_diag_count % 50u) == 0u) {
                        Serial_Printf (SERIAL_PORT_DEBUG,
                                       "IPCDIAG,ack=%lu,notify=%lu,ok=%lu,bad=%lu,v5fhb=%lu,ENA=%08lX,STS=%08lX,ISR=%08lX\r\n",
                                       (unsigned long)DualCore_IPC_GetAckCount(),
                                       (unsigned long)DualCore_IPC_GetNotifyCount(),
                                       (unsigned long)DualCore_IPC_GetParseOKCount(),
                                       (unsigned long)DualCore_IPC_GetParseBadCount(),
                                       (unsigned long)DualCore_IPC_GetV5FHandlerCount(),
                                       (unsigned long)IPC->ENA,
                                       (unsigned long)IPC->STS,
                                       (unsigned long)IPC->ISR);
                        Serial_Printf (SERIAL_PORT_WIFI,
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

            ADS1299_ParseRawFrame (frame_buf, NULL, ch_data);

            float raw_vals[NUM_CHANNELS];
            raw_vals[0] = ADS1299_CodeToVolt (ch_data[ADS1299_EEG_CH_OZ], ADS1299_VREF_DEFAULT, ADS1299_GAIN_DEFAULT);
            raw_vals[1] = ADS1299_CodeToVolt (ch_data[ADS1299_EEG_CH_O1], ADS1299_VREF_DEFAULT, ADS1299_GAIN_DEFAULT);
            raw_vals[2] = ADS1299_CodeToVolt (ch_data[ADS1299_EEG_CH_F3], ADS1299_VREF_DEFAULT, ADS1299_GAIN_DEFAULT);
            raw_vals[3] = ADS1299_CodeToVolt (ch_data[ADS1299_EEG_CH_F4], ADS1299_VREF_DEFAULT, ADS1299_GAIN_DEFAULT);
            raw_vals[4] = ADS1299_CodeToVolt (ch_data[ADS1299_EEG_CH_CP3], ADS1299_VREF_DEFAULT, ADS1299_GAIN_DEFAULT);
            raw_vals[5] = ADS1299_CodeToVolt (ch_data[ADS1299_EEG_CH_CP4], ADS1299_VREF_DEFAULT, ADS1299_GAIN_DEFAULT);
            raw_vals[6] = ADS1299_CodeToVolt (ch_data[ADS1299_EEG_CH_C3], ADS1299_VREF_DEFAULT, ADS1299_GAIN_DEFAULT);
            raw_vals[7] = ADS1299_CodeToVolt (ch_data[ADS1299_EEG_CH_C4], ADS1299_VREF_DEFAULT, ADS1299_GAIN_DEFAULT);

            float baseline_vals[NUM_CHANNELS];
            Waveform_RemoveDisplayBaseline_8CH (raw_vals, baseline_vals);

            float drift_vals[NUM_CHANNELS];
            drift_vals[0] = EEG_RemoveRealtimeDrift (raw_vals[0], &AB_Drift_CH0);
            drift_vals[1] = EEG_RemoveRealtimeDrift (raw_vals[1], &AB_Drift_CH1);
            drift_vals[2] = EEG_RemoveRealtimeDrift (raw_vals[2], &AB_Drift_CH2);
            drift_vals[3] = EEG_RemoveRealtimeDrift (raw_vals[3], &AB_Drift_CH3);
            drift_vals[4] = EEG_RemoveRealtimeDrift (raw_vals[4], &AB_Drift_CH4);
            drift_vals[5] = EEG_RemoveRealtimeDrift (raw_vals[5], &AB_Drift_CH5);
            drift_vals[6] = EEG_RemoveRealtimeDrift (raw_vals[6], &AB_Drift_CH6);
            drift_vals[7] = EEG_RemoveRealtimeDrift (raw_vals[7], &AB_Drift_CH7);

#if AB_REALTIME_NOTCH_ENABLE
            float ab_pre_vals[NUM_CHANNELS];
            ab_pre_vals[0] = IIR_SOS_Step (drift_vals[0], &g_notch_coeff, &g_notch_state[0]);
            ab_pre_vals[1] = IIR_SOS_Step (drift_vals[1], &g_notch_coeff, &g_notch_state[1]);
            ab_pre_vals[2] = IIR_SOS_Step (drift_vals[2], &g_notch_coeff, &g_notch_state[2]);
            ab_pre_vals[3] = IIR_SOS_Step (drift_vals[3], &g_notch_coeff, &g_notch_state[3]);
            ab_pre_vals[4] = IIR_SOS_Step (drift_vals[4], &g_notch_coeff, &g_notch_state[4]);
            ab_pre_vals[5] = IIR_SOS_Step (drift_vals[5], &g_notch_coeff, &g_notch_state[5]);
            ab_pre_vals[6] = IIR_SOS_Step (drift_vals[6], &g_notch_coeff, &g_notch_state[6]);
            ab_pre_vals[7] = IIR_SOS_Step (drift_vals[7], &g_notch_coeff, &g_notch_state[7]);
#else
            float ab_pre_vals[NUM_CHANNELS];
            ab_pre_vals[0] = drift_vals[0];
            ab_pre_vals[1] = drift_vals[1];
            ab_pre_vals[2] = drift_vals[2];
            ab_pre_vals[3] = drift_vals[3];
            ab_pre_vals[4] = drift_vals[4];
            ab_pre_vals[5] = drift_vals[5];
            ab_pre_vals[6] = drift_vals[6];
            ab_pre_vals[7] = drift_vals[7];
#endif

            float filt_vals[NUM_CHANNELS];
            filt_vals[0] = DisplayConfig_NeedsBandpass(0u) ?
                           IIR_SOS_Step (ab_pre_vals[0], &g_bandpass_coeff, &g_bandpass_state[0]) : 0.0f;
            filt_vals[1] = DisplayConfig_NeedsBandpass(1u) ?
                           IIR_SOS_Step (ab_pre_vals[1], &g_bandpass_coeff, &g_bandpass_state[1]) : 0.0f;
            filt_vals[2] = IIR_SOS_Step (ab_pre_vals[2], &g_bandpass_coeff, &g_bandpass_state[2]);
            filt_vals[3] = IIR_SOS_Step (ab_pre_vals[3], &g_bandpass_coeff, &g_bandpass_state[3]);
            filt_vals[4] = IIR_SOS_Step (ab_pre_vals[4], &g_bandpass_coeff, &g_bandpass_state[4]);
            filt_vals[5] = IIR_SOS_Step (ab_pre_vals[5], &g_bandpass_coeff, &g_bandpass_state[5]);
            filt_vals[6] = IIR_SOS_Step (ab_pre_vals[6], &g_bandpass_coeff, &g_bandpass_state[6]);
            filt_vals[7] = IIR_SOS_Step (ab_pre_vals[7], &g_bandpass_coeff, &g_bandpass_state[7]);

#if !DIR_1S_TEST_TEXT_ONLY
            {
                for (int wi = 0; wi < DISPLAY_NUM_CH; wi++) {
                    if (g_display_config.wave_type[wi] == WAVE_TYPE_NONE) continue;
                    uint8_t ch = g_display_config.wave_ch[wi];
                    float val;
                    switch (g_display_config.wave_type[wi]) {
                    case WAVE_TYPE_FILT:     val = filt_vals[ch]; break;
                    case WAVE_TYPE_BASELINE: val = baseline_vals[ch]; break;
                    default:                 val = raw_vals[ch]; break;
                    }
                    CmdType wave_cmd = DisplayConfig_GetWaveCmd(g_display_config.wave_type[wi]);
                    int dup = 0;
                    for (int pj = 0; pj < wi; pj++) {
                        if (g_display_config.wave_ch[pj] == ch &&
                            g_display_config.wave_type[pj] == g_display_config.wave_type[wi]) {
                            dup = 1;
                            break;
                        }
                    }
                    if (!dup) {
                        Send_WaveformSingle(wave_cmd, ch, val);
                    }
                }
            }
#endif


            if (DisplayConfig_NeedsBandpass(0u))
                RingBufFiltered.CH0[RingBufFiltered.WriteIdx] = filt_vals[0];
            if (DisplayConfig_NeedsBandpass(1u))
                RingBufFiltered.CH1[RingBufFiltered.WriteIdx] = filt_vals[1];
            RingBufFiltered.CH2[RingBufFiltered.WriteIdx] = filt_vals[2];
            RingBufFiltered.CH3[RingBufFiltered.WriteIdx] = filt_vals[3];
            RingBufFiltered.CH4[RingBufFiltered.WriteIdx] = filt_vals[4];
            RingBufFiltered.CH5[RingBufFiltered.WriteIdx] = filt_vals[5];
            RingBufFiltered.CH6[RingBufFiltered.WriteIdx] = filt_vals[6];
            RingBufFiltered.CH7[RingBufFiltered.WriteIdx] = filt_vals[7];
            RingBufFiltered.WriteIdx = (RingBufFiltered.WriteIdx + 1) % FFT_SIZE;
            if (csp_filtered_sample_count < 0xFFFFFFFFUL) {
                csp_filtered_sample_count++;
            }

            RingBuf.CH0[RingBuf.WriteIdx] = ab_pre_vals[0];
            RingBuf.CH1[RingBuf.WriteIdx] = ab_pre_vals[1];
            RingBuf.CH2[RingBuf.WriteIdx] = ab_pre_vals[2];
            RingBuf.CH3[RingBuf.WriteIdx] = ab_pre_vals[3];
            RingBuf.CH4[RingBuf.WriteIdx] = ab_pre_vals[4];
            RingBuf.CH5[RingBuf.WriteIdx] = ab_pre_vals[5];
            RingBuf.CH6[RingBuf.WriteIdx] = ab_pre_vals[6];
            RingBuf.CH7[RingBuf.WriteIdx] = ab_pre_vals[7];
            RingBuf.WriteIdx = (RingBuf.WriteIdx + 1) % FFT_SIZE;

            Update_Waveform (baseline_vals[g_display_config.wave_ch[0]]);

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

            /* One CSP protocol item per ADS frame prevents TX-ring overflow. */
            Direction_CSPStreamTask();
        }

#ifdef HAS_ICM42605
        ICM42605_Task();

        {
            static uint32_t posture_last_print_tick = 0;
            static Posture_t s_last_wifi_posture = POSTURE_UNKNOWN;
            const PostureResult_t *pr = Posture_GetResult();
            if (g_posture_diag_enable) {
                uint32_t now_tick = g_icm42605_ms_tick;
                if ((uint32_t)(now_tick - posture_last_print_tick) >= 1000u) {
                    char posture_buf[112];
                    int32_t roll_deg10;
                    int32_t pitch_deg10;
                    int32_t yaw_deg10;
                    posture_last_print_tick = now_tick;
                    snprintf (posture_buf, sizeof (posture_buf),
                              "POSTURE,%s,gx=%d,gy=%d,gz=%d,conf=%d,stable=%lu\r\n",
                              Posture_ToString (pr->posture),
                              (int)(pr->gravity_x * 1000.0f),
                              (int)(pr->gravity_y * 1000.0f),
                              (int)(pr->gravity_z * 1000.0f),
                              (int)(pr->confidence * 100.0f),
                              (unsigned long)pr->stable_ms);
                    Serial_Printf (SERIAL_PORT_WIFI, "%s", posture_buf);

                    /*
                     * WIFI keeps the original POSTURE gravity packet exactly
                     * as before. DEBUG/Python receives only readable angles;
                     * no periodic gx/gy/gz packet is duplicated to DEBUG.
                     */
                    ICM42605_GetAngleDeg10 (&roll_deg10, &pitch_deg10, &yaw_deg10);
                    Serial_Printf (SERIAL_PORT_DEBUG,
                                   "POSTURE_ANGLE,roll=%ld.%ld,pitch=%ld.%ld,yaw=%ld.%ld\r\n",
                                   (long)(roll_deg10 / 10),
                                   (long)(roll_deg10 % 10),
                                   (long)(pitch_deg10 / 10),
                                   (long)(pitch_deg10 % 10),
                                   (long)(yaw_deg10 / 10),
                                   (long)(yaw_deg10 % 10));
                }
            }
            if (pr->posture != POSTURE_UNKNOWN && pr->posture != s_last_wifi_posture) {
                s_last_wifi_posture = pr->posture;
                const PM_Result_t *pm = PM_GetResult();
                uint8_t seq = Retry_GetSeq();
                char buf[80];
                snprintf (buf, sizeof (buf), "POSTURE_STATE,seq=%u,%s,turns=%lu\r\n",
                          (unsigned)seq, Posture_ToString (pr->posture), (unsigned long)pm->turn_count);
                Serial_Printf (SERIAL_PORT_DEBUG, "%s", buf);
                Serial_Printf (SERIAL_PORT_WIFI, "%s", buf);
                Retry_StoreEx (buf, 20);
            }
        }

        if (g_icm42605_pm_event_pending) {
            g_icm42605_pm_event_pending = 0u;
            const PM_Result_t *pm = PM_GetResult();
            switch ((PM_Event_t)g_icm42605_pm_event_type) {
            case PM_EVENT_TURN: {
                uint8_t seq = Retry_GetSeq();
                char buf[96];
                snprintf (buf, sizeof (buf), "TURN_EVENT,seq=%u,count=%lu,from=%s,to=%s,tick=%lu\r\n",
                          (unsigned)seq,
                          (unsigned long)pm->turn_count,
                          Posture_ToString (pm->turn_from),
                          Posture_ToString (pm->turn_to),
                          (unsigned long)pm->last_turn_tick);
                Serial_Printf (SERIAL_PORT_DEBUG, "%s", buf);
                Serial_Printf (SERIAL_PORT_WIFI, "%s", buf);
                Retry_StoreEx (buf, 20);
                break;
            }
            case PM_EVENT_FALL: {
                uint8_t seq = Retry_GetSeq();
                char buf[96];
                snprintf (buf, sizeof (buf), "FALL_EVENT,seq=%u,gyro=%d,acc=%d,posture=%s,tick=%lu\r\n",
                          (unsigned)seq,
                          (int)(pm->fall_peak_gyro * 100.0f),
                          (int)(pm->fall_peak_acc * 100.0f),
                          Posture_ToString (pm->fall_posture),
                          (unsigned long)pm->fall_tick);
                Serial_Printf (SERIAL_PORT_DEBUG, "%s", buf);
                Serial_Printf (SERIAL_PORT_WIFI, "%s", buf);
                Retry_StoreEx (buf, 20);
                PM_ClearFall();
                break;
            }
            case PM_EVENT_NO_TURN: {
                uint8_t seq = Retry_GetSeq();
                char buf[64];
                snprintf (buf, sizeof (buf), "NO_TURN_ALERT,seq=%u,duration_min=%lu\r\n",
                          (unsigned)seq,
                          (unsigned long)(pm->no_turn_duration_ms / 60000u));
                Serial_Printf (SERIAL_PORT_DEBUG, "%s", buf);
                Serial_Printf (SERIAL_PORT_WIFI, "%s", buf);
                Retry_StoreEx (buf, 20);
                PM_ClearNoTurnAlert();
                break;
            }
            default:
                break;
            }
        }
#endif /* HAS_ICM42605 */
    }
}
