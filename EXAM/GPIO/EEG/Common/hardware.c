/********************************** (C) COPYRIGHT *******************************
 * File Name          : hardware.c
 * Description        : Top-level hardware mode selection.
 *                      v2.7: dual-core IPC and V5F EEG backend moved out of
 *                      this file into Common/dualcore_ipc.c.
 *******************************************************************************/

#include "hardware.h"

#if defined(Core_V3F)
#include "OLED.h"
#include "signal_analysis.h"
#include "ADS1299.h"
#include "Serial.h"
#include "ICM42605.h"
#endif

#define MODE_EEG_ANALYSIS    1
#define MODE_SPI_TEST        2
#define ICM_42605_Mode       4
#define SYSTEM_MODE          1

void Hardware(void)
{
#if defined(Core_V3F)
    OLED_Init();
    OLED_Clear_All();

#if (SYSTEM_MODE == MODE_EEG_ANALYSIS)
    Signal_Analysis_Start();

#elif (SYSTEM_MODE == MODE_SPI_TEST)
    Serial_Init(SERIAL_PORT_DEBUG);
    OLED_ShowHexNum(0, 0, ADS1299_Init(), 4, SPI);
    uint8_t frame[ADS1299_FRAME_BYTE_NUM];
    uint32_t status;
    int32_t ch_raw[ADS1299_CHANNEL_NUM];

    while (1) {
        if (ADS1299_ReadFrameIfReady(frame)) {
            ADS1299_ParseRawFrame(frame, &status, ch_raw);
            Serial_Printf(SERIAL_PORT_DEBUG, "ch0_v=%d\r\n", ch_raw[0]);
            Serial_Printf(SERIAL_PORT_DEBUG, "ch1_v=%d\r\n", ch_raw[1]);
        }
    }

#elif (SYSTEM_MODE == ICM_42605_Mode)
    ICM_42605_Read();
#endif

#elif defined(Core_V5F)
    while (1) {
        /* V5F is intentionally idle outside IPC_CH0 interrupt. */
    }
#endif
}
