#include "ADS1299.h"
#include "Serial.h"
#include "Message_Parser.h"
#include "Config.h"
#include <string.h>

/*============================ 宏定义 ============================*/
/* 引脚操作宏 */
#define ADS1299_CS_LOW() Hal_GPIO_Reset (ADS1299_CS_PIN_ENC)
#define ADS1299_CS_HIGH() Hal_GPIO_Set (ADS1299_CS_PIN_ENC)

#define ADS1299_RESET_LOW() Hal_GPIO_Reset (ADS1299_RESET_PIN_ENC)
#define ADS1299_RESET_HIGH() Hal_GPIO_Set (ADS1299_RESET_PIN_ENC)

#define ADS1299_START_LOW() Hal_GPIO_Reset (ADS1299_START_PIN_ENC)
#define ADS1299_START_HIGH() Hal_GPIO_Set (ADS1299_START_PIN_ENC)


/* 缓冲区配置 */
#define RING_BUF_SIZE 64 /* 环形缓冲区深度（帧数） */
#define FRAME_SIZE ADS1299_FRAME_BYTE_NUM    /* ADS1299 连续读完整 8 通道帧：3 + 8*3 = 27 字节 */

/*============================ 静态全局变量 ============================*/
/* DMA 传输用 dummy TX 缓冲区（全 0） */
static uint8_t ads1299_dma_tx_dummy[FRAME_SIZE] = {0};

/* 环形缓冲区（二维数组） */
static uint8_t ring_buffer[RING_BUF_SIZE][FRAME_SIZE];
static volatile uint8_t head = 0; /* 写索引（中断中修改） */
static volatile uint8_t tail = 0; /* 读索引（主循环修改） */

/* 状态统计变量 */
volatile uint8_t g_ads1299_spi_dma_busy = 0;
volatile uint32_t g_ads1299_spi_dma_ok_count = 0;
volatile uint32_t g_ads1299_spi_dma_lost_count = 0;
volatile uint32_t g_ads1299_spi_dma_error_count = 0;

/*============================ 私有函数声明（static） ============================*/
/* 硬件初始化（按调用顺序） */
static void ADS1299_GPIO_Init (void);
static void ADS1299_SPI_Init (void);
static void ADS1299_DMA_Init (void);
static void ADS1299_Register_Init (void);

/* 底层 SPI 辅助函数 */
static uint8_t ADS1299_SPI_TxRxByte (uint8_t tx_data);
static void ADS1299_SPI_WaitBusy (void);
static void ADS1299_SPI_ClearRxNE (void);

/* 硬件复位与命令 */
static void ADS1299_ResetByPin (void);
static void ADS1299_SendCommand (uint8_t cmd);
static int32_t ADS1299_SignExtend24 (uint32_t raw24);

/* 寄存器操作 */
static uint8_t ADS1299_ReadReg (uint8_t reg);
// static void    ADS1299_ReadRegs(uint8_t start_reg, uint8_t *buf, uint8_t len);
static void ADS1299_WriteReg (uint8_t reg, uint8_t value);
// static void    ADS1299_WriteRegs(uint8_t start_reg, const uint8_t *buf, uint8_t len);

/* 连续模式控制 */
static void ADS1299_StartContinuous (void);
// static void    ADS1299_StopContinuous(void);

/* DMA 传输 */
static void ADS1299_DMA_StartRead (void);
static void ADS1299_DMA_StopAndReleaseCS (void);

/* 数据解析辅助 */
static int32_t ADS1299_SignExtend24 (uint32_t raw24);


/* 硬件初始化（按调用顺序） */
uint8_t ADS1299_Init (void) {
    ADS1299_GPIO_Init();
    ADS1299_SPI_Init();
    ADS1299_DMA_Init();
    ADS1299_Register_Init();

    uint8_t id = ADS1299_ReadReg (ADS1299_REG_ID);
    ADS1299_StartContinuous();

    return id;  // 返回 ID 供外部检查
}

static void ADS1299_GPIO_Init (void) {
    AFIO_ClockEnable();

    GPIO_ClockEnable (ADS1299_CS_PORT);
    GPIO_ClockEnable (ADS1299_DRDY_PORT);
    GPIO_ClockEnable (ADS1299_RESET_PORT);
    GPIO_ClockEnable (ADS1299_START_PORT);
    GPIO_ClockEnable (ADS1299_SCK_PORT);
    GPIO_ClockEnable (ADS1299_MISO_PORT);
    GPIO_ClockEnable (ADS1299_MOSI_PORT);

    /*-------------------- SPI SCK 引脚：复用推挽 --------------------*/
    Hal_GPIO_Init (ADS1299_SCK_ENC, HAL_GPIO_MODE_AF_PP,
                   HAL_GPIO_SPEED_VERY_HIGH, ADS1299_SCK_AF);

    /*-------------------- SPI MOSI 引脚：复用推挽 --------------------*/
    Hal_GPIO_Init (ADS1299_MOSI_ENC, HAL_GPIO_MODE_AF_PP,
                   HAL_GPIO_SPEED_VERY_HIGH, ADS1299_MOSI_AF);

    /*-------------------- SPI MISO 引脚：复用输入 --------------------*/
    Hal_GPIO_Init (ADS1299_MISO_ENC, HAL_GPIO_MODE_AF_INPUT,
                   HAL_GPIO_SPEED_VERY_HIGH, ADS1299_MISO_AF);

    /*-------------------- CS 引脚：推挽输出 --------------------*/
    Hal_GPIO_Init (ADS1299_CS_PIN_ENC, HAL_GPIO_MODE_OUTPUT_PP,
                   HAL_GPIO_SPEED_VERY_HIGH, 0);
    ADS1299_CS_HIGH();

    /*-------------------- RESET 引脚：推挽输出 --------------------*/
    Hal_GPIO_Init (ADS1299_RESET_PIN_ENC, HAL_GPIO_MODE_OUTPUT_PP,
                   HAL_GPIO_SPEED_VERY_HIGH, 0);
    ADS1299_RESET_HIGH();

    /*-------------------- START 引脚：推挽输出 --------------------*/
    Hal_GPIO_Init (ADS1299_START_PIN_ENC, HAL_GPIO_MODE_OUTPUT_PP,
                   HAL_GPIO_SPEED_VERY_HIGH, 0);
    ADS1299_START_LOW();

    /*-------------------- DRDY 引脚：上拉输入 + 中断配置 --------------------*/
    Hal_GPIO_Init (ADS1299_DRDY_PIN_ENC, HAL_GPIO_MODE_INPUT_PU,
                   HAL_GPIO_SPEED_LOW, 0);

    /* 注册中断回调，触发下降沿，优先级 (0,0) */
    Hal_GPIO_IRQ_Config (ADS1299_DRDY_PIN_ENC, HAL_GPIO_IRQ_FALLING,
                         0, 0, ADS1299_DMA_StartRead);
    Hal_GPIO_IRQ_Enable (ADS1299_DRDY_PIN_ENC, true);
}

static void ADS1299_SPI_Init (void) {
    /* 开启 SPI2 外设时钟。 */
    SPI_ClockEnable (ADS1299_SPI_INSTANCE);

    SPI_InitTypeDef SPI_InitStructure = {0};
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    /* CH32H417 作为 SPI 主机，ADS1299 作为 SPI 从机。 */
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    /* ADS1299 命令、寄存器、数据均按 8 bit 字节传输。 */
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    /* ADS1299 使用 SPI Mode 1：CPOL=0，CPHA=1。 */
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    /* NSS 软件管理，真正的 CS 由 GPIO 手动控制。 */
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    /* 先使用较低 SPI 频率，硬件稳定后可根据 ADS1299 时序和主频适当提高。 */
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_Mode6;
    /* ADS1299 SPI 数据高位先传。 */
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    /* 不使用 CRC。 */
    SPI_InitStructure.SPI_CRCPolynomial = 7;

    SPI_Init (ADS1299_SPI_INSTANCE, &SPI_InitStructure);
    SPI_Cmd (ADS1299_SPI_INSTANCE, ENABLE);
}

static void ADS1299_DMA_Init (void) {

    DMA_InitTypeDef DMA_InitStructure = {0};
    DMA_ClockEnable (ADS1299_DMA_INSTANCE);

    SPI_I2S_DMACmd (ADS1299_SPI_INSTANCE, SPI_I2S_DMAReq_Rx, DISABLE);
    SPI_I2S_DMACmd (ADS1299_SPI_INSTANCE, SPI_I2S_DMAReq_Tx, DISABLE);

    DMA_Cmd (ADS1299_RX_DMA_CHANNEL, DISABLE);
    DMA_Cmd (ADS1299_TX_DMA_CHANNEL, DISABLE);

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADS1299_SPI_INSTANCE->DATAR);
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)ring_buffer[0];
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = FRAME_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;

    DMA_Init (ADS1299_RX_DMA_CHANNEL, &DMA_InitStructure);
    DMA_MuxChannelConfig (ADS1299_RX_DMAMUX_CHANNEL, ADS1299_RX_DMA_REQUEST);
    DMA_ITConfig (ADS1299_RX_DMA_CHANNEL, DMA_IT_TC, ENABLE);
    DMA_ITConfig (ADS1299_RX_DMA_CHANNEL, DMA_IT_TE, ENABLE);

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADS1299_SPI_INSTANCE->DATAR);
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)ads1299_dma_tx_dummy;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = FRAME_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;

    DMA_Init (ADS1299_TX_DMA_CHANNEL, &DMA_InitStructure);
    DMA_MuxChannelConfig (ADS1299_TX_DMAMUX_CHANNEL, ADS1299_TX_DMA_REQUEST);
    DMA_ITConfig (ADS1299_TX_DMA_CHANNEL, DMA_IT_TE, ENABLE);

    DMA_ClearITPendingBit (ADS1299_DMA_INSTANCE, ADS1299_RX_DMA_TC_FLAG);
    DMA_ClearITPendingBit (ADS1299_DMA_INSTANCE, ADS1299_RX_DMA_TE_FLAG);
    DMA_ClearITPendingBit (ADS1299_DMA_INSTANCE, ADS1299_TX_DMA_TC_FLAG);
    DMA_ClearITPendingBit (ADS1299_DMA_INSTANCE, ADS1299_TX_DMA_TE_FLAG);

    NVIC_SetPriority (ADS1299_RX_DMA_IRQn, 1);
    NVIC_EnableIRQ (ADS1299_RX_DMA_IRQn);

    NVIC_SetPriority (ADS1299_TX_DMA_IRQn, 1);
    NVIC_EnableIRQ (ADS1299_TX_DMA_IRQn);


    g_ads1299_spi_dma_busy = 0;
    g_ads1299_spi_dma_ok_count = 0;
    g_ads1299_spi_dma_lost_count = 0;
    g_ads1299_spi_dma_error_count = 0;
}

static void ADS1299_Register_Init (void) {
    /* 1. 上电等待 */
    Delay_Ms (ADS1299_POWER_ON_DELAY_MS);

    /* 2. 硬件复位 */
    ADS1299_ResetByPin();

    /* 3. 退出连续读模式 */
    ADS1299_SendCommand (ADS1299_CMD_SDATAC);
    Delay_Ms (10);

    /* 4. 配置 CONFIG1~CONFIG4 */
    ADS1299_WriteReg (ADS1299_REG_CONFIG1, 0x96);
    Delay_Ms (10);
    ADS1299_WriteReg (ADS1299_REG_CONFIG2, 0xC0);
    Delay_Ms (10);
    ADS1299_WriteReg (ADS1299_REG_CONFIG3, 0xEC);
    Delay_Ms (ADS1299_REF_STABLE_DELAY_MS);
    ADS1299_WriteReg (ADS1299_REG_CONFIG4, 0x02);
    Delay_Ms (ADS1299_REF_STABLE_DELAY_MS);

    /* 5. BIAS 配置 */
    ADS1299_WriteReg (ADS1299_REG_BIAS_SENSP, 0x0F);  /* CH1~CH4 加入 BIAS 正端检测/驱动 */
    Delay_Ms (10);
    ADS1299_WriteReg (ADS1299_REG_BIAS_SENSN, 0x0F);  /* CH1~CH4 加入 BIAS 负端检测/驱动 */
    Delay_Ms (10);

    /* 6. MISC1 */
    ADS1299_WriteReg (ADS1299_REG_MISC1, 0x00);
    Delay_Ms (10);

    /* 7. 通道配置 */
    ADS1299_WriteReg (ADS1299_REG_CH1SET, 0x60);
    Delay_Ms (10);
    ADS1299_WriteReg (ADS1299_REG_CH2SET, 0x60);
    Delay_Ms (10);
    ADS1299_WriteReg (ADS1299_REG_CH3SET, 0x60);
    Delay_Ms (10);
    ADS1299_WriteReg (ADS1299_REG_CH4SET, 0x60);
    Delay_Ms (10);
    ADS1299_WriteReg (ADS1299_REG_CH5SET, 0x81);
    Delay_Ms (10);
    ADS1299_WriteReg (ADS1299_REG_CH6SET, 0x81);
    Delay_Ms (10);
    ADS1299_WriteReg (ADS1299_REG_CH7SET, 0x81);
    Delay_Ms (10);
    ADS1299_WriteReg (ADS1299_REG_CH8SET, 0x81);
    Delay_Ms (10);
}

/* 底层 SPI 辅助函数 */
static uint8_t ADS1299_SPI_TxRxByte (uint8_t tx_data) {
    uint32_t timeout;

    /* 等待发送缓冲区为空，表示可以写入下一个字节。 */
    timeout = ADS1299_SPI_TIMEOUT;
    while (SPI_I2S_GetFlagStatus (ADS1299_SPI_INSTANCE, SPI_I2S_FLAG_TXE) == RESET) {
        if (timeout-- == 0) {
            return 0xFF;
        }
    }

    /* 写入一个字节后，SPI 硬件会自动产生 8 个 SCK 时钟。 */
    SPI_I2S_SendData (ADS1299_SPI_INSTANCE, tx_data);

    /* 等待接收缓冲区非空。SPI 是全双工：每发 1 字节，也会同时收到 1 字节。 */
    timeout = ADS1299_SPI_TIMEOUT;
    while (SPI_I2S_GetFlagStatus (ADS1299_SPI_INSTANCE, SPI_I2S_FLAG_RXNE) == RESET) {
        if (timeout-- == 0) {
            return 0xFF;
        }
    }

    return (uint8_t)SPI_I2S_ReceiveData (ADS1299_SPI_INSTANCE);
}

static void ADS1299_SPI_WaitBusy (void) {
    uint32_t timeout = ADS1299_SPI_TIMEOUT;

    /* 等待 SPI 总线不忙，防止最后一个 bit 还没发完就拉高 CS。 */
    while (SPI_I2S_GetFlagStatus (ADS1299_SPI_INSTANCE, SPI_I2S_FLAG_BSY) != RESET) {
        if (timeout-- == 0) {
            break;
        }
    }
}

static void ADS1299_SPI_ClearRxNE (void) {

    volatile uint16_t dummy;


    while (SPI_I2S_GetFlagStatus (ADS1299_SPI_INSTANCE, SPI_I2S_FLAG_RXNE) != RESET) {

        dummy = SPI_I2S_ReceiveData (ADS1299_SPI_INSTANCE);

        (void)dummy;
    }
}

/* 硬件复位与命令 */
static void ADS1299_ResetByPin (void) {
    /* 复位前先停止转换，避免复位期间 START 状态不确定。 */
    ADS1299_START_LOW();
    ADS1299_CS_HIGH();

    /* RESET 低脉冲。 */
    ADS1299_RESET_HIGH();
    Delay_Ms (10);
    ADS1299_RESET_LOW();
    Delay_Ms (ADS1299_RESET_LOW_DELAY_MS);
    ADS1299_RESET_HIGH();

    /* 等待 ADS1299 内部数字逻辑和寄存器恢复默认值。 */
    Delay_Ms (ADS1299_POST_RESET_DELAY_MS);
}

static void ADS1299_SendCommand (uint8_t cmd) {
    ADS1299_CS_LOW();
    Delay_Us (2);

    (void)ADS1299_SPI_TxRxByte (cmd);
    ADS1299_SPI_WaitBusy();

    Delay_Us (2);
    ADS1299_CS_HIGH();

    /* 命令之间留一点间隔，保证 ADS1299 有时间译码。 */
    Delay_Us (10);
}

/* 寄存器操作 */
static uint8_t ADS1299_ReadReg (uint8_t reg) {
    uint8_t value;

    ADS1299_CS_LOW();
    Delay_Us (2);

    /* 第 1 字节：RREG | 寄存器地址。 */
    (void)ADS1299_SPI_TxRxByte ((uint8_t)(ADS1299_CMD_RREG | (reg & 0x1F)));

    /* 第 2 字节：读取数量 - 1。读取 1 个寄存器，所以发送 0x00。 */
    (void)ADS1299_SPI_TxRxByte (0x00);

    /* 第 3 字节：发送 dummy byte，同时从 MISO 读回寄存器值。 */
    value = ADS1299_SPI_TxRxByte (0x00);
    ADS1299_SPI_WaitBusy();

    Delay_Us (2);
    ADS1299_CS_HIGH();
    Delay_Us (10);

    return value;
}

// static void ADS1299_ReadRegs (uint8_t start_reg, uint8_t *buf, uint8_t len) {
//     uint8_t i;
//     if ((buf == 0) || (len == 0)) {
//         return;
//     }
//     ADS1299_CS_LOW();
//     Delay_Us (2);
//     /* 起始寄存器地址。 */
//     (void)ADS1299_SPI_TxRxByte ((uint8_t)(ADS1299_CMD_RREG | (start_reg & 0x1F)));
//     /* 连续读取 len 个寄存器，第二字节要发送 len - 1。 */
//     (void)ADS1299_SPI_TxRxByte ((uint8_t)(len - 1));
//     for (i = 0; i < len; i++) {
//         buf[i] = ADS1299_SPI_TxRxByte (0x00);
//     }
//     ADS1299_SPI_WaitBusy();
//     Delay_Us (2);
//     ADS1299_CS_HIGH();
//     Delay_Us (10);
// }

static void ADS1299_WriteReg (uint8_t reg, uint8_t value) {
    ADS1299_CS_LOW();
    Delay_Us (2);

    /* 第 1 字节：WREG | 寄存器地址。 */
    (void)ADS1299_SPI_TxRxByte ((uint8_t)(ADS1299_CMD_WREG | (reg & 0x1F)));

    /* 第 2 字节：写入数量 - 1。写 1 个寄存器，所以发送 0x00。 */
    (void)ADS1299_SPI_TxRxByte (0x00);

    /* 第 3 字节：真正写入寄存器的数据。 */
    (void)ADS1299_SPI_TxRxByte (value);
    ADS1299_SPI_WaitBusy();

    Delay_Us (2);
    ADS1299_CS_HIGH();
    Delay_Us (10);
}

// static void ADS1299_WriteRegs (uint8_t start_reg, const uint8_t *buf, uint8_t len) {
//     uint8_t i;
//     if ((buf == 0) || (len == 0)) {
//         return;
//     }
//     ADS1299_CS_LOW();
//     Delay_Us (2);
//     /* 起始寄存器地址。 */
//     (void)ADS1299_SPI_TxRxByte ((uint8_t)(ADS1299_CMD_WREG | (start_reg & 0x1F)));
//     /* 连续写入 len 个寄存器，第二字节要发送 len - 1。 */
//     (void)ADS1299_SPI_TxRxByte ((uint8_t)(len - 1));
//     for (i = 0; i < len; i++) {
//         (void)ADS1299_SPI_TxRxByte (buf[i]);
//     }
//     ADS1299_SPI_WaitBusy();
//     Delay_Us (2);
//     ADS1299_CS_HIGH();
//     Delay_Us (10);
// }

/* 连续模式控制 */
static void ADS1299_StartContinuous (void) {
    /*
     * 拉高 START 引脚，启动 ADS1299 转换。
     * 随后发送 RDATAC，让 ADS1299 进入连续读数据模式。
     */
    ADS1299_START_HIGH();
    Delay_Ms (2);
    ADS1299_SendCommand (ADS1299_CMD_RDATAC);
    Delay_Ms (2);
}

// static void ADS1299_StopContinuous (void) {
//     /*
//      * 先发送 SDATAC 退出连续读模式，再拉低 START 停止转换。
//      * 后续如果要修改寄存器，必须先调用本函数或至少发送 SDATAC。
//      */
//     ADS1299_SendCommand (ADS1299_CMD_SDATAC);
//     Delay_Ms (2);
//     ADS1299_START_LOW();
// }

/* DMA 传输 */
void ADS1299_DMA_StartRead (void) {
    if (g_ads1299_spi_dma_busy != 0U) {
        g_ads1299_spi_dma_lost_count++;
        return;
    }

    g_ads1299_spi_dma_busy = 1U;

    SPI_I2S_DMACmd (ADS1299_SPI_INSTANCE, SPI_I2S_DMAReq_Rx, DISABLE);
    SPI_I2S_DMACmd (ADS1299_SPI_INSTANCE, SPI_I2S_DMAReq_Tx, DISABLE);

    DMA_Cmd (ADS1299_RX_DMA_CHANNEL, DISABLE);
    DMA_Cmd (ADS1299_TX_DMA_CHANNEL, DISABLE);

    DMA_ClearITPendingBit (ADS1299_DMA_INSTANCE, ADS1299_RX_DMA_TC_FLAG);
    DMA_ClearITPendingBit (ADS1299_DMA_INSTANCE, ADS1299_RX_DMA_TE_FLAG);
    DMA_ClearITPendingBit (ADS1299_DMA_INSTANCE, ADS1299_TX_DMA_TC_FLAG);
    DMA_ClearITPendingBit (ADS1299_DMA_INSTANCE, ADS1299_TX_DMA_TE_FLAG);


    ADS1299_SPI_ClearRxNE();


    ADS1299_RX_DMA_CHANNEL->MADDR = (uint32_t)ring_buffer[head];

    DMA_SetCurrDataCounter (ADS1299_RX_DMA_CHANNEL, FRAME_SIZE);


    ADS1299_TX_DMA_CHANNEL->MADDR = (uint32_t)ads1299_dma_tx_dummy;

    DMA_SetCurrDataCounter (ADS1299_TX_DMA_CHANNEL, FRAME_SIZE);


    ADS1299_CS_LOW();

    Delay_Us (2);


    DMA_Cmd (ADS1299_RX_DMA_CHANNEL, ENABLE);

    DMA_Cmd (ADS1299_TX_DMA_CHANNEL, ENABLE);


    SPI_I2S_DMACmd (ADS1299_SPI_INSTANCE, SPI_I2S_DMAReq_Rx, ENABLE);

    SPI_I2S_DMACmd (ADS1299_SPI_INSTANCE, SPI_I2S_DMAReq_Tx, ENABLE);
}

static void ADS1299_DMA_StopAndReleaseCS (void) {

    SPI_I2S_DMACmd (ADS1299_SPI_INSTANCE, SPI_I2S_DMAReq_Rx, DISABLE);

    SPI_I2S_DMACmd (ADS1299_SPI_INSTANCE, SPI_I2S_DMAReq_Tx, DISABLE);


    DMA_Cmd (ADS1299_RX_DMA_CHANNEL, DISABLE);

    DMA_Cmd (ADS1299_TX_DMA_CHANNEL, DISABLE);


    ADS1299_SPI_WaitBusy();

    Delay_Us (2);

    ADS1299_CS_HIGH();


    g_ads1299_spi_dma_busy = 0U;
}

/* 数据解析辅助 */
static int32_t ADS1299_SignExtend24 (uint32_t raw24) {
    /*
     * ADS1299 通道数据是 24 位二补码。
     * 若 bit23 为 1，表示负数，需要把高 8 位补 1，扩展成 32 位有符号数。
     */
    raw24 &= 0x00FFFFFFUL;
    if ((raw24 & 0x00800000UL) != 0U) {
        raw24 |= 0xFF000000UL;
    }

    return (int32_t)raw24;
}

float ADS1299_CodeToVolt (int32_t code, float vref, float gain) {
    return (float)((code * vref) / (gain * ADS1299_FULL_SCALE_CODE));
}

float ADS1299_CodeToMicroVolt (int32_t code, float vref, float gain) {
    return ADS1299_CodeToVolt (code, vref, gain) * 1000000.0f;
}

void ADS1299_ParseRawFrame (const uint8_t *frame_buf, uint32_t *status, int32_t ch_data[ADS1299_CHANNEL_NUM]) {
    uint8_t i;
    uint8_t index;
    uint32_t raw24;

    if ((frame_buf == 0) || (ch_data == 0)) {
        return;
    }

    /* 前 3 字节是状态字，不是通道脑电数据。 */
    if (status != 0) {
        *status = ((uint32_t)frame_buf[0] << 16) |
                  ((uint32_t)frame_buf[1] << 8) |
                  ((uint32_t)frame_buf[2]);
    }

    /* 后面 24 字节为 8 个通道，每通道 3 字节，24 bit 二补码，高字节在前。 */
    for (i = 0; i < ADS1299_CHANNEL_NUM; i++) {
        index = (uint8_t)(ADS1299_STATUS_BYTE_NUM + i * ADS1299_CHANNEL_BYTE_NUM);
        raw24 = ((uint32_t)frame_buf[index] << 16) |
                ((uint32_t)frame_buf[index + 1] << 8) |
                ((uint32_t)frame_buf[index + 2]);

        ch_data[i] = ADS1299_SignExtend24 (raw24);
    }
}

/* 从环形缓冲区读取一帧数据，成功返回 1，无数据返回 0 */
uint8_t ring_buffer_get_frame (uint8_t *dest) {
    if (head == tail) {
        return 0;  // 缓冲区为空
    }
    memcpy (dest, ring_buffer[tail], FRAME_SIZE);
    tail = (tail + 1) % RING_BUF_SIZE;
    return 1;
}

void DMA1_Channel1_IRQHandler (void) __attribute__ ((interrupt ("WCH-Interrupt-fast")));

void DMA1_Channel1_IRQHandler (void) {
    if (DMA_GetITStatus (ADS1299_DMA_INSTANCE, ADS1299_RX_DMA_TC_FLAG) != RESET) {
        DMA_ClearITPendingBit (ADS1299_DMA_INSTANCE, ADS1299_RX_DMA_TC_FLAG);

        ADS1299_DMA_StopAndReleaseCS();
        g_ads1299_spi_dma_ok_count++;

        head = (head + 1) % RING_BUF_SIZE;
        if (head == tail) {
            tail = (tail + 1) % RING_BUF_SIZE;
        }
    }
    if (DMA_GetITStatus (ADS1299_DMA_INSTANCE, ADS1299_RX_DMA_TE_FLAG) != RESET) {
        DMA_ClearITPendingBit (ADS1299_DMA_INSTANCE, ADS1299_RX_DMA_TE_FLAG);

        ADS1299_DMA_StopAndReleaseCS();
        g_ads1299_spi_dma_error_count++;
    }
}

void DMA1_Channel2_IRQHandler (void) __attribute__ ((interrupt ("WCH-Interrupt-fast")));

void DMA1_Channel2_IRQHandler (void) {
    if (DMA_GetITStatus (ADS1299_DMA_INSTANCE, ADS1299_TX_DMA_TE_FLAG) != RESET) {
        DMA_ClearITPendingBit (ADS1299_DMA_INSTANCE, ADS1299_TX_DMA_TE_FLAG);

        ADS1299_DMA_StopAndReleaseCS();
        g_ads1299_spi_dma_error_count++;
    }
}
