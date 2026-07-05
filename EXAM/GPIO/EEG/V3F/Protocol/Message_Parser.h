#ifndef WATCH_MESSAGE_PARSER_H
#define WATCH_MESSAGE_PARSER_H

#include "ch32h417_usb.h"
#include "Serial.h"
#include <string.h>

#define CMD_PREFIX_LEN   1
#define FRAME_CHAR       0x7E   // 帧头/帧尾
#define ESCAPE_CHAR      0x7D   // 转义前缀
#define ESCAPE_XOR       0x20

// 发送缓冲区最大膨胀（最坏情况每个字节都需转义）
#define TX_MESSAGE_BUF_SIZE    (2 * 1032 + 3)

typedef enum {
    CMD_FILT_WAVE         = 0x10,
    CMD_FILT_SPECTRUM_CH0 = 0x09,
    CMD_FILT_SPECTRUM_CH1 = 0x08,
    CMD_FREQ_SPECTRUM_CH0 = 0x07,
    CMD_FREQ_SPECTRUM_CH1 = 0x06,
    CMD_FOCUS             = 0x05,
    CMD_RAW_WAVE          = 0x04,
    CMD_RAW_SPECTRUM_CH0  = 0x03,
    CMD_RAW_SPECTRUM_CH1  = 0x02,
    CMD_NULL              = 0x01
} CmdType;

/* 接收状态机实例 */
typedef struct {
    uint8_t  state;          // 0:WAIT_HEADER, 1:IN_PAYLOAD, 2:IN_ESCAPE
    uint8_t *payload;        // 指向各自缓冲区的指针
    uint16_t capacity;       // 缓冲区容量
    uint16_t len;            // 当前已接收负载长度
} FrameParser;

void Parse_Serial_Data(Serial_Port port);   // 从指定端口接收并解析

/* 发送函数（指定端口） */
void Pack_Frame(Serial_Port port, CmdType Cmd, const uint8_t *payload, uint16_t len);
void Send_Waveform (CmdType wave_type, float val_ch0, float val_ch1);
// void Send_Spectrum (CmdType wave_type, float *mag);
void Send_Spectrum(CmdType spectrum_type, float *mag, uint8_t frag_idx);
void Send_Focus(float attn0, float attn1, float ema0, float ema1,
                uint8_t trend_state, uint8_t instant_state);

#endif