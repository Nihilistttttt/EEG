/********************************** (C) COPYRIGHT *******************************
 * File Name          : hardware.c
 * Description        : Top-level hardware mode selection.
 *                      v2.7: dual-core IPC and V5F EEG backend moved out of
 *                      this file into Common/dualcore_ipc.c.
 *******************************************************************************/

#include "hardware.h"

#if defined(Core_V3F)
#include "sd_card.h"
#include "OLED.h"
#include "signal_analysis.h"
#include "ADS1299.h"
#include "Serial.h"
#include "ICM42605.h"
#include "glxss_me.h"
#include "glxss_sd_fw.h"
#endif

#define MODE_EEG_ANALYSIS    1
#define MODE_SPI_TEST        2
#define MODE_SD_TEST         3
#define ICM_42605_Mode       4
#define MODE_GLXSS           5
#define MODE_GLXSS_BURN      6
#define SYSTEM_MODE          MODE_SD_TEST

void Hardware(void)
{
#if defined(Core_V3F)
#ifndef EEG_OLED_DISABLED
    OLED_Init();
    OLED_Clear_All();
#endif

#if (SYSTEM_MODE == MODE_EEG_ANALYSIS)
    Signal_Analysis_Start();

#elif (SYSTEM_MODE == MODE_SPI_TEST)
    Serial_Init(SERIAL_PORT_DEBUG);
    OLED_ShowHexNum(0, 0, ADS1299_Init(), 4, SPI);
    uint8_t frame[ADS1299_FRAME_BYTE_NUM];
    uint32_t status;
    int32_t ch_raw[ADS1299_CHANNEL_NUM];

    while (1) {
        if (ring_buffer_get_frame(frame)) {
            ADS1299_ParseRawFrame(frame, &status, ch_raw);
            Serial_Printf(SERIAL_PORT_DEBUG, "ch0_v=%d\r\n", ch_raw[0]);
            Serial_Printf(SERIAL_PORT_DEBUG, "ch1_v=%d\r\n", ch_raw[1]);
        }
    }

#elif (SYSTEM_MODE == MODE_SD_TEST)
    Serial_Printf(SERIAL_PORT_DEBUG, "[SD] start self test\r\n");
    {
        int ret = SD_SelfTest();
        Serial_Printf(SERIAL_PORT_DEBUG, "[SD] self test result: %s (ret=%d)\r\n",
                      ret == 0 ? "PASS" : "FAIL", ret);
    }
    while (1) {
    }

#elif (SYSTEM_MODE == ICM_42605_Mode)
    ICM_42605_Read();

#elif (SYSTEM_MODE == MODE_GLXSS_BURN)
    {
        int ret = SD_Init();
        Serial_Init(SERIAL_PORT_DEBUG);
        if (ret != 0) {
            Serial_Printf(SERIAL_PORT_DEBUG, "[GLXSS_BURN] SD init FAIL\r\n");
            while (1);
        }
        glxss_sd_fw_burn();
    }
    while (1) {
    }

#elif (SYSTEM_MODE == MODE_GLXSS)
    Serial_Init(SERIAL_PORT_DEBUG);
    {
        int ret = SD_Init();
        if (ret != 0) {
            Serial_Printf(SERIAL_PORT_DEBUG, "[GLXSS] SD init FAIL, aborting\r\n");
            while (1);
        }

        uint32_t fw_size = glxss_sd_fw_get_size();
        if (fw_size == 0) {
            Serial_Printf(SERIAL_PORT_DEBUG, "[GLXSS] No FW on SD, aborting\r\n");
            while (1);
        }
        Serial_Printf(SERIAL_PORT_DEBUG, "[GLXSS] FW %lu bytes, starting glasses...\r\n",
                      (unsigned long)fw_size);
        glxss_err_t err = glxss_init(glxss_sd_fw_read, fw_size, 10000);
        if (err == GLXSS_OK) {
            Serial_Printf(SERIAL_PORT_DEBUG, "[GLXSS] Glasses ready!\r\n");
        } else {
            Serial_Printf(SERIAL_PORT_DEBUG, "[GLXSS] Init failed: %d\r\n", err);
        }
    }
    while (1) {
    }
#endif

#elif defined(Core_V5F)
    while (1) {
        /* V5F is intentionally idle outside IPC_CH0 interrupt. */
    }
#endif
}
