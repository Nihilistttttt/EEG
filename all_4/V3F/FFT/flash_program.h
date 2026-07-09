#ifndef __FLASH_PROGRAM_H
#define __FLASH_PROGRAM_H

#include "ch32h417.h"

#define FLASH_TOTAL_SAMPLES  128000
#define FLASH_DATA_SIZE      (FLASH_TOTAL_SAMPLES * 8)
#define EE_DATA_BYTES        FLASH_DATA_SIZE 

void Flash_Program_Mode(void);
void Flash_Verify_Mode(void);

#endif