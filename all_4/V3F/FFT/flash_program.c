#include "flash_program.h"
#include "W25Q64.h"
#include "Serial.h"
#include "debug.h"
#include "OLED.h"
#include <string.h>

#define PAGE_SIZE 256                // W25Q64 每页 256 字节
#define SECTOR_SIZE 4096             // 一个扇区 4KB

static uint8_t page_buf[PAGE_SIZE];  // 数据接收缓冲区

/**
 * @brief  擦除从地址 0 开始的若干扇区
 * @param  total_bytes: 需要擦除的字节总数（会被向上对齐到扇区边界）
 * @retval 无
 * @note   循环擦除，每次擦除 4KB 块，直到覆盖请求的所有字节。
 *         擦除前会自动等待忙（W25Q64_SectorErase 内部已调用 W25Q64_WaitBusy）。
 */
static void Flash_Erase_Blocks (uint32_t total_bytes) {
    uint32_t addr = 0;
    while (addr < total_bytes) {
        W25Q64_SectorErase (addr);  // 擦除一个扇区
        addr += SECTOR_SIZE;        // 移动到下一扇区起始地址
    }
}

/**
 * @brief  Flash 烧录模式主函数
 * @note   通过串口与上位机（Python）握手，接收待烧录数据并写入 W25Q64。
 *         支持动态数据长度、擦除范围自动对齐、实时进度显示。
 *         烧录完成后在 OLED 上显示 DONE 并循环发送结束信息。
 */
void Flash_Program_Mode (void) {
    /* ========== 1. 硬件初始化 ========== */
    W25Q64_Init();                              // 初始化 SPI Flash
    Serial_Init(SERIAL_PORT_DEBUG);                              // 初始化串口（USART1，与 ESP8266/PC 通信）
    OLED_Clear (SPI);                           // 清 SPI OLED 屏幕
    OLED_ShowString (SPI, 0, 0, "Burning...");  // 显示起始提示

    /* ========== 2. 握手：等待上位机发送开始命令 ========== */
    while (1) {
        if (Serial_IsDataReady(SERIAL_PORT_DEBUG)) {  // 检查串口是否有数据到达
            uint8_t *buf;
            uint16_t len = Serial_GetDataPacket (SERIAL_PORT_DEBUG,&buf);
            // 如果接收到的第一个字节是 0xAA，表示上位机请求开始
            if (len >= 1 && buf[0] == 0xAA)
                break;
        }
    }

    // 回复 0xBB，表明 MCU 已准备好接收数据
    Serial_SendByte (SERIAL_PORT_DEBUG,0xBB);

    /* ========== 3. 接收数据总长度（4 字节，小端模式） ========== */
    uint32_t total_data_bytes = 0;
    uint8_t len_buf[4];  // 用于存放长度字节
    uint16_t idx = 0;    // 已接收字节数

    while (idx < 4) {    // 循环直到收满 4 字节
        if (Serial_IsDataReady(SERIAL_PORT_DEBUG)) {
            uint8_t *buf;
            uint16_t len = Serial_GetDataPacket (SERIAL_PORT_DEBUG,&buf);
            // 将最新收到的字节逐个拷贝到 len_buf
            for (uint16_t i = 0; i < len && idx < 4; i++) {
                len_buf[idx++] = buf[i];
            }
        }
    }

    /* 将 4 字节小端数据还原为 32 位无符号整数 */
    total_data_bytes = len_buf[0] | (len_buf[1] << 8) | (len_buf[2] << 16) | (len_buf[3] << 24);


    /* ========== 4. 擦除 Flash 所需扇区 ========== */
    OLED_Clear (SPI);
    OLED_ShowString (SPI, 0, 0, "Erasing...");

    /*
     * 向上取整算法：将 total_data_bytes 对齐到扇区大小的整数倍。
     * 公式：(x + N - 1) / N * N 是经典的向上取整写法。
     * 例如：
     *   total_data_bytes = 128000 (125 页)
     *   (128000 + 4095) / 4096 = 132095 / 4096 = 32（整除）
     *   32 * 4096 = 131072 字节，即 32 个扇区。
     */
    uint32_t erase_bytes = ((total_data_bytes + SECTOR_SIZE - 1) / SECTOR_SIZE) * SECTOR_SIZE;
    Flash_Erase_Blocks (erase_bytes);  // 执行擦除
    Serial_SendByte (SERIAL_PORT_DEBUG,0xDD);
    /* ========== 5. 计算总页数并显示 ========== */
    // 同样使用向上取整计算需要的页数
    uint32_t total_pages = (total_data_bytes + PAGE_SIZE - 1) / PAGE_SIZE;

    char info[30];
    // 注意：原文此处有 bug，info 未赋值就显示，应改为显示页数。
    // 修正：用 sprintf 把页数写入 info
    sprintf (info, "%u", total_pages);

    OLED_Clear (SPI);
    OLED_ShowString (SPI, 2, 0, "Pages: ");
    OLED_ShowString (SPI, 2, 7, info);  // 显示总页数
    OLED_ShowString (SPI, 3, 0, "Program...");

    /* ========== 6. 逐页接收并烧录 ========== */
    uint32_t addr = 0;     // Flash 目标地址
    uint8_t progress = 0;  // 上次显示的进度百分比

    OLED_Clear (SPI);      // 清屏准备显示进度
    for (uint32_t page = 0; page < total_pages; page++) {

        /* ---- 6.1 接收一页 (256 字节) ---- */
        uint32_t received = 0;  // 本页已接收字节数
        while (received < PAGE_SIZE) {
            if (Serial_IsDataReady(SERIAL_PORT_DEBUG)) {
                uint8_t *buf;
                uint16_t len = Serial_GetDataPacket (SERIAL_PORT_DEBUG,&buf);
                // 拷贝数据，但不能超过一页
                uint16_t copy = (received + len > PAGE_SIZE) ? (PAGE_SIZE - received) : len;
                memcpy (page_buf + received, buf, copy);
                received += copy;
            }
        }

        /* ---- 6.2 写入 Flash ---- */
        W25Q64_PageProgram (addr, page_buf, PAGE_SIZE);  // 页编程，内部自动等待忙
        addr += PAGE_SIZE;                               // 地址递增

        // 发送 0xCC 告知上位机本页写入成功
        Serial_SendByte (SERIAL_PORT_DEBUG,0xEE);

        /* ---- 6.3 更新进度 ---- */
        uint8_t new_progress = (uint8_t)((page + 1) * 100 / total_pages);
        if (new_progress != progress) {  // 仅在百分比变化时刷新
            progress = new_progress;
            OLED_ShowString (SPI, 3, 0, "Progress:    %");
            OLED_ShowNum (SPI, 3, 10, progress, 3);
        }
    }

    /* ========== 7. 烧录完成 ========== */
    OLED_Clear (SPI);
    OLED_ShowString (SPI, 0, 0, "DONE");
    while (1) {
        Serial_SendString (SERIAL_PORT_DEBUG,"DONE\r\n");  // 持续通知上位机已完成
        Delay_Ms (1000);
    }
}

/* python代码留档
import numpy as np
import mne
from mne.datasets import eegbci
from scipy.signal import resample
import serial
import struct
import time
import matplotlib.pyplot as plt

# ========== 模式开关 ==========
# 改成 'flash' 为烧录模式，'plot' 为电脑端显示波形对比
MODE = 'plot'  # 也可以使用 input("Enter mode (flash/plot): ") 动态选择

# ---------- 公共部分：加载并重采样数据 ----------
print("Loading EEG data...")
raw_fnames = eegbci.load_data(subjects=1, runs=[3])
raw = mne.io.read_raw_edf(raw_fnames[0], preload=True)
mne.datasets.eegbci.standardize(raw)

ch0_data = raw.get_data(picks='Fp1')[0]
ch1_data = raw.get_data(picks='Fp2')[0]
sfreq_orig = raw.info['sfreq']  # 160 Hz

# 重采样到 128 Hz
fs_target = 128
n = int(len(ch0_data) * fs_target / sfreq_orig)
eeg_fp1 = resample(ch0_data, n).astype(np.float32)
eeg_fp2 = resample(ch1_data, n).astype(np.float32)
print(f"Original samples: {len(ch0_data)}, resampled to: {n} at {fs_target} Hz")

if MODE == 'plot':
    # ---------- 电脑端显示波形对比 ----------
    # 截取前 10 秒数据用于显示（可按需修改）
    plot_duration = 5.0
    plot_samples = int(plot_duration * fs_target)
    if plot_samples > n:
        plot_samples = n

    time_axis = np.arange(plot_samples) / fs_target
    plt.figure(figsize=(12, 6))
    plt.plot(time_axis, eeg_fp1[:plot_samples], label='Fp1', alpha=0.8)
    # plt.plot(time_axis, eeg_fp2[:plot_samples], label='Fp2', alpha=0.8)
    plt.xlabel('Time (s)')
    plt.ylabel('Amplitude')
    plt.title('EEG Waveform Comparison (Fp1 vs Fp2)')
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()
    print("Waveform plot displayed.")
elif MODE == 'flash':
    # 全量真实数据，不拼贴不重复
    data_bytes = b''.join(struct.pack('<ff', ch0, ch1) for ch0, ch1 in zip(eeg_fp1, eeg_fp2))
    print(f"Real data size: {len(data_bytes)} bytes ({len(eeg_fp1)} sample pairs)")

    # 如果超过 7.2 MB 就截断，否则全烧
    MAX_BYTES = 900000 * 8
    if len(data_bytes) > MAX_BYTES:
        print(f"Warning: data too large ({len(data_bytes)} > {MAX_BYTES}), truncating.")
        data_bytes = data_bytes[:MAX_BYTES]
    # 不足就不再填零，Flash 擦除后就是 0xFF

    SERIAL_PORT = 'COM20'
    BAUDRATE = 2000000

    ser = serial.Serial(SERIAL_PORT, BAUDRATE, timeout=2)
    time.sleep(2)

    print("Waiting for MCU to be ready...")
    # 发送开始命令
    ser.write(b'\xAA')

    # 等待 MCU 回复准备好信号 (0xBB)
    while True:
        b = ser.read(1)
        if b == b'\xBB':
            print("MCU ready")
            break
        if b:
            print(f"Unexpected byte: {b}")

    # ***** 新增：发送数据总长度（4字节小端）*****
    total_bytes = len(data_bytes)
    ser.write(struct.pack('<I', total_bytes))
    print(f"Sent data length: {total_bytes} bytes")
    # ***** 等待 MCU 确认（可再收一次 0xBB）*****
    ack = ser.read(1)
    if ack != b'\xBB':
        print(f"Length ack error: {ack}")
        exit()

    # 按页发送数据
    page_size = 256
    total_pages = (total_bytes + page_size - 1) // page_size
    print(f"Total pages: {total_pages}")

    for i in range(total_pages):
        offset = i * page_size
        chunk = data_bytes[offset:offset + page_size]
        # 不满一页时用零填充(由MCU根据总长度判断)
        if len(chunk) < page_size:
            chunk = chunk + b'\x00' * (page_size - len(chunk))
        ser.write(chunk)
        # 等待 MCU 确认 (0xCC)
        ack = ser.read(1)
        if ack != b'\xCC':
            print(f"Error at page {i}, ack = {ack}")
            break
        if i % 10 == 0 or i == total_pages - 1:
            print(f"Sent {i + 1}/{total_pages} pages")

    print("Flashing complete!")
    ser.close()
*/