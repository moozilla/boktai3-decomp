#!/usr/bin/env python3
"""Retake the README's Studio screenshots (docs/img/studio-*.png) from a running Studio.

  python3 tools/studio/shots.py [--url http://localhost:8642] [--only code,text] [--out docs/img]

Drives headless Google Chrome over the DevTools protocol (the client in
tools/showcase/studio_tour.py) in dark mode at 1400 px wide, waits for each
page's content, and saves a PNG per entry in SHOTS. Start the Studio first
(tools/studio/serve.py) and let its preload finish.
"""
import argparse
import base64
import json
import os
import shutil
import subprocess
import sys
import tempfile
import time
import urllib.request

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, os.path.join(ROOT, "tools", "showcase"))
from studio_tour import CHROME, WS  # noqa: E402

W = 1400
# (name, path, height): the pages the README shows
SHOTS = [
    ("browse", "/atlas#monster:9858", 900),
    ("code", "/code#0804A15C", 1000),
    ("code-overview", "/code", 900),
    ("code-type", "/code#type:TetherActor", 900),
    ("text", "/text#7542", 900),
    ("sprites", "/gfx#sprites/DE23", 900),
    ("map", "/gfx#maps/EBA7", 1000),
    ("world", "/world", 900),
    ("scripts", "/scripts#5757", 1250),
    ("actors", "/actors#C3BC", 900),
    ("captures", "/captures#12_field", 900),
]
READY = ("(() => { const t = document.body.innerText;"
         " return !/Loading/.test(t) && t.length > 300 && !document.querySelector('#view .empty:only-child'); })()")


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--url", default="http://localhost:8642")
    ap.add_argument("--only", help="comma-separated names from SHOTS")
    ap.add_argument("--out", default=os.path.join(ROOT, "docs", "img"))
    a = ap.parse_args()
    shots = [s for s in SHOTS if not a.only or s[0] in a.only.split(",")]
    prof = tempfile.mkdtemp(prefix="shots-")
    chrome = subprocess.Popen([CHROME, "--headless=new", "--remote-debugging-port=9334", f"--user-data-dir={prof}",
                               f"--window-size={W},900", "--hide-scrollbars", "--force-dark-mode", "about:blank"],
                              stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    try:
        for _ in range(50):
            try:
                tabs = json.load(urllib.request.urlopen("http://127.0.0.1:9334/json"))
                break
            except OSError:
                time.sleep(0.2)
        ws = WS(next(t for t in tabs if t["type"] == "page")["webSocketDebuggerUrl"])
        ws.call("Emulation.setEmulatedMedia", features=[{"name": "prefers-color-scheme", "value": "dark"}])
        for name, path, height in shots:
            ws.call("Emulation.setDeviceMetricsOverride", width=W, height=height, deviceScaleFactor=1, mobile=False)
            ws.call("Page.navigate", url="about:blank")
            ws.call("Page.navigate", url=a.url + path)
            for _ in range(400):  # until the page has content (indexes can take a while on first use)
                r = ws.call("Runtime.evaluate", expression=READY, returnByValue=True)
                if r.get("result", {}).get("value"):
                    break
                time.sleep(0.25)
            time.sleep(2.0)  # let thumbnails and canvases draw
            shot = ws.call("Page.captureScreenshot", format="png")
            out = os.path.join(a.out, f"studio-{name}.png")
            with open(out, "wb") as f:
                f.write(base64.b64decode(shot["data"]))
            print(out)
    finally:
        chrome.terminate()
        shutil.move(prof, os.path.expanduser(f"~/.Trash/{os.path.basename(prof)}"))


if __name__ == "__main__":
    sys.exit(main())
