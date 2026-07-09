// #include "ch32h417.h"
// #include "ADS1299.h"
// #include "W25Q64.h"
// #include "Serial.h"
// #include <string.h>
// /* ================= SPI3 从机，模拟 ADS1299 ================= */

// #define SPI3_SCK_PORT GPIOC
// #define SPI3_SCK_PIN GPIO_Pin_10
// #define SPI3_SCK_SRC GPIO_PinSource10

// #define SPI3_MISO_PORT GPIOC
// #define SPI3_MISO_PIN GPIO_Pin_11
// #define SPI3_MISO_SRC GPIO_PinSource11

// #define SPI3_MOSI_PORT GPIOC
// #define SPI3_MOSI_PIN GPIO_Pin_12
// #define SPI3_MOSI_SRC GPIO_PinSource12

// #define SPI3_CS_PORT GPIOA
// #define SPI3_CS_PIN GPIO_Pin_15
// #define SPI3_CS_SRC GPIO_PinSource15


// /* ================= SPI4 主机，读取 SPI3 ================= */

// #define SPI4_PORT GPIOE

// #define SPI4_SCK_PIN GPIO_Pin_12
// #define SPI4_SCK_SRC GPIO_PinSource12

// #define SPI4_MISO_PIN GPIO_Pin_13
// #define SPI4_MISO_SRC GPIO_PinSource13

// #define SPI4_MOSI_PIN GPIO_Pin_14
// #define SPI4_MOSI_SRC GPIO_PinSource14

// #define SPI4_CS_PIN GPIO_Pin_11


// /* ================= DRDY 模拟 ================= */

// #define DRDY_PORT GPIOC
// #define DRDY_OUT_PIN GPIO_Pin_8 /* PC8：模拟 ADS1299 DRDY 输出 */
// #define DRDY_IN_PIN GPIO_Pin_9  /* PC9：主机侧 DRDY 输入，EXTI9 */
// #define DRDY_IN_SRC GPIO_PinSource9


// /* ========== 常量 ========== */
// #define FRAME_SIZE 27
// #define W25Q64_SIZE_BYTES 0x800000UL /* W25Q64 = 64Mbit = 8MByte */

// /* ========== 全局变量 ========== */
// volatile uint8_t Frame_Available_Flag = RESET;
// volatile uint8_t Timer_Tick_Flag = RESET;
// volatile uint8_t DRDY_Pending_Flag = RESET;

// static uint32_t flash_addr = 0;
// static uint8_t txBuf[FRAME_SIZE];
// static uint8_t rxBuf[FRAME_SIZE];

// /* ========== 内部函数声明 ========== */
// static void SPI3_Slave_Init (void);
// static void SPI4_Master_Init (void);
// static void DRDY_GPIO_EXTI_Init (void);
// static void TIM_DRDY_Init (void);
// static void Prepare_Next_Frame_From_Flash (void);
// static void DRDY_Generate_FallingEdge (void);
// static void SPI3_Clear_RX_OVR (void);

// uint8_t get_Frame_Available_Flag (void) {
//     uint8_t f = Frame_Available_Flag;
//     Frame_Available_Flag = RESET;
//     return f;
// }

// /* ================= TIM3 初始化：周期性产生 DRDY 节拍 ================= */

// static void TIM3_DRDY_Init (void) {
//     TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

//     RCC_HB1PeriphClockCmd (RCC_HB1Periph_TIM3, ENABLE);

//     TIM_InternalClockConfig (TIM3);

//     TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//     TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
//     TIM_TimeBaseInitStructure.TIM_Period = 10 - 1;
//     TIM_TimeBaseInitStructure.TIM_Prescaler = 40000 - 1;
//     TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
//     TIM_TimeBaseInit (TIM3, &TIM_TimeBaseInitStructure);

//     TIM_ClearITPendingBit (TIM3, TIM_IT_Update);
//     TIM_ITConfig (TIM3, TIM_IT_Update, ENABLE);

//     NVIC_SetPriority (TIM3_IRQn, 0);
//     NVIC_EnableIRQ (TIM3_IRQn);

//     TIM_Cmd (TIM3, ENABLE);
// }

// /**
//  * @brief  ADS1299 仿真初始化 + 自收发测试
//  */
// void ADS1299_Init (void) {
//     uint32_t rx_count = 0;

//     Serial_Init();
//     Serial_Printf ("ADS1299 Self-Loop Simulation Start\r\n");

//     W25Q64_Init();
//     flash_addr = 0;
//     memset (txBuf, 0, sizeof (txBuf));
//     memset (rxBuf, 0, sizeof (rxBuf));
//     Serial_Printf ("W25Q64 initialized\r\n");

//     SPI3_Slave_Init();
//     Serial_Printf ("SPI3 Slave initialized\r\n");

//     SPI4_Master_Init();
//     Serial_Printf ("SPI4 Master initialized\r\n");

//     DRDY_GPIO_EXTI_Init();
//     Serial_Printf ("DRDY EXTI configured, waiting for data...\r\n");

//     /* 先准备第一帧，避免第一次 DRDY 后没有数据 */
//     Prepare_Next_Frame_From_Flash();

//     /* 最后启动 TIM3，避免定时器过早进入中断 */
//     TIM3_DRDY_Init();
//     Serial_Printf ("TIM3 DRDY started (250 Hz)\r\n");

//     while (1) {
//         /* TIM3 到时间，只在主循环中做真正的数据准备和 DRDY 输出 */
//         if (Timer_Tick_Flag) {
//             Timer_Tick_Flag = RESET;

//             /* 上一帧还没被主机读走时，不产生新的 DRDY，避免 PC8 一直低电平 */
//             if (DRDY_Pending_Flag == RESET) {
//                 Prepare_Next_Frame_From_Flash();
//                 DRDY_Generate_FallingEdge();
//                 DRDY_Pending_Flag = SET;
//             }
//         }
//     }
// }

// static void SPI3_Clear_RX (void) {
//     volatile uint16_t temp;

//     while (SPI_I2S_GetFlagStatus (SPI3, SPI_I2S_FLAG_RXNE) != RESET) {
//         temp = SPI_I2S_ReceiveData (SPI3);
//         (void)temp;
//     }
// }

// static void SPI4_Clear_RX (void) {
//     volatile uint16_t temp;

//     while (SPI_I2S_GetFlagStatus (SPI4, SPI_I2S_FLAG_RXNE) != RESET) {
//         temp = SPI_I2S_ReceiveData (SPI4);
//         (void)temp;
//     }
// }

// static void SPI3_Slave_Init (void) {
//     GPIO_InitTypeDef GPIO_InitStructure = {0};
//     SPI_InitTypeDef SPI_InitStructure = {0};

//     RCC_HB2PeriphClockCmd (RCC_HB2Periph_GPIOA |
//                                RCC_HB2Periph_GPIOC |
//                                RCC_HB2Periph_AFIO,
//                            ENABLE);

//     RCC_HB1PeriphClockCmd (RCC_HB1Periph_SPI3, ENABLE);

//     /*
//      * SPI3 复用：
//      * PA15 -> SPI3_NSS  AF6
//      * PC10 -> SPI3_SCK  AF6
//      * PC11 -> SPI3_MISO AF6
//      * PC12 -> SPI3_MOSI AF6
//      */
//     GPIO_PinAFConfig (SPI3_CS_PORT, SPI3_CS_SRC, GPIO_AF6);
//     GPIO_PinAFConfig (SPI3_SCK_PORT, SPI3_SCK_SRC, GPIO_AF6);
//     GPIO_PinAFConfig (SPI3_MISO_PORT, SPI3_MISO_SRC, GPIO_AF6);
//     GPIO_PinAFConfig (SPI3_MOSI_PORT, SPI3_MOSI_SRC, GPIO_AF6);

//     /* SPI3 从机输入：NSS / SCK / MOSI */
//     GPIO_InitStructure.GPIO_Pin = SPI3_CS_PIN;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
//     GPIO_Init (SPI3_CS_PORT, &GPIO_InitStructure);

//     GPIO_InitStructure.GPIO_Pin = SPI3_SCK_PIN | SPI3_MOSI_PIN;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
//     GPIO_Init (SPI3_SCK_PORT, &GPIO_InitStructure);

//     /* SPI3_MISO 是从机输出 */
//     GPIO_InitStructure.GPIO_Pin = SPI3_MISO_PIN;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
//     GPIO_Init (SPI3_MISO_PORT, &GPIO_InitStructure);

//     SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
//     SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
//     SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
//     SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
//     SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
//     SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
//     SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
//     SPI_InitStructure.SPI_CRCPolynomial = 7;

//     SPI_Cmd (SPI3, DISABLE);
//     SPI_Init (SPI3, &SPI_InitStructure);
//     SPI_Cmd (SPI3, ENABLE);

//     SPI3_Clear_RX();
// }

// /* ================= SPI4 主机初始化 ================= */

// static void SPI4_Master_Init (void) {
//     GPIO_InitTypeDef GPIO_InitStructure = {0};
//     SPI_InitTypeDef SPI_InitStructure = {0};

//     RCC_HB2PeriphClockCmd (RCC_HB2Periph_GPIOE |
//                                RCC_HB2Periph_AFIO,
//                            ENABLE);

//     RCC_HB1PeriphClockCmd (RCC_HB1Periph_SPI4, ENABLE);

//     /*
//      * SPI4 复用：
//      * PE12 -> SPI4_SCK  AF5
//      * PE13 -> SPI4_MISO AF5
//      * PE14 -> SPI4_MOSI AF5
//      * PE11 这里作为普通 GPIO 软件 CS 使用
//      */
//     GPIO_PinAFConfig (GPIOE, SPI4_SCK_SRC, GPIO_AF5);
//     GPIO_PinAFConfig (GPIOE, SPI4_MISO_SRC, GPIO_AF5);
//     GPIO_PinAFConfig (GPIOE, SPI4_MOSI_SRC, GPIO_AF5);

//     /* 主机输出：SCK / MOSI */
//     GPIO_InitStructure.GPIO_Pin = SPI4_SCK_PIN | SPI4_MOSI_PIN;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
//     GPIO_Init (SPI4_PORT, &GPIO_InitStructure);

//     /* 主机输入：MISO */
//     GPIO_InitStructure.GPIO_Pin = SPI4_MISO_PIN;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
//     GPIO_Init (SPI4_PORT, &GPIO_InitStructure);

//     /* 软件 CS，物理连接到 SPI3 的 PA15/NSS */
//     GPIO_InitStructure.GPIO_Pin = SPI4_CS_PIN;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
//     GPIO_Init (SPI4_PORT, &GPIO_InitStructure);

//     GPIO_WriteBit (SPI4_PORT, SPI4_CS_PIN, Bit_SET);

//     SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
//     SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
//     SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
//     SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
//     SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_Mode7;
//     SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
//     SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
//     SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
//     SPI_InitStructure.SPI_CRCPolynomial = 7;

//     SPI_Cmd (SPI4, DISABLE);
//     SPI_Init (SPI4, &SPI_InitStructure);
//     SPI_Cmd (SPI4, ENABLE);

//     SPI4_Clear_RX();
// }

// /* ================= DRDY GPIO + EXTI 初始化 ================= */

// static void DRDY_GPIO_EXTI_Init (void) {
//     GPIO_InitTypeDef GPIO_InitStructure = {0};
//     EXTI_InitTypeDef EXTI_InitStructure = {0};

//     RCC_HB2PeriphClockCmd (RCC_HB2Periph_GPIOC |
//                                RCC_HB2Periph_AFIO,
//                            ENABLE);

//     /* PC8: DRDY 输出，空闲高电平 */
//     GPIO_InitStructure.GPIO_Pin = DRDY_OUT_PIN;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
//     GPIO_Init (DRDY_PORT, &GPIO_InitStructure);
//     GPIO_WriteBit (DRDY_PORT, DRDY_OUT_PIN, Bit_SET);

//     /* PC9: DRDY 输入，上拉，检测下降沿 */
//     GPIO_InitStructure.GPIO_Pin = DRDY_IN_PIN;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
//     GPIO_Init (DRDY_PORT, &GPIO_InitStructure);

//     GPIO_EXTILineConfig (GPIO_PortSourceGPIOC, DRDY_IN_SRC);

//     EXTI_ClearITPendingBit (EXTI_Line9);

//     EXTI_InitStructure.EXTI_Line = EXTI_Line9;
//     EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//     EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//     EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//     EXTI_Init (&EXTI_InitStructure);

//     NVIC_SetPriority (EXTI15_8_IRQn, 1);
//     NVIC_EnableIRQ (EXTI15_8_IRQn);
// }

// /* ---------- 从 W25Q64 准备下一帧 ---------- */
// static void Prepare_Next_Frame_From_Flash (void) {
//     W25Q64_ReadData (flash_addr, txBuf, FRAME_SIZE);

//     flash_addr += FRAME_SIZE;

//     if (flash_addr >= (W25Q64_SIZE_BYTES - FRAME_SIZE)) {
//         flash_addr = 0;
//     }
// }

// /* ---------- 产生一次 DRDY 下降沿 ---------- */
// static void DRDY_Generate_FallingEdge (void) {
//     GPIO_WriteBit (DRDY_PORT, DRDY_OUT_PIN, Bit_SET);
//     Delay_Us (5);
//     GPIO_WriteBit (DRDY_PORT, DRDY_OUT_PIN, Bit_RESET);
// }

// /* ---------- TIM3 中断：只置位，不能打印/读Flash/操作SPI ---------- */
// void TIM3_IRQHandler (void) __attribute__ ((interrupt ("WCH-Interrupt-fast")));

// void TIM3_IRQHandler (void) {
//     if (TIM_GetITStatus (TIM3, TIM_IT_Update) != RESET) {
//         TIM_ClearITPendingBit (TIM3, TIM_IT_Update);
//         Timer_Tick_Flag = SET;
//     }
// }

// /* ---------- EXTI 中断：只置位，不能打印 ---------- */
// void EXTI15_8_IRQHandler (void) __attribute__ ((interrupt ("WCH-Interrupt-fast")));

// void EXTI15_8_IRQHandler (void) {
//     if (EXTI_GetITStatus (EXTI_Line9) != RESET) {
//         EXTI_ClearITPendingBit (EXTI_Line9);
//         Frame_Available_Flag = SET;
//     }
// }

// /* ---------- 主机读取一帧，同时给 SPI3 从机喂数据 ---------- */
// void ADS1299_ReadFrame (uint8_t *rxBuf) {
//     uint8_t i;
//     volatile uint16_t dummy;
//     uint32_t timeout;


//     SPI3_Clear_RX();
//     SPI4_Clear_RX();

//     GPIO_WriteBit (SPI4_PORT, SPI4_CS_PIN, Bit_RESET);
//     Delay_Us (2);

//     for (i = 0; i < FRAME_SIZE; i++) {
//         /* 1. 等待 SPI3 从机 TXE 空闲 */
//         timeout = 100000;
//         while (SPI_I2S_GetFlagStatus (SPI3, SPI_I2S_FLAG_TXE) == RESET) {
//             if (--timeout == 0) {
//                 Serial_Printf ("Timeout waiting SPI3 TXE at byte %d\r\n", i);
//                 goto read_error;
//             }
//         }
//         SPI_I2S_SendData (SPI3, txBuf[i]);

//         /* 2. 等待 SPI4 主机 TXE 空闲 */
//         timeout = 100000;
//         while (SPI_I2S_GetFlagStatus (SPI4, SPI_I2S_FLAG_TXE) == RESET) {
//             if (--timeout == 0) {
//                 Serial_Printf ("Timeout waiting SPI4 TXE at byte %d\r\n", i);
//                 goto read_error;
//             }
//         }
//         SPI_I2S_SendData (SPI4, 0x00);

//         /* 3. 等待 SPI4 接收数据 */
//         timeout = 100000;
//         while (SPI_I2S_GetFlagStatus (SPI4, SPI_I2S_FLAG_RXNE) == RESET) {
//             if (--timeout == 0) {
//                 Serial_Printf ("Timeout waiting SPI4 RXNE at byte %d\r\n", i);
//                 goto read_error;
//             }
//         }
//         rxBuf[i] = (uint8_t)SPI_I2S_ReceiveData (SPI4);

//         /* 4. 读走 SPI3 从机收到的 dummy，防止溢出 */
//         timeout = 100000;
//         while (SPI_I2S_GetFlagStatus (SPI3, SPI_I2S_FLAG_RXNE) == RESET) {
//             if (--timeout == 0) {
//                 Serial_Printf ("Timeout waiting SPI3 RXNE (dummy) at byte %d\r\n", i);
//                 goto read_error;
//             }
//         }
//         dummy = SPI_I2S_ReceiveData (SPI3);
//         (void)dummy;
//     }

//     while (SPI_I2S_GetFlagStatus (SPI4, SPI_I2S_FLAG_BSY) == SET);
//     Delay_Us (2);
//     GPIO_WriteBit (SPI4_PORT, SPI4_CS_PIN, Bit_SET);

//     /* 读完释放 DRDY，准备下一次下降沿 */
//     GPIO_WriteBit (DRDY_PORT, DRDY_OUT_PIN, Bit_SET);
//     DRDY_Pending_Flag = RESET;
//     return;

// read_error:
//     /* 出错时释放 CS 并清空从机接收缓冲 */
//     GPIO_WriteBit (SPI4_PORT, SPI4_CS_PIN, Bit_SET);

//     SPI3_Clear_RX();
//     SPI4_Clear_RX();
// }