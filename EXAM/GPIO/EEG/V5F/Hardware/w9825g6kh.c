#include "w9825g6kh.h"
#include "ch32h417.h"
#include "debug.h"

#define FMC_SDRAM_CMD_TARGET_BANK1     FMC_SDCMR_CTB1
#define FMC_SDRAM_CMD_CLK_ENABLE       1u
#define FMC_SDRAM_CMD_PALL             2u
#define FMC_SDRAM_CMD_AUTOREFRESH      3u
#define FMC_SDRAM_CMD_LOAD_MODE        4u

#define MODEREG_BURST_LENGTH_1         0x0000u
#define MODEREG_BURST_TYPE_SEQUENTIAL  0x0000u
#define MODEREG_CAS_LATENCY_3          0x0030u
#define MODEREG_OPERATING_STANDARD     0x0000u
#define MODEREG_WRITEBURST_SINGLE      0x0200u

#define SDRAM_AF_PP(port, pin, af) \
    do { \
        GPIO_PinAFConfig(port, pin, af); \
        g.GPIO_Pin = (1U << pin); \
        GPIO_Init(port, &g); \
    } while(0)

static void sdram_gpio_config(void)
{
    GPIO_InitTypeDef g = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOB, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOC, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOD, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOE, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOF, ENABLE);

    g.GPIO_Speed = GPIO_Speed_Very_High;
    g.GPIO_Mode  = GPIO_Mode_AF_PP;

    SDRAM_AF_PP(W9825_SDCLK_PORT,  W9825_SDCLK_PIN,  W9825_SDCLK_AF);
    SDRAM_AF_PP(W9825_SDCKE_PORT,  W9825_SDCKE_PIN,  W9825_SDCKE_AF);
    SDRAM_AF_PP(W9825_SDNE_PORT,   W9825_SDNE_PIN,   W9825_SDNE_AF);
    SDRAM_AF_PP(W9825_NRAS_PORT,   W9825_NRAS_PIN,   W9825_NRAS_AF);
    SDRAM_AF_PP(W9825_NCAS_PORT,   W9825_NCAS_PIN,   W9825_NCAS_AF);
    SDRAM_AF_PP(W9825_SDNWE_PORT,  W9825_SDNWE_PIN,  W9825_SDNWE_AF);
    SDRAM_AF_PP(W9825_DQM0_PORT,   W9825_DQM0_PIN,   W9825_DQM0_AF);
    SDRAM_AF_PP(W9825_DQM1_PORT,   W9825_DQM1_PIN,   W9825_DQM1_AF);
    SDRAM_AF_PP(W9825_BA0_PORT,    W9825_BA0_PIN,    W9825_BA0_AF);
    SDRAM_AF_PP(W9825_BA1_PORT,    W9825_BA1_PIN,    W9825_BA1_AF);

    SDRAM_AF_PP(W9825_A0_PORT,  W9825_A0_PIN,  W9825_A0_AF);
    SDRAM_AF_PP(W9825_A1_PORT,  W9825_A1_PIN,  W9825_A1_AF);
    SDRAM_AF_PP(W9825_A2_PORT,  W9825_A2_PIN,  W9825_A2_AF);
    SDRAM_AF_PP(W9825_A3_PORT,  W9825_A3_PIN,  W9825_A3_AF);
    SDRAM_AF_PP(W9825_A4_PORT,  W9825_A4_PIN,  W9825_A4_AF);
    SDRAM_AF_PP(W9825_A5_PORT,  W9825_A5_PIN,  W9825_A5_AF);
    SDRAM_AF_PP(W9825_A6_PORT,  W9825_A6_PIN,  W9825_A6_AF);
    SDRAM_AF_PP(W9825_A7_PORT,  W9825_A7_PIN,  W9825_A7_AF);
    SDRAM_AF_PP(W9825_A8_PORT,  W9825_A8_PIN,  W9825_A8_AF);
    SDRAM_AF_PP(W9825_A9_PORT,  W9825_A9_PIN,  W9825_A9_AF);
    SDRAM_AF_PP(W9825_A10_PORT, W9825_A10_PIN, W9825_A10_AF);
    SDRAM_AF_PP(W9825_A11_PORT, W9825_A11_PIN, W9825_A11_AF);
    SDRAM_AF_PP(W9825_A12_PORT, W9825_A12_PIN, W9825_A12_AF);

    SDRAM_AF_PP(W9825_D0_PORT,  W9825_D0_PIN,  W9825_D0_AF);
    SDRAM_AF_PP(W9825_D1_PORT,  W9825_D1_PIN,  W9825_D1_AF);
    SDRAM_AF_PP(W9825_D2_PORT,  W9825_D2_PIN,  W9825_D2_AF);
    SDRAM_AF_PP(W9825_D3_PORT,  W9825_D3_PIN,  W9825_D3_AF);
    SDRAM_AF_PP(W9825_D4_PORT,  W9825_D4_PIN,  W9825_D4_AF);
    SDRAM_AF_PP(W9825_D5_PORT,  W9825_D5_PIN,  W9825_D5_AF);
    SDRAM_AF_PP(W9825_D6_PORT,  W9825_D6_PIN,  W9825_D6_AF);
    SDRAM_AF_PP(W9825_D7_PORT,  W9825_D7_PIN,  W9825_D7_AF);
    SDRAM_AF_PP(W9825_D8_PORT,  W9825_D8_PIN,  W9825_D8_AF);
    SDRAM_AF_PP(W9825_D9_PORT,  W9825_D9_PIN,  W9825_D9_AF);
    SDRAM_AF_PP(W9825_D10_PORT, W9825_D10_PIN, W9825_D10_AF);
    SDRAM_AF_PP(W9825_D11_PORT, W9825_D11_PIN, W9825_D11_AF);
    SDRAM_AF_PP(W9825_D12_PORT, W9825_D12_PIN, W9825_D12_AF);
    SDRAM_AF_PP(W9825_D13_PORT, W9825_D13_PIN, W9825_D13_AF);
    SDRAM_AF_PP(W9825_D14_PORT, W9825_D14_PIN, W9825_D14_AF);
    SDRAM_AF_PP(W9825_D15_PORT, W9825_D15_PIN, W9825_D15_AF);
}

static void sdram_send_cmd(uint8_t cmd, uint8_t refresh, uint16_t regval)
{
    FMC_SDRAM_SendCMDConfig(FMC_SDRAM_CMD_TARGET_BANK1, cmd, refresh, regval);
}

static void sdram_init_sequence(void)
{
    sdram_send_cmd(FMC_SDRAM_CMD_CLK_ENABLE, 1, 0);
    sdram_send_cmd(FMC_SDRAM_CMD_PALL, 1, 0);
    sdram_send_cmd(FMC_SDRAM_CMD_AUTOREFRESH, 8, 0);
    uint32_t mode = MODEREG_BURST_LENGTH_1 |
                    MODEREG_BURST_TYPE_SEQUENTIAL |
                    MODEREG_CAS_LATENCY_3 |
                    MODEREG_OPERATING_STANDARD |
                    MODEREG_WRITEBURST_SINGLE;
    sdram_send_cmd(FMC_SDRAM_CMD_LOAD_MODE, 1, mode);
    FMC_SDRAM_SetRefreshCnt(677);
}

int W9825_Init(void)
{
    FMC_SDRAM_InitTypeDef init = {0};
    FMC_SDRAM_TimingTypeDef timing = {0};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_FMC, ENABLE);
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR, ENABLE);
    PWR_VIO18ModeCfg(PWR_VIO18CFGMODE_SW);
    PWR_VIO18LevelCfg(PWR_VIO18Level_MODE3);

    sdram_gpio_config();

    init.FMC_Bank               = FMC_Bank5_SDRAM;
    init.FMC_ColumnBitsNumber   = FMC_ColumnBitsNumber_9;
    init.FMC_RowBitsNumber      = FMC_ROWBitsNumber_13;
    init.FMC_MemoryDataWidth    = FMC_MemoryDataWidth_16;
    init.FMC_InternalBankNumber = FMC_InternalBankNumber_4;
    init.FMC_CASLatency         = FMC_CASLatency_3CLk;
    init.FMC_WriteProtection    = FMC_WriteProtection_Disable;
    init.FMC_SDClockPeriod      = 1;
    init.FMC_ReadBurst          = FMC_ReadBurst_Disable;
    init.FMC_ReadPipeDelay      = FMC_ReadPipeDelay_none;
    init.FMC_PHASE_SEL          = 0xA;

    timing.FMC_LoadToActiveDelay    = 2;
    timing.FMC_ExitSelfRefreshDelay = 8;
    timing.FMC_SelfRefreshTime      = 5;
    timing.FMC_RowCycleDelay         = 6;
    timing.FMC_WriteRecoveryTime    = 2;
    timing.FMC_RPDelay              = 2;
    timing.FMC_RCDDelay             = 2;
    init.FMC_SDRAM_Timing = &timing;

    FMC_SDRAM_Init(&init);
    FMC_Bank5_6->MISC |= (1 << 15);
    FMC_Bank5_6->MISC |= (1 << 16);
    sdram_init_sequence();
    FMC_Bank1->BTCR[0] |= (1 << 24);

    return 0;
}

void W9825_WriteBuffer(uint8_t *buf, uint32_t addr, uint32_t n)
{
    for (; n != 0; n--) {
        *(volatile uint8_t *)(W9825_SDRAM_BASE + addr) = *buf;
        addr++;
        buf++;
    }
}

void W9825_ReadBuffer(uint8_t *buf, uint32_t addr, uint32_t n)
{
    for (; n != 0; n--) {
        *buf++ = *(volatile uint8_t *)(W9825_SDRAM_BASE + addr);
        addr++;
    }
}
