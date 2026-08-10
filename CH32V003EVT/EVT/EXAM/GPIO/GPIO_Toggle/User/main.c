/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Target MCU         : CH32V003F4U6, QFN20
 * Project            : Power-management co-processor
 * Description        : PD0/V_EN self-lock + 4-rail EN control + key long-press shutdown.
 *                      Power-up/down sequence is stored in Flash (last page) and
 *                      adjustable via I2C from the main MCU.
 *
 * Pin map (from PCB netlist, CH32V003F4U6 QFN-20):
 *   PD0  -> V_EN    power self-lock output (high = lock).
 *   PD5  -> V_KEY   key detect input (low = pressed, internal pull-up).
 *   PD3  -> 5V_EN   rail enable output (TPS63020 EN)
 *   PC4  -> P1_EN   rail enable output (EA3056 EN1)
 *   PC5  -> P2_EN   rail enable output (EA3056 EN2)
 *   PC7  -> VOUT_EN rail enable output (SY6280 EN)
 *   PC6  -> ISO_EN  rail enable output (digital isolator EN)
 *   PC1  -> I2C1 SDA (to U1 main MCU + U9 IMU via FPC, 4.7K pullup R30)
 *   PC2  -> I2C1 SCL (to U1 main MCU + U9 IMU via FPC, 4.7K pullup R29)
 *   PD6  -> USART1 TX (debug printf, Remap2)
 *
 * Key logic:
 *   - V_KEY/PD5: pressed = low (internal pull-up). Digital input, no ADC.
 *   - Pressed > 3 s  -> shutdown.
 *
 * Default power-up sequence (configurable in Flash):
 *   wait t_ven_ms(500) -> V_EN high -> 5V -> 300ms -> ISO_EN+P2 -> 500ms -> P1
 *   VOUT is NOT powered at boot; controlled via I2C (CMD_VOUT_ON/OFF).
 *
 * Shutdown sequence:
 *   all EN pins low -> V_EN stays high t_off_ven_ms(3000) -> V_EN low.
 *******************************************************************************/

#include "debug.h"
#include "ch32v00x_flash.h"
#include <string.h>

/* ============================================================================
 * 1. Compile-time configuration
 * ==========================================================================*/

#define USE_PRINTF_DEBUG         1       /* 1: USART1 printf on PD6(TX) Remap2 @115200 */

#define TICK_MS                  10U
#define KEY_DEBOUNCE_MS          20U
#define KEY_LONG_PRESS_MS        3000U    /* >3s press -> shutdown */

/* Key pressed level: low = pressed (internal pull-up).
 * If your key is active-high, change to Bit_SET and use IN_FLOATING. */
#define KEY_PRESSED_LEVEL        Bit_RESET

/* I2C slave configuration */
#define I2C_SLAVE_ADDR           0x20U   /* 7-bit I2C address (change as needed) */
#define I2C_CLOCK_SPEED          100000U /* 100 kHz standard mode */

/* I2C register map */
#define REG_CHIP_ID              0x00U   /* R: chip ID = 0x11 */
#define REG_FW_VER               0x01U   /* R: firmware version = 0x02 */
#define REG_STATE                0x02U   /* R: bit0=running, bit1=shutting_down */
#define REG_KEY_STATE            0x03U   /* R: bit0=pressed */
#define REG_KEY_HOLD_L           0x04U   /* R: key hold time ms (low byte) */
#define REG_KEY_HOLD_H           0x05U   /* R: key hold time ms (high byte) */
#define REG_RAIL_STATUS          0x10U   /* R: bit0=5V, bit1=P1, bit2=P2, bit3=VOUT, bit4=ISO */
#define REG_RAIL_CTRL            0x11U   /* R/W: same bits as RAIL_STATUS */
#define REG_VEN_STATE            0x12U   /* R: bit0=V_EN self-lock */
#define REG_CMD                  0x20U   /* W: see CMD_* */
#define REG_CFG_BASE             0x30U   /* R/W: power-up sequence config (maps pmic_cfg_t) */
#define REG_CFG_SAVE             0x50U   /* W: 0x5A -> save RAM config to Flash */
#define REG_CFG_RELOAD           0x51U   /* W: 0x5A -> reload config from Flash */

#define CMD_SHUTDOWN             0x01U
#define CMD_RAILS_ON             0x02U   /* re-run power-up sequence from Flash config */
#define CMD_RAILS_OFF            0x03U   /* all EN pins low */
#define CMD_VOUT_ON              0x04U
#define CMD_VOUT_OFF             0x05U

/* ============================================================================
 * 2. Pin map
 * ==========================================================================*/

#define VEN_PORT                 GPIOD
#define VEN_PIN                  GPIO_Pin_0      /* PD0, V_EN self-lock */

#define KEY_PORT                 GPIOD
#define KEY_PIN                  GPIO_Pin_5      /* PD5, V_KEY key detect */

#define EN_5V_PORT               GPIOD
#define EN_5V_PIN                GPIO_Pin_3      /* PD3, 5V_EN (TPS63020) */
#define EN_ISO_PORT              GPIOC
#define EN_ISO_PIN               GPIO_Pin_6      /* PC6, ISO_EN (isolator) */
#define EN_P1_PORT               GPIOC
#define EN_P1_PIN                GPIO_Pin_4      /* PC4, P1_EN (EA3056 EN1) */
#define EN_P2_PORT               GPIOC
#define EN_P2_PIN                GPIO_Pin_5      /* PC5, P2_EN (EA3056 EN2) */
#define EN_VOUT_PORT             GPIOC
#define EN_VOUT_PIN              GPIO_Pin_7      /* PC7, VOUT_EN (SY6280) */

/* Sequence step mask bits (used in Flash config steps) */
#define STEP_5V                  0x01U
#define STEP_ISO                 0x02U
#define STEP_P2                  0x04U
#define STEP_P1                  0x08U
#define STEP_VOUT                0x10U

/* ============================================================================
 * 2b. Flash-stored power-up sequence config (pmic_cfg_t)
 *     Stored in the last 64B Flash page @ 0x08003FC0.
 *     Offsets below are the on-wire I2C layout (little-endian).
 * ==========================================================================*/

#define PMIC_CFG_FLASH_ADDR      0x08003FC0U
#define PMIC_CFG_MAGIC           0x4D50U   /* 'P' 'M' */
#define PMIC_CFG_NUM_STEPS       6U
#define CFG_FLAG_VOUT_AUTO_ON    0x01U     /* if set, VOUT powers on at end of sequence */

typedef struct {
    uint16_t magic;               /* 0:  0x4D50 */
    uint8_t  version;             /* 2:  1 */
    uint8_t  flags;               /* 3:  CFG_FLAG_* */
    uint16_t t_ven_ms;            /* 4:  boot -> V_EN high */
    uint16_t t_off_ven_ms;        /* 6:  shutdown: EN low -> V_EN low */
    uint8_t  n_steps;             /* 8:  valid step count */
    uint8_t  reserved[3];         /* 9-11 */
    struct {
        uint8_t  mask;            /* step: pins to raise (STEP_* bits) */
        uint16_t delay_ms;        /*      delay to next step */
    } steps[PMIC_CFG_NUM_STEPS];  /* 12-29 */
    uint8_t  crc;                 /* 30: sum of bytes 0..29 & 0xFF */
} pmic_cfg_t;                     /* total 31 bytes */

/* Default sequence (matches spec: V_EN@500ms, 5V, +300ms ISO_EN+P2, +500ms P1) */
static const pmic_cfg_t cfg_default = {
    .magic        = PMIC_CFG_MAGIC,
    .version      = 1,
    .flags        = 0,            /* VOUT stays off at boot */
    .t_ven_ms     = 500,
    .t_off_ven_ms = 3000,
    .n_steps      = 3,
    .reserved     = {0, 0, 0},
    .steps = {
        { STEP_5V,            300 },
        { STEP_ISO | STEP_P2, 500 },
        { STEP_P1,            0   },
        { 0, 0 }, { 0, 0 }, { 0, 0 },
    },
    .crc = 0,
};

static pmic_cfg_t g_cfg;

/* ============================================================================
 * 3. State
 * ==========================================================================*/

#define STATE_RUN                0U
#define STATE_SHUTDOWN           1U

static volatile uint8_t g_state = STATE_RUN;

static uint8_t  key_raw_last    = 0;
static uint8_t  key_stable      = 0;
static uint16_t key_debounce_ms = 0;
static uint16_t key_hold_ms     = 0;

/* I2C slave state */
static volatile uint8_t  i2c_reg_ptr     = 0;   /* current register pointer */
static volatile uint8_t  i2c_first_byte  = 1;   /* 1 = next RX byte is register address */
static volatile uint8_t  i2c_cmd_pending = 0;   /* command from master, consumed by main loop */

/* ============================================================================
 * 4. Helpers
 * ==========================================================================*/

static void app_delay_ms(uint16_t ms)
{
    Delay_Ms(ms);
}

/* ============================================================================
 * 4a. Flash config: checksum
 * ==========================================================================*/

static uint8_t cfg_crc(const pmic_cfg_t *c)
{
    const uint8_t *p = (const uint8_t *)c;
    uint8_t sum = 0;
    for (uint32_t i = 0; i < 30U; i++) {
        sum = (uint8_t)(sum + p[i]);
    }
    return sum;
}

/* ============================================================================
 * 4b. Flash config: load / save
 * ==========================================================================*/

static void cfg_load(void)
{
    const uint8_t *p = (const uint8_t *)PMIC_CFG_FLASH_ADDR;
    memcpy(&g_cfg, p, sizeof(pmic_cfg_t));
    if (g_cfg.magic != PMIC_CFG_MAGIC || g_cfg.version != 1 ||
        g_cfg.n_steps == 0 || g_cfg.n_steps > PMIC_CFG_NUM_STEPS ||
        g_cfg.crc != cfg_crc(&g_cfg)) {
        memcpy(&g_cfg, &cfg_default, sizeof(pmic_cfg_t));
    }
    g_cfg.crc = cfg_crc(&g_cfg);
}

static void cfg_save(void)
{
    g_cfg.crc = cfg_crc(&g_cfg);

    __disable_irq();
    FLASH_Unlock();
    FLASH_ErasePage(PMIC_CFG_FLASH_ADDR);
    {
        const uint8_t *p = (const uint8_t *)&g_cfg;
        for (uint32_t i = 0; i < sizeof(pmic_cfg_t); i += 2) {
            uint16_t w = (uint16_t)p[i];
            if (i + 1 < sizeof(pmic_cfg_t)) w |= (uint16_t)(p[i + 1] << 8);
            FLASH_ProgramHalfWord(PMIC_CFG_FLASH_ADDR + i, w);
        }
    }
    FLASH_Lock();
    __enable_irq();
}

/* ============================================================================
 * 4b. UART debug (PD6 TX, USART1 115200 8N1, Remap2)
 *     V_KEY is on PD5, so USART1 TX cannot use default PD5 (NoRemap).
 *     Remap2 puts USART1 TX on PD6, which is unused on the PCB.
 * ========================================================================== */

#if USE_PRINTF_DEBUG
static void uart_debug_init(void)
{
    GPIO_InitTypeDef  gpio = {0};
    USART_InitTypeDef usart = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);

    /* USART1 Remap2: TX -> PD6 */
    GPIO_PinRemapConfig(GPIO_PartialRemap2_USART1, ENABLE);

    /* PD6: USART1 TX, AF push-pull */
    gpio.GPIO_Pin   = GPIO_Pin_6;
    gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOD, &gpio);

    usart.USART_BaudRate            = 115200;
    usart.USART_WordLength          = USART_WordLength_8b;
    usart.USART_StopBits            = USART_StopBits_1;
    usart.USART_Parity              = USART_Parity_No;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart.USART_Mode                = USART_Mode_Tx;
    USART_Init(USART1, &usart);
    USART_Cmd(USART1, ENABLE);
}
#endif

/* ============================================================================
 * 5. V_EN self-lock
 * ==========================================================================*/

static void ven_on(void)
{
    GPIO_SetBits(VEN_PORT, VEN_PIN);
}

static void ven_off(void)
{
    GPIO_ResetBits(VEN_PORT, VEN_PIN);
}

/* ============================================================================
 * 6. GPIO configuration
 * ==========================================================================*/

static void gpio_init_all(void)
{
    GPIO_InitTypeDef gpio = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);

    /* PD0/V_EN: push-pull output. Start LOW; raised later by power_up_sequence().
     * SystemInit already raised it, so explicitly pull it back down here. */
    gpio.GPIO_Pin   = VEN_PIN;
    gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(VEN_PORT, &gpio);
    ven_off();

    /* PD5/V_KEY: key detect input, internal pull-up (pressed = low). */
    gpio.GPIO_Pin  = KEY_PIN;
    gpio.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(KEY_PORT, &gpio);

    /* All rail EN pins: push-pull outputs, start low (off). */
    gpio.GPIO_Pin   = EN_5V_PIN;
    gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(EN_5V_PORT, &gpio);
    GPIO_ResetBits(EN_5V_PORT, EN_5V_PIN);

    gpio.GPIO_Pin   = EN_ISO_PIN | EN_P1_PIN | EN_P2_PIN | EN_VOUT_PIN;
    gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOC, &gpio);
    GPIO_ResetBits(EN_ISO_PORT,  EN_ISO_PIN);
    GPIO_ResetBits(EN_P1_PORT,   EN_P1_PIN);
    GPIO_ResetBits(EN_P2_PORT,   EN_P2_PIN);
    GPIO_ResetBits(EN_VOUT_PORT, EN_VOUT_PIN);
}

/* ============================================================================
 * 7. Rail EN control
 *    Power-up sequence and timing come from Flash config (g_cfg.steps[]).
 *    Power-off: all EN low immediately (V_EN released later by shutdown).
 * ==========================================================================*/

/* Raise only the pins in mask; pins already on stay on (cumulative sequence). */
static void en_apply_mask(uint8_t mask)
{
    if(mask & STEP_5V)   GPIO_SetBits(EN_5V_PORT,   EN_5V_PIN);
    if(mask & STEP_ISO)  GPIO_SetBits(EN_ISO_PORT,  EN_ISO_PIN);
    if(mask & STEP_P2)   GPIO_SetBits(EN_P2_PORT,   EN_P2_PIN);
    if(mask & STEP_P1)   GPIO_SetBits(EN_P1_PORT,   EN_P1_PIN);
    if(mask & STEP_VOUT) GPIO_SetBits(EN_VOUT_PORT, EN_VOUT_PIN);
}

static void en_all_off(void)
{
    GPIO_ResetBits(EN_5V_PORT,   EN_5V_PIN);
    GPIO_ResetBits(EN_ISO_PORT,  EN_ISO_PIN);
    GPIO_ResetBits(EN_P2_PORT,   EN_P2_PIN);
    GPIO_ResetBits(EN_P1_PORT,   EN_P1_PIN);
    GPIO_ResetBits(EN_VOUT_PORT, EN_VOUT_PIN);
}

static void en_vout_set(uint8_t on)
{
    if (on) GPIO_SetBits(EN_VOUT_PORT, EN_VOUT_PIN);
    else    GPIO_ResetBits(EN_VOUT_PORT, EN_VOUT_PIN);
}

/* Power-up: wait t_ven_ms, lock V_EN, then run the Flash-configured rail steps. */
static void power_up_sequence(void)
{
#if USE_PRINTF_DEBUG
    printf("power-up: wait %ums, V_EN...\r\n", g_cfg.t_ven_ms);
#endif
    app_delay_ms(g_cfg.t_ven_ms);
    ven_on();
#if USE_PRINTF_DEBUG
    printf("V_EN high, rails: ");
#endif
    for (uint8_t i = 0; i < g_cfg.n_steps && i < PMIC_CFG_NUM_STEPS; i++) {
        en_apply_mask(g_cfg.steps[i].mask);
#if USE_PRINTF_DEBUG
        printf("step%d(mask=0x%02X) ", i, g_cfg.steps[i].mask);
#endif
        if (g_cfg.steps[i].delay_ms) {
            app_delay_ms(g_cfg.steps[i].delay_ms);
        }
    }
    if (g_cfg.flags & CFG_FLAG_VOUT_AUTO_ON) {
        en_vout_set(1);
    }
#if USE_PRINTF_DEBUG
    printf("\r\n");
#endif
}

/* Shutdown: all EN low, keep V_EN for t_off_ven_ms, then release self-lock. */
static void shutdown_sequence(void)
{
    g_state = STATE_SHUTDOWN;
    en_all_off();
#if USE_PRINTF_DEBUG
    printf("rails off, hold V_EN %ums...\r\n", g_cfg.t_off_ven_ms);
#endif
    app_delay_ms(g_cfg.t_off_ven_ms);
    ven_off();
#if USE_PRINTF_DEBUG
    printf("V_EN low, power loss expected\r\n");
#endif

    /* After V_EN goes low, the LDO enable chain loses EN and MCU will power off.
     * If the key is still held, MCU may remain powered until the user releases it.
     */
    while(1)
    {
        __WFI();
    }
}

/* ============================================================================
 * 7b. I2C slave: register map read/write
 *     PC1=SDA, PC2=SCL, I2C1, 7-bit address, interrupt-driven.
 *     Protocol: write [reg_addr][data...], then read [data...] from reg_ptr.
 * ========================================================================== */

static uint8_t rail_get_status(void)
{
    uint8_t s = 0;
    if(GPIO_ReadOutputDataBit(EN_5V_PORT,   EN_5V_PIN))   s |= 0x01; /* 5V */
    if(GPIO_ReadOutputDataBit(EN_P1_PORT,   EN_P1_PIN))   s |= 0x02; /* P1 */
    if(GPIO_ReadOutputDataBit(EN_P2_PORT,   EN_P2_PIN))   s |= 0x04; /* P2 */
    if(GPIO_ReadOutputDataBit(EN_VOUT_PORT, EN_VOUT_PIN)) s |= 0x08; /* VOUT */
    if(GPIO_ReadOutputDataBit(EN_ISO_PORT,  EN_ISO_PIN))  s |= 0x10; /* ISO */
    return s;
}

static void rail_set_ctrl(uint8_t mask)
{
    if(mask & 0x01) GPIO_SetBits(EN_5V_PORT,   EN_5V_PIN);
    else            GPIO_ResetBits(EN_5V_PORT,   EN_5V_PIN);
    if(mask & 0x02) GPIO_SetBits(EN_P1_PORT,   EN_P1_PIN);
    else            GPIO_ResetBits(EN_P1_PORT,   EN_P1_PIN);
    if(mask & 0x04) GPIO_SetBits(EN_P2_PORT,   EN_P2_PIN);
    else            GPIO_ResetBits(EN_P2_PORT,   EN_P2_PIN);
    if(mask & 0x08) GPIO_SetBits(EN_VOUT_PORT, EN_VOUT_PIN);
    else            GPIO_ResetBits(EN_VOUT_PORT, EN_VOUT_PIN);
    if(mask & 0x10) GPIO_SetBits(EN_ISO_PORT,  EN_ISO_PIN);
    else            GPIO_ResetBits(EN_ISO_PORT,  EN_ISO_PIN);
}

static uint8_t cfg_reg_read(uint8_t reg)
{
    if(reg >= REG_CFG_BASE && reg < REG_CFG_BASE + sizeof(pmic_cfg_t))
    {
        return ((const uint8_t *)&g_cfg)[reg - REG_CFG_BASE];
    }
    return 0xFFU;
}

static void cfg_reg_write(uint8_t reg, uint8_t val)
{
    if(reg >= REG_CFG_BASE && reg < REG_CFG_BASE + sizeof(pmic_cfg_t))
    {
        ((uint8_t *)&g_cfg)[reg - REG_CFG_BASE] = val;
    }
}

static uint8_t i2c_reg_read(uint8_t reg)
{
    switch(reg)
    {
        case REG_CHIP_ID:     return 0x11U;
        case REG_FW_VER:      return 0x02U;
        case REG_STATE:       return g_state;
        case REG_KEY_STATE:   return key_stable;
        case REG_KEY_HOLD_L:  return (uint8_t)(key_hold_ms & 0xFFU);
        case REG_KEY_HOLD_H:  return (uint8_t)(key_hold_ms >> 8);
        case REG_RAIL_STATUS: return rail_get_status();
        case REG_RAIL_CTRL:   return rail_get_status();
        case REG_VEN_STATE:   return (GPIO_ReadOutputDataBit(VEN_PORT, VEN_PIN) == Bit_SET) ? 1U : 0U;
        default:              return cfg_reg_read(reg);
    }
}

static void i2c_reg_write(uint8_t reg, uint8_t val)
{
    switch(reg)
    {
        case REG_RAIL_CTRL:
            rail_set_ctrl(val);
            break;
        case REG_CMD:
            i2c_cmd_pending = val;
            break;
        case REG_CFG_SAVE:
            if(val == 0x5AU)
            {
#if USE_PRINTF_DEBUG
                printf("cfg save to flash\r\n");
#endif
                cfg_save();
            }
            break;
        case REG_CFG_RELOAD:
            if(val == 0x5AU)
            {
                cfg_load();
#if USE_PRINTF_DEBUG
                printf("cfg reloaded\r\n");
#endif
            }
            break;
        default:
            cfg_reg_write(reg, val);
            break;
    }
}

static void i2c_slave_init(void)
{
    GPIO_InitTypeDef  gpio = {0};
    I2C_InitTypeDef   i2c  = {0};
    NVIC_InitTypeDef  nvic = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    /* PC1=SDA, PC2=SCL: AF open-drain (external 4.7K pullups R29/R30 on PCB) */
    gpio.GPIO_Pin   = GPIO_Pin_1 | GPIO_Pin_2;
    gpio.GPIO_Mode  = GPIO_Mode_AF_OD;
    gpio.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOC, &gpio);

    i2c.I2C_ClockSpeed          = I2C_CLOCK_SPEED;
    i2c.I2C_Mode                = I2C_Mode_I2C;
    i2c.I2C_DutyCycle           = I2C_DutyCycle_2;
    i2c.I2C_OwnAddress1         = I2C_SLAVE_ADDR;
    i2c.I2C_Ack                 = I2C_Ack_Enable;
    i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C1, &i2c);
    I2C_Cmd(I2C1, ENABLE);

    I2C_ITConfig(I2C1, I2C_IT_BUF, ENABLE);
    I2C_ITConfig(I2C1, I2C_IT_EVT, ENABLE);
    I2C_ITConfig(I2C1, I2C_IT_ERR, ENABLE);

    nvic.NVIC_IRQChannel                   = I2C1_EV_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 2;
    nvic.NVIC_IRQChannelSubPriority        = 0;
    nvic.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&nvic);

    nvic.NVIC_IRQChannel                   = I2C1_ER_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 2;
    nvic.NVIC_IRQChannelSubPriority        = 1;
    nvic.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&nvic);
}

/* I2C1 event interrupt: handle address match, RX, TX, stop */
void I2C1_EV_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void I2C1_EV_IRQHandler(void)
{
    if(I2C_GetITStatus(I2C1, I2C_IT_ADDR) != RESET)
    {
        if(I2C_GetFlagStatus(I2C1, I2C_FLAG_TRA) && I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE))
        {
            /* Master reads: slave transmits starting from reg_ptr */
            I2C_SendData(I2C1, i2c_reg_read(i2c_reg_ptr));
            i2c_reg_ptr++;
        }
        else
        {
            /* Master writes: slave receives, first byte is register address */
            i2c_first_byte = 1;
            (void)I2C_ReadRegister(I2C1, I2C_Register_STAR2);
        }
    }
    else if(I2C_GetITStatus(I2C1, I2C_IT_RXNE) != RESET)
    {
        uint8_t data = I2C_ReceiveData(I2C1);
        if(i2c_first_byte)
        {
            i2c_reg_ptr    = data;
            i2c_first_byte = 0;
        }
        else
        {
            i2c_reg_write(i2c_reg_ptr, data);
            i2c_reg_ptr++;
        }
    }
    else if(I2C_GetITStatus(I2C1, I2C_IT_TXE) != RESET)
    {
        I2C_SendData(I2C1, i2c_reg_read(i2c_reg_ptr));
        i2c_reg_ptr++;
    }
    else if(I2C_GetITStatus(I2C1, I2C_IT_STOPF) != RESET)
    {
        I2C1->CTLR1 = I2C1->CTLR1;
        (void)I2C1->STAR1;
    }
    else if(I2C_GetITStatus(I2C1, I2C_IT_BTF) != RESET)
    {
        (void)I2C_ReadRegister(I2C1, I2C_Register_STAR1);
        (void)I2C_ReceiveData(I2C1);
    }
    else if(I2C_GetITStatus(I2C1, I2C_IT_SB) != RESET)
    {
        (void)I2C_ReadRegister(I2C1, I2C_Register_STAR1);
        (void)I2C_ReceiveData(I2C1);
    }
}

/* I2C1 error interrupt: handle NACK (master finished reading) */
void I2C1_ER_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void I2C1_ER_IRQHandler(void)
{
    if(I2C_GetITStatus(I2C1, I2C_IT_AF) != RESET)
    {
        I2C_ClearITPendingBit(I2C1, I2C_IT_AF);
    }
}


/* ============================================================================
 * 8. Key task: PD5 digital input, debounce, track press duration.
 *    Returns 1 when long-press threshold reached.
 * ==========================================================================*/

static uint8_t key_is_pressed_raw(void)
{
    return (GPIO_ReadInputDataBit(KEY_PORT, KEY_PIN) == KEY_PRESSED_LEVEL) ? 1U : 0U;
}

static uint8_t key_task(void)
{
    uint8_t raw = key_is_pressed_raw();
    uint8_t long_press = 0;

    if(raw != key_raw_last)
    {
        key_raw_last    = raw;
        key_debounce_ms = KEY_DEBOUNCE_MS;
        return 0;
    }

    if(key_debounce_ms > 0)
    {
        if(key_debounce_ms >= TICK_MS) key_debounce_ms -= TICK_MS;
        else key_debounce_ms = 0;
        return 0;
    }

    if(raw != key_stable)
    {
        key_stable  = raw;
        key_hold_ms = 0;
    }

    if(key_stable)
    {
        if(key_hold_ms < KEY_LONG_PRESS_MS) key_hold_ms += TICK_MS;
        if(key_hold_ms >= KEY_LONG_PRESS_MS) long_press = 1;
    }

    return long_press;
}

/* ============================================================================
 * 9. Shutdown: all EN low, hold V_EN t_off_ven_ms, then release self-lock.
 * ==========================================================================*/

static void enter_shutdown(void)
{
    shutdown_sequence();
}

/* ============================================================================
 * 10. Main
 * ==========================================================================*/

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();

#if USE_PRINTF_DEBUG
    uart_debug_init();
    printf("CH32V003F4U6 power manager v2 start\r\n");
    printf("PD0/V_EN after SystemInit: %d\r\n",
           (GPIO_ReadInputDataBit(VEN_PORT, VEN_PIN) == Bit_SET) ? 1 : 0);
#endif

    /* Configure GPIO: all EN low, V_EN low (SystemInit raised it, pull back down). */
    gpio_init_all();

    /* I2C slave init (PC1=SDA, PC2=SCL, addr=0x20) for main MCU communication. */
    i2c_slave_init();

    /* Load power-up sequence config from Flash (falls back to defaults). */
    cfg_load();

#if USE_PRINTF_DEBUG
    printf("PD0/V_EN after gpio_init: %d\r\n",
           (GPIO_ReadInputDataBit(VEN_PORT, VEN_PIN) == Bit_SET) ? 1 : 0);
    printf("PD5/V_KEY: %d\r\n",
           (GPIO_ReadInputDataBit(KEY_PORT, KEY_PIN) == Bit_SET) ? 1 : 0);
    printf("I2C slave addr=0x%02X\r\n", I2C_SLAVE_ADDR);
    printf("cfg: t_ven=%ums t_off=%ums n_steps=%u\r\n",
           g_cfg.t_ven_ms, g_cfg.t_off_ven_ms, g_cfg.n_steps);
#endif

    /* Power-up sequence from Flash config. */
    power_up_sequence();

#if USE_PRINTF_DEBUG
    printf("entering RUN\r\n");
#endif
    g_state = STATE_RUN;

    while(1)
    {
        uint8_t key_long = key_task();

        /* Process I2C commands from main MCU */
        if(i2c_cmd_pending != 0)
        {
            uint8_t cmd = i2c_cmd_pending;
            i2c_cmd_pending = 0;
            switch(cmd)
            {
                case CMD_SHUTDOWN:
#if USE_PRINTF_DEBUG
                    printf("I2C cmd: shutdown\r\n");
#endif
                    enter_shutdown();
                    break;
                case CMD_RAILS_ON:
#if USE_PRINTF_DEBUG
                    printf("I2C cmd: rails on\r\n");
#endif
                    power_up_sequence();
                    break;
                case CMD_RAILS_OFF:
#if USE_PRINTF_DEBUG
                    printf("I2C cmd: rails off\r\n");
#endif
                    en_all_off();
                    break;
                case CMD_VOUT_ON:
#if USE_PRINTF_DEBUG
                    printf("I2C cmd: VOUT on\r\n");
#endif
                    en_vout_set(1);
                    break;
                case CMD_VOUT_OFF:
#if USE_PRINTF_DEBUG
                    printf("I2C cmd: VOUT off\r\n");
#endif
                    en_vout_set(0);
                    break;
                default:
                    break;
            }
        }

        if(g_state == STATE_RUN)
        {
            if(key_long)
            {
#if USE_PRINTF_DEBUG
                printf("shutdown: key long press %ums\r\n", key_hold_ms);
#endif
                enter_shutdown();
            }
        }

#if USE_PRINTF_DEBUG
        {
            static uint16_t log_ms = 0;
            log_ms += TICK_MS;
            if(log_ms >= 1000U)
            {
                log_ms = 0;
                printf("RUN key=%d rails=0x%02X\r\n", key_stable, rail_get_status());
            }
        }
#endif

        app_delay_ms(TICK_MS);
    }
}
