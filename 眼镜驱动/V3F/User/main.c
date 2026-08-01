#include "debug.h"
#include "hardware.h"
#include "Serial.h"
#include "W25Q64.h"
#include "OLED.h"
#include "glxss_me.h"
#include "flash_program.h"
#include "usb_host_config.h"
#include "ch32h417_usbhs_host.h"
#include <string.h>

#define FLASH_MODE  0

#ifndef USB_PID_NAK
#define USB_PID_NAK  0x0A
#endif

extern void glxss_w25q64_init(void);
extern uint32_t glxss_fw_read_w25q64(uint32_t offset, uint8_t *buf, uint32_t len);

#define LCD_W  640
#define LCD_H  400
#define FRAME_TOTAL  (16 + (uint32_t)LCD_W * LCD_H * 4)

static uint8_t s_pkt[512];

static void fill_pkt(uint32_t offset, uint16_t len)
{
    static const uint8_t hdr[16] = {
        0x80, 0x02, 0x00, 0x00,
        0x90, 0x01, 0x00, 0x00,
        0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };

    for (uint16_t i = 0; i < len; i++) {
        uint32_t pos = offset + i;
        if (pos < 16) {
            s_pkt[i] = hdr[pos];
        } else {
            uint32_t pb = pos - 16;
            uint8_t ch = pb & 3;
            uint32_t pi = pb / 4;
            uint32_t y = pi / LCD_W;

            if (ch == 3) s_pkt[i] = 0xFF;
            else if (ch == 0) s_pkt[i] = (y < 134) ? 0xFF : 0x00;
            else if (ch == 1) s_pkt[i] = (y >= 134 && y < 267) ? 0xFF : 0x00;
            else s_pkt[i] = (y >= 267) ? 0xFF : 0x00;
        }
    }
}

static glxss_err_t send_test_frame(void)
{
    uint8_t index = DEF_USBHS_PORT_INDEX * DEF_ONE_USB_SUP_DEV_TOTAL;
    uint8_t out_ep = 0;
    uint16_t *out_tog = NULL;

    for (uint8_t n = 0; n < HostCtl[index].InterfaceNum; n++) {
        for (uint8_t e = 0; e < HostCtl[index].Interface[n].OutEndpNum; e++) {
            if (HostCtl[index].Interface[n].OutEndpType[e] == 0x02) {
                out_ep = HostCtl[index].Interface[n].OutEndpAddr[e];
                out_tog = &HostCtl[index].Interface[n].OutEndpTog[e];
                break;
            }
        }
        if (out_tog) break;
    }

    if (out_tog == NULL) return GLXSS_ERR_BULK_WRITE;

    uint32_t pos = 0;
    while (pos < FRAME_TOTAL) {
        uint16_t chunk = 512;
        if (pos + chunk > FRAME_TOTAL)
            chunk = (uint16_t)(FRAME_TOTAL - pos);

        fill_pkt(pos, chunk);

        uint8_t err;
        uint16_t nak_retry = 0;
        do {
            err = USBHSH_SendEndpData(out_ep, out_tog, s_pkt, chunk);
            if (err == (USB_PID_NAK | ERR_USB_TRANSFER)) {
                Delay_Us(100);
                if (++nak_retry >= 20000) return GLXSS_ERR_BULK_WRITE;
                continue;
            }
            break;
        } while (1);

        if (err != ERR_SUCCESS) {
            Serial_Printf(SERIAL_PORT_DEBUG, "SendEndpData err=0x%02X at %lu\r\n",
                          err, (unsigned long)pos);
            return GLXSS_ERR_BULK_WRITE;
        }

        pos += chunk;
        if (pos % (512 * 500) == 0) {
            Serial_Printf(SERIAL_PORT_DEBUG, "  %lu/%lu KB\r\n",
                          (unsigned long)(pos / 1024),
                          (unsigned long)(FRAME_TOTAL / 1024));
        }
    }

    return GLXSS_OK;
}

int main(void)
{
    SystemInit();
    SystemAndCoreClockUpdate();
    Delay_Init();

    Serial_Init(SERIAL_PORT_DEBUG);
    Delay_Ms(200);

    Serial_Printf(SERIAL_PORT_DEBUG, "\r\n[V3F] GLXSS W25Q64 v1.0\r\n");
    Serial_Printf(SERIAL_PORT_DEBUG, "[V3F] SystemCoreClk:%d\r\n", SystemCoreClock);
    Delay_Ms(200);

    W25Q64_Init();
    uint8_t mid;
    uint16_t did;
    W25Q64_ReadID(&mid, &did);
    Serial_Printf(SERIAL_PORT_DEBUG, "[V3F] W25Q64 MID=0x%02X DID=0x04X\r\n", mid, did);

#if (FLASH_MODE == 1)
    Serial_Printf(SERIAL_PORT_DEBUG, "\r\n===== FLASH MODE =====\r\n");
    Serial_Printf(SERIAL_PORT_DEBUG, "Run: python glxss_flash_fw.py COMx\r\n");
    Flash_Program_Mode();

#else
    OLED_Init();
    OLED_Clear_All();
    OLED_ShowString(SPI, 0, 0, "GLXSS W25Q64");
    OLED_ShowString(SPI, 1, 0, "MID:0x");
    OLED_ShowHexNum(SPI, 1, 6, mid, 2);
    OLED_ShowString(SPI, 2, 0, "DID:0x");
    OLED_ShowHexNum(SPI, 2, 6, did, 4);

    Serial_Printf(SERIAL_PORT_DEBUG, "\r\n===== NORMAL MODE =====\r\n");
    Serial_Printf(SERIAL_PORT_DEBUG, "Reading firmware from W25Q64...\r\n");

    glxss_w25q64_init();

    glxss_err_t err = glxss_init(
        glxss_fw_read_w25q64,
        GLXSS_FW_SIZE,
        15000
    );

    if (err == GLXSS_OK) {
        Serial_Printf(SERIAL_PORT_DEBUG, "GLXSS: READY!\r\n");
        OLED_ShowString(SPI, 3, 0, "READY!");

        Serial_Printf(SERIAL_PORT_DEBUG, "Sending 640x400 test frame (%lu bytes)...\r\n",
                      (unsigned long)FRAME_TOTAL);
        err = send_test_frame();
        Serial_Printf(SERIAL_PORT_DEBUG, "Display result: %d\r\n", err);
    } else {
        Serial_Printf(SERIAL_PORT_DEBUG, "GLXSS: FAILED (err=%d)\r\n", err);
        OLED_ShowString(SPI, 3, 0, "FAILED!");
    }
#endif

    while (1) {
    }
}
