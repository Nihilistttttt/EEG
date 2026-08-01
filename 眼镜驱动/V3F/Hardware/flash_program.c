#include "flash_program.h"
#include "W25Q64.h"
#include "Serial.h"
#include "debug.h"
#include "OLED.h"
#include <string.h>

#define PAGE_SIZE   256
#define SECTOR_SIZE 4096

static uint8_t page_buf[PAGE_SIZE];

static void Flash_Erase_Blocks(uint32_t total_bytes) {
    uint32_t addr = 0;
    while (addr < total_bytes) {
        W25Q64_SectorErase(addr);
        addr += SECTOR_SIZE;
    }
}

void Flash_Program_Mode(void) {
    W25Q64_Init();
    Serial_Init(SERIAL_PORT_DEBUG);
    OLED_Init();
    OLED_Clear(SPI);

    OLED_ShowString(SPI, 0, 0, "FW Flash");
    OLED_ShowString(SPI, 1, 0, "Waiting...");

    // Serial_Printf(SERIAL_PORT_DEBUG, "\r\n===== FW Flasher (glxss) =====\r\n");
    // Serial_Printf(SERIAL_PORT_DEBUG, "Waiting for PC...\r\n");

    /* 1. 接收固件大小 (4字节 LE) */
    uint32_t total_data_bytes = 0;
    uint8_t len_buf[4];
    uint16_t idx = 0;

    while (idx < 4) {
        if (Serial_IsDataReady(SERIAL_PORT_DEBUG)) {
            uint8_t *buf;
            uint16_t len = Serial_GetDataPacket(SERIAL_PORT_DEBUG, &buf);
            for (uint16_t i = 0; i < len && idx < 4; i++) {
                len_buf[idx++] = buf[i];
            }
        }
    }

    total_data_bytes = len_buf[0] | (len_buf[1] << 8) | (len_buf[2] << 16) | (len_buf[3] << 24);
    // Serial_Printf(SERIAL_PORT_DEBUG, "FW size: %lu bytes\r\n", (unsigned long)total_data_bytes);

    /* 2. ACK: 已收到大小 */
    Serial_SendByte(SERIAL_PORT_DEBUG, 0xBB);

    /* 3. 擦除 Flash */
    OLED_Clear(SPI);
    OLED_ShowString(SPI, 0, 0, "Erasing...");

    uint32_t erase_bytes = ((total_data_bytes + SECTOR_SIZE - 1) / SECTOR_SIZE) * SECTOR_SIZE;
    Flash_Erase_Blocks(erase_bytes);

    // Serial_Printf(SERIAL_PORT_DEBUG, "Erase done\r\n");

    /* 4. ACK: 擦除完成 */
    Serial_SendByte(SERIAL_PORT_DEBUG, 0xDD);

    /* 5. 逐页接收并烧录 */
    uint32_t total_pages = (total_data_bytes + PAGE_SIZE - 1) / PAGE_SIZE;
    uint32_t addr = 0;

    OLED_Clear(SPI);
    OLED_ShowString(SPI, 0, 0, "Writing...");
    OLED_ShowString(SPI, 2, 0, "Pages:");
    OLED_ShowNum(SPI, 2, 6, total_pages, 5);

    for (uint32_t page = 0; page < total_pages; page++) {
        uint32_t received = 0;
        while (received < PAGE_SIZE) {
            if (Serial_IsDataReady(SERIAL_PORT_DEBUG)) {
                uint8_t *buf;
                uint16_t len = Serial_GetDataPacket(SERIAL_PORT_DEBUG, &buf);
                uint16_t copy = (received + len > PAGE_SIZE) ? (PAGE_SIZE - received) : len;
                memcpy(page_buf + received, buf, copy);
                received += copy;
            }
        }

        W25Q64_PageProgram(addr, page_buf, PAGE_SIZE);
        addr += PAGE_SIZE;

        /* ACK: 页写入完成 */
        Serial_SendByte(SERIAL_PORT_DEBUG, 0xEE);

        uint8_t progress = (uint8_t)((page + 1) * 100 / total_pages);
        OLED_ShowString(SPI, 3, 0, "Progress:    %");
        OLED_ShowNum(SPI, 3, 10, progress, 3);
    }

    /* 6. ACK: 全部完成 */
    Serial_SendByte(SERIAL_PORT_DEBUG, 0xFF);

    OLED_Clear(SPI);
    OLED_ShowString(SPI, 0, 0, "DONE!");
    // Serial_Printf(SERIAL_PORT_DEBUG, "===== FW flash DONE! =====\r\n");

    while (1) {
        Delay_Ms(1000);
    }
}
