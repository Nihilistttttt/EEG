/**
 * glxss_me.c - GLXSS ME 智能眼镜 USB 驱动实现
 *
 * CH32H417 移植版, 基于:
 *   https://github.com/happyme531/glxss-client (client.py)
 *
 * USB 协议栈需要适配 WCH 的 USBHS/USBSS Host 库,
 * 见底部的 CH32H417 适配层。
 *
 * 内存规划建议:
 *   - 内部 SRAM:  堆栈、USB 控制结构体、小缓冲区
 *   - QSPI PSRAM: LVGL 帧缓冲 (512KB~1MB)
 *   - QSPI Flash: GLXSS ME 固件 (1.5MB)
 */

#include "glxss_me.h"
#include <string.h>

/* ====================================================================
 * 调试开关 (移除此文件时注释掉)
 * ==================================================================== */
// #define GLXSS_DEBUG

#ifdef GLXSS_DEBUG
#include <stdio.h>
#define GLXSS_LOG(fmt, ...)  printf("[GLXSS] " fmt "\r\n", ##__VA_ARGS__)
#define GLXSS_ERR(fmt, ...)  printf("[GLXSS_ERR] " fmt "\r\n", ##__VA_ARGS__)
#else
#define GLXSS_LOG(...)
#define GLXSS_ERR(...)
#endif

/* ====================================================================
 * 协议常量
 * ==================================================================== */

/* LCD 控制传输: 读信息 */
#define CTRL_LCD_INFO_bmReqType  0xA1    /* Dev→Host, Standard, Interface */
#define CTRL_LCD_INFO_bRequest   0x04
#define CTRL_LCD_INFO_wValue     0x0000
#define CTRL_LCD_INFO_wIndex     0x0003  /* Interface 3 (显示接口) */
#define CTRL_LCD_INFO_wLength    0x0008  /* 返回 8 字节 */

/* LCD 控制传输: 设置亮度 (假设, 未在 Python 中验证) */
#define CTRL_LCD_BRIGHT_bmReqType 0x21   /* Host→Dev, Class, Interface */
#define CTRL_LCD_BRIGHT_bRequest  0x01
#define CTRL_LCD_BRIGHT_wValue    0x0D0D /* 猜测的亮度控制值 */
#define CTRL_LCD_BRIGHT_wIndex    0x0003
#define CTRL_LCD_BRIGHT_DATA_LEN  0x01

/* 图像 Bulk 包格式:
 *   [0:4]   width      (uint32 LE)
 *   [4:8]   height     (uint32 LE)
 *   [8]     format     (uint8) = 1 (RGBA8888)
 *   [9:11]  reserved   (uint8[3]) = {0,0,0}
 *   [12:15] reserved   (uint8[4]) — 实际代码中是 4字节而不是注释说的 8字节
 *   [16:]   pixel data (RGBA8888)
 */
#define IMG_HEADER_SIZE     16  /* width(4) + height(4) + hdr(8) */

/* Bulk 分块大小 (实现在 glxss_me_ch32h417.c 中按 USBHS_MAX_PACKET_SIZE 分块) */

/* ====================================================================
 * 状态
 * ==================================================================== */
static glxss_lcd_info_t s_lcd_info;     /* 缓存的 LCD 信息 */
static uint8_t          s_initialized;  /* 设备是否已初始化 */

/* ====================================================================
 * USB Host 底层操作 (实现在 glxss_me_ch32h417.c 中)
 *
 * 这 4 个函数是平台相关的，由 glxss_me_ch32h417.c 提供真正实现。
 * 此文件中的桩函数仅作文档参考，不要在此文件中也定义它们，
 * 否则链接时会与 glxss_me_ch32h417.c 冲突。
 * ==================================================================== */

/* ====================================================================
 * GLXSS ME 协议实现
 * ==================================================================== */

/**
 * glxss_load_firmware - 上传固件到 Bootloader
 *
 * 流程:
 *   1. 等待 03e7:2150 (Movidius Bootloader) 连接
 *   2. 逐块读取固件, 通过 Bulk EP1 发送
 *   3. 固件上传完成后, 设备自动复位并重新枚举
 */
glxss_err_t glxss_load_firmware(glxss_fw_read_t fw_read, uint32_t fw_size)
{
    uint8_t  buf[GLXSS_FW_CHUNK_SIZE];
    uint32_t offset = 0;
    uint32_t chunk;
    glxss_err_t err;

    GLXSS_LOG("Loading firmware (%lu bytes)...", (unsigned long)fw_size);

    if (fw_read == NULL) {
        GLXSS_ERR("fw_read callback is NULL");
        return GLXSS_ERR_INVALID_PARAM;
    }

    if (fw_size > GLXSS_FW_MAX_SIZE) {
        GLXSS_ERR("Firmware too large: %lu > %lu",
                  (unsigned long)fw_size, (unsigned long)GLXSS_FW_MAX_SIZE);
        return GLXSS_ERR_FW_TOO_LARGE;
    }

    /* 1. 等待 Bootloader 设备 */
    GLXSS_LOG("Waiting for bootloader (03E7:2150)...");
    err = glxss_usb_wait_device(GLXSS_BOOTLOADER_VID, GLXSS_BOOTLOADER_PID, 10000);
    if (err != GLXSS_OK) {
        GLXSS_ERR("Bootloader not found");
        return err;
    }

    /* 2. 分块读取并发送固件
     *
     *    注意: 与 Python 的 dev.write(0x01, entire_fw) 不同,
     *    MCU 需要自行分块。CH32H417 的 USB 控制器有 FIFO 大小限制。
     */
    GLXSS_LOG("Uploading firmware...");
    while (offset < fw_size) {
        chunk = fw_size - offset;
        if (chunk > GLXSS_FW_CHUNK_SIZE)
            chunk = GLXSS_FW_CHUNK_SIZE;

        /* 从外部存储读取固件块 */
        if (fw_read(offset, buf, chunk) != chunk) {
            GLXSS_ERR("Firmware read failed at offset %lu", (unsigned long)offset);
            return GLXSS_ERR_FW_READ;
        }

        /* 通过 USB Bulk 发送 */
        err = glxss_usb_bulk_write(GLXSS_EP_BOOT_FW, buf, chunk);
        if (err != GLXSS_OK) {
            GLXSS_ERR("Bulk write failed at offset %lu", (unsigned long)offset);
            return err;
        }

        offset += chunk;

        /* 进度提示 (每 10%) */
#ifdef GLXSS_DEBUG
        if (fw_size > 0) {
            uint32_t pct = offset * 100 / fw_size;
            static uint32_t last_pct = 0;
            if (pct - last_pct >= 10) {
                GLXSS_LOG("  FW upload: %lu%% (%lu/%lu KB)",
                          (unsigned long)pct,
                          (unsigned long)(offset / 1024),
                          (unsigned long)(fw_size / 1024));
                last_pct = pct;
            }
        }
#endif
    }

    GLXSS_LOG("Firmware upload complete!");
    return GLXSS_OK;
}

/**
 * glxss_get_lcd_info - 获取 LCD 显示信息
 *
 * 对应 Python:
 *   data = device.ctrl_transfer(0xa1, 0x04, 0x00, 0x03, 0x08)
 *   width = data[0] | data[1] << 8
 *   height = data[2] | data[3] << 8
 *   ...
 */
glxss_err_t glxss_get_lcd_info(glxss_lcd_info_t *info)
{
    uint8_t  buf[8] = {0};
    glxss_err_t err;

    if (info == NULL)
        return GLXSS_ERR_INVALID_PARAM;

    /* 控制传输: 读 LCD 信息 */
    err = glxss_usb_ctrl_transfer(
        CTRL_LCD_INFO_bmReqType,
        CTRL_LCD_INFO_bRequest,
        CTRL_LCD_INFO_wValue,
        CTRL_LCD_INFO_wIndex,
        buf,
        CTRL_LCD_INFO_wLength
    );
    if (err != GLXSS_OK) {
        GLXSS_ERR("Failed to get LCD info");
        return err;
    }

    /* 解析返回数据 (Little-Endian) */
    info->width       = (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);
    info->height      = (uint16_t)buf[2] | ((uint16_t)buf[3] << 8);
    info->orientation = buf[4];
    info->rotation    = buf[5];
    info->brightness  = (uint16_t)buf[6] | ((uint16_t)buf[7] << 8);

    GLXSS_LOG("LCD: %dx%d orient=%d rot=%d bright=%d",
              info->width, info->height,
              info->orientation, info->rotation, info->brightness);

    /* 缓存 */
    s_lcd_info = *info;
    return GLXSS_OK;
}

/**
 * glxss_set_brightness - 设置屏幕亮度
 *
 * 注: 此功能基于协议的推测实现, 未在原始 Python 代码中验证。
 * 如果硬件不支持, GLXSS_ERR_CTL_TRANSFER 可以被忽略。
 */
glxss_err_t glxss_set_brightness(uint16_t brightness)
{
    uint8_t data[1];
    data[0] = (uint8_t)(brightness & 0xFF);

    glxss_err_t err = glxss_usb_ctrl_transfer(
        CTRL_LCD_BRIGHT_bmReqType,
        CTRL_LCD_BRIGHT_bRequest,
        CTRL_LCD_BRIGHT_wValue,
        CTRL_LCD_BRIGHT_wIndex,
        data,
        CTRL_LCD_BRIGHT_DATA_LEN
    );

    if (err == GLXSS_OK) {
        s_lcd_info.brightness = brightness;
        GLXSS_LOG("Brightness set to %d", brightness);
    }

    return err;
}

/**
 * glxss_display_frame - 发送一帧图像到眼镜
 *
 * 对应 Python:
 *   buf = width[4] + height[4] + b"\x01\x00\x00\x00\x00\x00\x00\x00" + data
 *   device.write(0x01, buf)
 *
 * @param width   图像宽度 (应与 LCD 宽度匹配)
 * @param height  图像高度 (应与 LCD 高度匹配)
 * @param data    RGBA8888 像素数据 (width * height * 4 字节)
 */
glxss_err_t glxss_display_frame(uint16_t width, uint16_t height, const uint8_t *data)
{
    /*
     * 构造发送缓冲区:
     *
     *   [0:4]   width (uint32 LE)
     *   [4:8]   height (uint32 LE)
     *   [8]     format = 1 (RGBA8888)
     *   [9:15]  reserved = 0
     *   [16:]   pixel data
     *
     * 有两种实现方式:
     *   A. 连续发送模式: 先发 header, 再发数据 (适合大帧)
     *   B. 合并发送: 拼好整个包再发 (适合小帧或有 PSRAM 的情况)
     */

    uint32_t pixel_bytes = (uint32_t)width * height * 4;
    glxss_err_t err;

    /* 参数校验 */
    if (data == NULL)
        return GLXSS_ERR_INVALID_PARAM;
    if (width == 0 || height == 0 || width > GLXSS_MAX_WIDTH || height > GLXSS_MAX_HEIGHT)
        return GLXSS_ERR_INVALID_PARAM;

    /* 方案 A: 分开发送 header + data (不申请大缓冲区) */
    uint8_t header[IMG_HEADER_SIZE];

    header[0]  = (uint8_t)(width & 0xFF);
    header[1]  = (uint8_t)((width >> 8) & 0xFF);
    header[2]  = (uint8_t)((width >> 16) & 0xFF);
    header[3]  = (uint8_t)((width >> 24) & 0xFF);

    header[4]  = (uint8_t)(height & 0xFF);
    header[5]  = (uint8_t)((height >> 8) & 0xFF);
    header[6]  = (uint8_t)((height >> 16) & 0xFF);
    header[7]  = (uint8_t)((height >> 24) & 0xFF);

    header[8]  = 0x01;  /* format = RGBA8888 */
    header[9]  = 0x00;  /* reserved */
    header[10] = 0x00;
    header[11] = 0x00;
    header[12] = 0x00;  /* reserved (4 bytes in actual implementation) */
    header[13] = 0x00;
    header[14] = 0x00;
    header[15] = 0x00;

    /*
     * 分开发送 header 和 data:
     *   大部分 USB 控制器支持连续 Bulk 传输, 不会在两次写入之间插入其他数据。
     *   如果 CH32H417 的 USB 库不支持, 需要改用方案 B (合并发送)。
     */
    err = glxss_usb_bulk_write(GLXSS_EP_DATA_OUT, header, IMG_HEADER_SIZE);
    if (err != GLXSS_OK) {
        GLXSS_ERR("Failed to send image header");
        return err;
    }

    err = glxss_usb_bulk_write(GLXSS_EP_DATA_OUT, data, pixel_bytes);
    if (err != GLXSS_OK) {
        GLXSS_ERR("Failed to send image data");
        return err;
    }

    GLXSS_LOG("Frame sent: %dx%d (%lu bytes)", width, height, (unsigned long)pixel_bytes);
    return GLXSS_OK;
}

/**
 * glxss_init - 完整初始化
 *
 * 完整流程:
 *   1. 等待 Bootloader 设备
 *   2. 上传固件
 *   3. 等待设备重新枚举为 G25 固件
 *   4. 获取 LCD 信息
 *
 * @param fw_read  固件读取回调
 * @param fw_size  固件大小
 * @param timeout_ms  设备超时
 */
glxss_err_t glxss_init(glxss_fw_read_t fw_read,
                       uint32_t        fw_size,
                       uint32_t        timeout_ms)
{
    glxss_err_t err;

    GLXSS_LOG("GLXSS ME init start");

    /* 1. 初始化 USB Host */
    if (glxss_usb_host_init() != 0) {
        GLXSS_ERR("USB Host init failed");
        return GLXSS_ERR_ENUM;
    }

    /* 2. 检查是否已经是运行模式 (可能之前已经初始化过) */
    err = glxss_usb_wait_device(GLXSS_DEVICE_VID, GLXSS_DEVICE_PID, 100);
    if (err == GLXSS_OK) {
        GLXSS_LOG("Device already in firmware mode");
        goto get_info;
    }

    /* 3. 上传固件 */
    GLXSS_LOG("Need to load firmware (size=%lu)", (unsigned long)fw_size);
    err = glxss_load_firmware(fw_read, fw_size);
    if (err != GLXSS_OK) {
        GLXSS_ERR("Firmware load failed");
        return err;
    }

    /* 4. 等待设备重新枚举 (Bootloader 复位 → 加载 G25 固件) */
    GLXSS_LOG("Waiting for device re-enumeration...");
    err = glxss_usb_wait_device(GLXSS_DEVICE_VID, GLXSS_DEVICE_PID, timeout_ms);
    if (err != GLXSS_OK) {
        GLXSS_ERR("Device not found after firmware load");
        return err;
    }

get_info:
    /* 5. 获取 LCD 信息 */
    err = glxss_get_lcd_info(&s_lcd_info);
    if (err != GLXSS_OK) {
        GLXSS_ERR("Failed to get LCD info");
        return err;
    }

    s_initialized = 1;
    GLXSS_LOG("GLXSS ME ready! LCD: %dx%d", s_lcd_info.width, s_lcd_info.height);
    return GLXSS_OK;
}

/* ====================================================================
 * LVGL 集成
 * ==================================================================== */
#if defined(LV_LVGL_H_INCLUDE_SIMPLE) || defined(LVGL_H)

void glxss_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p)
{
    if (!s_initialized) {
        GLXSS_ERR("GLXSS not initialized");
        lv_disp_flush_ready(drv);
        return;
    }

    int32_t width  = lv_area_get_width(area);
    int32_t height = lv_area_get_height(area);

    /*
     * 方案 A (推荐): 影子帧缓冲 + 主循环发送
     *
     *   flush 回调只把 LVGL 的脏矩形拷贝到影子缓冲，设置 frame_pending 标志，
     *   然后立即返回。主循环中定时调用 glxss_display_frame() 发送整帧。
     *
     *   这样 USB 传输不会阻塞 LVGL 渲染管线。
     *
     *   定义影子缓冲 (建议放在 PSRAM):
     *     static uint8_t shadow_fb[640 * 400 * 4];  // RGBA8888
     *     static volatile bool frame_pending = false;
     *
     *   在 flush 回调中:
     *     // ... 颜色转换后拷贝到 shadow_fb ...
     *     frame_pending = true;
     *     lv_disp_flush_ready(drv);
     *
     *   在主循环中:
     *     if (frame_pending) {
     *         frame_pending = false;
 *             glxss_display_frame(640, 400, shadow_fb);
     *     }
     *
     * 方案 B (简单): flush 回调直接发送 (会阻塞 LVGL)
     *   适用于调试或非实时场景。
     */
    uint32_t pixel_count = (uint32_t)width * height;

#if LV_COLOR_DEPTH == 32
    /*
     * LVGL ARGB8888 小端内存布局: byte[0]=B, byte[1]=G, byte[2]=R, byte[3]=A
     * GLXSS 要求 RGBA8888:         byte[0]=R, byte[1]=G, byte[2]=B, byte[3]=A
     *
     * 转换: 交换 byte[0] ↔ byte[2]
     *
     * 如果使用方案 A (影子缓冲)，可以直接在原缓冲区修改后拷贝，
     * 或转换到影子缓冲时一并处理。
     */
    uint8_t *p = (uint8_t *)color_p;
    for (uint32_t i = 0; i < pixel_count; i++) {
        uint8_t b = p[i * 4 + 0];
        p[i * 4 + 0] = p[i * 4 + 2];   // R ← 原来的 R
        p[i * 4 + 2] = b;               // B ← 原来的 B
        p[i * 4 + 3] = 0xFF;            // A 强制不透明
    }

    /* 直接发送 (方案 B) */
    glxss_display_frame((uint16_t)width, (uint16_t)height, (uint8_t *)color_p);

#elif LV_COLOR_DEPTH == 16
    /*
     * LVGL RGB565 → GLXSS RGBA8888 转换
     *
     * 每像素从 2 字节扩展到 4 字节，需要在临时缓冲中完成。
     * 建议用 PSRAM 或静态大数组做转换缓冲区。
     *
     * RGB565 格式: rrrrrggg gggbbbbb (大端)
     * 转换:
     *   R = (r5 << 3) | (r5 >> 2)  — 5bit → 8bit
     *   G = (g6 << 2) | (g6 >> 4)  — 6bit → 8bit
     *   B = (b5 << 3) | (b5 >> 2)  — 5bit → 8bit
     *   A = 255
     */
    static uint8_t conv_buf[640 * 400 * 4] __attribute__((aligned(16)));

    uint16_t *src = (uint16_t *)color_p;
    uint8_t  *dst = conv_buf;

    for (uint32_t i = 0; i < pixel_count; i++) {
        uint16_t rgb = src[i];
        uint8_t r5 = (rgb >> 11) & 0x1F;
        uint8_t g6 = (rgb >> 5)  & 0x3F;
        uint8_t b5 = (rgb)       & 0x1F;

        dst[i * 4 + 0] = (r5 << 3) | (r5 >> 2);   // R
        dst[i * 4 + 1] = (g6 << 2) | (g6 >> 4);   // G
        dst[i * 4 + 2] = (b5 << 3) | (b5 >> 2);   // B
        dst[i * 4 + 3] = 0xFF;                      // A
    }

    glxss_display_frame((uint16_t)width, (uint16_t)height, conv_buf);

#else
    #error "Unsupported LV_COLOR_DEPTH — only 16 (RGB565) and 32 (ARGB8888) are supported"
#endif

    lv_disp_flush_ready(drv);
}

#endif /* LVGL */

/* ====================================================================
 * 使用示例
 *
 * 下面是 CH32H417 main() 中集成此驱动的参考代码。
 * ==================================================================== */
#if 0  /* 取消 #if 0 启用此示例 */

/*
 * QSPI Flash 固件读取回调
 * 固件在烧录时写入 QSPI Flash (偏移地址: 0x000000)
 */
uint32_t fw_read_callback(uint32_t offset, uint8_t *buf, uint32_t len)
{
    /* 假设外挂 W25Q128, 映射到 QSPI 地址 0x90000000 */
    /* memcpy(buf, (void *)(0x90000000 + offset), len); */
    /* return len; */
    return 0;
}

int main(void)
{
    glxss_err_t err;

    /* 初始化硬件 (时钟、GPIO、QSPI Flash 等) */
    system_init();

    /* 初始化 GLXSS ME */
    err = glxss_init(fw_read_callback, 1.5 * 1024 * 1024, 15000);

    if (err == GLXSS_OK) {
        /* 显示测试画面: 蓝色全屏 */
        /* 需要 PSRAM 或大缓冲区来存一帧 */
        /* glxss_display_frame(640, 400, blue_frame); */

        /* 或者启动 LVGL */
        /* lv_init(); */
        /* ... */
    } else {
        /* 错误处理 */
        while (1);
    }

    /* 主循环 */
    while (1) {
        /* lv_task_handler(); */
        /* delay_ms(5); */
    }
}

#endif /* 0 */
