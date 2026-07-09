#include "Message_Parser.h"
#include "Config.h"
/* ---------- 命令处理表 ---------- */
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
    {CMD_FREQ_SPECTRUM_CH0, Handle_Spectrum_CH0},
    {CMD_FILT_SPECTRUM_CH0, Handle_Spectrum_CH0},
    {CMD_RAW_SPECTRUM_CH0,  Handle_Spectrum_CH0},
    {CMD_FREQ_SPECTRUM_CH1, Handle_Spectrum_CH1},
    {CMD_FILT_SPECTRUM_CH1, Handle_Spectrum_CH1},
    {CMD_RAW_SPECTRUM_CH1,  Handle_Spectrum_CH1},
    {CMD_NULL,              NULL               }
};

/* ---------- 两个串口独立的接收缓冲区和解析器 ---------- */
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

/* 根据端口获取对应解析器 */
static inline FrameParser* get_parser(Serial_Port port) {
    return (port == SERIAL_PORT_DEBUG) ? &parser_debug : &parser_wifi;
}

/* ---------- 内部函数：解析一个字节 ---------- */
static void Parse_Frame(FrameParser *p, uint8_t byte) {
    switch (p->state) {
        case 0: // WAIT_HEADER
            if (byte == FRAME_CHAR) {
                p->state = 1;   // 进入负载状态
                p->len = 0;
            }
            break;

        case 1: // IN_PAYLOAD
            if (byte == ESCAPE_CHAR) {
                p->state = 2;   // 转义状态
            } else if (byte == FRAME_CHAR) {
                // 帧结束，处理已接收的命令
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
                // 准备接收下一帧（帧尾也是帧头）
                p->len = 0;
                // state 仍为 IN_PAYLOAD
            } else {
                if (p->len < p->capacity) {
                    p->payload[p->len++] = byte;
                } else {
                    // 缓冲区溢出，放弃当前帧，重新同步
                    p->state = 0;
                }
            }
            break;

        case 2: // IN_ESCAPE
            {
                uint8_t original = byte ^ ESCAPE_XOR;
                if (p->len < p->capacity) {
                    p->payload[p->len++] = original;
                } else {
                    p->state = 0;   // 溢出
                    break;
                }
                p->state = 1;       // 回到负载状态
            }
            break;
    }
}

/* ---------- 公共函数：从端口读取并解析 ---------- */
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

/* ---------- 发送端：打包并转义 ---------- */
void Pack_Frame(Serial_Port port, CmdType Cmd, const uint8_t *payload, uint16_t len) {
    static uint8_t frame[TX_MESSAGE_BUF_SIZE];
    uint16_t idx = 0;

    // 帧头
    frame[idx++] = FRAME_CHAR;

    // 命令字转义
    if (Cmd == FRAME_CHAR || Cmd == ESCAPE_CHAR) {
        frame[idx++] = ESCAPE_CHAR;
        frame[idx++] = (uint8_t)Cmd ^ ESCAPE_XOR;
    } else {
        frame[idx++] = (uint8_t)Cmd;
    }

    // 负载转义
    for (uint16_t i = 0; i < len && idx < TX_MESSAGE_BUF_SIZE - 2; i++) {
        uint8_t b = payload[i];
        if (b == FRAME_CHAR || b == ESCAPE_CHAR) {
            frame[idx++] = ESCAPE_CHAR;
            frame[idx++] = b ^ ESCAPE_XOR;
        } else {
            frame[idx++] = b;
        }
    }

    // 帧尾
    frame[idx++] = FRAME_CHAR;

    Serial_SendArray_DMA(port, frame, idx);
}

void Pack_Frame_Fragmented(Serial_Port port, CmdType Cmd, const uint8_t *payload, uint16_t len,
                           uint8_t frag_idx, uint8_t total_frags) {
    static uint8_t extended_payload[TX_MESSAGE_BUF_SIZE - 4]; // 预留帧头/尾/命令字空间
    uint16_t ext_len = 0;
    
    // 添加分片信息
    extended_payload[ext_len++] = frag_idx;
    extended_payload[ext_len++] = total_frags;
    
    // 拷贝实际数据
    if (ext_len + len <= sizeof(extended_payload)) {
        memcpy(extended_payload + ext_len, payload, len);
        ext_len += len;
    } else {
        // 溢出保护：截断（实际应确保缓冲区足够大）
        ext_len = sizeof(extended_payload);
    }
    
    // 调用原有打包函数发送扩展后的负载
    Pack_Frame(port, Cmd, extended_payload, ext_len);
}

/* ---------- 具体数据发送函数（通过 USART2 发出）---------- */
void Send_Waveform(CmdType wave_type, float val_ch0, float val_ch1) {
    uint8_t payload[8];
    memcpy(payload, &val_ch0, 4);
    memcpy(payload + 4, &val_ch1, 4);
    if (wave_type == CMD_RAW_WAVE || wave_type == CMD_FILT_WAVE) {
        Pack_Frame(SERIAL_PORT_WIFI, wave_type, payload, 8);
    }
}

// void Send_Spectrum(CmdType spectrum_type, float *mag) {
//     uint8_t payload[512];
//     memcpy(payload, mag, 128 * sizeof(float));
//     if (spectrum_type == CMD_FILT_SPECTRUM_CH0 || spectrum_type == CMD_FREQ_SPECTRUM_CH0 ||
//         spectrum_type == CMD_FILT_SPECTRUM_CH1 || spectrum_type == CMD_FREQ_SPECTRUM_CH1 ||
//         spectrum_type == CMD_RAW_SPECTRUM_CH0   || spectrum_type == CMD_RAW_SPECTRUM_CH1) {
//         Pack_Frame(SERIAL_PORT_USART2, spectrum_type, payload, 128 * sizeof(float));
//     }
//     // Delay_Ms(25);
// }

void Send_Spectrum(CmdType spectrum_type, float *mag, uint8_t frag_idx) {
    // 原数据共 128 个 float，分 8 片，每片 16 个 float = 64 字节
    const uint8_t TOTAL_FRAGS = 32;
    const uint16_t FLOAT_PER_FRAG = 128 / TOTAL_FRAGS;  // 16
    const uint16_t FRAG_SIZE = FLOAT_PER_FRAG * sizeof(float); // 64 字节
    
    if (frag_idx >= TOTAL_FRAGS) return; // 无效分片号
    
    // 计算分片偏移
    uint16_t offset = frag_idx * FLOAT_PER_FRAG;
    uint8_t payload[FRAG_SIZE];
    memcpy(payload, mag + offset, FRAG_SIZE);
    
    if (spectrum_type == CMD_FILT_SPECTRUM_CH0 || spectrum_type == CMD_FREQ_SPECTRUM_CH0 ||
        spectrum_type == CMD_FILT_SPECTRUM_CH1 || spectrum_type == CMD_FREQ_SPECTRUM_CH1 ||
        spectrum_type == CMD_RAW_SPECTRUM_CH0   || spectrum_type == CMD_RAW_SPECTRUM_CH1) {
        // 使用分片打包函数发送，携带分片信息和总分片数
        Pack_Frame_Fragmented(SERIAL_PORT_WIFI, spectrum_type, payload, FRAG_SIZE,
                              frag_idx, TOTAL_FRAGS);
    }
    // 如需延时，可取消注释
    // Delay_Ms(25);
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