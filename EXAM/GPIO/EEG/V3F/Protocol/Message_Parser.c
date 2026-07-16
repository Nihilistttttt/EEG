#include "Message_Parser.h"
#include "Config.h"

typedef struct {
    CmdType Cmd;
    void (*handler)(uint8_t *payload, uint16_t Len);
} CmdEntry;

extern void Handle_Waveform(uint8_t *payload, uint16_t Len);
extern void Handle_Spectrum_CH0(uint8_t *payload, uint16_t Len);
extern void Handle_Spectrum_CH1(uint8_t *payload, uint16_t Len);

static CmdEntry cmd_table[] = {
    {CMD_RAW_WAVE,          Handle_Waveform    },
    {CMD_FILT_WAVE,         Handle_Waveform    },
    {CMD_BASELINE_WAVE,     Handle_Waveform    },
    {CMD_FREQ_SPECTRUM_CH0, Handle_Spectrum_CH0},
    {CMD_FILT_SPECTRUM_CH0, Handle_Spectrum_CH0},
    {CMD_RAW_SPECTRUM_CH0,  Handle_Spectrum_CH0},
    {CMD_FREQ_SPECTRUM_CH1, Handle_Spectrum_CH1},
    {CMD_FILT_SPECTRUM_CH1, Handle_Spectrum_CH1},
    {CMD_RAW_SPECTRUM_CH1,  Handle_Spectrum_CH1},
    {CMD_NULL,              NULL               }
};

DisplayConfig_t g_display_config;

void DisplayConfig_SetDefaults(DisplayConfig_t *cfg)
{
    cfg->wave_ch[0] = 2;
    cfg->wave_ch[1] = 2;
    cfg->wave_type[0] = WAVE_TYPE_RAW;
    cfg->wave_type[1] = WAVE_TYPE_FILT;
    cfg->spec_type[0] = SPEC_TYPE_RAW;
    cfg->spec_type[1] = SPEC_TYPE_TIME_FILTER;
}

CmdType DisplayConfig_GetWaveCmd(uint8_t wave_type)
{
    switch (wave_type) {
    case WAVE_TYPE_FILT:     return CMD_FILT_WAVE;
    case WAVE_TYPE_BASELINE: return CMD_BASELINE_WAVE;
    default:                 return CMD_RAW_WAVE;
    }
}

CmdType DisplayConfig_GetSpectrumCmd(uint8_t spec_type, uint8_t ch)
{
    if (ch >= DISPLAY_MAX_CH) ch = 0;
    switch (spec_type) {
    case SPEC_TYPE_FREQ_FILTER: return (CmdType)(CMD_FREQ_SPECTRUM_BASE + ch);
    case SPEC_TYPE_TIME_FILTER: return (CmdType)(CMD_FILT_SPECTRUM_BASE + ch);
    default:                    return (CmdType)(CMD_RAW_SPECTRUM_BASE + ch);
    }
}

static uint8_t payload_buf_debug[SERIAL_DEBUG_RX_BUF_SIZE];
static uint8_t payload_buf_wifi[SERIAL_WIFI_RX_BUF_SIZE];

static FrameParser parser_debug = {
    .state = 0,
    .payload = payload_buf_debug,
    .capacity = SERIAL_DEBUG_RX_BUF_SIZE,
    .len = 0
};
static FrameParser parser_wifi = {
    .state = 0,
    .payload = payload_buf_wifi,
    .capacity = SERIAL_WIFI_RX_BUF_SIZE,
    .len = 0
};

static inline FrameParser* get_parser(Serial_Port port) {
    return (port == SERIAL_PORT_DEBUG) ? &parser_debug : &parser_wifi;
}

static void Parse_Frame(FrameParser *p, uint8_t byte) {
    switch (p->state) {
        case 0:
            if (byte == FRAME_CHAR) {
                p->state = 1;
                p->len = 0;
            }
            break;

        case 1:
            if (byte == ESCAPE_CHAR) {
                p->state = 2;
            } else if (byte == FRAME_CHAR) {
                if (p->len >= CMD_PREFIX_LEN) {
                    uint8_t cmd = p->payload[0];
                    for (int i = 0; cmd_table[i].Cmd; i++) {
                        if (cmd == cmd_table[i].Cmd) {
                            cmd_table[i].handler(p->payload + CMD_PREFIX_LEN,
                                                 p->len - CMD_PREFIX_LEN);
                            break;
                        }
                    }
                }
                p->len = 0;
            } else {
                if (p->len < p->capacity) {
                    p->payload[p->len++] = byte;
                } else {
                    p->state = 0;
                }
            }
            break;

        case 2:
            {
                uint8_t original = byte ^ ESCAPE_XOR;
                if (p->len < p->capacity) {
                    p->payload[p->len++] = original;
                } else {
                    p->state = 0;
                    break;
                }
                p->state = 1;
            }
            break;
    }
}

void Parse_Serial_Data(Serial_Port port) {
    uint8_t *buf;
    uint16_t len;
    if (Serial_IsDataReady(port) == 0) return;
    len = Serial_GetDataPacket(port, &buf);
    FrameParser *p = get_parser(port);
    for (uint16_t i = 0; i < len; i++) {
        Parse_Frame(p, buf[i]);
    }
}

void Pack_Frame(Serial_Port port, CmdType Cmd, const uint8_t *payload, uint16_t len) {
    static uint8_t frame[TX_MESSAGE_BUF_SIZE];
    uint16_t idx = 0;

    frame[idx++] = FRAME_CHAR;

    if (Cmd == FRAME_CHAR || Cmd == ESCAPE_CHAR) {
        frame[idx++] = ESCAPE_CHAR;
        frame[idx++] = (uint8_t)Cmd ^ ESCAPE_XOR;
    } else {
        frame[idx++] = (uint8_t)Cmd;
    }

    for (uint16_t i = 0; i < len && idx < TX_MESSAGE_BUF_SIZE - 2; i++) {
        uint8_t b = payload[i];
        if (b == FRAME_CHAR || b == ESCAPE_CHAR) {
            frame[idx++] = ESCAPE_CHAR;
            frame[idx++] = b ^ ESCAPE_XOR;
        } else {
            frame[idx++] = b;
        }
    }

    frame[idx++] = FRAME_CHAR;

    Serial_SendArray_DMA(port, frame, idx);
}

void Pack_Frame_Fragmented(Serial_Port port, CmdType Cmd, const uint8_t *payload, uint16_t len,
                           uint8_t frag_idx, uint8_t total_frags) {
    static uint8_t extended_payload[TX_MESSAGE_BUF_SIZE - 4];
    uint16_t ext_len = 0;

    extended_payload[ext_len++] = frag_idx;
    extended_payload[ext_len++] = total_frags;

    if (ext_len + len <= sizeof(extended_payload)) {
        memcpy(extended_payload + ext_len, payload, len);
        ext_len += len;
    } else {
        ext_len = sizeof(extended_payload);
    }

    Pack_Frame(port, Cmd, extended_payload, ext_len);
}

void Send_WaveformSingle(CmdType wave_type, uint8_t ch, float val) {
    uint8_t payload[5];
    payload[0] = ch;
    memcpy(payload + 1, &val, 4);
    Pack_Frame(SERIAL_PORT_WIFI, wave_type, payload, 5);
}

void Send_Spectrum(CmdType spectrum_type, float *mag, uint8_t frag_idx) {
    const uint8_t TOTAL_FRAGS = 32;
    const uint16_t FLOAT_PER_FRAG = 128 / TOTAL_FRAGS;
    const uint16_t FRAG_SIZE = FLOAT_PER_FRAG * sizeof(float);

    if (frag_idx >= TOTAL_FRAGS) return;

    uint16_t offset = frag_idx * FLOAT_PER_FRAG;
    uint8_t payload[FRAG_SIZE];
    memcpy(payload, mag + offset, FRAG_SIZE);

    Pack_Frame_Fragmented(SERIAL_PORT_WIFI, spectrum_type, payload, FRAG_SIZE,
                          frag_idx, TOTAL_FRAGS);
}

void Send_Focus(float attn0, float attn1, float ema0, float ema1,
                uint8_t trend_state, uint8_t instant_state) {
    uint8_t payload[18];
    uint16_t off = 0;
    memcpy(payload + off, &attn0, 4); off += 4;
    memcpy(payload + off, &attn1, 4); off += 4;
    memcpy(payload + off, &ema0,  4); off += 4;
    memcpy(payload + off, &ema1,  4); off += 4;
    memcpy(payload + off, &trend_state, 1); off += 1;
    memcpy(payload + off, &instant_state, 1);
    Pack_Frame(SERIAL_PORT_WIFI, CMD_FOCUS, payload, 18);
}
