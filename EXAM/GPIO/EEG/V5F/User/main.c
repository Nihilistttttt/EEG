/********************************** (C) COPYRIGHT *******************************
 * File Name          : main_v5f.c
 * Description        : V5F core entry. Compile with V5F_MODE_GLXSS to drive
 *                      AR glasses display (W25Q64 architecture, command-driven).
 *******************************************************************************/

#include "debug.h"
#include "hardware.h"

#if defined(V5F_MODE_GLXSS) || defined(GLXSS_ENABLED)
#include "ipc_log.h"
#include "glxss_me.h"
#include "usb_host_config.h"
#include "ch32h417_usbhs_host.h"
#include <string.h>
#ifdef GLXSS_ENABLED
#include "dualcore_ipc.h"
#endif
#endif

#if defined(V5F_MODE_GLXSS) || defined(GLXSS_ENABLED)

#define MODE_IDLE        0
#define MODE_SSVEP       1
#define MODE_ARROW       2
#define MODE_ARROW_TRAIN 3

#define LCD_W  640
#define LCD_H  400
#define BYTES_PER_PIX  4

#ifndef USB_PID_NAK
#define USB_PID_NAK  0x0A
#endif

static volatile uint32_t g_tick_raw = 0;
static uint32_t g_ticks_per_ms = 1;

void TIM4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM4_IRQHandler(void)
{
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    g_tick_raw++;
}

static uint32_t tick_get_us(void)
{
    return (uint32_t)((uint64_t)g_tick_raw * 1000 / g_ticks_per_ms);
}

static void tick_init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_TIM4, ENABLE);
    TIM_Cmd(TIM4, DISABLE);
    TIM_TimeBaseStructure.TIM_Period        = 10 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler     = SystemCoreClock / 1000000 / 4 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    NVIC_EnableIRQ(TIM4_IRQn);
    TIM_Cmd(TIM4, ENABLE);
    g_tick_raw = 0;
    Delay_Ms(200);
    g_ticks_per_ms = g_tick_raw / 200;
    if (g_ticks_per_ms == 0) g_ticks_per_ms = 1;
    g_tick_raw = 0;
}

static uint16_t g_endp_tog = 0;
static uint8_t g_mode = MODE_IDLE;

static uint8_t find_bulk_out_ep(void)
{
    uint8_t index = DEF_USBHS_PORT_INDEX * DEF_ONE_USB_SUP_DEV_TOTAL;
    for (uint8_t n = 0; n < HostCtl[index].InterfaceNum; n++) {
        for (uint8_t e = 0; e < HostCtl[index].Interface[n].OutEndpNum; e++) {
            if (HostCtl[index].Interface[n].OutEndpType[e] == 0x02)
                return HostCtl[index].Interface[n].OutEndpAddr[e];
        }
    }
    return 0;
}

static void reset_toggle(void)
{
    glxss_usb_clear_halt(GLXSS_EP_DATA_OUT);
    g_endp_tog = 0;
    Delay_Ms(50);
}

/* ==================== SSVEP ==================== */

static __attribute__((aligned(4))) uint8_t s_white_first[512];
static __attribute__((aligned(4))) uint8_t s_white_mid[512];
static __attribute__((aligned(4))) uint8_t s_black_first[512];
static __attribute__((aligned(4))) uint8_t s_black_mid[512];

static void ssvep_precompute(void)
{
    uint8_t hdr[16] = {0x80,0x02,0x00,0x00, 0x90,0x01,0x00,0x00, 0x01,0,0,0,0,0,0,0};
    memcpy(s_white_first, hdr, 16);
    memset(s_white_first + 16, 0xFF, 496);
    memset(s_white_mid, 0xFF, 512);
    memcpy(s_black_first, hdr, 16);
    { uint32_t *p = (uint32_t *)(s_black_first + 16); for (int i = 0; i < 124; i++) p[i] = 0xFF000000; }
    { uint32_t *p = (uint32_t *)s_black_mid; for (int i = 0; i < 128; i++) p[i] = 0xFF000000; }
}

static glxss_err_t send_ssvep_frame(uint8_t visible)
{
    uint8_t out_ep = find_bulk_out_ep();
    if (out_ep == 0) return GLXSS_ERR_BULK_WRITE;
    uint8_t *first = visible ? s_white_first : s_black_first;
    uint8_t *mid   = visible ? s_white_mid   : s_black_mid;
    uint32_t saved_tx_dma = USBHSH->TX_DMA;
    uint16_t tog = g_endp_tog;
    uint16_t tog0 = 0;
    uint16_t tog1 = USBHS_UH_T_TOG_DATA1;
    USBHSH->TX_DMA = (uint32_t)first;
    USBHSH->TX_LEN = 512;
    USBHSH->CONTROL = USBHS_UH_HOST_ACTION | USB_PID_OUT | (out_ep << 4) | (tog ? tog1 : tog0);
    USBHSH->INT_FLAG = USBHS_UHIF_TRANSFER;
    for (uint16_t i = 20000; (i != 0) && ((USBHSH->INT_FLAG & USBHS_UHIF_TRANSFER) == 0); i--);
    USBHSH->CONTROL &= ~USBHS_UH_T_TOKEN_MASK;
    if (!(USBHSH->INT_FLAG & USBHS_UHIF_TRANSFER)) { USBHSH->TX_DMA = saved_tx_dma; return GLXSS_ERR_BULK_WRITE; }
    tog ^= USBHS_UH_T_TOG_DATA1;
    USBHSH->TX_DMA = (uint32_t)mid;
    for (uint32_t c = 1; c < 2000; c++) {
        USBHSH->TX_LEN = 512;
        USBHSH->CONTROL = USBHS_UH_HOST_ACTION | USB_PID_OUT | (out_ep << 4) | (tog ? tog1 : tog0);
        USBHSH->INT_FLAG = USBHS_UHIF_TRANSFER;
        for (uint16_t i = 20000; (i != 0) && ((USBHSH->INT_FLAG & USBHS_UHIF_TRANSFER) == 0); i--);
        USBHSH->CONTROL &= ~USBHS_UH_T_TOKEN_MASK;
        if (!(USBHSH->INT_FLAG & USBHS_UHIF_TRANSFER)) { USBHSH->TX_DMA = saved_tx_dma; return GLXSS_ERR_BULK_WRITE; }
        tog ^= USBHS_UH_T_TOG_DATA1;
    }
    USBHSH->TX_LEN = 16;
    USBHSH->CONTROL = USBHS_UH_HOST_ACTION | USB_PID_OUT | (out_ep << 4) | (tog ? tog1 : tog0);
    USBHSH->INT_FLAG = USBHS_UHIF_TRANSFER;
    for (uint16_t i = 20000; (i != 0) && ((USBHSH->INT_FLAG & USBHS_UHIF_TRANSFER) == 0); i--);
    USBHSH->CONTROL &= ~USBHS_UH_T_TOKEN_MASK;
    g_endp_tog = tog ^ USBHS_UH_T_TOG_DATA1;
    USBHSH->TX_DMA = saved_tx_dma;
    return GLXSS_OK;
}

static uint8_t g_ssvep_freqs[] = {11, 13, 15, 17};
static int g_ssvep_fi = 0;
static uint8_t g_ssvep_toggle = 1;
static uint32_t g_ssvep_count = 0;
static uint32_t g_ssvep_t_start = 0;

static void ssvep_mode_init(void)
{
    g_ssvep_toggle = 1;
    g_ssvep_count = 0;
    g_ssvep_t_start = tick_get_us();
    IPC_Log_Printf_V5F("[V5F] SSVEP %dHz\r\n", g_ssvep_freqs[g_ssvep_fi]);
}

static void ssvep_mode_set_freq(uint32_t freq_idx)
{
    if (freq_idx < 4) g_ssvep_fi = (int)freq_idx;
    ssvep_mode_init();
}

static glxss_err_t ssvep_mode_step(void)
{
    uint8_t freq = g_ssvep_freqs[g_ssvep_fi];
    uint32_t half_us = 500000 / freq;
    glxss_err_t err = send_ssvep_frame(g_ssvep_toggle);
    if (err != GLXSS_OK) return err;
    g_ssvep_toggle = !g_ssvep_toggle;
    g_ssvep_count++;
    uint32_t target = g_ssvep_t_start + (uint32_t)g_ssvep_count * half_us;
    while (tick_get_us() < target);
    if (tick_get_us() - g_ssvep_t_start >= 2000000) {
        uint32_t elapsed_us = tick_get_us() - g_ssvep_t_start;
        uint32_t meas_hz_x10 = (uint32_t)g_ssvep_count * 5000 / (elapsed_us / 1000);
        IPC_Log_Printf_V5F("[V5F] %lu frm %lums => %lu.%luHz (fixed %dHz)\r\n",
            (unsigned long)g_ssvep_count, (unsigned long)(elapsed_us / 1000),
            (unsigned long)(meas_hz_x10 / 10), (unsigned long)(meas_hz_x10 % 10),
            freq);
        g_ssvep_toggle = 1;
        g_ssvep_count = 0;
        g_ssvep_t_start = tick_get_us();
    }
    return GLXSS_OK;
}

/* ==================== TEXT RENDER ==================== */

static const uint8_t s_font5x7[26*7] = {
    0x0E,0x11,0x11,0x1F,0x11,0x11,0x11,
    0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,
    0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F,
    0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,
    0x0E,0x04,0x04,0x04,0x04,0x04,0x0E,
    0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,
    0x10,0x10,0x10,0x10,0x10,0x10,0x1F,
    0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,
    0x1E,0x11,0x11,0x1E,0x14,0x12,0x11,
    0,0,0,0,0,0,0,
    0x1F,0x04,0x04,0x04,0x04,0x04,0x04,
    0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,
    0x11,0x11,0x11,0x15,0x15,0x1B,0x11,
    0x11,0x11,0x0A,0x04,0x0A,0x11,0x11,
    0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,
};

static int text_pixel(int32_t x, int32_t y,
                       const char *str, int len,
                       int scale, int cx, int cy)
{
    int cw = 5, ch = 7, gap = 1;
    int step = cw + gap;
    int tw = len * step - gap;
    int x0 = cx - (tw * scale) / 2;
    int y0 = cy - (ch * scale) / 2;
    int dx = (int)x - x0;
    int dy = (int)y - y0;
    if (dx < 0 || dy < 0 || dy >= ch * scale) return 0;
    int ci = dx / (step * scale);
    if (ci >= len) return 0;
    int lx = dx - ci * step * scale;
    if (lx >= cw * scale) return 0;
    int row = dy / scale;
    int col = lx / scale;
    if (row >= ch || col >= cw) return 0;
    int idx = str[ci] - 'A';
    if (idx < 0 || idx >= 26) return 0;
    return (s_font5x7[idx * 7 + row] & (0x10 >> col)) ? 1 : 0;
}

/* ==================== ARROW ==================== */

static __attribute__((aligned(4))) uint8_t s_pkt[2048];
static uint8_t g_arrow_tgt_dir = 0;
static uint8_t g_arrow_res_dir = 0;
static uint8_t g_arrow_tgt_color = 0;
static uint8_t g_arrow_res_color = 0;
static uint8_t g_arrow_tgt_vis = 1;
static uint8_t g_arrow_res_vis = 0;
static uint8_t g_arrow_dash = 0;
static uint8_t g_arrow_dirty = 1;

static int is_in_arrow(int32_t px, int32_t py, int32_t cx, int32_t cy, int32_t sz, uint8_t dir)
{
    /* dir: 0=LEFT, 1=RIGHT (matches IPC_PRED_LEFT/RIGHT and DIR_LABEL_*) */
    int32_t dx = px - cx;
    int32_t dy = py - cy;
    int32_t half = sz / 2;
    if (dir == 1) {
        if (dx < -half || dx > half) return 0;
        int32_t edge = half - dx;
        if (edge < 0) edge = 0;
        return (dy >= -edge && dy <= edge);
    } else {
        if (dx < -half || dx > half) return 0;
        int32_t edge = half + dx;
        if (edge < 0) edge = 0;
        return (dy >= -edge && dy <= edge);
    }
}

static int is_in_dash(int32_t px, int32_t py, int32_t cx, int32_t cy, int32_t sz)
{
    int32_t dx = px - cx;
    int32_t dy = py - cy;
    int32_t hw = sz * 3 / 4;
    int32_t hh = sz / 6;
    int32_t gap = sz / 4;
    if (dy >= -hh && dy <= hh) {
        if ((dx >= -hw && dx <= -gap) || (dx >= gap && dx <= hw)) return 1;
    }
    return 0;
}

static void fill_arrow_pkt(uint32_t offset, uint16_t len)
{
    uint8_t hdr[16] = {0x80,0x02,0x00,0x00, 0x90,0x01,0x00,0x00, 0x01,0,0,0,0,0,0,0};
    const int32_t sz = 120, tgt_cx = 160, res_cx = 480, cy = 200;
    uint8_t tgt_r = 0xFF, tgt_g = 0xFF, tgt_b = 0xFF;
    if (g_arrow_tgt_color == 1) { tgt_r = 0x00; tgt_g = 0xFF; tgt_b = 0x00; }
    else if (g_arrow_tgt_color == 2) { tgt_r = 0xFF; tgt_g = 0x00; tgt_b = 0x00; }
    uint8_t res_r = 0xFF, res_g = 0xFF, res_b = 0xFF;
    if (g_arrow_res_color == 1) { res_r = 0x00; res_g = 0xFF; res_b = 0x00; }
    else if (g_arrow_res_color == 2) { res_r = 0xFF; res_g = 0x00; res_b = 0x00; }
    for (uint16_t i = 0; i < len; i++) {
        uint32_t pos = offset + i;
        if (pos < 16) { s_pkt[i] = hdr[pos]; continue; }
        uint32_t pb = pos - 16;
        uint32_t pi = pb / 4;
        uint8_t ch = pb % 4;
        uint32_t x = pi % 640;
        uint32_t y = pi / 640;
        uint8_t r = 0, g = 0, b = 0;
        if (g_arrow_tgt_vis && !g_arrow_dash && is_in_arrow((int32_t)x, (int32_t)y, tgt_cx, cy, sz, g_arrow_tgt_dir))
            { r = tgt_r; g = tgt_g; b = tgt_b; }
        if (g_arrow_tgt_vis && g_arrow_dash && is_in_dash((int32_t)x, (int32_t)y, 320, cy, 200))
            { r = tgt_r; g = tgt_g; b = tgt_b; }
        if (g_arrow_res_vis && is_in_arrow((int32_t)x, (int32_t)y, res_cx, cy, sz, g_arrow_res_dir))
            { r = res_r; g = res_g; b = res_b; }
        if (ch == 0) s_pkt[i] = r;
        else if (ch == 1) s_pkt[i] = g;
        else if (ch == 2) s_pkt[i] = b;
        else s_pkt[i] = 0xFF;
    }
}

static glxss_err_t send_arrow_frame(void)
{
    uint8_t out_ep = find_bulk_out_ep();
    if (out_ep == 0) return GLXSS_ERR_BULK_WRITE;
    uint32_t saved_tx_dma = USBHSH->TX_DMA;
    uint16_t tog = g_endp_tog;
    uint16_t tog0 = 0;
    uint16_t tog1 = USBHS_UH_T_TOG_DATA1;
    uint32_t frame_total = 1024016;
    uint32_t pos = 0;
    while (pos < frame_total) {
        uint16_t chunk = 512;
        if (pos + chunk > frame_total) chunk = (uint16_t)(frame_total - pos);
        fill_arrow_pkt(pos, chunk);
        USBHSH->TX_DMA = (uint32_t)s_pkt;
        USBHSH->TX_LEN = chunk;
        USBHSH->CONTROL = USBHS_UH_HOST_ACTION | USB_PID_OUT | (out_ep << 4) | (tog ? tog1 : tog0);
        USBHSH->INT_FLAG = USBHS_UHIF_TRANSFER;
        for (uint16_t i = 20000; (i != 0) && ((USBHSH->INT_FLAG & USBHS_UHIF_TRANSFER) == 0); i--);
        USBHSH->CONTROL &= ~USBHS_UH_T_TOKEN_MASK;
        if (!(USBHSH->INT_FLAG & USBHS_UHIF_TRANSFER)) { USBHSH->TX_DMA = saved_tx_dma; return GLXSS_ERR_BULK_WRITE; }
        tog ^= USBHS_UH_T_TOG_DATA1;
        pos += chunk;
    }
    g_endp_tog = tog;
    USBHSH->TX_DMA = saved_tx_dma;
    return GLXSS_OK;
}

/* 测试模式: 左三下右三下 + 推理结果对比 */
static uint32_t g_mi_infer_count = 0;
static uint8_t g_mi_sim_pred = IPC_PRED_UNKNOWN;
static int32_t g_mi_sim_sl = 5000;
static int32_t g_mi_sim_sr = 5000;
static uint32_t g_mi_sim_last_us = 0;
static uint8_t g_mi_infer_active = 0;

static uint8_t g_test_show_result = 0;
static uint8_t g_test_new_result = 0;
static uint8_t g_test_hide_target = 0;

static uint32_t g_test_target_start = 0;

static void arrow_mode_init(void)
{
    g_arrow_tgt_dir = 0;
    g_arrow_tgt_color = 0;
    g_arrow_tgt_vis = 0;
    g_arrow_res_dir = 0;
    g_arrow_res_color = 0;
    g_arrow_res_vis = 0;
    g_arrow_dash = 0;
    g_arrow_dirty = 1;
    g_test_show_result = 0;
    g_test_new_result = 0;
    g_test_hide_target = 0;
    g_test_target_start = tick_get_us();
}

static glxss_err_t arrow_mode_step(void)
{
    if (g_test_new_result) {
        g_test_new_result = 0;
        g_arrow_res_dir = (g_mi_sim_sr >= g_mi_sim_sl) ? 1u : 0u;
        g_arrow_res_color = (g_mi_sim_pred == IPC_PRED_UNKNOWN) ? 0u : 1u;
        g_arrow_res_vis = 1;
        g_arrow_dirty = 1;
    }

    if (!g_arrow_dirty) {
        return GLXSS_OK;
    }
    g_arrow_dirty = 0;
    return send_arrow_frame();
}


/* ==================== ARROW_TRAIN ==================== */

static uint8_t g_train_dir = 0;
static uint16_t g_train_progress = 0;
static uint8_t g_train_arrow_vis = 0;
static uint32_t g_train_phase_start = 0;
static uint8_t g_train_phase = 0;
static uint8_t g_train_active = 0;

static void fill_train_pkt(uint32_t offset, uint16_t len)
{
    uint8_t hdr[16] = {0x80,0x02,0x00,0x00, 0x90,0x01,0x00,0x00, 0x01,0,0,0,0,0,0,0};
    const int32_t sz = 160, cx = 320, cy = 160;
    const int32_t bar_y = 320, bar_h = 30, bar_x0 = 80, bar_x1 = 560, bar_w = bar_x1 - bar_x0;
    for (uint16_t i = 0; i < len; i++) {
        uint32_t pos = offset + i;
        if (pos < 16) { s_pkt[i] = hdr[pos]; continue; }
        uint32_t pb = pos - 16;
        uint32_t pi = pb / 4;
        uint8_t ch = pb % 4;
        uint32_t x = pi % 640;
        uint32_t y = pi / 640;
        uint8_t r = 0, g = 0, b = 0;
        if (g_train_arrow_vis && is_in_arrow((int32_t)x, (int32_t)y, cx, cy, sz, g_train_dir))
            { r = 0xFF; g = 0xFF; b = 0xFF; }
        if (!g_train_arrow_vis && g_train_active)
            if (text_pixel((int32_t)x, (int32_t)y, "RELAX", 5, 8, 320, 160))
                { r = 0xFF; g = 0xFF; b = 0xFF; }
        if ((int32_t)y >= bar_y && (int32_t)y < bar_y + bar_h &&
            (int32_t)x >= bar_x0 && (int32_t)x < bar_x1) {
            int32_t fill_x = bar_x0 + (int32_t)g_train_progress * bar_w / 1000;
            if ((int32_t)x < fill_x) { r = 0x00; g = 0x80; b = 0xFF; }
            else { r = 0x40; g = 0x40; b = 0x40; }
        }
        if (ch == 0) s_pkt[i] = r;
        else if (ch == 1) s_pkt[i] = g;
        else if (ch == 2) s_pkt[i] = b;
        else s_pkt[i] = 0xFF;
    }
}

static glxss_err_t send_train_frame(void)
{
    uint8_t out_ep = find_bulk_out_ep();
    if (out_ep == 0) return GLXSS_ERR_BULK_WRITE;
    uint32_t saved_tx_dma = USBHSH->TX_DMA;
    uint16_t tog = g_endp_tog;
    uint16_t tog0 = 0;
    uint16_t tog1 = USBHS_UH_T_TOG_DATA1;
    uint32_t frame_total = 1024016;
    uint32_t pos = 0;
    while (pos < frame_total) {
        uint16_t chunk = 512;
        if (pos + chunk > frame_total) chunk = (uint16_t)(frame_total - pos);
        fill_train_pkt(pos, chunk);
        USBHSH->TX_DMA = (uint32_t)s_pkt;
        USBHSH->TX_LEN = chunk;
        USBHSH->CONTROL = USBHS_UH_HOST_ACTION | USB_PID_OUT | (out_ep << 4) | (tog ? tog1 : tog0);
        USBHSH->INT_FLAG = USBHS_UHIF_TRANSFER;
        for (uint16_t i = 20000; (i != 0) && ((USBHSH->INT_FLAG & USBHS_UHIF_TRANSFER) == 0); i--);
        USBHSH->CONTROL &= ~USBHS_UH_T_TOKEN_MASK;
        if (!(USBHSH->INT_FLAG & USBHS_UHIF_TRANSFER)) { USBHSH->TX_DMA = saved_tx_dma; return GLXSS_ERR_BULK_WRITE; }
        tog ^= USBHS_UH_T_TOG_DATA1;
        pos += chunk;
    }
    g_endp_tog = tog;
    USBHSH->TX_DMA = saved_tx_dma;
    return GLXSS_OK;
}

static void fill_idle_pkt(uint32_t offset, uint16_t len)
{
    uint8_t hdr[16] = {0x80,0x02,0x00,0x00, 0x90,0x01,0x00,0x00, 0x01,0,0,0,0,0,0,0};
    for (uint16_t i = 0; i < len; i++) {
        uint32_t pos = offset + i;
        if (pos < 16) { s_pkt[i] = hdr[pos]; continue; }
        uint32_t pb = pos - 16;
        uint32_t pi = pb / 4;
        uint8_t ch = pb % 4;
        uint32_t x = pi % 640;
        uint32_t y = pi / 640;
        uint8_t r = 0, g = 0, b = 0;
        if (text_pixel((int32_t)x, (int32_t)y, "WAIT", 4, 8, 320, 200))
            { r = 0xFF; g = 0xFF; b = 0xFF; }
        if (ch == 0) s_pkt[i] = r;
        else if (ch == 1) s_pkt[i] = g;
        else if (ch == 2) s_pkt[i] = b;
        else s_pkt[i] = 0xFF;
    }
}

static glxss_err_t send_idle_frame(void)
{
    uint8_t out_ep = find_bulk_out_ep();
    if (out_ep == 0) return GLXSS_ERR_BULK_WRITE;
    uint32_t saved_tx_dma = USBHSH->TX_DMA;
    uint16_t tog = g_endp_tog;
    uint16_t tog0 = 0;
    uint16_t tog1 = USBHS_UH_T_TOG_DATA1;
    uint32_t frame_total = 1024016;
    uint32_t pos = 0;
    while (pos < frame_total) {
        uint16_t chunk = 512;
        if (pos + chunk > frame_total) chunk = (uint16_t)(frame_total - pos);
        fill_idle_pkt(pos, chunk);
        USBHSH->TX_DMA = (uint32_t)s_pkt;
        USBHSH->TX_LEN = chunk;
        USBHSH->CONTROL = USBHS_UH_HOST_ACTION | USB_PID_OUT | (out_ep << 4) | (tog ? tog1 : tog0);
        USBHSH->INT_FLAG = USBHS_UHIF_TRANSFER;
        for (uint16_t i = 20000; (i != 0) && ((USBHSH->INT_FLAG & USBHS_UHIF_TRANSFER) == 0); i--);
        USBHSH->CONTROL &= ~USBHS_UH_T_TOKEN_MASK;
        if (!(USBHSH->INT_FLAG & USBHS_UHIF_TRANSFER)) { USBHSH->TX_DMA = saved_tx_dma; return GLXSS_ERR_BULK_WRITE; }
        tog ^= USBHS_UH_T_TOG_DATA1;
        pos += chunk;
    }
    g_endp_tog = tog;
    USBHSH->TX_DMA = saved_tx_dma;
    return GLXSS_OK;
}

static glxss_err_t idle_mode_step(void)
{
    return send_idle_frame();
}

static void train_mode_init(void)
{
    g_train_active = 1;
    g_train_arrow_vis = 0;
    g_train_dir = 0;
    g_train_progress = 0;
    g_train_phase = 0;
    g_train_phase_start = tick_get_us();
    IPC_Log_Printf_V5F("[V5F] TRAIN init (command-driven)\r\n");
}

static glxss_err_t train_mode_step(void)
{
    uint32_t elapsed = tick_get_us() - g_train_phase_start;
    if (g_train_arrow_vis) {
        g_train_progress = (uint16_t)(elapsed / 15000);
        if (g_train_progress > 1000) g_train_progress = 1000;
    } else {
        g_train_progress = (uint16_t)(elapsed / 5000);
        if (g_train_progress > 1000) g_train_progress = 1000;
    }
    return send_train_frame();
}

/* ==================== MI SIM ==================== */

static void mi_sim_step(void)
{
    if (!g_mi_infer_active) return;
    uint32_t now = tick_get_us();
    if (now - g_mi_sim_last_us < 2000000) return;
    g_mi_sim_last_us = now;
    g_mi_infer_count++;
    uint32_t t = g_mi_infer_count;
    int32_t bias = (int32_t)((t * 137) % 10001) - 5000;
    g_mi_sim_sr = 5000 + bias;
    g_mi_sim_sl = 10000 - g_mi_sim_sr;
    int32_t conf = g_mi_sim_sr - g_mi_sim_sl;
    if (conf < 0) conf = -conf;
    if (conf < 300) g_mi_sim_pred = IPC_PRED_UNKNOWN;
    else g_mi_sim_pred = (g_mi_sim_sr >= g_mi_sim_sl) ? IPC_PRED_RIGHT : IPC_PRED_LEFT;
    IPC_SetMIResult_V5F(g_mi_sim_pred, g_mi_sim_sl, g_mi_sim_sr, conf);
    if (g_mode == MODE_ARROW && g_mi_sim_pred != IPC_PRED_UNKNOWN)
        g_test_new_result = 1;
}

/* ==================== SSVEP SIM ==================== */

static uint32_t g_ssvep_sim_timer = 0;
static int8_t g_ssvep_sim_idx = -1;

static void ssvep_sim_step(void)
{
    uint32_t ctrl = IPC_Ctrl_GetFlags_V5F();
    if (!(ctrl & IPC_CTRL_SSVEP_ENABLE)) return;
    g_ssvep_sim_timer++;
    if (g_ssvep_sim_timer >= 500) {
        g_ssvep_sim_timer = 0;
        if (ctrl & IPC_CTRL_SSVEP_SELFTEST) {
            g_ssvep_sim_idx = (int8_t)((g_mi_infer_count + 1) % 4);
        } else {
            g_ssvep_sim_idx = (g_ssvep_sim_idx + 1) % 4;
        }
        int32_t scores[4] = {0, 0, 0, 0};
        scores[g_ssvep_sim_idx] = 8000;
        IPC_SetSSVEPResult_V5F(g_ssvep_sim_idx, scores);
    }
}

/* ==================== COLLECT SIM ==================== */

static uint32_t g_collect_last_us = 0;
static uint32_t g_collect_seed = 1;
static uint8_t g_collect_mode = 0;

static uint32_t lcg_next(void)
{
    g_collect_seed = g_collect_seed * 1664525u + 1013904223u;
    return g_collect_seed;
}

static void collect_sim_step(void)
{
    if (g_mode != MODE_ARROW_TRAIN || g_collect_mode == 0) return;
    uint32_t now = tick_get_us();
    if (now - g_collect_last_us < 2000000) return;
    g_collect_last_us = now;
    IPC_IncSample_V5F(250);
    {
        int32_t feats[IPC_FEATURE_DIM];
        for (int i = 0; i < IPC_FEATURE_DIM; i++) {
            int32_t base = (i < 14) ? 1000 : 0;
            int32_t noise = (int32_t)(lcg_next() % 2001) - 1000;
            feats[i] = base + noise;
        }
        feats[0] += (g_train_dir == 0) ? 3000 : -3000;
        feats[7] += (g_train_dir == 0) ? -2000 : 2000;
        IPC_SetFeature_V5F(feats);
        IPC_Log_Printf_V5F("[V5F] FEAT seq=%lu\r\n", (unsigned long)IPC_LOG_SHARED->feature_seq);
    }
}

static void collect_mode_enter(uint32_t mode)
{
    g_collect_mode = (uint8_t)mode;
    g_collect_last_us = 0;
    g_collect_seed = 1;
    IPC_Log_Printf_V5F("[V5F] COLLECT mode=%lu (sim features)\r\n", (unsigned long)mode);
}

/* ==================== MODE SWITCH ==================== */

static uint32_t g_last_disp_us = 0;

static void switch_mode(uint8_t new_mode)
{
    if (new_mode == g_mode) return;
    IPC_Log_Printf_V5F("[V5F] switch %d->%d, reset toggle\r\n", g_mode, new_mode);
    reset_toggle();
    g_mode = new_mode;
    g_last_disp_us = 0;
    switch (g_mode) {
    case MODE_SSVEP:       ssvep_mode_init(); break;
    case MODE_ARROW:       arrow_mode_init(); break;
    case MODE_ARROW_TRAIN: train_mode_init(); break;
    }
}

static void handle_ipc_cmd(void)
{
    uint32_t param;
    uint32_t cmd = IPC_Cmd_Recv_V5F(&param);
    if (cmd == IPC_CMD_NONE) return;
    IPC_Log_Printf_V5F("[V5F] cmd=%lu param=%lu\r\n", (unsigned long)cmd, (unsigned long)param);
    uint32_t ctrl = IPC_Ctrl_GetFlags_V5F();
    if (ctrl & IPC_CTRL_RESET_DSP) {
        IPC_Log_Printf_V5F("[V5F] RESET DSP\r\n");
        g_mi_infer_count = 0;
        g_mi_sim_pred = IPC_PRED_UNKNOWN;
        g_mi_infer_active = 0;
        g_mi_sim_last_us = 0;
        g_ssvep_sim_timer = 0;
        g_ssvep_sim_idx = -1;
        g_collect_mode = 0;
        g_collect_last_us = 0;
        IPC_Ctrl_SetFlags_V5F(ctrl & ~IPC_CTRL_RESET_DSP);
    }
    switch (cmd) {
    case IPC_CMD_SSVEP:
        switch_mode(MODE_SSVEP);
        if (param < 4) ssvep_mode_set_freq(param);
        break;
    case IPC_CMD_ARROW:
        if (g_mode != MODE_ARROW) switch_mode(MODE_ARROW);
        g_arrow_tgt_dir = (uint8_t)(param & 1u);
        g_arrow_tgt_color = 1;
        g_arrow_tgt_vis = 1;
        g_arrow_res_vis = 0;
        g_arrow_dirty = 1;
        break;
    case IPC_CMD_ARROW_TRAIN:
        if (g_mode != MODE_ARROW_TRAIN) switch_mode(MODE_ARROW_TRAIN);
        g_train_active = 1;
        if (param <= 1) {
            g_train_dir = (uint8_t)param;
            g_train_arrow_vis = 1;
            g_train_progress = 0;
            g_train_phase_start = tick_get_us();
            IPC_Log_Printf_V5F("[V5F] TRAIN arrow dir=%s\r\n", param ? "RIGHT" : "LEFT");
        } else {
            g_train_arrow_vis = 0;
            g_train_progress = 0;
            g_train_phase_start = tick_get_us();
            IPC_Log_Printf_V5F("[V5F] TRAIN rest\r\n");
        }
        break;
    case IPC_CMD_MI_INFER:
        IPC_Log_Printf_V5F("[V5F] MI_INFER mode (sim)\r\n");
        g_mi_infer_active = 1;
        switch_mode(MODE_ARROW);
        break;
    case IPC_CMD_COLLECT:
        IPC_Log_Printf_V5F("[V5F] COLLECT cmd param=%lu\r\n", (unsigned long)param);
        collect_mode_enter(param ? param : 1);
        switch_mode(MODE_ARROW_TRAIN);
        break;
    case IPC_CMD_SSVEP_CTRL:
        IPC_Log_Printf_V5F("[V5F] SSVEP_CTRL flags=0x%02X\r\n", (unsigned)ctrl);
        switch_mode(MODE_SSVEP);
        break;
    case IPC_CMD_RESET:
        IPC_Log_Printf_V5F("[V5F] RESET -> IDLE\r\n");
        g_mi_infer_active = 0;
        g_collect_mode = 0;
        switch_mode(MODE_IDLE);
        break;
    default: break;
    }
}

#endif /* V5F_MODE_GLXSS || GLXSS_ENABLED */

/* ==================== MAIN ==================== */

int main(void)
{
    SystemAndCoreClockUpdate();
    Delay_Init();
    HSEM_FastTake(HSEM_ID0);
    HSEM_ReleaseOneSem(HSEM_ID0, 0);

#if defined(V5F_MODE_GLXSS)
    IPC_Log_Init_V5F();
    IPC_Log_Printf_V5F("[V5F] boot GLXSS mode, SystemCoreClk:%d\r\n", SystemCoreClock);
    Delay_Ms(500);
    Hardware();
    IPC_Log_SetStatus_V5F(IPC_LOG_STATUS_USB_INIT);
    IPC_Log_Printf_V5F("[V5F] USBHS Host init...\r\n");
    glxss_usb_host_init();
    IPC_Log_Printf_V5F("[V5F] USBHS Host init done, PORT=0x%08lX\r\n", (unsigned long)USBHSH->PORT_STATUS);
    IPC_Log_SetStatus_V5F(IPC_LOG_STATUS_USB_WAIT);
    IPC_Log_Printf_V5F("[V5F] Waiting for display device...\r\n");
    glxss_err_t err = glxss_usb_wait_device(GLXSS_DEVICE_VID, GLXSS_DEVICE_PID, 30000);
    if (err != GLXSS_OK) {
        IPC_Log_SetStatus_V5F(IPC_LOG_STATUS_ERROR);
        IPC_Log_Printf_V5F("[V5F] Device wait failed: %d\r\n", err);
        while (1) {}
    }
    IPC_Log_SetStatus_V5F(IPC_LOG_STATUS_USB_READY);
    IPC_Log_Printf_V5F("[V5F] Device found! Clearing halt...\r\n");
    glxss_usb_clear_halt(GLXSS_EP_DATA_OUT);
    g_endp_tog = 0;
    tick_init();
    IPC_Log_Printf_V5F("[V5F] TIM4 calibrated: %lu ticks/ms\r\n", (unsigned long)g_ticks_per_ms);
    ssvep_precompute();
    IPC_Log_SetStatus_V5F(IPC_LOG_STATUS_IDLE);
    IPC_Log_Printf_V5F("[V5F] IDLE - waiting for V3F commands...\r\n");

    for (;;) {
        handle_ipc_cmd();
        mi_sim_step();
        ssvep_sim_step();
        collect_sim_step();
        err = GLXSS_OK;
        switch (g_mode) {
        case MODE_IDLE:        err = idle_mode_step(); break;
        case MODE_SSVEP:       err = ssvep_mode_step(); break;
        case MODE_ARROW:       err = arrow_mode_step(); break;
        case MODE_ARROW_TRAIN: err = train_mode_step(); break;
        }
        if (err != GLXSS_OK) {
            IPC_Log_SetStatus_V5F(IPC_LOG_STATUS_ERROR);
            IPC_Log_Printf_V5F("[V5F] FAIL err=%d\r\n", err);
            break;
        }
    }
    while (1) {}

#elif defined(GLXSS_ENABLED)
    DualCore_IPC_Init_V5F();
    IPC_Log_Init_V5F();
    IPC_Log_Printf_V5F("[V5F] boot EEG+GLXSS merged mode, SystemCoreClk:%d\r\n", SystemCoreClock);
    Delay_Ms(500);
    Hardware();
    IPC_Log_SetStatus_V5F(IPC_LOG_STATUS_USB_INIT);
    IPC_Log_Printf_V5F("[V5F] USBHS Host init...\r\n");
    glxss_usb_host_init();
    IPC_Log_Printf_V5F("[V5F] USBHS Host init done, PORT=0x%08lX\r\n", (unsigned long)USBHSH->PORT_STATUS);
    IPC_Log_SetStatus_V5F(IPC_LOG_STATUS_USB_WAIT);
    IPC_Log_Printf_V5F("[V5F] Waiting for display device...\r\n");
    glxss_err_t err = glxss_usb_wait_device(GLXSS_DEVICE_VID, GLXSS_DEVICE_PID, 30000);
    if (err != GLXSS_OK) {
        IPC_Log_SetStatus_V5F(IPC_LOG_STATUS_ERROR);
        IPC_Log_Printf_V5F("[V5F] Device wait failed: %d, EEG-only mode\r\n", err);
    } else {
        IPC_Log_SetStatus_V5F(IPC_LOG_STATUS_USB_READY);
        IPC_Log_Printf_V5F("[V5F] Device found! Clearing halt...\r\n");
        glxss_usb_clear_halt(GLXSS_EP_DATA_OUT);
        g_endp_tog = 0;
        tick_init();
        IPC_Log_Printf_V5F("[V5F] TIM4 calibrated: %lu ticks/ms\r\n", (unsigned long)g_ticks_per_ms);
        ssvep_precompute();
    }
    IPC_Log_SetStatus_V5F(IPC_LOG_STATUS_IDLE);
    IPC_Log_Printf_V5F("[V5F] EEG+GLXSS main loop\r\n");

    for (;;) {
        DualCore_V5F_MainLoopProcess();
        DualCore_V5F_SSVEP_RunPending();
        handle_ipc_cmd();
#ifdef V5F_MODE_GLXSS
        mi_sim_step();
        ssvep_sim_step();
        collect_sim_step();
#else
        {
            extern volatile uint8_t g_ipc_v5f_pred;
            extern volatile int32_t g_ipc_v5f_score_left;
            extern volatile int32_t g_ipc_v5f_score_right;
            static uint8_t last_pred = IPC_PRED_UNKNOWN;
            static uint32_t last_infer_count = 0;
            extern volatile uint32_t g_ipc_v5f_infer_count;
            if (g_mi_infer_active) {
                uint8_t cur_pred = g_ipc_v5f_pred;
                uint32_t cur_count = g_ipc_v5f_infer_count;
                if (cur_count != last_infer_count || cur_pred != last_pred) {
                    uint8_t pred_changed = (cur_pred != last_pred);
                    last_pred = cur_pred;
                    last_infer_count = cur_count;
                    g_mi_sim_pred = cur_pred;
                    g_mi_sim_sl = g_ipc_v5f_score_left;
                    g_mi_sim_sr = g_ipc_v5f_score_right;
                    g_mi_infer_count = cur_count;
                    if (pred_changed)
                        IPC_Log_Printf_V5F("[V5F] pred=%u sl=%ld sr=%ld\r\n",
                            (unsigned)cur_pred, (long)g_mi_sim_sl, (long)g_mi_sim_sr);
                    if (g_mode == MODE_ARROW)
                        g_test_new_result = 1;
                }
            }
        }
#endif
        err = GLXSS_OK;
        if (g_mode == MODE_SSVEP) {
            err = ssvep_mode_step();
        } else {
            uint32_t now = tick_get_us();
            if (now - g_last_disp_us >= 1000000) {
                g_last_disp_us = now;
                switch (g_mode) {
                case MODE_IDLE:        err = idle_mode_step(); break;
                case MODE_ARROW:       err = arrow_mode_step(); break;
                case MODE_ARROW_TRAIN: err = train_mode_step(); break;
                default: break;
                }
            }
        }
        if (err != GLXSS_OK) {
            IPC_Log_SetStatus_V5F(IPC_LOG_STATUS_ERROR);
            IPC_Log_Printf_V5F("[V5F] FAIL err=%d\r\n", err);
            break;
        }
    }
    while (1) {}

#else
    DualCore_IPC_Init_V5F();

    while (1)
    {
        DualCore_V5F_MainLoopProcess();
        DualCore_V5F_SSVEP_RunPending();
        __WFI();
    }
#endif
}
