#ifndef WATCH_MESSAGE_PARSER_H
#define WATCH_MESSAGE_PARSER_H

#include "ch32h417_usb.h"
#include "Serial.h"
#include <string.h>

#define CMD_PREFIX_LEN   1
#define FRAME_CHAR       0x7E
#define ESCAPE_CHAR      0x7D
#define ESCAPE_XOR       0x20

#define TX_MESSAGE_BUF_SIZE    (2 * 1032 + 3)

#define DISPLAY_NUM_CH       4
#define DISPLAY_MAX_CH       8

typedef enum {
    CMD_NULL              = 0x01,

    CMD_RAW_SPECTRUM_BASE = 0x20,
    CMD_FREQ_SPECTRUM_BASE = 0x30,
    CMD_FILT_SPECTRUM_BASE = 0x40,

    CMD_FOCUS             = 0x05,
    CMD_RAW_WAVE          = 0x04,
    CMD_FILT_WAVE         = 0x10,
    CMD_BASELINE_WAVE     = 0x11,

    CMD_RAW_SPECTRUM_CH0  = 0x20,
    CMD_RAW_SPECTRUM_CH1  = 0x21,
    CMD_RAW_SPECTRUM_CH2  = 0x22,
    CMD_RAW_SPECTRUM_CH3  = 0x23,
    CMD_RAW_SPECTRUM_CH4  = 0x24,
    CMD_RAW_SPECTRUM_CH5  = 0x25,
    CMD_RAW_SPECTRUM_CH6  = 0x26,
    CMD_RAW_SPECTRUM_CH7  = 0x27,

    CMD_FREQ_SPECTRUM_CH0 = 0x30,
    CMD_FREQ_SPECTRUM_CH1 = 0x31,
    CMD_FREQ_SPECTRUM_CH2 = 0x32,
    CMD_FREQ_SPECTRUM_CH3 = 0x33,
    CMD_FREQ_SPECTRUM_CH4 = 0x34,
    CMD_FREQ_SPECTRUM_CH5 = 0x35,
    CMD_FREQ_SPECTRUM_CH6 = 0x36,
    CMD_FREQ_SPECTRUM_CH7 = 0x37,

    CMD_FILT_SPECTRUM_CH0 = 0x40,
    CMD_FILT_SPECTRUM_CH1 = 0x41,
    CMD_FILT_SPECTRUM_CH2 = 0x42,
    CMD_FILT_SPECTRUM_CH3 = 0x43,
    CMD_FILT_SPECTRUM_CH4 = 0x44,
    CMD_FILT_SPECTRUM_CH5 = 0x45,
    CMD_FILT_SPECTRUM_CH6 = 0x46,
    CMD_FILT_SPECTRUM_CH7 = 0x47,

    CMD_RAW_SPECTRUM_CH0_OLD  = 0x03,
    CMD_RAW_SPECTRUM_CH1_OLD  = 0x02,
    CMD_FREQ_SPECTRUM_CH0_OLD = 0x07,
    CMD_FREQ_SPECTRUM_CH1_OLD = 0x06,
    CMD_FILT_SPECTRUM_CH0_OLD = 0x09,
    CMD_FILT_SPECTRUM_CH1_OLD = 0x08,
} CmdType;

typedef enum {
    WAVE_TYPE_RAW      = 0,
    WAVE_TYPE_FILT     = 1,
    WAVE_TYPE_BASELINE = 2
} WaveType_t;

typedef enum {
    SPEC_TYPE_RAW          = 0,
    SPEC_TYPE_FREQ_FILTER  = 1,
    SPEC_TYPE_TIME_FILTER  = 2
} SpecType_t;

typedef struct {
    uint8_t wave_ch[DISPLAY_NUM_CH];
    uint8_t wave_type[DISPLAY_NUM_CH];
    uint8_t spec_type[DISPLAY_NUM_CH];
} DisplayConfig_t;

void DisplayConfig_SetDefaults(DisplayConfig_t *cfg);
CmdType DisplayConfig_GetWaveCmd(uint8_t wave_type);
CmdType DisplayConfig_GetSpectrumCmd(uint8_t spec_type, uint8_t ch);

extern DisplayConfig_t g_display_config;

typedef struct {
    uint8_t  state;
    uint8_t *payload;
    uint16_t capacity;
    uint16_t len;
} FrameParser;

void Parse_Serial_Data(Serial_Port port);

void Pack_Frame(Serial_Port port, CmdType Cmd, const uint8_t *payload, uint16_t len);
void Send_WaveformSingle(CmdType wave_type, uint8_t ch, float val);
void Send_Spectrum(CmdType spectrum_type, float *mag, uint8_t frag_idx);
void Send_Focus(float attn0, float attn1, float ema0, float ema1,
                uint8_t trend_state, uint8_t instant_state);

#endif
