#!/usr/bin/env python3
"""Record a scripted walk through the Studio as a video (for the showcase).

  python3 tools/showcase/studio_tour.py [--url http://localhost:8642] [--out build/showcase/studio.mp4]

Drives headless Chrome over the DevTools protocol (a small standard-library
WebSocket client, no packages), visiting Browse pages and the other Studio
views in TOUR, scrolling each, and saving a screenshot per frame; ffmpeg
encodes them at FPS. Needs Google Chrome and a running Studio
(tools/studio/serve.py).
"""
import argparse
import base64
import json
import os
import shutil
import socket
import struct
import subprocess
import sys
import tempfile
import time
import urllib.request

CHROME = "/Applications/Google Chrome.app/Contents/MacOS/Google Chrome"
FPS = 20
W, H = 1280, 800

# (path, seconds, scroll in pixels over the visit)
TOUR = [
    ("/atlas", 3.0, 0),
    ("/atlas#node:Story", 3.0, 300),
    ("/atlas#beat:19", 4.0, 500),
    ("/atlas#cut:0", 6.0, 1400),
    ("/atlas#actor:05A5", 6.0, 900),
    ("/atlas#char:9819", 5.0, 1000),
    ("/atlas#node:Cutscenes", 3.0, 300),
    ("/scripts#file=scene_6554", 5.0, 900),
    ("/rom", 4.0, 0),
]


class WS:
    """A minimal WebSocket client (text frames, client masking)."""

    def __init__(self, url):
        host, rest = url[len("ws://"):].split("/", 1)
        h, p = host.split(":")
        self.s = socket.create_connection((h, int(p)))
        key = base64.b64encode(os.urandom(16)).decode()
        self.s.sendall((f"GET /{rest} HTTP/1.1\r\nHost: {host}\r\nUpgrade: websocket\r\nConnection: Upgrade\r\n"
                        f"Sec-WebSocket-Key: {key}\r\nSec-WebSocket-Version: 13\r\n\r\n").encode())
        buf = b""
        while b"\r\n\r\n" not in buf:
            buf += self.s.recv(4096)
        self.rest = buf.split(b"\r\n\r\n", 1)[1]
        self.n = 0

    def send(self, obj):
        data = json.dumps(obj).encode()
        head = bytes([0x81])
        n = len(data)
        head += bytes([0x80 | n]) if n < 126 else bytes([0x80 | 126]) + struct.pack(">H", n) if n < 65536 \
            else bytes([0x80 | 127]) + struct.pack(">Q", n)
        mask = os.urandom(4)
        self.s.sendall(head + mask + bytes(b ^ mask[i % 4] for i, b in enumerate(data)))

    def _read(self, n):
        while len(self.rest) < n:
            self.rest += self.s.recv(1 << 16)
        out, self.rest = self.rest[:n], self.rest[n:]
        return out

    def recv(self):
        msg = b""
        while True:
            b0, b1 = self._read(2)
            n = b1 & 127
            if n == 126:
                n = struct.unpack(">H", self._read(2))[0]
            elif n == 127:
                n = struct.unpack(">Q", self._read(8))[0]
            msg += self._read(n)
            if b0 & 0x80:
                return json.loads(msg)

    def call(self, method, **params):
        self.n += 1
        self.send({"id": self.n, "method": method, "params": params})
        while True:
            m = self.recv()
            if m.get("id") == self.n:
                return m.get("result", {})


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--url", default="http://localhost:8642")
    ap.add_argument("--out", default=os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(
        os.path.abspath(__file__)))), "build", "showcase", "studio.mp4"))
    a = ap.parse_args()
    prof = tempfile.mkdtemp(prefix="tour-")
    frames = tempfile.mkdtemp(prefix="tour-frames-")
    chrome = subprocess.Popen([CHROME, "--headless=new", "--remote-debugging-port=9333", f"--user-data-dir={prof}",
                               f"--window-size={W},{H}", "--hide-scrollbars", "--force-dark-mode", "about:blank"],
                              stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    try:
        for _ in range(50):
            try:
                tabs = json.load(urllib.request.urlopen("http://127.0.0.1:9333/json"))
                break
            except OSError:
                time.sleep(0.2)
        ws = WS(next(t for t in tabs if t["type"] == "page")["webSocketDebuggerUrl"])
        ws.call("Emulation.setDeviceMetricsOverride", width=W, height=H, deviceScaleFactor=1, mobile=False)
        ws.call("Emulation.setEmulatedMedia", features=[{"name": "prefers-color-scheme", "value": "dark"}])
        k = 0
        for path, secs, scroll in TOUR:
            ws.call("Page.navigate", url=a.url + path)
            for _ in range(240):   # until the page has content (Browse loads a large index first)
                r = ws.call("Runtime.evaluate", expression="document.body.innerText.includes('Loading') || "
                                                           "document.body.innerText.length < 200", returnByValue=True)
                if not r.get("result", {}).get("value"):
                    break
                time.sleep(0.25)
            time.sleep(1.2)        # let thumbnails draw
            n = int(secs * FPS)
            for i in range(n):
                if scroll:
                    y = scroll * (0.5 - 0.5 * __import__("math").cos(3.14159 * i / max(n - 1, 1)))
                    ws.call("Runtime.evaluate", expression=f"(document.querySelector('#view')||document.scrollingElement)"
                                                           f".scrollTo(0,{y:.0f}); window.scrollTo(0,{y:.0f})")
                shot = ws.call("Page.captureScreenshot", format="jpeg", quality=90)
                open(os.path.join(frames, f"f{k:05d}.jpg"), "wb").write(base64.b64decode(shot["data"]))
                k += 1
        subprocess.run(["ffmpeg", "-v", "error", "-y", "-framerate", str(FPS), "-i", os.path.join(frames, "f%05d.jpg"),
                        "-c:v", "libx264", "-pix_fmt", "yuv420p", "-crf", "18", a.out], check=True)
        print(f"{a.out}: {k} frames")
    finally:
        chrome.terminate()
        for d in (prof, frames):
            shutil.move(d, os.path.expanduser(f"~/.Trash/{os.path.basename(d)}"))


if __name__ == "__main__":
    sys.exit(main())
