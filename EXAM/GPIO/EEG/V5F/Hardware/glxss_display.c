#include "glxss_display.h"
#include "glxss_me.h"
#include "usb_host_config.h"
#include "ch32h417_usbhs_host.h"
#include "ipc_log.h"
#include <string.h>

#define LCD_W  640
#define LCD_H  400

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
static uint8_t g_mode = GLXSS_DISP_MODE_IDLE;

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
    0,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,0,0,0,0,0,0,
    0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F,
    0,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,0,0,0,0,0,0,
    0x0E,0x04,0x04,0x04,0x04,0x04,0x0E,
    0,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,0,0,0,0,0,0,
    0x10,0x10,0x10,0x10,0x10,0x10,0x1F,
    0,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,0,0,0,0,0,0,
    0,0,0,0,0,0,0, 0,0,0,0,0,0,0,
    0x1E,0x11,0x11,0x1E,0x14,0x12,0x11,
    0,0,0,0,0,0,0, 0x1F,0x04,0x04,0x04,0x04,0x04,0x04,
    0,0,0,0,0,0,0, 0,0,0,0,0,0,0,
    0x11,0x11,0x11,0x15,0x15,0x1B,0x11,
    0x11,0x11,0x0A,0x04,0x0A,0x11,0x11,
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

static int is_in_arrow(int32_t px, int32_t py, int32_t cx, int32_t cy, int32_t sz, uint8_t dir)
{
    int32_t dx = px - cx;
    int32_t dy = py - cy;
    int32_t half = sz / 2;
    if (dir == 0) {
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

static uint8_t g_test_dir = 0;
static int g_test_same = 0;
static uint8_t g_test_show_result = 0;
static uint8_t g_test_new_result = 0;
static uint8_t g_test_hide_target = 0;
static uint32_t g_test_target_start = 0;
static uint8_t g_test_pred = 0;

static void arrow_mode_init(void)
{
    g_test_dir = 0;
    g_test_same = 0;
    g_test_show_result = 0;
    g_test_new_result = 0;
    g_test_hide_target = 0;
    g_arrow_tgt_dir = 0;
    g_arrow_tgt_color = 0;
    g_arrow_tgt_vis = 1;
    g_arrow_res_vis = 0;
    g_arrow_dash = 0;
    g_test_target_start = tick_get_us();
    IPC_Log_Printf_V5F("[V5F] TEST TARGET LEFT (1/3)\r\n");
}

static glxss_err_t arrow_mode_step(void)
{
    uint32_t now = tick_get_us();

    if (g_test_new_result) {
        g_test_new_result = 0;
        g_test_same++;
        g_arrow_res_dir = g_test_pred;
        g_arrow_tgt_color = (g_test_pred == g_test_dir) ? 1 : 2;
        g_arrow_res_color = g_arrow_tgt_color;
        g_arrow_tgt_vis = 1;
        g_arrow_res_vis = 1;
        g_test_show_result = 1;
        g_test_target_start = now;
        IPC_Log_Printf_V5F("[V5F] TEST %s vs %s %s\r\n",
            g_test_dir ? "R" : "L", g_test_pred ? "R" : "L",
            (g_test_pred == g_test_dir) ? "OK" : "FAIL");
    }

    if (g_test_show_result) {
        if (now - g_test_target_start >= 200000) {
            g_test_show_result = 0;
            g_arrow_tgt_color = 0;
            g_arrow_res_vis = 0;
            g_arrow_tgt_vis = 0;
            g_test_hide_target = 1;
            g_test_target_start = now;
        }
    } else if (g_test_hide_target && now - g_test_target_start >= 200000) {
        g_test_hide_target = 0;
        g_arrow_tgt_vis = 1;
        if (g_test_same >= 3) {
            g_test_same = 0;
            g_test_dir = !g_test_dir;
        }
        g_arrow_tgt_dir = g_test_dir;
        g_test_target_start = now;
        IPC_Log_Printf_V5F("[V5F] TEST TARGET %s (%d/3)\r\n",
            g_test_dir ? "R" : "L", g_test_same + 1);
    }

    glxss_err_t err = send_arrow_frame();
    if (err != GLXSS_OK) return err;
    uint32_t next = tick_get_us() + 200000;
    while (tick_get_us() < next);
    return GLXSS_OK;
}

/* ==================== ARROW_TRAIN ==================== */

static uint8_t g_train_dir = 0;
static uint16_t g_train_progress = 0;
static uint8_t g_train_arrow_vis = 0;
static uint8_t g_train_seq[] = {0, 1, 0, 1};
static int g_train_trial = 0;
static uint8_t g_train_phase = 0;
static uint32_t g_train_phase_start = 0;

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
        if (g_train_phase == 0 && !g_train_arrow_vis)
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
    glxss_err_t err = send_idle_frame();
    if (err != GLXSS_OK) return err;
    uint32_t next = tick_get_us() + 200000;
    while (tick_get_us() < next);
    return GLXSS_OK;
}

static void train_mode_init(void)
{
    g_train_trial = 0;
    g_train_phase = 0;
    g_train_dir = g_train_seq[0];
    g_train_arrow_vis = 0;
    g_train_progress = 0;
    g_train_phase_start = tick_get_us();
    IPC_Log_Printf_V5F("[V5F] TRAIN rest 5s, trial 1/4 %s\r\n", g_train_seq[0] ? "RIGHT" : "LEFT");
}

static glxss_err_t train_mode_step(void)
{
    uint32_t elapsed = tick_get_us() - g_train_phase_start;
    if (g_train_phase == 0) {
        g_train_progress = (uint16_t)(elapsed / 5000);
        if (g_train_progress > 1000) g_train_progress = 1000;
        g_train_arrow_vis = 0;
        if (elapsed >= 5000000) {
            g_train_phase = 1;
            g_train_dir = g_train_seq[g_train_trial];
            g_train_arrow_vis = 1;
            g_train_progress = 0;
            g_train_phase_start = tick_get_us();
            IPC_Log_Printf_V5F("[V5F] TRIAL %d/4 %s\r\n", g_train_trial + 1,
                g_train_dir ? "RIGHT" : "LEFT");
        }
    } else if (g_train_phase == 1) {
        g_train_progress = (uint16_t)(elapsed / 15000);
        if (g_train_progress > 1000) g_train_progress = 1000;
        g_train_arrow_vis = 1;
        if (elapsed >= 15000000) {
            g_train_trial++;
            if (g_train_trial >= 4) {
                g_train_phase = 2;
                g_train_arrow_vis = 0;
                g_train_progress = 1000;
                IPC_Log_Printf_V5F("[V5F] TRAIN COMPLETE\r\n");
            } else {
                g_train_phase = 0;
                g_train_arrow_vis = 0;
                g_train_progress = 0;
                g_train_phase_start = tick_get_us();
                IPC_Log_Printf_V5F("[V5F] TRAIN rest, trial %d/4\r\n", g_train_trial + 1);
            }
        }
    } else {
        g_train_arrow_vis = 0;
        g_train_progress = 1000;
    }
    glxss_err_t err = send_train_frame();
    if (err != GLXSS_OK) return err;
    uint32_t next = tick_get_us() + 200000;
    while (tick_get_us() < next);
    return GLXSS_OK;
}

/* ==================== PUBLIC API ==================== */

void glxss_display_init(void)
{
    tick_init();
    IPC_Log_Printf_V5F("[V5F] TIM4 calibrated: %lu ticks/ms\r\n", (unsigned long)g_ticks_per_ms);
    ssvep_precompute();
    g_mode = GLXSS_DISP_MODE_IDLE;
}

void glxss_display_set_mode(uint8_t mode)
{
    if (mode == g_mode) return;
    IPC_Log_Printf_V5F("[V5F] display switch %d->%d\r\n", g_mode, mode);
    reset_toggle();
    g_mode = mode;
    switch (g_mode) {
    case GLXSS_DISP_MODE_SSVEP:       ssvep_mode_init(); break;
    case GLXSS_DISP_MODE_ARROW:       arrow_mode_init(); break;
    case GLXSS_DISP_MODE_ARROW_TRAIN: train_mode_init(); break;
    default: break;
    }
}

void glxss_display_set_ssvep_freq(uint32_t freq_idx)
{
    if (freq_idx < 4) g_ssvep_fi = (int)freq_idx;
    ssvep_mode_init();
}

glxss_err_t glxss_display_step(void)
{
    switch (g_mode) {
    case GLXSS_DISP_MODE_IDLE:        return idle_mode_step();
    case GLXSS_DISP_MODE_SSVEP:       return ssvep_mode_step();
    case GLXSS_DISP_MODE_ARROW:       return arrow_mode_step();
    case GLXSS_DISP_MODE_ARROW_TRAIN: return train_mode_step();
    default: return idle_mode_step();
    }
}

uint8_t glxss_display_get_mode(void)
{
    return g_mode;
}

void glxss_display_notify_result(uint8_t pred)
{
    g_test_pred = pred;
    g_test_new_result = 1;
}

void glxss_display_notify_collect(uint32_t mode)
{
    (void)mode;
}