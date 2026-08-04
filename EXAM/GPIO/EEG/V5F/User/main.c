/********************************** (C) COPYRIGHT *******************************
 * File Name          : main_v5f.c
 * Author             : WCH
 * Version            : V1.1.0
 * Date               : 2026/07/01
 * Description        : V5F core entry. Compile with V5F_MODE_GLXSS to drive
 *                      AR glasses instead of EEG backend.
 *******************************************************************************/

#include "debug.h"
#include "hardware.h"

#ifdef V5F_MODE_GLXSS
#include "glxss_me.h"
#include "glxss_display.h"
#include "usb_host_config.h"
#include "ch32h417_usbhs_host.h"
#include "ipc_log.h"
#else
#include "dualcore_v5f_ssvep.h"
#endif

int main(void)
{
    SystemAndCoreClockUpdate();
    Delay_Init();

#ifdef V5F_MODE_GLXSS
    HSEM_FastTake(HSEM_ID0);
    HSEM_ReleaseOneSem(HSEM_ID0, 0);
    IPC_Log_Init_V5F();
    IPC_Log_Printf_V5F("[V5F] boot GLXSS mode, SystemCoreClk:%d\r\n", SystemCoreClock);
    Delay_Ms(500);
    Hardware();
    IPC_Log_Printf_V5F("[V5F] USBHS Host init...\r\n");
    glxss_usb_host_init();
    IPC_Log_Printf_V5F("[V5F] Waiting for display device...\r\n");
    glxss_err_t err = glxss_usb_wait_device(GLXSS_DEVICE_VID, GLXSS_DEVICE_PID, 30000);
    if (err != GLXSS_OK) {
        IPC_Log_Printf_V5F("[V5F] Device wait failed: %d\r\n", err);
        while (1) {}
    }
    IPC_Log_Printf_V5F("[V5F] Device found!\r\n");
    glxss_usb_clear_halt(GLXSS_EP_DATA_OUT);
    glxss_display_init();
    IPC_Log_Printf_V5F("[V5F] IDLE - waiting for V3F commands...\r\n");

    for (;;) {
        uint32_t param;
        uint32_t cmd = IPC_Cmd_Recv_V5F(&param);
        if (cmd != IPC_CMD_NONE) {
            IPC_Log_Printf_V5F("[V5F] cmd=%lu param=%lu\r\n", (unsigned long)cmd, (unsigned long)param);
            switch (cmd) {
            case IPC_CMD_SSVEP:
                glxss_display_set_mode(GLXSS_DISP_MODE_SSVEP);
                if (param < 4) glxss_display_set_ssvep_freq(param);
                break;
            case IPC_CMD_ARROW:
                glxss_display_set_mode(GLXSS_DISP_MODE_ARROW);
                break;
            case IPC_CMD_ARROW_TRAIN:
                glxss_display_set_mode(GLXSS_DISP_MODE_ARROW_TRAIN);
                break;
            case IPC_CMD_MI_INFER:
                glxss_display_set_mode(GLXSS_DISP_MODE_ARROW);
                break;
            case IPC_CMD_RESET:
                glxss_display_set_mode(GLXSS_DISP_MODE_IDLE);
                break;
            default: break;
            }
        }
        err = glxss_display_step();
        if (err != GLXSS_OK) {
            IPC_Log_Printf_V5F("[V5F] FAIL err=%d\r\n", err);
            while (1) {}
        }
    }

#else
#if (Run_Core == Run_Core_V3FandV5F)
    HSEM_FastTake(HSEM_ID0);
    HSEM_ReleaseOneSem(HSEM_ID0, 0);
    DualCore_IPC_Init_V5F();

#elif (Run_Core == Run_Core_V5F)
    Hardware();
#endif

    while (1)
    {
        DualCore_V5F_MainLoopProcess();
        DualCore_V5F_SSVEP_RunPending();
        __WFI();
    }
#endif
}
