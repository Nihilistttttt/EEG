#include "W25Q64.h"
#include "Config.h"
#include "hal_gpio.h"
#include "debug.h"

#define HARDWARE 0
#define SOFTWARE 1
#define SPI_TYPE HARDWARE

#define SPIx W25Q64_SPI_INSTANCE

void SPI_W_CS(uint8_t BitValue) {
    Hal_GPIO_Write(W25Q64_CS_PIN_ENC, BitValue);
}

#if (SPI_TYPE == SOFTWARE)
void SPI_W_SCK(uint8_t BitValue) {
    Hal_GPIO_Write(W25Q64_SCK_PIN_ENC, BitValue);
}

void SPI_W_MOSI(uint8_t BitValue) {
    Hal_GPIO_Write(W25Q64_MOSI_PIN_ENC, BitValue);
}

uint8_t SPI_R_MISO(void) {
    return Hal_GPIO_Read(W25Q64_MISO_PIN_ENC);
}
#endif

void W25Q64_Init(void) {
#if (SPI_TYPE == SOFTWARE)
    AFIO_ClockEnable();
    GPIO_ClockEnable(W25Q64_CS_PORT);
    GPIO_ClockEnable(W25Q64_SCK_PORT);
    GPIO_ClockEnable(W25Q64_MOSI_PORT);
    GPIO_ClockEnable(W25Q64_MISO_PORT);

    Hal_GPIO_Init(W25Q64_CS_PIN_ENC, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_SPEED_VERY_HIGH, 0);
    Hal_GPIO_Init(W25Q64_SCK_PIN_ENC, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_SPEED_VERY_HIGH, 0);
    Hal_GPIO_Init(W25Q64_MOSI_PIN_ENC, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_SPEED_VERY_HIGH, 0);
    Hal_GPIO_Init(W25Q64_MISO_PIN_ENC, HAL_GPIO_MODE_INPUT_PU, HAL_GPIO_SPEED_VERY_HIGH, 0);

    SPI_W_CS(1);
    SPI_W_SCK(0);
#else
    AFIO_ClockEnable();
    GPIO_ClockEnable(W25Q64_CS_PORT);
    GPIO_ClockEnable(W25Q64_SCK_PORT);
    GPIO_ClockEnable(W25Q64_MISO_PORT);
    GPIO_ClockEnable(W25Q64_MOSI_PORT);
    SPI_ClockEnable(W25Q64_SPI_INSTANCE);

    Hal_GPIO_Init(W25Q64_CS_PIN_ENC, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_SPEED_VERY_HIGH, 0);

    Hal_GPIO_Init(W25Q64_SCK_PIN_ENC, HAL_GPIO_MODE_AF_PP, HAL_GPIO_SPEED_VERY_HIGH, W25Q64_SCK_AF);
    Hal_GPIO_Init(W25Q64_MOSI_PIN_ENC, HAL_GPIO_MODE_AF_PP, HAL_GPIO_SPEED_VERY_HIGH, W25Q64_MOSI_AF);
    Hal_GPIO_Init(W25Q64_MISO_PIN_ENC, HAL_GPIO_MODE_AF_INPUT, HAL_GPIO_SPEED_VERY_HIGH, W25Q64_MISO_AF);

    SPI_InitTypeDef SPI_InitStructure;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_Mode2;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPIx, &SPI_InitStructure);

    SPI_Cmd(SPIx, ENABLE);

    SPI_W_CS(1);
#endif
}

void SPI_Start(void) {
    SPI_W_CS(0);
#if (SPI_TYPE == HARDWARE)
    Delay_Us(1);
#endif
}

void SPI_Stop(void) {
#if (SPI_TYPE == HARDWARE)
    Delay_Us(1);
#endif
    SPI_W_CS(1);
}

uint8_t SPI_SwapByte(uint8_t ByteSend) {
#if (SPI_TYPE == SOFTWARE)
    for (uint8_t i = 0; i < 8; ++i) {
        SPI_W_MOSI(ByteSend & 0x80);
        ByteSend <<= 1;
        SPI_W_SCK(1);
        if (SPI_R_MISO() == 1)
            ByteSend |= 0x01;
        SPI_W_SCK(0);
    }
    return ByteSend;
#else
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) != SET);
    SPI_I2S_SendData(SPIx, ByteSend);

    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) != SET);
    return SPI_I2S_ReceiveData(SPIx);
#endif
}

void W25Q64_WriteEnable(void) {
    SPI_Start();
    SPI_SwapByte(W25Q64_WRITE_ENABLE);
    SPI_Stop();
}

void W25Q64_WaitBusy(void) {
    uint32_t Timeout = 100000;
    SPI_Start();
    SPI_SwapByte(W25Q64_READ_STATUS_REGISTER_1);
    while ((SPI_SwapByte(W25Q64_DUMMY_BYTE) & 0x01) == 0x01) {
        Timeout--;
        if (Timeout == 0) {
            break;
        }
    }
    SPI_Stop();
}

void W25Q64_ReadID(uint8_t *MID, uint16_t *DID) {
    W25Q64_WaitBusy();
    SPI_Start();
    SPI_SwapByte(W25Q64_JEDEC_ID);
    *MID = SPI_SwapByte(W25Q64_DUMMY_BYTE);
    *DID = SPI_SwapByte(W25Q64_DUMMY_BYTE);
    *DID <<= 8;
    *DID |= SPI_SwapByte(W25Q64_DUMMY_BYTE);
    SPI_Stop();
}

void W25Q64_PageProgram(uint32_t Address, uint8_t *DataArray, uint16_t Count) {
    W25Q64_WaitBusy();
    W25Q64_WriteEnable();
    SPI_Start();
    SPI_SwapByte(W25Q64_PAGE_PROGRAM);
    SPI_SwapByte(Address >> 16);
    SPI_SwapByte(Address >> 8);
    SPI_SwapByte(Address);
    for (uint16_t i = 0; i < Count; ++i) {
        SPI_SwapByte(DataArray[i]);
    }
    SPI_Stop();
}

void W25Q64_SectorErase(uint32_t Address) {
    W25Q64_WaitBusy();
    W25Q64_WriteEnable();
    SPI_Start();
    SPI_SwapByte(W25Q64_SECTOR_ERASE_4KB);
    SPI_SwapByte(Address >> 16);
    SPI_SwapByte(Address >> 8);
    SPI_SwapByte(Address);
    SPI_Stop();
}

void W25Q64_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count) {
    W25Q64_WaitBusy();
    SPI_Start();
    SPI_SwapByte(W25Q64_READ_DATA);
    SPI_SwapByte(Address >> 16);
    SPI_SwapByte(Address >> 8);
    SPI_SwapByte(Address);
    for (uint32_t i = 0; i < Count; ++i) {
        DataArray[i] = SPI_SwapByte(W25Q64_DUMMY_BYTE);
    }
    SPI_Stop();
}
