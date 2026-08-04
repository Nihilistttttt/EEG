# -*- coding: utf-8 -*-
"""
GLXSS AR眼镜 SD卡固件烧录 + 控制工具
直接修改下方参数后运行: python glxss_sd_control.py
MCU烧录模式: 先等PC连接(发0xAA), 收到大小后才初始化SD卡
"""

import os
import struct
import time

try:
    import serial
except ImportError:
    print("请安装 pyserial: pip install pyserial")
    exit(1)

# ==================== 参数设置 ====================
ACTION      = "burn"        # burn / bright / power / display
SERIAL_PORT = "COM7"
SERIAL_BAUD = 2000000
FW_PATH     = r"D:\Libraries\Projects\EEG\眼镜驱动\G25_MainFW_1.9.2.decrypted.img"
BRIGHT_VAL  = 50            # 0-100
POWER_ON    = True          # True=on, False=off
DISPLAY_VAL = 0             # 0=off, 1=on
# ================================================

SECTOR_SIZE = 512


def wait_byte(ser, expected, label):
    text_buf = bytearray()
    while True:
        b = ser.read(1)
        if not b:
            continue
        v = b[0]
        if v == expected:
            if text_buf:
                try:
                    print(text_buf.decode("utf-8", errors="replace"), end="", flush=True)
                except Exception:
                    pass
            return True
        if 0xE0 <= v <= 0xEF:
            if text_buf:
                try:
                    print(text_buf.decode("utf-8", errors="replace"), end="", flush=True)
                except Exception:
                    pass
            print(f"\n[{label}] MCU错误: 0x{v:02X}")
            return False
        text_buf.append(v)
        if len(text_buf) >= 256:
            try:
                print(text_buf.decode("utf-8", errors="replace"), end="", flush=True)
            except Exception:
                pass
            text_buf = bytearray()


def burn_firmware(ser, fw_path):
    if not os.path.exists(fw_path):
        print(f"固件文件不存在: {fw_path}")
        return False

    with open(fw_path, "rb") as f:
        fw_data = f.read()

    fw_size = len(fw_data)
    total_sectors = (fw_size + SECTOR_SIZE - 1) // SECTOR_SIZE
    print(f"[烧录] 固件: {os.path.basename(fw_path)}")
    print(f"[烧录] 大小: {fw_size} 字节 ({fw_size/1024:.1f} KB), {total_sectors} 扇区")

    print("[烧录] 等待MCU就绪(0xAA)...")
    if not wait_byte(ser, 0xAA, "READY"):
        print("[烧录] MCU未就绪")
        return False
    print("[烧录] MCU已就绪，发送固件大小...")

    ser.write(struct.pack('<I', fw_size))

    if not wait_byte(ser, 0xBB, "SIZE_ACK"):
        print("[烧录] 未收到 SIZE_ACK")
        return False

    print("[烧录] 等待SD卡就绪(0xCC)...")
    if not wait_byte(ser, 0xCC, "SD_READY"):
        print("[烧录] SD卡初始化失败")
        return False

    print(f"[烧录] 开始写入 {total_sectors} 扇区...")
    t_start = time.time()

    for sec in range(total_sectors):
        offset = sec * SECTOR_SIZE
        chunk = fw_data[offset:offset + SECTOR_SIZE]
        if len(chunk) < SECTOR_SIZE:
            chunk = chunk + b'\xFF' * (SECTOR_SIZE - len(chunk))

        ser.write(chunk)

        if not wait_byte(ser, 0xEE, f"SEC_{sec}"):
            print(f"\n[烧录] 扇区 {sec} 写入失败")
            return False

        if (sec + 1) % 100 == 0 or sec == total_sectors - 1:
            pct = (sec + 1) * 100 // total_sectors
            elapsed = time.time() - t_start
            print(f"\r  [进度] {pct}% ({sec+1}/{total_sectors} 扇区) {elapsed:.1f}s", end="", flush=True)

    print()
    if not wait_byte(ser, 0xFF, "ALL_DONE"):
        print("[烧录] 未收到 ALL_DONE")
        return False

    elapsed = time.time() - t_start
    print(f"[烧录] 完成! {fw_size/1024:.1f} KB, 耗时 {elapsed:.1f}s")
    return True


def main():
    try:
        ser = serial.Serial()
        ser.port = SERIAL_PORT
        ser.baudrate = SERIAL_BAUD
        ser.dtr = False
        ser.rts = False
        ser.timeout = None
        ser.open()
    except Exception as e:
        print(f"无法打开 {SERIAL_PORT}: {e}")
        return
    print(f"[工具] {SERIAL_PORT} @ {SERIAL_BAUD} baud 已连接")

    ser.reset_input_buffer()
    ser.reset_output_buffer()

    if ACTION == "burn":
        burn_firmware(ser, FW_PATH)
    elif ACTION == "bright":
        print(f"[控制] 亮度: {BRIGHT_VAL} (待MCU实现)")
    elif ACTION == "power":
        print(f"[控制] 电源: {'on' if POWER_ON else 'off'} (待MCU实现)")
    elif ACTION == "display":
        print(f"[控制] 显示: {DISPLAY_VAL} (待MCU实现)")
    else:
        print(f"未知 ACTION: {ACTION}")

    ser.close()


if __name__ == "__main__":
    main()
