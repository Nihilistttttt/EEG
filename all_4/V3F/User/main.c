/********************************** (C) COPYRIGHT *******************************
 * File Name          : main_v3f.c
 * Author             : WCH
 * Version            : V1.1.0
 * Date               : 2026/07/01
 * Description        : Dual-core EEG v2.7 structure-optimized build. V3F handles acquisition
 *                      and peripherals; V5F handles feature/inference backend.
 *******************************************************************************/

#include "debug.h"
#include "hardware.h"
#include "Serial.h"

int main(void)
{
    SystemInit();
    SystemAndCoreClockUpdate();
    Delay_Init();

    /*
     * Use the user's own DEBUG serial configuration instead of WCH default printf UART.
     * This matches the custom board wiring and avoids invisible/garbled boot logs.
     */
    Serial_Init(SERIAL_PORT_DEBUG);
    Delay_Ms(200);

    Serial_Printf(SERIAL_PORT_DEBUG, "\r\n[V3F] boot\r\n");
    Serial_Printf(SERIAL_PORT_DEBUG, "[V3F] SystemCoreClk:%d\r\n", SystemCoreClock);
    Delay_Ms(200);

#if (Run_Core == Run_Core_V3FandV5F)

    Serial_Printf(SERIAL_PORT_DEBUG, "[V3F] wake V5F...\r\n");

    /* Wake V5F from its start address. */
    NVIC_WakeUp_V5F(Core_V5F_StartAddr);

    /* Wait until V5F releases HSEM_ID0. If V5F does not start, V3F will stop here. */
    HSEM_ITConfig(HSEM_ID0, ENABLE);
    NVIC->SCTLR |= 1 << 4;
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR, ENABLE);
    PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFE);
    HSEM_ClearFlag(HSEM_ID0);

    Serial_Printf(SERIAL_PORT_DEBUG, "[V3F] V5F HSEM released\r\n");
    DualCore_IPC_Init_V3F();
    Serial_Printf(SERIAL_PORT_DEBUG, "[V3F] IPC EEG dual-core v2.7 structured enabled\r\n");
    Serial_Printf(SERIAL_PORT_DEBUG, "[V3F] Select mode in V3F/Hardware/Signal_Analysis.c: EEG_APP_MODE\r\n");
    Delay_Ms(300);

    Hardware();

#elif (Run_Core == Run_Core_V3F)

    Serial_Printf(SERIAL_PORT_DEBUG, "[V3F] single-core mode\r\n");
    Delay_Ms(300);
    Hardware();

#elif (Run_Core == Run_Core_V5F)

    NVIC_WakeUp_V5F(Core_V5F_StartAddr);
    PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFE);

#endif

    while (1)
    {
    }
}
