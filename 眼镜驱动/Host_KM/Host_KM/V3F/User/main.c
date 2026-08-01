/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Description        : GLXSS ME 眼镜 USBHS 检测
 *********************************************************************************/

#include "debug.h"
#include "hardware.h"
#include "GLXSS/glxss_me.h"
#include "GLXSS/W25Q64.h"

#define VERSION  "GLXSS_W25Q64 v1.0"

/* ── 模式选择 ──────────────────────────────────────────
 *   1 = 烧录模式: PC串口 → W25Q64 (烧完改回 0 重新编译)
 *   0 = 正常模式: W25Q64 → USBHS → GLXSS 眼镜
 * ──────────────────────────────────────────────────── */
#define FLASH_MODE  1

/* fw_read 回调: 从 W25Q64 读取固件 */
static uint32_t fw_read_w25q64(uint32_t offset, uint8_t *buf, uint32_t len)
{
    extern uint32_t glxss_fw_read_w25q64(uint32_t, uint8_t *, uint32_t);
    return glxss_fw_read_w25q64(offset, buf, len);
}

int main(void)
{
    SystemInit();
    SystemAndCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    printf("\r\n========================================\r\n");
    printf("  %s\r\n", VERSION);
    printf("  Build: %s %s\r\n", __DATE__, __TIME__);
    printf("========================================\r\n");
    printf("SystemClk:%d\r\n", SystemClock);
    printf("V3F SystemCoreClk:%d\r\n", SystemCoreClock);

    /* ── 初始化 W25Q64 ── */
    {
        extern void glxss_w25q64_init(void);
        glxss_w25q64_init();
    }

#if FLASH_MODE == 1
    /* ══════════════════════════════════════════════════════
     * 烧录模式: PC串口 → W25Q64
     *
     * 运行 fw_flash_w25q64.py 脚本前先烧录此固件,
     * CH32 启动后自动等待 PC 发数据。
     * 烧完后把 FLASH_MODE 改成 0 重新编译。
     * ══════════════════════════════════════════════════════ */
    {
        extern void glxss_flash_fw_from_serial(void);
        printf("\r\n");
        printf("=============================================\r\n");
        printf("  FLASH MODE\n");
        printf("  Run: python fw_flash_w25q64.py COMx\n");
        printf("=============================================\r\n");
        printf("Waiting for PC to send firmware data...\r\n");
        glxss_flash_fw_from_serial();
        printf("\r\nFlash complete! Set FLASH_MODE=0 and re-flash.\r\n");
        while (1);
    }
#else
    /* ══════════════════════════════════════════════════════
     * 正常模式: W25Q64 → USBHS → GLXSS 眼镜
     * ══════════════════════════════════════════════════════ */
    {
        extern void glxss_usb_host_init(void);
        glxss_usb_host_init();

        printf("\r\n");
        printf("=============================================\r\n");
        printf("  NORMAL MODE\n");
        printf("  Reading firmware from W25Q64...\n");
        printf("=============================================\r\n");
        printf("GLXSS: waiting for bootloader...\r\n");

        glxss_err_t err = glxss_init(
            fw_read_w25q64,                          /* 从 W25Q64 读固件 */
            1525136,                                 /* 固件 1.5MB */
            15000                                    /* 超时 15s */
        );

        if (err == GLXSS_OK) {
            printf("\r\n========== GLXSS: READY! 640x400 ==========\r\n");
        } else {
            printf("\r\n========== GLXSS: FAILED (err=%d) ==========\r\n", err);
        }
    }

    while (1) {
        /* 可以在这里加显示画面代码 */
    }
#endif
}
