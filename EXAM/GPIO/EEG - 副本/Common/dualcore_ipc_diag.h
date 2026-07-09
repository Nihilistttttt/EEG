#ifndef __DUALCORE_IPC_DIAG_H
#define __DUALCORE_IPC_DIAG_H

#include <stdint.h>

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

uint32_t DualCore_IPC_GetV5FHandlerCount(void);

#endif