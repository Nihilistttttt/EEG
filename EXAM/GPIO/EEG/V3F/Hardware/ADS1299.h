#ifndef __ADS1299_H
#define __ADS1299_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ch32h417.h"
#include "debug.h"
#include <stdint.h>


/*============================ ADS1299 命令字 ============================*/

#define ADS1299_CMD_WAKEUP                 0x02U   /* 从待机模式唤醒 */
#define ADS1299_CMD_STANDBY                0x04U   /* 进入待机模式 */
#define ADS1299_CMD_RESET                  0x06U   /* 软件复位 */
#define ADS1299_CMD_START                  0x08U   /* 命令方式启动转换 */
#define ADS1299_CMD_STOP                   0x0AU   /* 命令方式停止转换 */
#define ADS1299_CMD_RDATAC                 0x10U   /* 连续读数据模式 */
#define ADS1299_CMD_SDATAC                 0x11U   /* 停止连续读数据模式 */
#define ADS1299_CMD_RDATA                  0x12U   /* 单次读一帧数据 */
#define ADS1299_CMD_RREG                   0x20U   /* 读寄存器命令基值：001r rrrr */
#define ADS1299_CMD_WREG                   0x40U   /* 写寄存器命令基值：010r rrrr */

/*============================ ADS1299 寄存器地址 ============================*/

#define ADS1299_REG_ID                     0x00U
#define ADS1299_REG_CONFIG1                0x01U
#define ADS1299_REG_CONFIG2                0x02U
#define ADS1299_REG_CONFIG3                0x03U
#define ADS1299_REG_LOFF                   0x04U
#define ADS1299_REG_CH1SET                 0x05U
#define ADS1299_REG_CH2SET                 0x06U
#define ADS1299_REG_CH3SET                 0x07U
#define ADS1299_REG_CH4SET                 0x08U
#define ADS1299_REG_CH5SET                 0x09U
#define ADS1299_REG_CH6SET                 0x0AU
#define ADS1299_REG_CH7SET                 0x0BU
#define ADS1299_REG_CH8SET                 0x0CU
#define ADS1299_REG_BIAS_SENSP             0x0DU
#define ADS1299_REG_BIAS_SENSN             0x0EU
#define ADS1299_REG_LOFF_SENSP             0x0FU
#define ADS1299_REG_LOFF_SENSN             0x10U
#define ADS1299_REG_LOFF_FLIP              0x11U
#define ADS1299_REG_LOFF_STATP             0x12U
#define ADS1299_REG_LOFF_STATN             0x13U
#define ADS1299_REG_GPIO                   0x14U
#define ADS1299_REG_MISC1                  0x15U
#define ADS1299_REG_MISC2                  0x16U
#define ADS1299_REG_CONFIG4                0x17U

/*============================ 数据长度定义 ============================*/

#define ADS1299_CHANNEL_NUM                8U
#define ADS1299_STATUS_BYTE_NUM            3U
#define ADS1299_CHANNEL_BYTE_NUM           3U
#define ADS1299_FRAME_BYTE_NUM             (ADS1299_STATUS_BYTE_NUM + ADS1299_CHANNEL_NUM * ADS1299_CHANNEL_BYTE_NUM)  /* 3 + 8*3 = 27 字节 */

/* 本工程实际启用前 4 路脑电：CH1=CP3, CH2=CP4, CH3=C3, CH4=C4。
 * ADS1299 连续读数时仍然读取完整 8 通道帧，未启用的 CH5~CH8 在寄存器中关闭。
 */
#define ADS1299_ACTIVE_EEG_CHANNEL_NUM      4U
#define ADS1299_EEG_CH_CP3                  0U
#define ADS1299_EEG_CH_CP4                  1U
#define ADS1299_EEG_CH_C3                   2U
#define ADS1299_EEG_CH_C4                   3U


/*============================ ADS1299 延时 ============================*/
#define ADS1299_SPI_TIMEOUT                0x000FFFFFUL
#define ADS1299_POWER_ON_DELAY_MS          1000U   /* 上电后等待模拟前端、电源与参考电压初步稳定 */
#define ADS1299_RESET_LOW_DELAY_MS         2U      /* RESET 拉低保持时间 */
#define ADS1299_POST_RESET_DELAY_MS        100U    /* RESET 拉高后等待芯片完成内部复位 */
#define ADS1299_REF_STABLE_DELAY_MS        200U    /* 开启内部参考后等待稳定 */


/*============================ ADS1299 电压转换 ============================*/
#define ADS1299_VREF_DEFAULT 4.5f
#define ADS1299_GAIN_DEFAULT 24.0f
#define ADS1299_FULL_SCALE_CODE 8388608.0f

/*============================ 对外函数声明 ============================*/
uint8_t   ADS1299_Init(void);
void ADS1299_ParseRawFrame(const uint8_t *frame_buf, uint32_t *status, int32_t ch_data[ADS1299_CHANNEL_NUM]);
float ADS1299_CodeToVolt (int32_t code, float vref, float gain) ;
float ADS1299_CodeToMicroVolt (int32_t code, float vref, float gain);
uint8_t ring_buffer_get_frame(uint8_t *dest);

#ifdef __cplusplus
}
#endif

#endif /* __ADS1299_H */
