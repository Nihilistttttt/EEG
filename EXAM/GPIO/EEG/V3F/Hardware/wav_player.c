#include "wav_player.h"
#include "max98357a.h"
#include "FATFS/ff.h"
#include "sd_card.h"
#include "Serial.h"
#include <string.h>

static int16_t  s_dma_buf[WAV_PLAYER_BUF_SAMPLES];
static uint8_t  s_raw_buf[WAV_PLAYER_HALF_SAMPLES * 4];
static FIL      s_wav_file;
static wav_header_t s_wav_hdr;
static volatile int s_playing = 0;
static volatile int s_dma_ht_flag = 0;
static volatile int s_dma_tc_flag = 0;
static volatile int s_eof = 0;
static uint32_t s_bytes_remaining;
static uint32_t s_bytes_per_sample;
static uint8_t  s_volume_pct = 5;

static void dma_callback(int half) {
    if (half == 0) s_dma_ht_flag = 1;
    else           s_dma_tc_flag = 1;
}

static int parse_wav_header(wav_header_t *hdr) {
    uint8_t buf[512];
    UINT br = 0;
    if (f_read(&s_wav_file, buf, 512, &br) != FR_OK || br < 12) return -1;

    if (memcmp(buf, "RIFF", 4) != 0) return -2;
    if (memcmp(buf + 8, "WAVE", 4) != 0) return -3;

    uint32_t pos = 12;
    int fmt_found = 0;
    while (pos + 8 <= br) {
        uint32_t chunk_size = buf[pos+4] | (buf[pos+5]<<8) | (buf[pos+6]<<16) | (buf[pos+7]<<24);

        if (memcmp(buf + pos, "fmt ", 4) == 0) {
            if (pos + 8 + 16 > br) return -4;
            uint16_t audio_format = buf[pos+8] | (buf[pos+9]<<8);
            if (audio_format != 1) return -5;
            hdr->channels        = buf[pos+10] | (buf[pos+11]<<8);
            hdr->sample_rate     = buf[pos+12] | (buf[pos+13]<<8) | (buf[pos+14]<<16) | (buf[pos+15]<<24);
            hdr->bits_per_sample = buf[pos+22] | (buf[pos+23]<<8);
            fmt_found = 1;
        } else if (memcmp(buf + pos, "data", 4) == 0) {
            if (!fmt_found) return -6;
            hdr->data_size = chunk_size;
            hdr->data_offset = pos + 8;
            f_lseek(&s_wav_file, hdr->data_offset);
            return 0;
        }
        pos += 8 + chunk_size;
        if (chunk_size & 1) pos++;
    }
    return -7;
}

static void fill_half(int half) {
    int16_t *dst = s_dma_buf + (half ? WAV_PLAYER_HALF_SAMPLES : 0);

    if (s_eof) {
        memset(dst, 0, WAV_PLAYER_HALF_SAMPLES * 2);
        return;
    }

    uint32_t frames = WAV_PLAYER_HALF_SAMPLES / 2;
    uint32_t to_read = frames * s_bytes_per_sample;
    if (to_read > s_bytes_remaining) to_read = s_bytes_remaining;

    UINT br = 0;
    if (to_read > 0) {
        if (f_read(&s_wav_file, s_raw_buf, to_read, &br) != FR_OK) br = 0;
    }
    s_bytes_remaining -= br;

    uint32_t frames_read = br / s_bytes_per_sample;

    if (s_wav_hdr.channels == 1) {
        for (uint32_t i = 0; i < frames_read; i++) {
            int16_t s = (int16_t)(s_raw_buf[i*2] | (s_raw_buf[i*2+1] << 8));
            dst[i*2]   = s;
            dst[i*2+1] = s;
        }
        for (uint32_t i = frames_read * 2; i < WAV_PLAYER_HALF_SAMPLES; i++)
            dst[i] = 0;
    } else {
        memcpy(dst, s_raw_buf, br);
        for (uint32_t i = br / 2; i < WAV_PLAYER_HALF_SAMPLES; i++)
            dst[i] = 0;
    }

    if (s_volume_pct < 100) {
        for (uint32_t i = 0; i < WAV_PLAYER_HALF_SAMPLES; i++)
            dst[i] = (int16_t)((dst[i] * s_volume_pct) / 100);
    }

    if (s_bytes_remaining == 0) s_eof = 1;
}

int wav_player_play(const char *filename) {
    static FATFS fs;
    if (s_playing) wav_player_stop();

    if (f_mount(&fs, "", 1) != FR_OK) {
        Serial_Printf(SERIAL_PORT_DEBUG, "[WAV] mount FAIL\r\n");
        return -1;
    }

    if (f_open(&s_wav_file, filename, FA_READ) != FR_OK) {
        Serial_Printf(SERIAL_PORT_DEBUG, "[WAV] open %s FAIL\r\n", filename);
        return -2;
    }

    memset(&s_wav_hdr, 0, sizeof(s_wav_hdr));
    int ret = parse_wav_header(&s_wav_hdr);
    if (ret != 0) {
        Serial_Printf(SERIAL_PORT_DEBUG, "[WAV] header parse FAIL: %d\r\n", ret);
        f_close(&s_wav_file);
        return -3;
    }

    Serial_Printf(SERIAL_PORT_DEBUG, "[WAV] %s: %luHz %uch %ubit, data=%lu\r\n",
                  filename, (unsigned long)s_wav_hdr.sample_rate,
                  s_wav_hdr.channels, s_wav_hdr.bits_per_sample,
                  (unsigned long)s_wav_hdr.data_size);

    if (s_wav_hdr.bits_per_sample != 16) {
        Serial_Printf(SERIAL_PORT_DEBUG, "[WAV] only 16bit supported\r\n");
        f_close(&s_wav_file);
        return -4;
    }
    if (s_wav_hdr.channels != 1 && s_wav_hdr.channels != 2) {
        Serial_Printf(SERIAL_PORT_DEBUG, "[WAV] only mono/stereo supported\r\n");
        f_close(&s_wav_file);
        return -5;
    }

    s_bytes_per_sample = s_wav_hdr.channels * 2;
    s_bytes_remaining = s_wav_hdr.data_size;
    s_eof = 0;
    s_dma_ht_flag = 0;
    s_dma_tc_flag = 0;

    fill_half(0);
    fill_half(1);

    max98357a_init(MAX98357A_GAIN_15DB_I2S);
    if (max98357a_play_stream(s_dma_buf, WAV_PLAYER_BUF_SAMPLES, dma_callback) != 0) {
        Serial_Printf(SERIAL_PORT_DEBUG, "[WAV] play_stream FAIL\r\n");
        f_close(&s_wav_file);
        return -6;
    }

    s_playing = 1;
    Serial_Printf(SERIAL_PORT_DEBUG, "[WAV] playing...\r\n");
    return 0;
}

void wav_player_stop(void) {
    if (!s_playing) return;
    max98357a_stop();
    f_close(&s_wav_file);
    s_playing = 0;
    s_eof = 0;
    s_dma_ht_flag = 0;
    s_dma_tc_flag = 0;
}

int wav_player_is_playing(void) {
    return s_playing && !s_eof;
}

void wav_player_set_volume(uint8_t vol_pct) {
    if (vol_pct > 100) vol_pct = 100;
    s_volume_pct = vol_pct;
}

void wav_player_poll(void) {
    if (!s_playing) return;
    if (s_dma_ht_flag) {
        s_dma_ht_flag = 0;
        fill_half(0);
    }
    if (s_dma_tc_flag) {
        s_dma_tc_flag = 0;
        fill_half(1);
    }
    if (s_eof && !s_dma_ht_flag && !s_dma_tc_flag) {
        Delay_Ms(50);
        wav_player_stop();
        Serial_Printf(SERIAL_PORT_DEBUG, "[WAV] done\r\n");
    }
}
