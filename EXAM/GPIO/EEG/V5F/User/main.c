/********************************** (C) COPYRIGHT *******************************
 * File Name          : main_v5f.c
 * Author             : WCH
 * Version            : V1.1.0
 * Date               : 2026/07/01
 * Description        : V5F EEG backend core; IPC/DSP code is split from hardware.c.
 *******************************************************************************/

#include "debug.h"
#include "hardware.h"

int main(void)
{
    SystemAndCoreClockUpdate();
    Delay_Init();

#if (Run_Core == Run_Core_V3FandV5F)
    /*
     * v2.7: V5F reads shared EEG frames and runs preprocessing, FFT,
     * feature extraction, and LEFT/RIGHT inference. IPC/DSP implementation is in Common. Do not initialize
     * USART/OLED/SPI/DMA/ADS1299 here.
     */
    HSEM_FastTake(HSEM_ID0);
    HSEM_ReleaseOneSem(HSEM_ID0, 0);

    /* Enable IPC_CH0 interrupt only. V5F ACKs checksum and parse result through shared slot. */
    DualCore_IPC_Init_V5F();

#elif (Run_Core == Run_Core_V5F)
    Hardware();
#endif

    while (1)
    {
    }
}
