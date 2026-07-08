#ifndef __EEG_CMD_PARSER_H
#define __EEG_CMD_PARSER_H

#include <stdint.h>

void Parse_CommandEx(const char *cmd, const char *source);
void Retry_Tick(void);
void Retry_Store(const char *msg);
void Retry_StoreEx(const char *msg, uint8_t max_count);
uint8_t Retry_GetSeq(void);

#endif