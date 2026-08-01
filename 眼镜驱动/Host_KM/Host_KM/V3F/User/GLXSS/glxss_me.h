/**
 * glxss_me.h - GLXSS ME 智能眼镜 USB 驱动 (CH32H417 移植)
 *
 * 基于 https://github.com/happyme531/glxss-client 的 Python 代码移植
 *
 * USB 协议概要:
 *   上电 → 03e7:2150 (Movidius Myriad 2 Bootloader)
 *       ↓  Bulk OUT EP1: 上传 1.5MB 固件
 *   重新枚举 → 2e09:0030 (LLVISION G25 Firmware)
 *       ↓  Control Transfer (0xA1, 0x04): 获取 LCD 信息
 *       ↓  Bulk OUT EP1: 发送 ARGB8888 图像数据
 *
 * 依赖:
 *   - CH32H417 USBHS Host 模式 (或 USBSS Host)
 *   - 外部 QSPI Flash (存 1.5MB 固件)
 *   - 外部 QSPI PSRAM (存帧缓冲, 可选但推荐)
 *   - LVGL (可选, 用于 UI 渲染)
 */

#ifndef _GLXSS_ME_H_
#define _GLXSS_ME_H_

#include <stdint.h>

/* ====================================================================
 * USB 设备标识
 * ==================================================================== */
#define GLXSS_BOOTLOADER_VID    0x03E7
#define GLXSS_BOOTLOADER_PID    0x2150
#define GLXSS_DEVICE_VID        0x2E09
#define GLXSS_DEVICE_PID        0x0030

/* ====================================================================
 * USB 端点
 * ==================================================================== */
#define GLXSS_EP_BOOT_FW        0x01    /* Bulk OUT: 上传固件到 Bootloader */
#define GLXSS_EP_DATA_OUT       0x01    /* Bulk OUT: 发送图像数据 */

/* ====================================================================
 * 固件信息
 * ==================================================================== */
#define GLXSS_FW_MAX_SIZE       (2 * 1024 * 1024)   /* 最大固件 2MB */
#define GLXSS_FW_CHUNK_SIZE     512                  /* 上传分块大小 */

/* ====================================================================
 * 最大分辨率 (安全上限)
 * ==================================================================== */
#define GLXSS_MAX_WIDTH         1280
#define GLXSS_MAX_HEIGHT        720
#define GLXSS_MAX_FRAME_SIZE    (GLXSS_MAX_WIDTH * GLXSS_MAX_HEIGHT * 4)  /* ~3.7MB */

/* ====================================================================
 * LCD 信息结构体 (从控制传输返回数据解析)
 *
 * 原始数据格式:
 *   offset 0: width      (int16 LE)
 *   offset 2: height     (int16 LE)
 *   offset 4: orientation (int8)
 *   offset 5: rotation   (int8)
 *   offset 6: brightness (int16 LE)
 * ==================================================================== */
typedef struct {
    uint16_t width;         /* 像素宽度 (通常是 640) */
    uint16_t height;        /* 像素高度 (通常是 400) */
    uint8_t  orientation;   /* 方向 */
    uint8_t  rotation;      /* 旋转 */
    uint16_t brightness;    /* 亮度 */
} glxss_lcd_info_t;

/* ====================================================================
 * 错误码
 * ==================================================================== */
typedef enum {
    GLXSS_OK                = 0,
    GLXSS_ERR_NOT_FOUND     = -1,   /* USB 设备未找到 */
    GLXSS_ERR_ENUM          = -2,   /* 设备枚举失败 */
    GLXSS_ERR_CTL_TRANSFER  = -3,   /* 控制传输失败 */
    GLXSS_ERR_BULK_WRITE    = -4,   /* 批量写入失败 */
    GLXSS_ERR_FW_TOO_LARGE  = -5,   /* 固件太大 */
    GLXSS_ERR_FW_READ       = -6,   /* 读取固件失败 */
    GLXSS_ERR_FW_TIMEOUT    = -7,   /* 固件上传超时 */
    GLXSS_ERR_NO_MEM        = -8,   /* 内存不足 */
    GLXSS_ERR_TIMEOUT       = -9,   /* 操作超时 */
    GLXSS_ERR_INVALID_PARAM = -10,  /* 参数错误 */
} glxss_err_t;

/* ====================================================================
 * 固件读取回调
 *
 * 用户实现此回调, 从存储介质读取固件数据块。
 * 由于 CH32H417 内部 Flash 仅 960KB, 放不下 1.5MB 固件,
 * 需要外挂 QSPI Flash / SD 卡等。
 *
 * @param offset  读取偏移 (字节)
 * @param buf     输出缓冲区
 * @param len     读取长度
 * @return        实际读取字节数, 0 表示错误
 * ==================================================================== */
typedef uint32_t (*glxss_fw_read_t)(uint32_t offset, uint8_t *buf, uint32_t len);

/* ====================================================================
 * USB Host 底层操作 (需要适配 CH32H417 的 USB 库)
 *
 * 这些函数需要对接 WCH 的 USBHS/USBSS Host 库。
 * 见 glxss_me_port/README.md 中的适配说明。
 * ==================================================================== */

/**
 * @brief 初始化 USB Host 模式
 * @return 0 = 成功, 非0 = 失败
 */
int glxss_usb_host_init(void);

/**
 * @brief 等待 USB 设备连接并枚举
 * @param vid  期望的 VID (0 = 不检查)
 * @param pid  期望的 PID (0 = 不检查)
 * @param timeout_ms  超时毫秒
 * @return GLXSS_OK 或错误码
 */
glxss_err_t glxss_usb_wait_device(uint16_t vid, uint16_t pid, uint32_t timeout_ms);

/**
 * @brief USB 控制传输 (对应 Python 的 device.ctrl_transfer)
 * @param bmRequestType  请求类型
 * @param bRequest       请求码
 * @param wValue         值
 * @param wIndex         索引
 * @param data           数据缓冲区
 * @param wLength        数据长度
 * @return GLXSS_OK 或错误码
 */
glxss_err_t glxss_usb_ctrl_transfer(uint8_t  bmRequestType,
                                    uint8_t  bRequest,
                                    uint16_t wValue,
                                    uint16_t wIndex,
                                    uint8_t *data,
                                    uint16_t wLength);

/**
 * @brief USB Bulk 写入 (对应 Python 的 device.write)
 * @param ep    端点地址
 * @param data  数据缓冲区
 * @param len   数据长度
 * @return GLXSS_OK 或错误码
 */
glxss_err_t glxss_usb_bulk_write(uint8_t ep, const uint8_t *data, uint32_t len);

/* ====================================================================
 * 高级 API (GLXSS ME 协议层)
 * ==================================================================== */

/**
 * @brief 完整初始化流程: 找 Bootloader → 上传固件 → 等待设备
 *
 * 调用此函数前需要:
 *   1. 已调用 glxss_usb_host_init()
 *   2. 已注册固件读取回调 (通过 glxss_load_firmware 或提前设置)
 *
 * @param fw_read  固件读取回调 (从 QSPI Flash/SD卡 读数据)
 * @param fw_size  固件总大小 (字节)
 * @param timeout_ms  等待设备超时
 * @return GLXSS_OK 或错误码
 */
glxss_err_t glxss_init(glxss_fw_read_t fw_read,
                       uint32_t        fw_size,
                       uint32_t        timeout_ms);

/**
 * @brief 直接上传固件到 Bootloader (不等待后续设备)
 *
 * 用在需要分段控制的场景。
 *
 * @param fw_read  固件读取回调
 * @param fw_size  固件总大小
 * @return GLXSS_OK 或错误码
 */
glxss_err_t glxss_load_firmware(glxss_fw_read_t fw_read, uint32_t fw_size);

/**
 * @brief 获取 LCD 显示信息 (分辨率、亮度等)
 * @param info  输出结构体
 * @return GLXSS_OK 或错误码
 */
glxss_err_t glxss_get_lcd_info(glxss_lcd_info_t *info);

/**
 * @brief 发送一帧图像到眼镜显示
 *
 * @param width   图像宽度
 * @param height  图像高度
 * @param data    RGBA8888 像素数据 (w * h * 4 字节)
 * @return GLXSS_OK 或错误码
 */
glxss_err_t glxss_display_frame(uint16_t width, uint16_t height, const uint8_t *data);

/**
 * @brief 设置屏幕亮度
 * @param brightness  亮度值 (0-255)
 * @return GLXSS_OK 或错误码
 */
glxss_err_t glxss_set_brightness(uint16_t brightness);

/* ====================================================================
 * LVGL 集成 (可选)
 * ==================================================================== */
#if defined(LV_LVGL_H_INCLUDE_SIMPLE) || defined(LVGL_H)

/**
 * @brief LVGL flush 回调
 *
 * 在 lv_disp_drv_t 中注册此回调, LVGL 渲染完成后自动发送到眼镜。
 *
 * 用法:
 *   static lv_disp_drv_t disp_drv;
 *   lv_disp_drv_init(&disp_drv);
 *   disp_drv.flush_cb = glxss_lvgl_flush_cb;
 *   disp_drv.draw_buf = &draw_buf;
 *   disp_drv.hor_res  = 640;
 *   disp_drv.ver_res  = 400;
 *   lv_disp_drv_register(&disp_drv);
 */
void glxss_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p);

#endif /* LVGL */

#endif /* _GLXSS_ME_H_ */
