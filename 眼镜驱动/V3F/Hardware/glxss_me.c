#include "glxss_me.h"
#include <string.h>

#define GLXSS_DEBUG
#ifdef GLXSS_DEBUG
#include "Serial.h"
#define GLXSS_LOG(fmt, ...) Serial_Printf(SERIAL_PORT_DEBUG, "[GLXSS] " fmt "\r\n", ##__VA_ARGS__)
#define GLXSS_ERR(fmt, ...) Serial_Printf(SERIAL_PORT_DEBUG, "[GLXSS_ERR] " fmt "\r\n", ##__VA_ARGS__)
#else
#define GLXSS_LOG(...)
#define GLXSS_ERR(...)
#endif

#define CTRL_LCD_INFO_bmReqType  0xA1
#define CTRL_LCD_INFO_bRequest   0x04
#define CTRL_LCD_INFO_wValue     0x0000
#define CTRL_LCD_INFO_wIndex     0x0003
#define CTRL_LCD_INFO_wLength    0x0008

#define CTRL_LCD_BRIGHT_bmReqType 0x21
#define CTRL_LCD_BRIGHT_bRequest  0x01
#define CTRL_LCD_BRIGHT_wValue    0x0D0D
#define CTRL_LCD_BRIGHT_wIndex    0x0003
#define CTRL_LCD_BRIGHT_DATA_LEN  0x01

#define IMG_HEADER_SIZE     16

static glxss_lcd_info_t s_lcd_info;
static uint8_t          s_initialized;

glxss_err_t glxss_load_firmware(glxss_fw_read_t fw_read, uint32_t fw_size)
{
    uint8_t  buf[GLXSS_FW_CHUNK_SIZE];
    uint32_t offset = 0;
    uint32_t chunk;
    glxss_err_t err;

    GLXSS_LOG("Loading firmware (%lu bytes)...", (unsigned long)fw_size);

    if (fw_read == NULL) return GLXSS_ERR_INVALID_PARAM;
    if (fw_size > GLXSS_FW_MAX_SIZE) return GLXSS_ERR_FW_TOO_LARGE;

    GLXSS_LOG("Waiting for bootloader (03E7:2150)...");
    err = glxss_usb_wait_device(GLXSS_BOOTLOADER_VID, GLXSS_BOOTLOADER_PID, 10000);
    if (err != GLXSS_OK) {
        GLXSS_ERR("Bootloader not found");
        return err;
    }

    GLXSS_LOG("Uploading firmware...");
    while (offset < fw_size) {
        chunk = fw_size - offset;
        if (chunk > GLXSS_FW_CHUNK_SIZE)
            chunk = GLXSS_FW_CHUNK_SIZE;

        if (fw_read(offset, buf, chunk) != chunk) {
            GLXSS_ERR("Firmware read failed at offset %lu", (unsigned long)offset);
            return GLXSS_ERR_FW_READ;
        }

        err = glxss_usb_bulk_write(GLXSS_EP_BOOT_FW, buf, chunk);
        if (err != GLXSS_OK) {
            GLXSS_ERR("Bulk write failed at offset %lu", (unsigned long)offset);
            return err;
        }

        offset += chunk;

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

glxss_err_t glxss_get_lcd_info(glxss_lcd_info_t *info)
{
    uint8_t  buf[8] = {0};
    glxss_err_t err;

    if (info == NULL) return GLXSS_ERR_INVALID_PARAM;

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

    info->width       = (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);
    info->height      = (uint16_t)buf[2] | ((uint16_t)buf[3] << 8);
    info->orientation = buf[4];
    info->rotation    = buf[5];
    info->brightness  = (uint16_t)buf[6] | ((uint16_t)buf[7] << 8);

    GLXSS_LOG("LCD: %dx%d orient=%d rot=%d bright=%d",
              info->width, info->height,
              info->orientation, info->rotation, info->brightness);

    s_lcd_info = *info;
    return GLXSS_OK;
}

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

glxss_err_t glxss_display_frame(uint16_t width, uint16_t height, const uint8_t *data)
{
    uint32_t pixel_bytes = (uint32_t)width * height * 4;
    glxss_err_t err;

    if (data == NULL) return GLXSS_ERR_INVALID_PARAM;
    if (width == 0 || height == 0 || width > GLXSS_MAX_WIDTH || height > GLXSS_MAX_HEIGHT)
        return GLXSS_ERR_INVALID_PARAM;

    uint8_t header[IMG_HEADER_SIZE];

    header[0]  = (uint8_t)(width & 0xFF);
    header[1]  = (uint8_t)((width >> 8) & 0xFF);
    header[2]  = (uint8_t)((width >> 16) & 0xFF);
    header[3]  = (uint8_t)((width >> 24) & 0xFF);

    header[4]  = (uint8_t)(height & 0xFF);
    header[5]  = (uint8_t)((height >> 8) & 0xFF);
    header[6]  = (uint8_t)((height >> 16) & 0xFF);
    header[7]  = (uint8_t)((height >> 24) & 0xFF);

    header[8]  = 0x01;
    header[9]  = 0x00;
    header[10] = 0x00;
    header[11] = 0x00;
    header[12] = 0x00;
    header[13] = 0x00;
    header[14] = 0x00;
    header[15] = 0x00;

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

glxss_err_t glxss_init(glxss_fw_read_t fw_read,
                       uint32_t        fw_size,
                       uint32_t        timeout_ms)
{
    glxss_err_t err;

    GLXSS_LOG("GLXSS ME init start");

    if (glxss_usb_host_init() != 0) {
        GLXSS_ERR("USB Host init failed");
        return GLXSS_ERR_ENUM;
    }
    GLXSS_LOG("USB Host init OK");


    GLXSS_LOG("Waiting for bootloader (03E7:2150)...");
    err = glxss_load_firmware(fw_read, fw_size);
    if (err != GLXSS_OK) {
        GLXSS_ERR("Firmware load failed");
        return err;
    }

    GLXSS_LOG("Waiting for device re-enumeration...");
    err = glxss_usb_wait_device(GLXSS_DEVICE_VID, GLXSS_DEVICE_PID, timeout_ms);
    if (err != GLXSS_OK) {
        GLXSS_ERR("Device not found after firmware load");
        return err;
    }

get_info:
    err = glxss_get_lcd_info(&s_lcd_info);
    if (err != GLXSS_OK) {
        GLXSS_ERR("Failed to get LCD info");
        return err;
    }

    s_initialized = 1;
    GLXSS_LOG("GLXSS ME ready! LCD: %dx%d", s_lcd_info.width, s_lcd_info.height);
    return GLXSS_OK;
}