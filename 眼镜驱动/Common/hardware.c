#include "hardware.h"

#if defined(Core_V3F)
#include "OLED.h"
#include "W25Q64.h"
#include "Serial.h"
#endif

void Hardware(void)
{
#if defined(Core_V3F)
    OLED_Init();
    OLED_Clear_All();

    W25Q64_Init();
    uint8_t mid;
    uint16_t did;
    W25Q64_ReadID(&mid, &did);

    OLED_ShowString(SPI, 1, 0, "MID:");
    OLED_ShowNum(SPI, 1, 4, mid, 2);
    OLED_ShowString(SPI, 2, 0, "DID:");
    OLED_ShowNum(SPI, 2, 4, did, 4);

#elif defined(Core_V5F)
    while (1) {
    }
#endif
}
