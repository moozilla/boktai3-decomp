#!/usr/bin/env python3
"""Local studio: browse, search and edit the script text with a live preview in
the game font, then build a modified ROM; browse sprites, palettes, the font and
raw tiles from the resource archives (/gfx).

  python3 tools/studio/serve.py [--port 8642]    then open http://localhost:8642

Everything it reads is ROM-derived and extracted under build/ (run `make` once
first); it writes only build/text/bank.tsv and, on request, build/mod.gba.
The server listens on localhost only.
"""
import argparse
import http.server
import json
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, os.path.join(ROOT, "tools"))
from datafmt import load_manifest  # noqa: E402
from datafmt import text_bank  # noqa: E402

BANK = os.path.join(ROOT, "build", "text", "bank.tsv")


def font_path():
    for e in load_manifest():
        if e.name == "res_fnt":
            if not os.path.exists(e.file):
                e.extract()
            d = os.path.dirname(e.file)
            name = [ln.split()[1] for ln in open(e.file) if ln[:1] not in "#h" and ln.strip()][0]
            return os.path.join(d, name)
    raise SystemExit("no res_fnt entry in data/manifest.csv")


def archives():
    """{label: (index path, [(hash, file)])} for the extracted resource archives."""
    out = {}
    for e in load_manifest():
        if e.format != "resource_archive":
            continue
        if not os.path.exists(e.file):
            e.extract()
        d = os.path.dirname(e.file)
        ents = [ln.split() for ln in open(e.file) if ln[:1] not in "#h" and ln.strip()]
        out[e.name[4:]] = [(h, os.path.join(d, f)) for h, f in ents]
    return out


def resource_index():
    return {label: [[h, os.path.getsize(f)] for h, f in ents] for label, ents in archives().items()}


def resource_file(label, h):
    for eh, f in archives().get(label, []):
        if eh == h.upper():
            return f
    return None


def stages():
    """Stage loads in the scripts: proc_884(map, tileset, hp) -> the procedures using them."""
    out = subprocess.run([sys.executable, os.path.join(ROOT, "tools", "assets", "gcx.py"), "dis"],
                         cwd=ROOT, capture_output=True, text=True).stdout
    found, proc = {}, None
    for ln in out.split("\n"):
        if ln.startswith("proc "):
            proc = int(ln.split()[1])
            continue
        f = ln.split()
        if f and f[0] == "proc_884" and len(f) >= 4 and all(x.startswith("0x") for x in f[1:4]):
            key = tuple(int(x, 16) for x in f[1:4])
            found.setdefault(key, []).append(proc)
    return [{"map": f"{m:04X}", "tileset": f"{t:04X}", "hp": f"{h:04X}", "procs": ps}
            for (m, t, h), ps in sorted(found.items())]


def ensure_bank():
    if not os.path.exists(BANK):
        for e in load_manifest():
            if e.name == "text_bank":
                e.extract()


def read_strings():
    out = []
    with open(BANK, encoding="utf-8") as f:
        for ln in f:
            if ln.startswith("#") or not ln.strip():
                continue
            parts = ln.rstrip("\n").split("\t")
            if parts[1] == "T":
                out.append({"id": int(parts[0]), "text": "\t".join(parts[2:])})
    return out


def original_strings():
    """Strings as in the ROM, to mark edited entries."""
    from romlib import load_rom
    for e in load_manifest():
        if e.name == "text_bank":
            src = text_bank.decode(load_rom(), e.start, e.end)
    out = {}
    for ln in src.split("\n"):
        p = ln.split("\t")
        if len(p) > 2 and p[1] == "T":
            out[int(p[0])] = "\t".join(p[2:])
    return out


def write_string(sid, text):
    if "\n" in text or "\t" in text:
        raise ValueError("use \\n and \\t escapes, not literal newlines or tabs")
    text_bank.encode_string(text, {c: g for g, c in text_bank.charmap().items()})
    with open(BANK, encoding="utf-8") as f:
        lines = f.read().split("\n")
    prefix = f"{sid}\tT\t"
    for i, ln in enumerate(lines):
        if ln.startswith(prefix):
            lines[i] = prefix + text
            break
    else:
        raise ValueError(f"no string {sid}")
    tmp = BANK + ".tmp"
    with open(tmp, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))
    os.replace(tmp, BANK)


class Handler(http.server.BaseHTTPRequestHandler):
    def send(self, code, body, ctype="application/json"):
        if isinstance(body, (dict, list)):
            body = json.dumps(body, ensure_ascii=False)
        if isinstance(body, str):
            body = body.encode("utf-8")
        self.send_response(code)
        self.send_header("Content-Type", ctype)
        self.send_header("Content-Length", str(len(body)))
        self.send_header("Cache-Control", "no-store")
        self.end_headers()
        self.wfile.write(body)

    def log_message(self, fmt, *args):
        pass

    def do_GET(self):
        self.path = self.path.split("?")[0]
        if self.path in ("/", "/index.html"):
            self.send(200, open(os.path.join(HERE, "index.html"), "rb").read(), "text/html; charset=utf-8")
        elif self.path == "/api/strings":
            orig = self.server.original
            self.send(200, [dict(s, edited=orig.get(s["id"]) != s["text"]) for s in read_strings()])
        elif self.path == "/api/original":
            self.send(200, self.server.original)
        elif self.path == "/api/charmap":
            self.send(200, {f"{g:04X}": c for g, c in text_bank.charmap().items()})
        elif self.path == "/api/font":
            self.send(200, open(font_path(), "rb").read(), "application/octet-stream")
        elif self.path in ("/gfx", "/gfx.html"):
            self.send(200, open(os.path.join(HERE, "gfx.html"), "rb").read(), "text/html; charset=utf-8")
        elif self.path == "/api/stages":
            self.send(200, stages())
        elif self.path == "/api/res":
            self.send(200, resource_index())
        elif self.path.startswith("/api/res/"):
            f = resource_file(*self.path[len("/api/res/"):].split("/")[:2])
            if f:
                self.send(200, open(f, "rb").read(), "application/octet-stream")
            else:
                self.send(404, {"error": "no such resource"})
        else:
            self.send(404, {"error": "not found"})

    def do_POST(self):
        n = int(self.headers.get("Content-Length", 0))
        try:
            req = json.loads(self.rfile.read(n) or b"{}")
        except ValueError:
            return self.send(400, {"error": "bad json"})
        if self.path == "/api/string":
            try:
                write_string(int(req["id"]), req["text"])
            except (ValueError, KeyError) as e:
                return self.send(400, {"error": str(e)})
            self.send(200, {"ok": True})
        elif self.path == "/api/build":
            p = subprocess.run(["make", "mod"], cwd=ROOT, capture_output=True, text=True)
            self.send(200, {"ok": p.returncode == 0, "log": (p.stdout + p.stderr)[-4000:]})
        else:
            self.send(404, {"error": "not found"})


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--port", type=int, default=8642)
    a = ap.parse_args()
    ensure_bank()
    srv = http.server.ThreadingHTTPServer(("127.0.0.1", a.port), Handler)
    srv.original = original_strings()
    print(f"studio: http://localhost:{a.port}")
    srv.serve_forever()


if __name__ == "__main__":
    main()
