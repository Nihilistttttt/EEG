#include "glxss_sd_fw.h"
#include "sd_card.h"
#include "Serial.h"

#include <string.h>

#define FW_SIZE_SECTOR  0


uint32_t glxss_sd_fw_read(uint32_t offset, uint8_t *buf, uint32_t len)
{
    uint32_t done = 0;
    uint8_t sec_buf[SD_SECTOR_SIZE];

    while (done < len) {
        uint32_t sec = GLXSS_FW_START_SECTOR + (offset + done) / SD_SECTOR_SIZE;
        uint32_t off_in = (offset + done) % SD_SECTOR_SIZE;
        uint32_t chunk = SD_SECTOR_SIZE - off_in;
        if (chunk > len - done) chunk = len - done;

        if (off_in == 0 && chunk == SD_SECTOR_SIZE) {
            if (SD_ReadSector(sec, buf + done) != 0) break;
        } else {
            if (SD_ReadSector(sec, sec_buf) != 0) break;
            memcpy(buf + done, sec_buf + off_in, chunk);
        }
        done += chunk;
    }
    return done;
}

uint32_t glxss_sd_fw_get_size(void)
{
    uint8_t sec_buf[SD_SECTOR_SIZE];
    if (SD_ReadSector(FW_SIZE_SECTOR, sec_buf) != 0) return 0;
    uint32_t size = sec_buf[0] | (sec_buf[1] << 8) | (sec_buf[2] << 16) | (sec_buf[3] << 24);
    if (size == 0 || size > GLXSS_FW_MAX_SIZE) return 0;
    return size;
}

int glxss_sd_fw_burn(void)
{
    uint8_t sec_buf[SD_SECTOR_SIZE];
    uint16_t idx;
    uint32_t total_data_bytes;


    Serial_SendByte(SERIAL_PORT_DEBUG, 0xAA);

    idx = 0;
    uint8_t len_buf[4];
    uint32_t poll_cnt = 0;
    while (idx < 4) {
        if (Serial_IsDataReady(SERIAL_PORT_DEBUG)) {
            uint8_t *buf;
            uint16_t len = Serial_GetDataPacket(SERIAL_PORT_DEBUG, &buf);
            for (uint16_t i = 0; i < len && idx < 4; i++)
                len_buf[idx++] = buf[i];
        }
        poll_cnt++;
        if (poll_cnt >= 200000) {
            Serial_SendByte(SERIAL_PORT_DEBUG, 0xAA);
            poll_cnt = 0;
        }
    }

    total_data_bytes = len_buf[0] | (len_buf[1] << 8) | (len_buf[2] << 16) | (len_buf[3] << 24);

    if (total_data_bytes > GLXSS_FW_MAX_SIZE) {
        Serial_SendByte(SERIAL_PORT_DEBUG, 0xE0);
        return -1;
    }

    Serial_SendByte(SERIAL_PORT_DEBUG, 0xBB);


    Serial_SendByte(SERIAL_PORT_DEBUG, 0xCC);

    memset(sec_buf, 0, SD_SECTOR_SIZE);
    sec_buf[0] = len_buf[0];
    sec_buf[1] = len_buf[1];
    sec_buf[2] = len_buf[2];
    sec_buf[3] = len_buf[3];
    if (SD_WriteSector(FW_SIZE_SECTOR, sec_buf) != 0) {
        Serial_SendByte(SERIAL_PORT_DEBUG, 0xE1);
        return -2;
    }

    uint32_t total_sectors = (total_data_bytes + SD_SECTOR_SIZE - 1) / SD_SECTOR_SIZE;
    uint32_t cur_sector = GLXSS_FW_START_SECTOR;

    for (uint32_t sec = 0; sec < total_sectors; sec++) {
        memset(sec_buf, 0xFF, SD_SECTOR_SIZE);

        uint32_t received = 0;
        while (received < SD_SECTOR_SIZE) {
            if (Serial_IsDataReady(SERIAL_PORT_DEBUG)) {
                uint8_t *buf;
                uint16_t len = Serial_GetDataPacket(SERIAL_PORT_DEBUG, &buf);
                uint16_t copy = (received + len > SD_SECTOR_SIZE) ? (SD_SECTOR_SIZE - received) : len;
                memcpy(sec_buf + received, buf, copy);
                received += copy;
            }
        }

        if (SD_WriteSector(cur_sector, sec_buf) != 0) {
            Serial_Printf(SERIAL_PORT_DEBUG, "[BURN] Write fail at sector %lu\r\n", (unsigned long)cur_sector);
            Serial_SendByte(SERIAL_PORT_DEBUG, 0xE2);
            return -2;
        }
        cur_sector++;
        Delay_Ms(5);

        Serial_SendByte(SERIAL_PORT_DEBUG, 0xEE);
    }

    Serial_SendByte(SERIAL_PORT_DEBUG, 0xFF);
    return 0;
}
