# -*- coding: utf-8 -*-
# GLXSS 固件上传工具  v3.2 (有线透传版)
# ESP8266 USB 连电脑, D7/D8 连 CH32
# 用法: 双击运行, 自动连 COM8

import sys
import time
import os
try:
    import serial
except ImportError:
    input("请安装 pyserial: pip install pyserial\n按回车退出...")
    sys.exit(1)

PORT = "COM8"
BAUD = 115200
FW_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                       "G25_MainFW_1.9.2.decrypted.img")
CHUNK_SIZE = 512


def main():
    if not os.path.exists(FW_PATH):
        input(f"固件文件不存在: {FW_PATH}\n按回车退出...")
        return

    with open(FW_PATH, "rb") as f:
        fw_data = f.read()
    print(f"[工具] 固件: {os.path.basename(FW_PATH)} ({len(fw_data)/1024:.1f} KB)")

    try:
        ser = serial.Serial(PORT, BAUD, timeout=3)
    except Exception as e:
        input(f"[错误] {e}\n请关掉串口助手再试\n按回车退出...")
        return

    print(f"[工具] {PORT} ({BAUD} baud) 已连接, 等待 FW_READY...")
    print("-" * 50)

    buf = b""
    sent = 0
    uploading = False

    try:
        while True:
            data = ser.read(1024)
            if data:
                print(data.decode(errors="replace"), end="", flush=True)
                buf += data

                if not uploading and b"FW_READY" in buf:
                    uploading = True
                    sent = 0
                    buf = b""
                    print(f"\n[工具] 检测到 FW_READY, 发送固件...")
                    ser.write(fw_data[0:CHUNK_SIZE])
                    sent = CHUNK_SIZE

            if uploading and sent < len(fw_data):
                idx = buf.find(b".")
                if idx >= 0:
                    buf = buf[idx + 1:]
                    chunk = fw_data[sent:sent + CHUNK_SIZE]
                    ser.write(chunk)
                    sent += len(chunk)
                    if sent % (50 * CHUNK_SIZE) == 0:
                        pct = sent * 100 // len(fw_data)
                        print(f"\r  [进度] {pct}% ({sent//1024} KB / {len(fw_data)//1024} KB)",
                              end="", flush=True)

            if uploading and sent >= len(fw_data):
                print(f"\r  [进度] 100% ({sent//1024} KB / {len(fw_data)//1024} KB)")
                print("[工具] 上传完成!")
                uploading = False

            time.sleep(0.01)

    except KeyboardInterrupt:
        print("\n[工具] 中断")
    except Exception as e:
        print(f"\n[错误] {e}")
    finally:
        ser.close()
        input("按回车退出...")


if __name__ == "__main__":
    main()
