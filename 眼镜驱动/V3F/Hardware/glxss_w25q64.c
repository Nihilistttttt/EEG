#include "glxss_me.h"
#include "W25Q64.h"
#include "Serial.h"
#include "debug.h"
#include <string.h>

#define FW_FLASH_ADDR       0x000000
#define PAGE_SIZE           256

void glxss_w25q64_init(void)
{
    Serial_Printf(SERIAL_PORT_DEBUG, "W25Q64: Init...\r\n");
    W25Q64_Init();

    uint8_t  mid;
    uint16_t did;
    W25Q64_ReadID(&mid, &did);
    Serial_Printf(SERIAL_PORT_DEBUG, "W25Q64: MID=0x%02X DID=0x%04X\r\n", mid, did);
}

uint32_t glxss_fw_read_w25q64(uint32_t offset, uint8_t *buf, uint32_t len)
{
    W25Q64_ReadData(FW_FLASH_ADDR + offset, buf, len);
    return len;
}