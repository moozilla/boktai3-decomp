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
#cap { position:absolute; left:1370px; top:140px; width:510px; height:800px; display:flex; flex-direction:column; }
#kicker { margin-bottom:22px; }
#title { margin-bottom:30px; }
#body .gf { margin-bottom:22px; }
#foot { margin-top:auto; color:var(--muted); font-size:16px; display:flex; gap:8px; align-items:center; }
#dots i { display:inline-block; width:8px; height:8px; border-radius:4px; background:var(--line); margin-right:5px; }
#dots i.on { background:var(--accent); }
#head { position:absolute; left:40px; top:60px; }
#card { position:absolute; inset:0; display:none; background:var(--bg); }
#card.on { display:flex; flex-direction:column; justify-content:center; padding:0 150px; }
#card .grid { display:grid; grid-template-columns:1fr 1fr; gap:34px 70px; }
#card .gap { height:40px; }
canvas.gfc { image-rendering: pixelated; display:block; }
</style></head><body>
<div id="head" class="gf" data-scale="2" data-text="<m>BOKTAI 3 STUDIO  -  the decompilation's workbench</m>"></div>
<div id="frame"><iframe id="f" src="about:blank"></iframe></div>
<div id="cap"><div id="kicker" class="gf" data-scale="2"></div><div id="title" class="gf" data-scale="3"></div>
<div id="body"></div><div id="gamebox"></div>
<div id="foot"><span id="dots"></span><span id="where"></span></div></div>
<div id="card"></div>
<script>
// Text in the game's own 8x16 font (the Studio's /api/font: glyph c at
// 0xC + (c - 0x20) * 64, two 4bpp tiles; 1 is the face, 2 and 3 its edge).
// Markup: <b>accent</b>, <m>muted</m>; \\n breaks a line; words wrap.
let FONT = null;
const INKS = { n: ['#ece6dc', '#a89f92', '#5c554c'], b: ['#e0745a', '#a8553f', '#5a3024'], m: ['#9a9083', '#6b6359', '#3e3933'] };
async function font() { return FONT ??= new Uint8Array(await (await fetch('/api/font')).arrayBuffer()); }
function runs(text) {
  const out = []; let style = 'n';
  for (const part of text.split(/(<\\/?[bm]>)/)) {
    if (/^<[bm]>$/.test(part)) style = part[1]; else if (/^<\\/[bm]>$/.test(part)) style = 'n';
    else for (const ch of part) out.push([ch, style]);
  }
  return out;
}
function wrap(chars, cols) {
  const lines = [[]]; let word = [];
  const flush = () => { const l = lines[lines.length - 1];
    if (l.length && l.length + word.length > cols) { while (l.length && l[l.length - 1][0] === ' ') l.pop(); lines.push([]); }
    lines[lines.length - 1].push(...word); word = []; };
  for (const c of chars) {
    if (c[0] === '\\n') { flush(); lines.push([]); continue; }
    word.push(c);
    if (c[0] === ' ') flush();
  }
  flush();
  return lines.map(l => { while (l.length && l[0][0] === ' ' && l !== lines[0]) l.shift(); return l; });
}
async function draw(el) {
  const f = await font(), scale = +(el.dataset.scale || 2), text = el.dataset.text || '';
  const width = el.clientWidth || 500, cols = Math.max(4, Math.floor(width / (8 * scale)));
  const lines = wrap(runs(text), cols), lh = 18;
  const cv = document.createElement('canvas'); cv.className = 'gfc';
  cv.width = Math.max(8, cols * 8); cv.height = lines.length * lh;
  cv.style.width = cv.width * scale + 'px'; cv.style.height = cv.height * scale + 'px';
  const ctx = cv.getContext('2d');
  lines.forEach((l, y) => l.forEach(([ch, st], x) => {
    const code = ch.charCodeAt(0); if (code < 0x20 || code > 0x7e) return;
    const o = 0xC + (code - 0x20) * 64;
    for (let t = 0; t < 2; t++) for (let r = 0; r < 8; r++) for (let c = 0; c < 8; c++) {
      const v = (f[o + t * 32 + r * 4 + (c >> 1)] >> (4 * (c & 1))) & 15;
      if (v >= 1 && v <= 3) { ctx.fillStyle = INKS[st][v - 1]; ctx.fillRect(x * 8 + c, y * lh + t * 8 + r, 1, 1); }
    }
  }));
  el.replaceChildren(cv);
}
async function drawAll(root) { for (const el of (root || document).querySelectorAll('.gf')) await draw(el); return 1; }
async function setCaption(kicker, title, body, dots, where) {
  document.getElementById('card').className = '';
  document.getElementById('kicker').dataset.text = '<m>' + kicker + '</m>';
  document.getElementById('title').dataset.text = title;
  document.getElementById('body').innerHTML = body.map(p => '<div class="gf" data-scale="2"></div>').join('');
  [...document.getElementById('body').children].forEach((d, i) => d.dataset.text = body[i]);
  document.getElementById('dots').innerHTML = dots;
  document.getElementById('where').textContent = where;
  await drawAll(document.getElementById('cap'));
  return 1;
}
async function setCard(html) {
  const c = document.getElementById('card'); c.innerHTML = html; c.className = 'on';
  await drawAll(c); return 1;
}
drawAll(document.getElementById('head'));
</script>
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
        time.sleep(0.8)
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
        """The caption panel, drawn in the game's font (<b>accent</b>, <m>muted</m>)."""
        dots = "".join(f"<i class='{'on' if i == k else ''}'></i>" for i in range(n))
        self.top("setCaption(%s, %s, %s, %s, %s)" % tuple(json.dumps(x) for x in (kicker, title, body, dots, where)))

    def card(self, html):
        """A full-frame card: elements with class gf and data-text are drawn in the game's font."""
        self.top("setCard(%s)" % json.dumps(html))

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
