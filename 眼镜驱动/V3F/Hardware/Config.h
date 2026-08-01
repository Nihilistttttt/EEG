#ifndef CONFIG_H
#define CONFIG_H

#include "ch32h417.h"
#include "hal_gpio.h"

/*============================================================================
 * Board Version Selection — define exactly ONE of: BOARD_V1, BOARD_V2, BOARD_V3
 * If none is defined, BOARD_V1 is assumed.
 *============================================================================*/
#if !defined(BOARD_V1) && !defined(BOARD_V2) && !defined(BOARD_V3)
#define BOARD_V1
#endif
/*============================================================================
 ******************************************************************************
 *                          BOARD V1
 ******************************************************************************
 *============================================================================*/
#if defined(BOARD_V1)

#ifdef __OLED_H
/* OLED Display */
#define OLED_SPI_INSTANCE       SPI1
#define OLED_CS_PORT            0
#define OLED_CS_PIN             4
#define OLED_CS_PIN_ENC         HAL_GPIO_PIN(OLED_CS_PORT, OLED_CS_PIN)
#define OLED_RES_PORT           0
#define OLED_RES_PIN            3
#define OLED_RES_PIN_ENC        HAL_GPIO_PIN(OLED_RES_PORT, OLED_RES_PIN)
#define OLED_DC_PORT            0
#define OLED_DC_PIN             6
#define OLED_DC_PIN_ENC         HAL_GPIO_PIN(OLED_DC_PORT, OLED_DC_PIN)
#define OLED_SCK_PORT           0
#define OLED_SCK_PIN            5
#define OLED_SCK_PIN_ENC        HAL_GPIO_PIN(OLED_SCK_PORT, OLED_SCK_PIN)
#define OLED_SCK_AF             GPIO_AF5
#define OLED_MOSI_PORT          0
#define OLED_MOSI_PIN           7
#define OLED_MOSI_PIN_ENC       HAL_GPIO_PIN(OLED_MOSI_PORT, OLED_MOSI_PIN)
#define OLED_MOSI_AF            GPIO_AF5
#define OLED_I2C_INSTANCE       I2C3
#define OLED_I2C_ADDR           0x78
#define OLED_SCL_PORT           0
#define OLED_SCL_PIN            8
#define OLED_SCL_PIN_ENC        HAL_GPIO_PIN(OLED_SCL_PORT, OLED_SCL_PIN)
#define OLED_SCL_AF             GPIO_AF4
#define OLED_SDA_PORT           2
#define OLED_SDA_PIN            9
#define OLED_SDA_PIN_ENC        HAL_GPIO_PIN(OLED_SDA_PORT, OLED_SDA_PIN)
#define OLED_SDA_AF             GPIO_AF4
#endif

#ifdef __SERIAL_H
/* Serial DEBUG (USART1) */
#define SERIAL_DEBUG_USART_INSTANCE    USART1
#define SERIAL_DEBUG_BAUDRATE          2000000
#define SERIAL_DEBUG_TX_PORT           0
#define SERIAL_DEBUG_TX_PIN            9
#define SERIAL_DEBUG_TX_PIN_ENC        HAL_GPIO_PIN(SERIAL_DEBUG_TX_PORT, SERIAL_DEBUG_TX_PIN)
#define SERIAL_DEBUG_TX_AF             GPIO_AF7
#define SERIAL_DEBUG_RX_PORT           0
#define SERIAL_DEBUG_RX_PIN            10
#define SERIAL_DEBUG_RX_PIN_ENC        HAL_GPIO_PIN(SERIAL_DEBUG_RX_PORT, SERIAL_DEBUG_RX_PIN)
#define SERIAL_DEBUG_RX_AF             GPIO_AF7
#define SERIAL_DEBUG_DMA_INSTANCE       DMA1
#define SERIAL_DEBUG_TX_DMA_CHANNEL     DMA1_Channel4
#define SERIAL_DEBUG_TX_DMAMUX_CHANNEL  DMA_MuxChannel4
#define SERIAL_DEBUG_TX_DMA_REQUEST     85U
#define SERIAL_DEBUG_RX_DMA_CHANNEL     DMA1_Channel6
#define SERIAL_DEBUG_RX_DMAMUX_CHANNEL  DMA_MuxChannel6
#define SERIAL_DEBUG_RX_DMA_REQUEST     86U
#define SERIAL_DEBUG_TX_DMA_TC_FLAG     DMA1_IT_TC4
#define SERIAL_DEBUG_TX_DMA_TE_FLAG     DMA1_IT_TE4
#define SERIAL_DEBUG_USART_IRQn         USART1_IRQn
#define SERIAL_DEBUG_TX_DMA_IRQn        DMA1_Channel4_IRQn
#define SERIAL_DEBUG_RX_BUF_SIZE        1024
#define SERIAL_DEBUG_TX_RING_SIZE       2048

/* Serial WiFi (USART2) */
#define SERIAL_WIFI_USART_INSTANCE      USART2
#define SERIAL_WIFI_BAUDRATE            2000000
#define SERIAL_WIFI_TX_PORT             3
#define SERIAL_WIFI_TX_PIN              5
#define SERIAL_WIFI_TX_PIN_ENC          HAL_GPIO_PIN(SERIAL_WIFI_TX_PORT, SERIAL_WIFI_TX_PIN)
#define SERIAL_WIFI_TX_AF               GPIO_AF7
#define SERIAL_WIFI_RX_PORT             3
#define SERIAL_WIFI_RX_PIN              6
#define SERIAL_WIFI_RX_PIN_ENC          HAL_GPIO_PIN(SERIAL_WIFI_RX_PORT, SERIAL_WIFI_RX_PIN)
#define SERIAL_WIFI_RX_AF               GPIO_AF7
#define SERIAL_WIFI_DMA_INSTANCE       DMA1
#define SERIAL_WIFI_TX_DMA_CHANNEL      DMA1_Channel5
#define SERIAL_WIFI_TX_DMAMUX_CHANNEL   DMA_MuxChannel5
#define SERIAL_WIFI_TX_DMA_REQUEST      87U
#define SERIAL_WIFI_RX_DMA_CHANNEL      DMA1_Channel7
#define SERIAL_WIFI_RX_DMAMUX_CHANNEL   DMA_MuxChannel7
#define SERIAL_WIFI_RX_DMA_REQUEST      88U
#define SERIAL_WIFI_TX_DMA_TC_FLAG      DMA1_IT_TC5
#define SERIAL_WIFI_TX_DMA_TE_FLAG      DMA1_IT_TE5
#define SERIAL_WIFI_USART_IRQn          USART2_IRQn
#define SERIAL_WIFI_TX_DMA_IRQn         DMA1_Channel5_IRQn
#define SERIAL_WIFI_RX_BUF_SIZE         2048
#define SERIAL_WIFI_TX_RING_SIZE        8192
#endif

#ifdef __W25Q64_H
/* W25Q64 SPI Flash (SPI2, PB12-PB15) */
#define W25Q64_SPI_INSTANCE     SPI3
#define W25Q64_CS_PORT          0
#define W25Q64_CS_PIN           15
#define W25Q64_CS_PIN_ENC       HAL_GPIO_PIN(W25Q64_CS_PORT, W25Q64_CS_PIN)
#define W25Q64_SCK_PORT         2
#define W25Q64_SCK_PIN          10
#define W25Q64_SCK_PIN_ENC      HAL_GPIO_PIN(W25Q64_SCK_PORT, W25Q64_SCK_PIN)
#define W25Q64_SCK_AF           GPIO_AF6
#define W25Q64_MISO_PORT        2
#define W25Q64_MISO_PIN         11
#define W25Q64_MISO_PIN_ENC     HAL_GPIO_PIN(W25Q64_MISO_PORT, W25Q64_MISO_PIN)
#define W25Q64_MISO_AF          GPIO_AF6
#define W25Q64_MOSI_PORT        2
#define W25Q64_MOSI_PIN         12
#define W25Q64_MOSI_PIN_ENC     HAL_GPIO_PIN(W25Q64_MOSI_PORT, W25Q64_MOSI_PIN)
#define W25Q64_MOSI_AF          GPIO_AF6
#endif

/*============================================================================
 ******************************************************************************
 *                          BOARD V2
 ******************************************************************************
 *============================================================================*/
#elif defined(BOARD_V2)

#ifdef __OLED_H
/* OLED Display */
#define OLED_SPI_INSTANCE       SPI1
#define OLED_CS_PORT            1
#define OLED_CS_PIN             0
#define OLED_CS_PIN_ENC         HAL_GPIO_PIN(OLED_CS_PORT, OLED_CS_PIN)
#define OLED_RES_PORT           2
#define OLED_RES_PIN            4
#define OLED_RES_PIN_ENC        HAL_GPIO_PIN(OLED_RES_PORT, OLED_RES_PIN)
#define OLED_DC_PORT            2
#define OLED_DC_PIN             5
#define OLED_DC_PIN_ENC         HAL_GPIO_PIN(OLED_DC_PORT, OLED_DC_PIN)
#define OLED_SCK_PORT           0
#define OLED_SCK_PIN            5
#define OLED_SCK_PIN_ENC        HAL_GPIO_PIN(OLED_SCK_PORT, OLED_SCK_PIN)
#define OLED_SCK_AF             GPIO_AF5
#define OLED_MOSI_PORT          0
#define OLED_MOSI_PIN           7
#define OLED_MOSI_PIN_ENC       HAL_GPIO_PIN(OLED_MOSI_PORT, OLED_MOSI_PIN)
#define OLED_MOSI_AF            GPIO_AF5
#define OLED_I2C_INSTANCE       I2C3
#define OLED_I2C_ADDR           0x78
#define OLED_SCL_PORT           0
#define OLED_SCL_PIN            8
#define OLED_SCL_PIN_ENC        HAL_GPIO_PIN(OLED_SCL_PORT, OLED_SCL_PIN)
#define OLED_SCL_AF             GPIO_AF4
#define OLED_SDA_PORT           2
#define OLED_SDA_PIN            9
#define OLED_SDA_PIN_ENC        HAL_GPIO_PIN(OLED_SDA_PORT, OLED_SDA_PIN)
#define OLED_SDA_AF             GPIO_AF4

#endif /* __OLED_H */

#ifdef __SERIAL_H
/* Serial DEBUG (USART1) */
#define SERIAL_DEBUG_USART_INSTANCE    USART1
#define SERIAL_DEBUG_BAUDRATE          2000000
#define SERIAL_DEBUG_TX_PORT           1
#define SERIAL_DEBUG_TX_PIN            6
#define SERIAL_DEBUG_TX_PIN_ENC        HAL_GPIO_PIN(SERIAL_DEBUG_TX_PORT, SERIAL_DEBUG_TX_PIN)
#define SERIAL_DEBUG_TX_AF             GPIO_AF7
#define SERIAL_DEBUG_RX_PORT           1
#define SERIAL_DEBUG_RX_PIN            7
#define SERIAL_DEBUG_RX_PIN_ENC        HAL_GPIO_PIN(SERIAL_DEBUG_RX_PORT, SERIAL_DEBUG_RX_PIN)
#define SERIAL_DEBUG_RX_AF             GPIO_AF7
#define SERIAL_DEBUG_DMA_INSTANCE       DMA1
#define SERIAL_DEBUG_TX_DMA_CHANNEL     DMA1_Channel4
#define SERIAL_DEBUG_TX_DMAMUX_CHANNEL  DMA_MuxChannel4
#define SERIAL_DEBUG_TX_DMA_REQUEST     85U
#define SERIAL_DEBUG_RX_DMA_CHANNEL     DMA1_Channel6
#define SERIAL_DEBUG_RX_DMAMUX_CHANNEL  DMA_MuxChannel6
#define SERIAL_DEBUG_RX_DMA_REQUEST     86U
#define SERIAL_DEBUG_TX_DMA_TC_FLAG     DMA1_IT_TC4
#define SERIAL_DEBUG_TX_DMA_TE_FLAG     DMA1_IT_TE4
#define SERIAL_DEBUG_USART_IRQn         USART1_IRQn
#define SERIAL_DEBUG_TX_DMA_IRQn        DMA1_Channel4_IRQn
#define SERIAL_DEBUG_RX_BUF_SIZE        1024
#define SERIAL_DEBUG_TX_RING_SIZE       2048

/* Serial WiFi (USART2) */
#define SERIAL_WIFI_USART_INSTANCE      USART2
#define SERIAL_WIFI_BAUDRATE            2000000
#define SERIAL_WIFI_TX_PORT             0
#define SERIAL_WIFI_TX_PIN              2
#define SERIAL_WIFI_TX_PIN_ENC          HAL_GPIO_PIN(SERIAL_WIFI_TX_PORT, SERIAL_WIFI_TX_PIN)
#define SERIAL_WIFI_TX_AF               GPIO_AF7
#define SERIAL_WIFI_RX_PORT             0
#define SERIAL_WIFI_RX_PIN              3
#define SERIAL_WIFI_RX_PIN_ENC          HAL_GPIO_PIN(SERIAL_WIFI_RX_PORT, SERIAL_WIFI_RX_PIN)
#define SERIAL_WIFI_RX_AF               GPIO_AF7
#define SERIAL_WIFI_DMA_INSTANCE       DMA1
#define SERIAL_WIFI_TX_DMA_CHANNEL      DMA1_Channel5
#define SERIAL_WIFI_TX_DMAMUX_CHANNEL   DMA_MuxChannel5
#define SERIAL_WIFI_TX_DMA_REQUEST      87U
#define SERIAL_WIFI_RX_DMA_CHANNEL      DMA1_Channel7
#define SERIAL_WIFI_RX_DMAMUX_CHANNEL   DMA_MuxChannel7
#define SERIAL_WIFI_RX_DMA_REQUEST      88U
#define SERIAL_WIFI_TX_DMA_TC_FLAG      DMA1_IT_TC5
#define SERIAL_WIFI_TX_DMA_TE_FLAG      DMA1_IT_TE5
#define SERIAL_WIFI_USART_IRQn          USART2_IRQn
#define SERIAL_WIFI_TX_DMA_IRQn         DMA1_Channel5_IRQn
#define SERIAL_WIFI_RX_BUF_SIZE         2048
#define SERIAL_WIFI_TX_RING_SIZE        8192

#endif /* __SERIAL_H */

/*============================================================================
 ******************************************************************************
 *                          BOARD V3
 ******************************************************************************
 *============================================================================*/
#elif defined(BOARD_V3)

#ifdef __OLED_H
/* OLED Display */
#define OLED_SPI_INSTANCE       SPI1
#define OLED_CS_PORT            1
#define OLED_CS_PIN             0
#define OLED_CS_PIN_ENC         HAL_GPIO_PIN(OLED_CS_PORT, OLED_CS_PIN)
#define OLED_RES_PORT           2
#define OLED_RES_PIN            4
#define OLED_RES_PIN_ENC        HAL_GPIO_PIN(OLED_RES_PORT, OLED_RES_PIN)
#define OLED_DC_PORT            2
#define OLED_DC_PIN             5
#define OLED_DC_PIN_ENC         HAL_GPIO_PIN(OLED_DC_PORT, OLED_DC_PIN)
#define OLED_SCK_PORT           0
#define OLED_SCK_PIN            5
#define OLED_SCK_PIN_ENC        HAL_GPIO_PIN(OLED_SCK_PORT, OLED_SCK_PIN)
#define OLED_SCK_AF             GPIO_AF5
#define OLED_MOSI_PORT          0
#define OLED_MOSI_PIN           7
#define OLED_MOSI_PIN_ENC       HAL_GPIO_PIN(OLED_MOSI_PORT, OLED_MOSI_PIN)
#define OLED_MOSI_AF            GPIO_AF5
#define OLED_I2C_INSTANCE       I2C3
#define OLED_I2C_ADDR           0x78
#define OLED_SCL_PORT           0
#define OLED_SCL_PIN            8
#define OLED_SCL_PIN_ENC        HAL_GPIO_PIN(OLED_SCL_PORT, OLED_SCL_PIN)
#define OLED_SCL_AF             GPIO_AF4
#define OLED_SDA_PORT           2
#define OLED_SDA_PIN            9
#define OLED_SDA_PIN_ENC        HAL_GPIO_PIN(OLED_SDA_PORT, OLED_SDA_PIN)
#define OLED_SDA_AF             GPIO_AF4

#endif /* __OLED_H */

#ifdef __SERIAL_H
/* Serial DEBUG (USART1) */
#define SERIAL_DEBUG_USART_INSTANCE    USART1
#define SERIAL_DEBUG_BAUDRATE          2000000
#define SERIAL_DEBUG_TX_PORT           1
#define SERIAL_DEBUG_TX_PIN            6
#define SERIAL_DEBUG_TX_PIN_ENC        HAL_GPIO_PIN(SERIAL_DEBUG_TX_PORT, SERIAL_DEBUG_TX_PIN)
#define SERIAL_DEBUG_TX_AF             GPIO_AF7
#define SERIAL_DEBUG_RX_PORT           1
#define SERIAL_DEBUG_RX_PIN            7
#define SERIAL_DEBUG_RX_PIN_ENC        HAL_GPIO_PIN(SERIAL_DEBUG_RX_PORT, SERIAL_DEBUG_RX_PIN)
#define SERIAL_DEBUG_RX_AF             GPIO_AF7
#define SERIAL_DEBUG_DMA_INSTANCE       DMA1
#define SERIAL_DEBUG_TX_DMA_CHANNEL     DMA1_Channel4
#define SERIAL_DEBUG_TX_DMAMUX_CHANNEL  DMA_MuxChannel4
#define SERIAL_DEBUG_TX_DMA_REQUEST     85U
#define SERIAL_DEBUG_RX_DMA_CHANNEL     DMA1_Channel6
#define SERIAL_DEBUG_RX_DMAMUX_CHANNEL  DMA_MuxChannel6
#define SERIAL_DEBUG_RX_DMA_REQUEST     86U
#define SERIAL_DEBUG_TX_DMA_TC_FLAG     DMA1_IT_TC4
#define SERIAL_DEBUG_TX_DMA_TE_FLAG     DMA1_IT_TE4
#define SERIAL_DEBUG_USART_IRQn         USART1_IRQn
#define SERIAL_DEBUG_TX_DMA_IRQn        DMA1_Channel4_IRQn
#define SERIAL_DEBUG_RX_BUF_SIZE        1024
#define SERIAL_DEBUG_TX_RING_SIZE       2048

/* Serial WiFi (USART2) */
#define SERIAL_WIFI_USART_INSTANCE      USART2
#define SERIAL_WIFI_BAUDRATE            2000000
#define SERIAL_WIFI_TX_PORT             0
#define SERIAL_WIFI_TX_PIN              2
#define SERIAL_WIFI_TX_PIN_ENC          HAL_GPIO_PIN(SERIAL_WIFI_TX_PORT, SERIAL_WIFI_TX_PIN)
#define SERIAL_WIFI_TX_AF               GPIO_AF7
#define SERIAL_WIFI_RX_PORT             0
#define SERIAL_WIFI_RX_PIN              3
#define SERIAL_WIFI_RX_PIN_ENC          HAL_GPIO_PIN(SERIAL_WIFI_RX_PORT, SERIAL_WIFI_RX_PIN)
#define SERIAL_WIFI_RX_AF               GPIO_AF7
#define SERIAL_WIFI_DMA_INSTANCE       DMA1
#define SERIAL_WIFI_TX_DMA_CHANNEL      DMA1_Channel5
#define SERIAL_WIFI_TX_DMAMUX_CHANNEL   DMA_MuxChannel5
#define SERIAL_WIFI_TX_DMA_REQUEST      87U
#define SERIAL_WIFI_RX_DMA_CHANNEL      DMA1_Channel7
#define SERIAL_WIFI_RX_DMAMUX_CHANNEL   DMA_MuxChannel7
#define SERIAL_WIFI_RX_DMA_REQUEST      88U
#define SERIAL_WIFI_TX_DMA_TC_FLAG      DMA1_IT_TC5
#define SERIAL_WIFI_TX_DMA_TE_FLAG      DMA1_IT_TE5
#define SERIAL_WIFI_USART_IRQn          USART2_IRQn
#define SERIAL_WIFI_TX_DMA_IRQn         DMA1_Channel5_IRQn
#define SERIAL_WIFI_RX_BUF_SIZE         2048
#define SERIAL_WIFI_TX_RING_SIZE        8192

#endif /* __SERIAL_H */

#endif /* BOARD_Vx */


/*============================================================================
 * Common (board-independent)
 *============================================================================*/
void SPI_ClockEnable(SPI_TypeDef *SPIx);
void I2C_ClockEnable (I2C_TypeDef *I2Cx);
void USART_ClockEnable (USART_TypeDef *USARTx);
void DMA_ClockEnable (DMA_TypeDef *DMAx);
void AFIO_ClockEnable (void);
#endif /* CONFIG_H */
