import socket
import struct
import time
import threading
import matplotlib
matplotlib.use('TkAgg')
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from collections import deque

ESP8266_IP = "192.168.192.172"
ESP8266_PORT = 8888

FRAME_DELIM = 0x7E
ESCAPE_CHAR = 0x7D
ESCAPE_XOR = 0x20

CMD_WAVE_RAW = 0x04
CMD_WAVE_FILT = 0x10
CMD_FOCUS = 0x05


NUM_CHANNELS = 8
BUFFER_SIZE = 500

channel_data = [deque(maxlen=BUFFER_SIZE) for _ in range(NUM_CHANNELS)]
channel_colors = ['#FF6B6B', '#4ECDC4', '#45B7D1', '#96CEB4',
                  '#FFEAA7', '#DDA0DD', '#98D8C8', '#F7DC6F']
channel_names = [f"CH{i}" for i in range(NUM_CHANNELS)]
connected = False
frame_count = 0
text_lines = []


def unescape_frame(raw):
    result = bytearray()
    i = 0
    while i < len(raw):
        if raw[i] == ESCAPE_CHAR and i + 1 < len(raw):
            result.append(raw[i + 1] ^ ESCAPE_XOR)
            i += 2
        else:
            result.append(raw[i])
            i += 1
    return bytes(result)


def parse_frame(raw):
    global frame_count
    if len(raw) < 1:
        return

    cmd = raw[0] & 0xFF
    payload = raw[1:]
    load_len = len(payload)

    if cmd == CMD_WAVE_RAW or cmd == CMD_WAVE_FILT:
        if load_len == 5:
            ch = payload[0]
            val = struct.unpack_from('<f', payload, 1)[0]
            if ch < NUM_CHANNELS:
                channel_data[ch].append(val)
            frame_count += 1
        elif load_len == 10:
            chA = payload[0]
            chB = payload[1]
            valA = struct.unpack_from('<f', payload, 2)[0]
            valB = struct.unpack_from('<f', payload, 6)[0]
            if chA < NUM_CHANNELS:
                channel_data[chA].append(valA)
            if chB < NUM_CHANNELS:
                channel_data[chB].append(valB)
            frame_count += 1
        elif load_len == 8:
            valA = struct.unpack_from('<f', payload, 0)[0]
            valB = struct.unpack_from('<f', payload, 4)[0]
            channel_data[0].append(valA)
            channel_data[1].append(valB)
            frame_count += 1
    elif (0x20 <= cmd <= 0x27) or (0x30 <= cmd <= 0x37) or (0x40 <= cmd <= 0x47):
        pass
    elif cmd == CMD_FOCUS:
        if load_len == 18:
            a0, a1, e0, e1 = struct.unpack_from('<ffff', payload, 0)
            trend = payload[16]
            instant = payload[17]
            print(f"[FOCUS] attn0={a0*100:.1f}% attn1={a1*100:.1f}% ema0={e0*100:.1f}% ema1={e1*100:.1f}% trend={trend} instant={instant}")
            frame_count += 1
    else:
        hex_str = payload.hex()
        print(f"[FRAME] cmd=0x{cmd:02X} len={load_len} hex={hex_str[:40]}...")
        frame_count += 1


def tcp_client():
    global connected
    state = 0
    payload = bytearray()
    text_buf = ""

    while True:
        try:
            print(f"[TCP] Connecting to {ESP8266_IP}:{ESP8266_PORT}...")
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(5)
            sock.connect((ESP8266_IP, ESP8266_PORT))
            sock.settimeout(0.1)
            connected = True
            print(f"[TCP] Connected!")

            while True:
                try:
                    data = sock.recv(4096)
                    if not data:
                        break
                except socket.timeout:
                    continue
                except Exception:
                    break

                for b in data:
                    if state == 0:
                        if b == FRAME_DELIM:
                            state = 1
                            payload = bytearray()
                            text_buf = ""
                        elif b == 0x0A or b == 0x0D:
                            if text_buf.strip():
                                print(f"[TEXT] {text_buf.strip()}")
                                text_lines.append(text_buf.strip())
                            text_buf = ""
                        elif 0x20 <= b < 0x7F:
                            text_buf += chr(b)
                        else:
                            state = 1
                            payload = bytearray()
                            text_buf = ""
                            payload.append(b)
                    elif state == 1:
                        if b == ESCAPE_CHAR:
                            state = 2
                        elif b == FRAME_DELIM:
                            if len(payload) >= 1:
                                unescaped = unescape_frame(payload)
                                parse_frame(unescaped)
                            state = 0
                            payload = bytearray()
                        else:
                            payload.append(b)
                    elif state == 2:
                        payload.append(b ^ ESCAPE_XOR)
                        state = 1

        except Exception as e:
            print(f"[TCP] Error: {e}")
            connected = False
            time.sleep(3)


def update_plot(frame):
    for i in range(NUM_CHANNELS):
        if len(channel_data[i]) > 1:
            y = list(channel_data[i])
            x = list(range(len(y)))
            lines[i].set_data(x, y)
            ax[i].relim()
            ax[i].autoscale_view()
    status.set_text(f"Connected: {'Yes' if connected else 'No'} | Frames: {frame_count}")
    return lines + [status]


fig, ax = plt.subplots(NUM_CHANNELS, 1, figsize=(12, 8), sharex=True)
fig.suptitle("EEG Data Viewer (ESP8266 TCP)", fontsize=14, fontweight='bold')
fig.canvas.manager.set_window_title('EEG Data Viewer')

lines = []
for i in range(NUM_CHANNELS):
    line, = ax[i].plot([], [], color=channel_colors[i], linewidth=0.8)
    ax[i].set_ylabel(channel_names[i], fontsize=8, rotation=0, labelpad=25)
    ax[i].tick_params(labelsize=6)
    ax[i].grid(True, alpha=0.3)
    lines.append(line)

ax[-1].set_xlabel("Samples")
status = fig.text(0.5, 0.01, "Connecting...", ha='center', fontsize=10)
plt.tight_layout(rect=[0, 0.03, 1, 0.95])

t = threading.Thread(target=tcp_client, daemon=True)
t.start()

ani = FuncAnimation(fig, update_plot, interval=100, blit=False, cache_frame_data=False)
plt.show()