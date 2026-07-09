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
    CMD_NULL              = 0x01,
    CMD_RAW_SPECTRUM_CH0  = 0x02,
    CMD_RAW_SPECTRUM_CH1  = 0x03,
    CMD_FOCUS             = 0x05,
    CMD_RAW_WAVE          = 0x04,
    CMD_FILT_WAVE         = 0x10,
    CMD_RAW_WAVE_8CH      = 0x20,
    CMD_FILT_WAVE_8CH     = 0x21,
    CMD_RAW_SPECTRUM_CH2  = 0x30,
    CMD_RAW_SPECTRUM_CH3  = 0x31,
    CMD_RAW_SPECTRUM_CH4  = 0x32,
    CMD_RAW_SPECTRUM_CH5  = 0x33,
    CMD_RAW_SPECTRUM_CH6  = 0x34,
    CMD_RAW_SPECTRUM_CH7  = 0x35,
    CMD_FILT_SPECTRUM_CH0 = 0x38,
    CMD_FILT_SPECTRUM_CH1 = 0x39,
    CMD_FILT_SPECTRUM_CH2 = 0x3A,
    CMD_FILT_SPECTRUM_CH3 = 0x3B,
    CMD_FILT_SPECTRUM_CH4 = 0x3C,
    CMD_FILT_SPECTRUM_CH5 = 0x3D,
    CMD_FILT_SPECTRUM_CH6 = 0x3E,
    CMD_FILT_SPECTRUM_CH7 = 0x3F,
    CMD_FREQ_SPECTRUM_CH0 = 0x07,
    CMD_FREQ_SPECTRUM_CH1 = 0x06,
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
void Send_Waveform8ch(CmdType wave_type, const float vals[8]);
void Send_Spectrum(CmdType spectrum_type, float *mag, uint8_t frag_idx);
void Send_Focus(float attn0, float attn1, float ema0, float ema1,
                uint8_t trend_state, uint8_t instant_state);

#endif