#ifndef CONFIG_H
#define CONFIG_H

#include "ch32h417.h"
#include "hal_gpio.h"

#ifdef __ADS1299_H
/*============================ ADS1299 引脚配置 ============================*/
/* CS 片选 */
#define ADS1299_CS_PORT          0       // GPIOB
#define ADS1299_CS_PIN           8
#define ADS1299_CS_PIN_ENC       HAL_GPIO_PIN(ADS1299_CS_PORT, ADS1299_CS_PIN)

/* DRDY 数据就绪 */
#define ADS1299_DRDY_PORT        2       // GPIOB
#define ADS1299_DRDY_PIN         6
#define ADS1299_DRDY_PIN_ENC     HAL_GPIO_PIN(ADS1299_DRDY_PORT, ADS1299_DRDY_PIN)

/* RESET 复位 */
#define ADS1299_RESET_PORT       2       // GPIOE
#define ADS1299_RESET_PIN        7
#define ADS1299_RESET_PIN_ENC    HAL_GPIO_PIN(ADS1299_RESET_PORT, ADS1299_RESET_PIN)

/* START 启动转换 */
#define ADS1299_START_PORT       0       // GPIOB
#define ADS1299_START_PIN        9
#define ADS1299_START_PIN_ENC    HAL_GPIO_PIN(ADS1299_START_PORT, ADS1299_START_PIN)

/*============================ SPI 外设配置 ============================*/
#define ADS1299_SPI_INSTANCE     SPI3
/*============================ SPI 引脚配置 ============================*/
/* SCK - 时钟 */
#define ADS1299_SCK_PORT     0       // GPIOB
#define ADS1299_SCK_PIN      14
#define ADS1299_SCK_AF       GPIO_AF1
#define ADS1299_SCK_ENC      HAL_GPIO_PIN(ADS1299_SCK_PORT, ADS1299_SCK_PIN)

/* MISO - 主入从出 */
#define ADS1299_MISO_PORT    2      
#define ADS1299_MISO_PIN     9
#define ADS1299_MISO_AF      GPIO_AF5
#define ADS1299_MISO_ENC     HAL_GPIO_PIN(ADS1299_MISO_PORT, ADS1299_MISO_PIN)

/* MOSI - 主出从入 */
#define ADS1299_MOSI_PORT    0       // GPIOB
#define ADS1299_MOSI_PIN     13
#define ADS1299_MOSI_AF      GPIO_AF1
#define ADS1299_MOSI_ENC     HAL_GPIO_PIN(ADS1299_MOSI_PORT, ADS1299_MOSI_PIN)

/*============================ DMA 通道配置 ============================*/
#define ADS1299_DMA_INSTANCE     DMA1

#define ADS1299_TX_DMA_REQUEST   67U
#define ADS1299_RX_DMA_REQUEST   68U

#define ADS1299_RX_DMA_CHANNEL    DMA1_Channel1
#define ADS1299_TX_DMA_CHANNEL    DMA1_Channel2

#define ADS1299_RX_DMAMUX_CHANNEL DMA_MuxChannel1
#define ADS1299_TX_DMAMUX_CHANNEL DMA_MuxChannel2

/*============================ DMA 中断标志配置 ============================*/
#define ADS1299_RX_DMA_TC_FLAG    DMA1_IT_TC1   // 通道1传输完成
#define ADS1299_RX_DMA_TE_FLAG    DMA1_IT_TE1   // 通道1传输错误
#define ADS1299_TX_DMA_TC_FLAG    DMA1_IT_TC2   // 通道2传输完成
#define ADS1299_TX_DMA_TE_FLAG    DMA1_IT_TE2   // 通道2传输错误

/*============================ DMA 中断号配置 ============================*/
#define ADS1299_RX_DMA_IRQn       DMA1_Channel1_IRQn
#define ADS1299_TX_DMA_IRQn       DMA1_Channel2_IRQn
#endif

#ifdef __OLED_H
/* 硬件 SPI 实例 */
#define OLED_SPI_INSTANCE       SPI1

/* SPI 引脚定义（硬件/软件共用） */
#define OLED_CS_PORT            1   // GPIOA
#define OLED_CS_PIN             0
#define OLED_CS_PIN_ENC         HAL_GPIO_PIN(OLED_CS_PORT, OLED_CS_PIN)

#define OLED_RES_PORT           2   // GPIOA
#define OLED_RES_PIN            4
#define OLED_RES_PIN_ENC        HAL_GPIO_PIN(OLED_RES_PORT, OLED_RES_PIN)

#define OLED_DC_PORT            2   // GPIOA
#define OLED_DC_PIN             5
#define OLED_DC_PIN_ENC         HAL_GPIO_PIN(OLED_DC_PORT, OLED_DC_PIN)

#define OLED_SCK_PORT           0   // GPIOA
#define OLED_SCK_PIN            5
#define OLED_SCK_PIN_ENC        HAL_GPIO_PIN(OLED_SCK_PORT, OLED_SCK_PIN)
#define OLED_SCK_AF             GPIO_AF5   // 硬件 SPI 时使用

#define OLED_MOSI_PORT          0   // GPIOA
#define OLED_MOSI_PIN           7
#define OLED_MOSI_PIN_ENC       HAL_GPIO_PIN(OLED_MOSI_PORT, OLED_MOSI_PIN)
#define OLED_MOSI_AF            GPIO_AF5

/* I2C 引脚定义 */
#define OLED_I2C_INSTANCE       I2C3
#define OLED_I2C_ADDR           0x78

#define OLED_SCL_PORT           0   // GPIOA
#define OLED_SCL_PIN            8
#define OLED_SCL_PIN_ENC        HAL_GPIO_PIN(OLED_SCL_PORT, OLED_SCL_PIN)
#define OLED_SCL_AF             GPIO_AF4

#define OLED_SDA_PORT           2   // GPIOC
#define OLED_SDA_PIN            9
#define OLED_SDA_PIN_ENC        HAL_GPIO_PIN(OLED_SDA_PORT, OLED_SDA_PIN)
#define OLED_SDA_AF             GPIO_AF4

#endif /* __OLED_H */
/*============================ SERIAL 配置 ============================*/
#ifdef __SERIAL_H
/* ---------- 调试串口 (DEBUG) – 对应 USART1 ---------- */
#define SERIAL_DEBUG_USART_INSTANCE    USART1
#define SERIAL_DEBUG_BAUDRATE          2000000

/* DEBUG TX 引脚 (PA9) */
#define SERIAL_DEBUG_TX_PORT           1       // GPIOA
#define SERIAL_DEBUG_TX_PIN            6
#define SERIAL_DEBUG_TX_PIN_ENC        HAL_GPIO_PIN(SERIAL_DEBUG_TX_PORT, SERIAL_DEBUG_TX_PIN)
#define SERIAL_DEBUG_TX_AF             GPIO_AF7

/* DEBUG RX 引脚 (PA10) */
#define SERIAL_DEBUG_RX_PORT           1       // GPIOA
#define SERIAL_DEBUG_RX_PIN            7
#define SERIAL_DEBUG_RX_PIN_ENC        HAL_GPIO_PIN(SERIAL_DEBUG_RX_PORT, SERIAL_DEBUG_RX_PIN)
#define SERIAL_DEBUG_RX_AF             GPIO_AF7

/*============================ DMA 通道配置 ============================*/
#define SERIAL_DEBUG_DMA_INSTANCE       DMA1
/* DEBUG DMA 配置 */
#define SERIAL_DEBUG_TX_DMA_CHANNEL     DMA1_Channel4
#define SERIAL_DEBUG_TX_DMAMUX_CHANNEL  DMA_MuxChannel4
#define SERIAL_DEBUG_TX_DMA_REQUEST     85U
#define SERIAL_DEBUG_RX_DMA_CHANNEL     DMA1_Channel6
#define SERIAL_DEBUG_RX_DMAMUX_CHANNEL  DMA_MuxChannel6
#define SERIAL_DEBUG_RX_DMA_REQUEST     86U

/* DEBUG DMA 中断标志 */
#define SERIAL_DEBUG_TX_DMA_TC_FLAG     DMA1_IT_TC4
#define SERIAL_DEBUG_TX_DMA_TE_FLAG     DMA1_IT_TE4

/* DEBUG 中断号 */
#define SERIAL_DEBUG_USART_IRQn         USART1_IRQn
#define SERIAL_DEBUG_TX_DMA_IRQn        DMA1_Channel4_IRQn

/* 缓冲区大小 */
#define SERIAL_DEBUG_RX_BUF_SIZE        1024
#define SERIAL_DEBUG_TX_RING_SIZE       2048


/* ---------- 数据转发串口 (WIFI) – 对应 USART2 ---------- */
#define SERIAL_WIFI_USART_INSTANCE      USART2
#define SERIAL_WIFI_BAUDRATE            2000000

/* WIFI TX 引脚 (PD5) */
#define SERIAL_WIFI_TX_PORT             0       // GPIOD
#define SERIAL_WIFI_TX_PIN              2
#define SERIAL_WIFI_TX_PIN_ENC          HAL_GPIO_PIN(SERIAL_WIFI_TX_PORT, SERIAL_WIFI_TX_PIN)
#define SERIAL_WIFI_TX_AF               GPIO_AF7

/* WIFI RX 引脚 (PD6) */
#define SERIAL_WIFI_RX_PORT             0       // GPIOD
#define SERIAL_WIFI_RX_PIN              3
#define SERIAL_WIFI_RX_PIN_ENC          HAL_GPIO_PIN(SERIAL_WIFI_RX_PORT, SERIAL_WIFI_RX_PIN)
#define SERIAL_WIFI_RX_AF               GPIO_AF7

/*============================ DMA 通道配置 ============================*/
#define SERIAL_WIFI_DMA_INSTANCE       DMA1
/* WIFI DMA 配置 */
#define SERIAL_WIFI_TX_DMA_CHANNEL      DMA1_Channel5
#define SERIAL_WIFI_TX_DMAMUX_CHANNEL   DMA_MuxChannel5
#define SERIAL_WIFI_TX_DMA_REQUEST      87U
#define SERIAL_WIFI_RX_DMA_CHANNEL      DMA1_Channel7
#define SERIAL_WIFI_RX_DMAMUX_CHANNEL   DMA_MuxChannel7
#define SERIAL_WIFI_RX_DMA_REQUEST      88U

/* WIFI DMA 中断标志 */
#define SERIAL_WIFI_TX_DMA_TC_FLAG      DMA1_IT_TC5
#define SERIAL_WIFI_TX_DMA_TE_FLAG      DMA1_IT_TE5

/* WIFI 中断号 */
#define SERIAL_WIFI_USART_IRQn          USART2_IRQn
#define SERIAL_WIFI_TX_DMA_IRQn         DMA1_Channel5_IRQn

/* 缓冲区大小 */
#define SERIAL_WIFI_RX_BUF_SIZE         2048
#define SERIAL_WIFI_TX_RING_SIZE        8192

#endif /* __SERIAL_H */

void SPI_ClockEnable(SPI_TypeDef *SPIx);
void I2C_ClockEnable (I2C_TypeDef *I2Cx);
void USART_ClockEnable (USART_TypeDef *USARTx);
void DMA_ClockEnable (DMA_TypeDef *DMAx);
void AFIO_ClockEnable (void);
#endif /* CONFIG_H */