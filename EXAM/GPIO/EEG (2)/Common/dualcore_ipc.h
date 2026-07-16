#ifndef __DUALCORE_IPC_H
#define __DUALCORE_IPC_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include "ch32h417.h"
#include "debug.h"
#include "dualcore_ipc_shared.h"
#include "dualcore_ipc_diag.h"

void IPC_Config(IPC_Channel_TypeDef IPC_CHx, IPC_TxCID_TypeDef IPC_TxCIDx, IPC_RxCID_TypeDef IPC_RxCIDx);

void DualCore_IPC_Init_V3F(void);
void DualCore_IPC_Init_V5F(void);
void DualCore_IPC_NotifyFromV3F(void);
void DualCore_IPC_SendFrameFromV3F(const uint8_t *frame, uint16_t len);
void DualCore_V5F_MainLoopProcess(void);

#ifdef __cplusplus
 }
#endif

#endif
