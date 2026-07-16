/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.h
* Description        : Top-level hardware entry. Dual-core IPC public APIs are
*                      declared in dualcore_ipc.h and included here for
*                      backward compatibility with existing V3F modules.
*******************************************************************************/
#ifndef __HARDWARE_H
#define __HARDWARE_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include "ch32h417.h"
#include "debug.h"
#include "dualcore_ipc.h"

void Hardware(void);

#ifdef __cplusplus
}
#endif

#endif
