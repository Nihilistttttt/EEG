/**
 * glxss_me_ch32h417.c - GLXSS ME 智能眼镜 CH32H417 平台移植层
 *
 * 基于 WCH USBHS Host SDK 实现 glxss_me.h 中声明的 4 个 USB 底层函数。
 *
 * 硬件接线:
 *   USBHS: CH32H417 PB8 (GPIO_PinSource8, GPIO_AF10) ─── GLXSS USB D+
 *          CH32H417 PB9 (GPIO_PinSource9, GPIO_AF10) ─── GLXSS USB D-
 *   UART:  CH32 PA2 (USART2_TX, GPIO_AF7) ─── ESP8266 D1 (GPIO5)
 *          CH32 PA3 (USART2_RX, GPIO_AF7) ─── ESP8266 D2 (GPIO4)
 */

#include "glxss_me.h"
#include "debug.h"
#include "usb_host_config.h"         /* ERR_SUCCESS, ROOT_DEV_*, USBHS_MAX_PACKET_SIZE */
#include "ch32h417_usbhs_host.h"     /* USBHS_Host_Init, USBHSH_* */
#include <string.h>

/* ====================================================================
 * 调试输出
 * ==================================================================== */
#ifndef GLXSS_LOG
#define GLXSS_LOG(...)
#endif
#ifndef GLXSS_ERR
#define GLXSS_ERR(...)
#endif

/* ====================================================================
 * 常量
 * ==================================================================== */
#define GLXSS_USB_ADDR          2     /* USB 设备地址 (主机分配) */

/* ====================================================================
 * 状态
 * ==================================================================== */
static uint8_t  s_ep0_size = 64;     /* EP0 最大包长 */
static uint16_t s_bulk_tog = 0;      /* Bulk OUT 翻转位 */

/* ====================================================================
 * 毫秒计数器 (RISC-V rdcycle)
 * ==================================================================== */
extern uint32_t SystemCoreClock;

static uint32_t glxss_get_ms(void)
{
    uint32_t cycle;
    __asm__ volatile("rdcycle %0" : "=r"(cycle));
    return cycle / (SystemCoreClock / 1000);
}

/* ====================================================================
 * 函数 1: glxss_usb_host_init
 * ==================================================================== */
int glxss_usb_host_init(void)
{
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOB, ENABLE);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF10);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF10);

    USBHS_Host_Init(ENABLE);

    s_ep0_size = 64;
    s_bulk_tog = 0;
    return 0;
}

/* ====================================================================
 * 函数 2: glxss_usb_wait_device
 * ==================================================================== */
glxss_err_t glxss_usb_wait_device(uint16_t vid, uint16_t pid, uint32_t timeout_ms)
{
    uint32_t tick_start = glxss_get_ms();
    uint8_t  dev_desc[18], cfg_buf[256];
    uint16_t cfg_len;
    uint8_t  speed, retry;

    while (1) {
        uint8_t status = USBHSH_CheckRootHubPortStatus(ROOT_DEV_DISCONNECT);

        if (status == ROOT_DEV_CONNECTED) {
            USBHSH_ResetRootHubPort();
            for (retry = 0; retry < 200; retry++) {
                if (USBHSH_EnableRootHubPort(&speed) == ERR_SUCCESS) break;
                Delay_Ms(1);
            }
            if (retry >= 200) continue;

            if (USBHSH_GetDeviceDescr(&s_ep0_size, dev_desc) != ERR_SUCCESS) continue;

            uint16_t dv = dev_desc[8] | ((uint16_t)dev_desc[9] << 8);
            uint16_t dp = dev_desc[10] | ((uint16_t)dev_desc[11] << 8);
            if ((vid && dv != vid) || (pid && dp != pid)) continue;

            if (USBHSH_SetUsbAddress(s_ep0_size, GLXSS_USB_ADDR) != ERR_SUCCESS) continue;
            Delay_Ms(5);

            if (USBHSH_GetConfigDescr(s_ep0_size, cfg_buf, sizeof(cfg_buf), &cfg_len) != ERR_SUCCESS) continue;
            if (USBHSH_SetUsbConfig(s_ep0_size, ((PUSB_CFG_DESCR)cfg_buf)->bConfigurationValue) != ERR_SUCCESS) continue;

            return GLXSS_OK;
        }

        if ((glxss_get_ms() - tick_start) > timeout_ms) return GLXSS_ERR_TIMEOUT;
        Delay_Ms(5);
    }
}

/* ====================================================================
 * 函数 3: glxss_usb_ctrl_transfer
 * ==================================================================== */
glxss_err_t glxss_usb_ctrl_transfer(uint8_t  bmRequestType,
                                    uint8_t  bRequest,
                                    uint16_t wValue,
                                    uint16_t wIndex,
                                    uint8_t *data,
                                    uint16_t wLength)
{
    pUSBHS_SetupRequest->bRequestType = bmRequestType;
    pUSBHS_SetupRequest->bRequest     = bRequest;
    pUSBHS_SetupRequest->wValue       = wValue;
    pUSBHS_SetupRequest->wIndex       = wIndex;
    pUSBHS_SetupRequest->wLength      = wLength;

    uint16_t xfer_len = wLength;
    uint8_t err = USBHSH_CtrlTransfer(s_ep0_size, data, &xfer_len);

    return (err == ERR_SUCCESS) ? GLXSS_OK : GLXSS_ERR_CTL_TRANSFER;
}

/* ====================================================================
 * 函数 4: glxss_usb_bulk_write
 * ==================================================================== */
glxss_err_t glxss_usb_bulk_write(uint8_t ep, const uint8_t *data, uint32_t len)
{
    uint32_t offset = 0;
    while (offset < len) {
        uint32_t chunk = len - offset;
        if (chunk > USBHS_MAX_PACKET_SIZE) chunk = USBHS_MAX_PACKET_SIZE;

        uint8_t err = USBHSH_SendEndpData(ep, &s_bulk_tog,
                                          (uint8_t *)&data[offset], (uint16_t)chunk);
        if (err != ERR_SUCCESS) return GLXSS_ERR_BULK_WRITE;
        offset += chunk;
    }
    return GLXSS_OK;
}
