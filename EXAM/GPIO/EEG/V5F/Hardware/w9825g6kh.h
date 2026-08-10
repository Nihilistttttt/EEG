#ifndef __W9825G6KH_H
#define __W9825G6KH_H

#include "stdint.h"

#define W9825_SDRAM_BASE      0x60000000u
#define W9825_SDRAM_SIZE      (4u * 1024u * 1024u)

/* V4板 W9825G6KH-6 SDRAM 引脚配置 (仅V4板V5核使用)
 * W9825_xxx_PIN 存储引脚号(0-15), 用于GPIO_PinAFConfig的pin source参数
 * 位掩码(1U << pin)在SDRAM_AF_PP宏中生成, 用于GPIO_Init的GPIO_Pin字段 */
#define W9825_SDCLK_PORT      GPIOF
#define W9825_SDCLK_PIN       2
#define W9825_SDCLK_AF        GPIO_AF12

#define W9825_SDCKE_PORT      GPIOC
#define W9825_SDCKE_PIN       5
#define W9825_SDCKE_AF        GPIO_AF12

#define W9825_SDNE_PORT       GPIOC
#define W9825_SDNE_PIN        2
#define W9825_SDNE_AF         GPIO_AF12

#define W9825_NRAS_PORT       GPIOF
#define W9825_NRAS_PIN        11
#define W9825_NRAS_AF         GPIO_AF12

#define W9825_NCAS_PORT       GPIOF
#define W9825_NCAS_PIN        12
#define W9825_NCAS_AF         GPIO_AF12

#define W9825_SDNWE_PORT      GPIOC
#define W9825_SDNWE_PIN       0
#define W9825_SDNWE_AF        GPIO_AF12

#define W9825_DQM0_PORT       GPIOC
#define W9825_DQM0_PIN        12
#define W9825_DQM0_AF         GPIO_AF0

#define W9825_DQM1_PORT       GPIOC
#define W9825_DQM1_PIN        11
#define W9825_DQM1_AF         GPIO_AF0

#define W9825_BA0_PORT        GPIOB
#define W9825_BA0_PIN         14
#define W9825_BA0_AF          GPIO_AF12

#define W9825_BA1_PORT        GPIOB
#define W9825_BA1_PIN         15
#define W9825_BA1_AF          GPIO_AF12

#define W9825_A0_PORT         GPIOF
#define W9825_A0_PIN          5
#define W9825_A0_AF           GPIO_AF12

#define W9825_A1_PORT         GPIOB
#define W9825_A1_PIN          3
#define W9825_A1_AF           GPIO_AF12

#define W9825_A2_PORT         GPIOB
#define W9825_A2_PIN          4
#define W9825_A2_AF           GPIO_AF12

#define W9825_A3_PORT         GPIOB
#define W9825_A3_PIN          8
#define W9825_A3_AF           GPIO_AF12

#define W9825_A4_PORT         GPIOB
#define W9825_A4_PIN          9
#define W9825_A4_AF           GPIO_AF12

#define W9825_A5_PORT         GPIOB
#define W9825_A5_PIN          6
#define W9825_A5_AF           GPIO_AF11

#define W9825_A6_PORT         GPIOA
#define W9825_A6_PIN          10
#define W9825_A6_AF           GPIO_AF10

#define W9825_A7_PORT         GPIOA
#define W9825_A7_PIN          11
#define W9825_A7_AF           GPIO_AF10

#define W9825_A8_PORT         GPIOA
#define W9825_A8_PIN          12
#define W9825_A8_AF           GPIO_AF10

#define W9825_A9_PORT         GPIOA
#define W9825_A9_PIN          13
#define W9825_A9_AF           GPIO_AF10

#define W9825_A10_PORT        GPIOB
#define W9825_A10_PIN         10
#define W9825_A10_AF          GPIO_AF12

#define W9825_A11_PORT        GPIOB
#define W9825_A11_PIN         11
#define W9825_A11_AF          GPIO_AF12

#define W9825_A12_PORT        GPIOB
#define W9825_A12_PIN         12
#define W9825_A12_AF          GPIO_AF12

#define W9825_D0_PORT         GPIOD
#define W9825_D0_PIN          14
#define W9825_D0_AF           GPIO_AF12

#define W9825_D1_PORT         GPIOD
#define W9825_D1_PIN          15
#define W9825_D1_AF           GPIO_AF12

#define W9825_D2_PORT         GPIOD
#define W9825_D2_PIN          0
#define W9825_D2_AF           GPIO_AF12

#define W9825_D3_PORT         GPIOD
#define W9825_D3_PIN          1
#define W9825_D3_AF           GPIO_AF12

#define W9825_D4_PORT         GPIOE
#define W9825_D4_PIN          7
#define W9825_D4_AF           GPIO_AF12

#define W9825_D5_PORT         GPIOE
#define W9825_D5_PIN          8
#define W9825_D5_AF           GPIO_AF12

#define W9825_D6_PORT         GPIOE
#define W9825_D6_PIN          9
#define W9825_D6_AF           GPIO_AF12

#define W9825_D7_PORT         GPIOE
#define W9825_D7_PIN          10
#define W9825_D7_AF           GPIO_AF12

#define W9825_D8_PORT         GPIOE
#define W9825_D8_PIN          11
#define W9825_D8_AF           GPIO_AF12

#define W9825_D9_PORT         GPIOE
#define W9825_D9_PIN          12
#define W9825_D9_AF           GPIO_AF12

#define W9825_D10_PORT        GPIOE
#define W9825_D10_PIN         13
#define W9825_D10_AF          GPIO_AF12

#define W9825_D11_PORT        GPIOE
#define W9825_D11_PIN         14
#define W9825_D11_AF          GPIO_AF12

#define W9825_D12_PORT        GPIOE
#define W9825_D12_PIN         15
#define W9825_D12_AF          GPIO_AF12

#define W9825_D13_PORT        GPIOD
#define W9825_D13_PIN         8
#define W9825_D13_AF          GPIO_AF12

#define W9825_D14_PORT        GPIOD
#define W9825_D14_PIN         9
#define W9825_D14_AF          GPIO_AF12

#define W9825_D15_PORT        GPIOD
#define W9825_D15_PIN         10
#define W9825_D15_AF          GPIO_AF12

int  W9825_Init(void);
void W9825_WriteBuffer(uint8_t *buf, uint32_t addr, uint32_t n);
void W9825_ReadBuffer(uint8_t *buf, uint32_t addr, uint32_t n);

#endif
