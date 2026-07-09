#ifndef __WATCHDOG_FIX_H
#define __WATCHDOG_FIX_H

#include "ch32h417.h"
#include <stdint.h>

void Watchdog_Fix_Init(void);
void Watchdog_Feed(void);
void Delay_Ms(uint32_t ms);

#endif