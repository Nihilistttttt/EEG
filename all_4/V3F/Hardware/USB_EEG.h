// /********************************** (C) COPYRIGHT *******************************
// * File Name          : ch32h417_usbfs_device.h
// * Author             : WCH
// * Version            : V1.0.0
// * Date               : 2022/08/20
// * Description        : This file contains all the functions prototypes for the
// *                      USBFS firmware library.
// *********************************************************************************
// * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
// * Attention: This software (modified or not) and binary are used for 
// * microcontroller manufactured by Nanjing Qinheng Microelectronics.
// *******************************************************************************/
// #ifndef __USB_EEG_
// #define __USB_EEG_

// #ifdef __cplusplus
// extern "C" {
// #endif

// #include "debug.h"
// #include "string.h"
// #include "ch32h417_usb.h"
// #include <stdint.h>

// /******************************************************************************/
// /* Global Define */
// #ifndef __PACKED
//   #define __PACKED   __attribute__((packed))
// #endif

// /* 初始化 USB CDC 设备（时钟、引脚、上拉、枚举） */
// void USB_EEG_CDC_Init(void);

// /* 发送一包数据（阻塞直到完成），最大长度 64 字节 */
// void USB_EEG_CDC_SendPacket(uint8_t *data, uint16_t len);

// #ifdef __cplusplus
// }
// #endif

// #endif