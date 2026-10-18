#!/usr/bin/env python3
"""Showcase v3, part 2: the Studio, page by page, with a caption panel.

  python3 tools/showcase/v3/tour.py [--url http://localhost:8642] [--only N,M] [--still]
      -> build/showcase/v3/rec/studio.mp4

Headless Chrome (DevTools protocol, tools/showcase/studio_tour.py's client)
renders a 1920x1080 frame: the Studio in a 1280x800 iframe on the left and a
caption panel on the right in the Studio's own colors and type. Each stop is
a page, a few purposeful interactions (open, click, hover, scroll to a
place) and a caption; frames are captured as the stop plays and encoded at
FPS. --still writes one frame per stop (a contact sheet) instead.

Needs Google Chrome and a running Studio (tools/studio/serve.py).
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
sys.path.insert(0, os.path.dirname(HERE))
from studio_tour import CHROME, WS  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(HERE)))
OUT = os.path.join(ROOT, "build", "showcase", "v3", "rec")
W, H = 1920, 1080
PW, PH = 1280, 800
FPS = 15

TEMPLATE = """<!doctype html><html><head><meta charset="utf-8">
<style>
:root { --bg:#1b1916; --panel:#24211d; --ink:#ece6dc; --muted:#9a9083; --line:#3a352e; --accent:#e0745a; }
html, body { margin:0; width:1920px; height:1080px; overflow:hidden; background:var(--bg); color:var(--ink);
  font: 14px/1.45 system-ui, -apple-system, "Hiragino Sans", sans-serif; }
#frame { position:absolute; left:40px; top:140px; width:1280px; height:800px; border:1px solid var(--line);
  border-radius:10px; overflow:hidden; box-shadow:0 18px 60px rgba(0,0,0,.45); background:var(--bg); }
#frame iframe { width:1280px; height:800px; border:0; display:block; }
#cap { position:absolute; left:1370px; top:140px; width:500px; height:800px; display:flex; flex-direction:column; }
#kicker { color:var(--muted); font-size:17px; letter-spacing:.08em; text-transform:uppercase; margin-bottom:14px; }
#title { font-size:40px; line-height:1.15; font-weight:650; color:var(--ink); margin-bottom:26px; }
#title b { color:var(--accent); font-weight:650; }
#body p { font-size:24px; line-height:1.5; margin:0 0 18px; color:var(--ink); }
#body p.m { color:var(--muted); font-size:20px; }
#body code { font-size:20px; color:var(--accent); }
#foot { margin-top:auto; color:var(--muted); font-size:16px; display:flex; gap:8px; align-items:center; }
#dots i { display:inline-block; width:8px; height:8px; border-radius:4px; background:var(--line); margin-right:5px; }
#dots i.on { background:var(--accent); }
#head { position:absolute; left:40px; top:56px; font-size:22px; color:var(--muted); letter-spacing:.02em; }
#head b { color:var(--ink); }
#card { position:absolute; inset:0; display:none; background:var(--bg); }
#card.on { display:flex; flex-direction:column; justify-content:center; padding:0 160px; }
#card h1 { font-size:64px; margin:0 0 30px; font-weight:650; }
#card h1 b { color:var(--accent); font-weight:650; }
#card .grid { display:grid; grid-template-columns:1fr 1fr; gap:26px 60px; }
#card .grid div { font-size:24px; line-height:1.45; color:var(--muted); }
#card .grid div b { display:block; color:var(--ink); font-size:30px; margin-bottom:4px; }
#card p { font-size:28px; color:var(--muted); max-width:1300px; line-height:1.5; }
</style></head><body>
<div id="head"><b>Boktai 3 Studio</b> &nbsp;·&nbsp; the decompilation's workbench</div>
<div id="frame"><iframe id="f" src="about:blank"></iframe></div>
<div id="cap"><div id="kicker"></div><div id="title"></div><div id="body"></div>
<div id="foot"><span id="dots"></span><span id="where"></span></div></div>
<div id="card"></div>
</body></html>"""


class Tour:
    def __init__(self, url):
        self.url = url
        self.prof = tempfile.mkdtemp(prefix="tour3-")
        self.chrome = subprocess.Popen([CHROME, "--headless=new", "--remote-debugging-port=9334",
                                        f"--user-data-dir={self.prof}", f"--window-size={W},{H}", "--hide-scrollbars",
                                        "--force-dark-mode", "--autoplay-policy=no-user-gesture-required",
"about:blank"],
                                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        for _ in range(60):
            try:
                tabs = json.load(urllib.request.urlopen("http://127.0.0.1:9334/json"))
                break
            except OSError:
                time.sleep(0.25)
        self.ws = WS(next(t for t in tabs if t["type"] == "page")["webSocketDebuggerUrl"])
        self.ws.call("Emulation.setDeviceMetricsOverride", width=W, height=H, deviceScaleFactor=1, mobile=False)
        self.ws.call("Emulation.setEmulatedMedia", features=[{"name": "prefers-color-scheme", "value": "dark"}])
        self.ws.call("Page.enable")
        # the template is written into a page on the Studio's origin, so the
        # Studio's iframe is same-origin (and allowed: a file:// page may not
        # load localhost)
        self.ws.call("Page.navigate", url=url + "/tour-frame")
        time.sleep(0.8)
        self.top("document.open(); document.write(%s); document.close(); 1" % json.dumps(TEMPLATE))
        time.sleep(0.3)
        self.frames = []    # (jpeg bytes, seconds)
        self.meta = {}      # times (seconds into the recording) other tracks line up with

    def close(self):
        self.chrome.terminate()
        shutil.move(self.prof, os.path.expanduser(f"~/.Trash/{os.path.basename(self.prof)}"))

    # ---- the top page and the Studio frame ----
    def top(self, js):
        r = self.ws.call("Runtime.evaluate", expression=js, returnByValue=True, awaitPromise=True)
        return r.get("result", {}).get("value")

    def inner(self, js):
        """Evaluate in the Studio page (the iframe, same-origin)."""
        return self.top("(function(){ var w = document.getElementById('f').contentWindow; "
                        "return w.eval(%s); })()" % json.dumps(js))

    def go(self, path, ready=None, settle=1.5):
        # a fresh document every time: a hash-only change would not reload the page
        self.top("document.getElementById('f').src = 'about:blank'")
        time.sleep(0.3)
        self.top(f"document.getElementById('f').src = {json.dumps(self.url + path)}")
        time.sleep(0.6)
        for _ in range(240):
            try:
                ok = self.inner("document.readyState == 'complete' && !document.body.innerText.includes('Loading') "
                                "&& document.body.innerText.length > 200" + (f" && !!({ready})" if ready else ""))
            except Exception:
                ok = False
            if ok:
                break
            time.sleep(0.25)
        time.sleep(settle)

    def caption(self, kicker, title, body, k, n, where=""):
        dots = "".join(f"<i class='{'on' if i == k else ''}'></i>" for i in range(n))
        self.top("document.getElementById('kicker').textContent = %s; document.getElementById('title').innerHTML = %s;"
                 "document.getElementById('body').innerHTML = %s; document.getElementById('dots').innerHTML = %s;"
                 "document.getElementById('where').textContent = %s; document.getElementById('card').className = ''"
                 % tuple(json.dumps(x) for x in (kicker, title, "".join(f"<p>{p}</p>" for p in body), dots, where)))

    def card(self, html):
        self.top("var c = document.getElementById('card'); c.innerHTML = %s; c.className = 'on'" % json.dumps(html))

    # ---- capture ----
    def shot(self, secs):
        r = self.ws.call("Page.captureScreenshot", format="jpeg", quality=92)
        self.frames.append((base64.b64decode(r["data"]), secs))

    def hold(self, secs, step=None):
        """Capture for secs: one frame (a still) or every `step` seconds."""
        if step is None:
            self.shot(secs)
            return
        t = 0.0
        while t < secs - 1e-6:
            self.shot(min(step, secs - t))
            t += step

    def mouse(self, x, y, kind="mouseMoved", button="none", clicks=0):
        self.ws.call("Input.dispatchMouseEvent", type=kind, x=40 + x, y=140 + y, button=button, clickCount=clicks)

    def click(self, x, y):
        self.mouse(x, y)
        self.mouse(x, y, "mousePressed", "left", 1)
        self.mouse(x, y, "mouseReleased", "left", 1)

    def wheel(self, x, y, dy):
        self.ws.call("Input.dispatchMouseEvent", type="mouseWheel", x=40 + x, y=140 + y, deltaX=0, deltaY=dy)

    def now(self):
        """Seconds of video recorded so far."""
        return sum(secs for _, secs in self.frames)

    def encode(self, out):
        d = tempfile.mkdtemp(prefix="tour3-frames-")
        lst = []
        for i, (jpg, secs) in enumerate(self.frames):
            p = os.path.join(d, f"f{i:05d}.jpg")
            open(p, "wb").write(jpg)
            lst.append(f"file '{p}'\nduration {secs:.4f}\n")
        if self.frames:
            lst.append(f"file '{p}'\n")
        open(os.path.join(d, "list.txt"), "w").write("".join(lst))
        os.makedirs(os.path.dirname(out), exist_ok=True)
        subprocess.run(["ffmpeg", "-v", "error", "-y", "-f", "concat", "-safe", "0", "-i", os.path.join(d, "list.txt"),
                        "-vf", "fps=30,format=yuv420p", "-c:v", "libx264", "-crf", "16", out], check=True)
        shutil.move(d, os.path.expanduser(f"~/.Trash/{os.path.basename(d)}"))


def stops():
    """(kicker, title, lines, path, actions(tour)) for every stop, in order."""
    from tour_stops import STOPS
    return STOPS


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--url", default="http://localhost:8642")
    ap.add_argument("--only")
    ap.add_argument("--still", action="store_true")
    ap.add_argument("--out", default=os.path.join(OUT, "studio.mp4"))
    a = ap.parse_args()
    sys.path.insert(0, HERE)
    import tour_stops
    ss = tour_stops.STOPS
    pick = [int(x) for x in a.only.split(",")] if a.only else range(len(ss))
    t = Tour(a.url)
    try:
        for i in pick:
            s = ss[i]
            print(f"{i}: {s['title']}", flush=True)
            s["run"](t, i, len(ss), still=a.still)
        if a.still:
            d = tempfile.mkdtemp(prefix="tour3-still-")
            files = []
            for k, (jpg, _) in enumerate(t.frames):
                p = os.path.join(d, f"s{k:03d}.jpg")
                open(p, "wb").write(jpg)
                files.append(p)
            out = a.out.replace(".mp4", "_stills.jpg")
            subprocess.run(["magick", "montage", "-font", "/System/Library/Fonts/Supplemental/Arial.ttf", *files,
                            "-tile", "3x", "-geometry", "640x360+2+2", out], check=True)
            print(out)
        else:
            t.encode(a.out)
            json.dump(t.meta, open(a.out.replace(".mp4", ".json"), "w"), indent=1)
            print(a.out)
    finally:
        t.close()


if __name__ == "__main__":
    main()
