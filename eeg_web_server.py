#!/usr/bin/env python3
"""NeuroWave Web Bridge - WebSocket<->TCP bridge for EEG data"""

import asyncio
import json
import struct
import os
import sys
import threading
from http.server import HTTPServer, SimpleHTTPRequestHandler

try:
    import websockets
except ImportError:
    import subprocess
    subprocess.check_call([sys.executable, "-m", "pip", "install", "websockets"])
    import websockets

WS_PORT = 8765
HTTP_PORT = 8088
DEFAULT_TCP_IP = "192.168.192.172"
DEFAULT_TCP_PORT = 8888

FRAME_DELIM = 0x7E
ESCAPE_CHAR = 0x7D
ESCAPE_XOR = 0x20

CMD_WAVE_RAW = 0x04
CMD_WAVE_FILT = 0x10
CMD_WAVE_BASELINE = 0x11
CMD_FOCUS = 0x05



class SpectrumReassembler:
    def __init__(self):
        self.fragments = {}

    def add(self, cmd, frag_idx, total_frags, mags):
        key = cmd
        if key not in self.fragments:
            self.fragments[key] = {"total": total_frags, "data": {}}
        self.fragments[key]["data"][frag_idx] = mags
        info = self.fragments[key]
        if len(info["data"]) >= info["total"]:
            all_mags = []
            for i in range(info["total"]):
                if i in info["data"]:
                    all_mags.extend(info["data"][i])
            del self.fragments[key]
            if all_mags:
                return all_mags
        return None


class FrameParser:
    def __init__(self):
        self.state = 0
        self.buf = bytearray()
        self.text_buf = ""
        self.spectrum = SpectrumReassembler()

    def feed(self, data):
        results = []
        for b in data:
            if self.state == 0:
                if b == FRAME_DELIM:
                    self.state = 1
                    self.buf = bytearray()
                    self.text_buf = ""
                elif b == 0x0A or b == 0x0D:
                    if self.text_buf.strip():
                        results.append({"type": "text", "data": self.text_buf.strip()})
                    self.text_buf = ""
                elif 0x20 <= b < 0x7F:
                    self.text_buf += chr(b)
                else:
                    self.state = 1
                    self.buf = bytearray()
                    self.text_buf = ""
                    self.buf.append(b)
            elif self.state == 1:
                if b == ESCAPE_CHAR:
                    self.state = 2
                elif b == FRAME_DELIM:
                    if self.buf:
                        parsed = self._parse_frame(self.buf)
                        if parsed:
                            if isinstance(parsed, list):
                                results.extend(parsed)
                            else:
                                results.append(parsed)
                    self.state = 0
                    self.buf = bytearray()
                else:
                    self.buf.append(b)
            elif self.state == 2:
                self.buf.append(b ^ ESCAPE_XOR)
                self.state = 1
        return results

    def _parse_frame(self, raw):
        if len(raw) < 1:
            return None
        cmd = raw[0] & 0xFF
        p = raw[1:]
        load_len = len(p)
        if cmd in (CMD_WAVE_RAW, CMD_WAVE_FILT, CMD_WAVE_BASELINE):
            return self._parse_wave(cmd, p, load_len)
        elif cmd == CMD_FOCUS:
            return self._parse_focus(p, load_len)
        elif self._is_spectrum_cmd(cmd):
            return self._parse_spectrum(cmd, p, load_len)
        else:
            return {"type": "raw", "cmd": cmd, "len": load_len, "hex": p[:20].hex()}

    @staticmethod
    def _is_spectrum_cmd(cmd):
        return (0x20 <= cmd <= 0x27) or (0x30 <= cmd <= 0x37) or (0x40 <= cmd <= 0x47)

    def _parse_wave(self, cmd, p, load_len):
        if load_len == 5:
            return {"type": "wave", "cmd": cmd, "ch": p[0], "val": struct.unpack_from('<f', p, 1)[0]}
        elif load_len == 10:
            return {"type": "wave_multi", "cmd": cmd, "channels": [
                {"ch": p[0], "val": struct.unpack_from('<f', p, 2)[0]},
                {"ch": p[1], "val": struct.unpack_from('<f', p, 6)[0]}
            ]}
        elif load_len == 8:
            return {"type": "wave_multi", "cmd": cmd, "channels": [
                {"ch": 0, "val": struct.unpack_from('<f', p, 0)[0]},
                {"ch": 1, "val": struct.unpack_from('<f', p, 4)[0]}
            ]}
        return None

    def _parse_focus(self, p, load_len):
        if load_len == 18:
            a0, a1, e0, e1 = struct.unpack_from('<ffff', p, 0)
            print(f"[FOCUS] hex={p.hex()} a0={a0:.4f} a1={a1:.4f} e0={e0:.4f} e1={e1:.4f} trend={p[16]} instant={p[17]}")
            return {"type": "focus", "attn0": round(a0, 4), "attn1": round(a1, 4),
                    "ema0": round(e0, 4), "ema1": round(e1, 4),
                    "trend": p[16], "instant": p[17]}
        else:
            print(f"[FOCUS] unexpected load_len={load_len} hex={p[:20].hex()}")
        return None

    def _parse_spectrum(self, cmd, p, load_len):
        if load_len >= 2:
            frag_idx = p[0] & 0xFF
            total = p[1] & 0xFF
            data_len = load_len - 2
            if data_len == 16:
                mags = []
                for i in range(4):
                    mags.append(struct.unpack_from('<f', p, 2 + i * 4)[0])
                complete = self.spectrum.add(cmd, frag_idx, total, mags)
                if complete:
                    print(f"[SPEC] cmd=0x{cmd:02X} complete! bins={len(complete)} first4={[round(v,6) for v in complete[:4]]}")
                    return {"type": "spectrum", "cmd": cmd, "mags": [round(v, 6) for v in complete]}
            else:
                print(f"[SPEC] cmd=0x{cmd:02X} frag={frag_idx}/{total} dataLen={data_len} (expected 16)")
        return None


class BridgeServer:
    def __init__(self):
        self.parser = FrameParser()
        self.tcp_reader = None
        self.tcp_writer = None
        self.tcp_connected = False
        self.tcp_ip = DEFAULT_TCP_IP
        self.tcp_port = DEFAULT_TCP_PORT
        self.ws_clients = set()

    async def tcp_connect(self, ip=None, port=None):
        self.tcp_ip = ip or self.tcp_ip
        self.tcp_port = port or self.tcp_port
        try:
            self.tcp_reader, self.tcp_writer = await asyncio.wait_for(
                asyncio.open_connection(self.tcp_ip, self.tcp_port), timeout=5)
            self.tcp_connected = True
            self.parser = FrameParser()
            await self._broadcast({"type": "status", "connected": True, "ip": self.tcp_ip, "port": self.tcp_port})
            print(f"[TCP] Connected to {self.tcp_ip}:{self.tcp_port}")
            return True
        except Exception as e:
            self.tcp_connected = False
            await self._broadcast({"type": "status", "connected": False, "error": str(e)})
            print(f"[TCP] Connection failed: {e}")
            return False

    async def tcp_disconnect(self):
        if self.tcp_writer:
            try:
                self.tcp_writer.close()
                await self.tcp_writer.wait_closed()
            except:
                pass
        self.tcp_reader = None
        self.tcp_writer = None
        self.tcp_connected = False
        await self._broadcast({"type": "status", "connected": False})

    async def tcp_read_loop(self):
        try:
            while self.tcp_connected and self.tcp_reader:
                try:
                    data = await asyncio.wait_for(self.tcp_reader.read(4096), timeout=10)
                except asyncio.TimeoutError:
                    continue
                if not data:
                    break
                hex_preview = data[:64].hex()
                await self._broadcast({"type": "raw_hex", "hex": hex_preview, "len": len(data)})
                messages = self.parser.feed(data)
                for msg in messages:
                    await self._broadcast(msg)
        except Exception as e:
            print(f"[TCP] Read error: {e}")
        finally:
            await self.tcp_disconnect()

    async def tcp_send(self, text):
        if self.tcp_writer and self.tcp_connected:
            try:
                self.tcp_writer.write((text + '\n').encode())
                await self.tcp_writer.drain()
                print(f"[TCP] Sent: {text}")
            except Exception as e:
                print(f"[TCP] Send error: {e}")

    async def _broadcast(self, msg):
        if not self.ws_clients:
            return
        data = json.dumps(msg, ensure_ascii=False)
        disconnected = set()
        for ws in self.ws_clients:
            try:
                await ws.send(data)
            except:
                disconnected.add(ws)
        self.ws_clients -= disconnected

    async def handle_ws(self, websocket):
        self.ws_clients.add(websocket)
        await websocket.send(json.dumps({
            "type": "status", "connected": self.tcp_connected,
            "ip": self.tcp_ip, "port": self.tcp_port
        }))
        try:
            async for raw in websocket:
                try:
                    msg = json.loads(raw)
                    mtype = msg.get("type")
                    if mtype == "connect":
                        if self.tcp_connected:
                            await self.tcp_disconnect()
                        ok = await self.tcp_connect(msg.get("ip"), msg.get("port"))
                        if ok:
                            asyncio.create_task(self.tcp_read_loop())
                    elif mtype == "disconnect":
                        await self.tcp_disconnect()
                    elif mtype == "command":
                        await self.tcp_send(msg.get("data", ""))
                except json.JSONDecodeError:
                    pass
        except:
            pass
        finally:
            self.ws_clients.discard(websocket)


async def main():
    bridge = BridgeServer()
    async with websockets.serve(bridge.handle_ws, "0.0.0.0", WS_PORT):
        print(f"[WS] WebSocket server on ws://0.0.0.0:{WS_PORT}")
        base_dir = os.path.dirname(os.path.abspath(__file__))

        class Handler(SimpleHTTPRequestHandler):
            def __init__(self, *a, **kw):
                super().__init__(*a, directory=base_dir, **kw)
            def log_message(self, *a):
                pass

        httpd = HTTPServer(("0.0.0.0", HTTP_PORT), Handler)
        threading.Thread(target=httpd.serve_forever, daemon=True).start()
        print(f"[HTTP] Server on http://0.0.0.0:{HTTP_PORT}")
        print(f"[INFO] Open http://localhost:{HTTP_PORT}/eeg_web.html")
        await asyncio.Future()


if __name__ == "__main__":
    asyncio.run(main())