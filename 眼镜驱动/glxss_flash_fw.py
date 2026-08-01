# -*- coding: utf-8 -*-
"""
GLXSS 固件 W25Q64 烧录工具 v1.0
协议与 flash_program.c (glxss模式) 对应:
  PC → 4字节固件大小(LE) → MCU
  MCU → 0xBB (已收到大小)
  MCU → 0xDD (擦除完成)
  循环每页:
    PC → 256字节 → MCU
    MCU → 0xEE (页写入完成)
  MCU → 0xFF (全部完成)

用法: python glxss_flash_fw.py COMx [firmware.img]
"""

import sys
import os
import struct
import time

try:
    import serial
except ImportError:
    input("请安装 pyserial: pip install pyserial\n按回车退出...")
    sys.exit(1)

# ==================== 配置区 ====================
PORT = "COM9"
BAUD = 2000000
FW_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                       "G25_MainFW_1.9.2.decrypted.img")
# ================================================

PAGE_SIZE = 256
TIMEOUT = 30


def wait_ack(ser, expected, label, timeout=TIMEOUT):
    t0 = time.time()
    while time.time() - t0 < timeout:
        b = ser.read(1)
        if b:
            if b[0] == expected:
                return True
            else:
                print(f"\n[{label}] 期望 0x{expected:02X}, 收到 0x{b[0]:02X}")
                return False
    print(f"\n[{label}] 超时 ({timeout}s)")
    return False


def main():
    port = PORT
    baud = BAUD
    fw_path = FW_PATH

    if not os.path.exists(fw_path):
        input(f"固件文件不存在: {fw_path}\n按回车退出...")
        return

    with open(fw_path, "rb") as f:
        fw_data = f.read()

    fw_size = len(fw_data)
    total_pages = (fw_size + PAGE_SIZE - 1) // PAGE_SIZE
    print(f"[工具] 固件: {os.path.basename(fw_path)}")
    print(f"[工具] 大小: {fw_size} 字节 ({fw_size/1024:.1f} KB), {total_pages} 页")

    try:
        ser = serial.Serial(port, baud, timeout=3)
    except Exception as e:
        input(f"[错误] 无法打开 {port}: {e}\n按回车退出...")
        return

    print(f"[工具] {port} @ {baud} baud 已连接")
    ser.reset_input_buffer()
    ser.reset_output_buffer()

    try:
        # 1. 发送固件大小 (4字节 LE)
        print("[工具] 发送固件大小...")
        ser.write(struct.pack('<I', fw_size))

        # 2. 等待 0xBB (MCU已收到大小)
        if not wait_ack(ser, 0xBB, "SIZE_ACK"):
            raise Exception("未收到 SIZE_ACK (0xBB)")

        # 3. 等待 0xDD (擦除完成)
        print("[工具] 等待擦除完成...")
        if not wait_ack(ser, 0xDD, "ERASE_DONE", timeout=60):
            raise Exception("未收到 ERASE_DONE (0xDD)")

        # 4. 逐页发送
        print(f"[工具] 开始烧录 {total_pages} 页...")
        t_start = time.time()

        for page in range(total_pages):
            offset = page * PAGE_SIZE
            chunk = fw_data[offset:offset + PAGE_SIZE]
            if len(chunk) < PAGE_SIZE:
                chunk = chunk + b'\xFF' * (PAGE_SIZE - len(chunk))

            ser.write(chunk)

            if not wait_ack(ser, 0xEE, f"PAGE_{page}", timeout=5):
                raise Exception(f"页 {page} 写入失败")

            if (page + 1) % 100 == 0 or page == total_pages - 1:
                pct = (page + 1) * 100 // total_pages
                elapsed = time.time() - t_start
                print(f"\r  [进度] {pct}% ({page+1}/{total_pages} 页) {elapsed:.1f}s", end="", flush=True)

        # 5. 等待 0xFF (全部完成)
        print()
        if not wait_ack(ser, 0xFF, "ALL_DONE"):
            raise Exception("未收到 ALL_DONE (0xFF)")

        elapsed = time.time() - t_start
        print(f"\n[工具] ===== 烧录完成! {fw_size/1024:.1f} KB, 耗时 {elapsed:.1f}s =====")
        print("[工具] 将 FLASH_MODE 改为 0 重新编译即可启动眼镜")

    except Exception as e:
        print(f"\n[错误] {e}")
    finally:
        ser.close()


if __name__ == "__main__":
    main()