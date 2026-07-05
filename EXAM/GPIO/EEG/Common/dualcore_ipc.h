/********************************** (C) COPYRIGHT *******************************
 * File Name          : dualcore_ipc.h
 * Description        : Public dual-core IPC interface between V3F acquisition
 *                      side and V5F EEG processing/inference side.
 *******************************************************************************/
#ifndef __DUALCORE_IPC_H
#define __DUALCORE_IPC_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include "ch32h417.h"
#include "debug.h"

void IPC_Config(IPC_Channel_TypeDef IPC_CHx, IPC_TxCID_TypeDef IPC_TxCIDx, IPC_RxCID_TypeDef IPC_RxCIDx);

void DualCore_IPC_Init_V3F(void);
void DualCore_IPC_Init_V5F(void);
void DualCore_IPC_NotifyFromV3F(void);
void DualCore_IPC_SendFrameFromV3F(const uint8_t *frame, uint16_t len);

uint32_t DualCore_IPC_GetNotifyCount(void);
uint32_t DualCore_IPC_GetAckCount(void);
uint32_t DualCore_IPC_GetTxChecksum(void);
uint32_t DualCore_IPC_GetAckChecksum(void);
uint32_t DualCore_IPC_GetAckTxChecksum(void);
uint32_t DualCore_IPC_GetChecksumOKCount(void);
uint32_t DualCore_IPC_GetChecksumBadCount(void);
uint32_t DualCore_IPC_GetParseOKCount(void);
uint32_t DualCore_IPC_GetParseBadCount(void);

int32_t  DualCore_IPC_GetLastV3FCh0(void);
int32_t  DualCore_IPC_GetLastV5FCh0(void);
int32_t  DualCore_IPC_GetLastV5FCh0uVX1000(void);
int32_t  DualCore_IPC_GetLastV5FPre0uVX1000(void);
int32_t  DualCore_IPC_GetLastV5FFilt0uVX1000(void);
uint32_t DualCore_IPC_GetLastV5FSampleCount(void);
uint32_t DualCore_IPC_GetLastV5FWindowCount(void);
uint32_t DualCore_IPC_GetLastV5FFFTCount(void);
uint32_t DualCore_IPC_GetLastV5FFeatureValid(void);
int32_t  DualCore_IPC_GetLastV5FFeature(uint8_t idx);

uint32_t DualCore_IPC_GetLastV5FInferValid(void);
uint32_t DualCore_IPC_GetLastV5FPred(void);
uint32_t DualCore_IPC_GetLastV5FModelTrained(void);
int32_t  DualCore_IPC_GetLastV5FScoreLeft(void);
int32_t  DualCore_IPC_GetLastV5FScoreRight(void);
int32_t  DualCore_IPC_GetLastV5FConfidence(void);
uint32_t DualCore_IPC_GetLastV5FInferCount(void);

#ifdef __cplusplus
}
#endif

#endif
