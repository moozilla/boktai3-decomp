#!/usr/bin/env python3
"""Local studio: browse, search and edit the script text with a live preview in
the game font, then build a modified ROM; browse sprites, palettes, the font and
raw tiles from the resource archives (/gfx); and a live view of the game
running in mGBA with tools/mgba/centaur.lua (/live, link on port 8643); the
script actor registry (/actors), script procedures (/scripts), how the rooms
connect (/world, tools/assets/areas.py), the code explorer: every function's C
and disassembly by category and unit, its concessions and cross-references
(/code, tools/studio/codemap.py), and every MP2K song, playable in the
browser (/sound, tools/studio/sound.py and mp2k.js).

  python3 tools/studio/serve.py [--port 8642] [--link-port 8643]    then open http://localhost:8642

At startup it builds the slow indexes in the background (--no-preload skips
this) and logs each step and request to the terminal.

Everything it reads is ROM-derived and extracted under build/ (run `make` once
first); it writes only build/text/bank.tsv and, on request, build/mod.gba.
The server listens on localhost only.
"""
import argparse
import csv
import html
import http.server
import json
import logging
import os
import subprocess
import sys
import threading
import time
import urllib.parse

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, os.path.join(ROOT, "tools"))
sys.path.insert(0, HERE)
from datafmt import load_manifest  # noqa: E402
from datafmt import text_bank  # noqa: E402
import live  # noqa: E402
sys.path.insert(0, os.path.join(ROOT, "tools", "assets"))
import actors  # noqa: E402

BANK = os.path.join(ROOT, "build", "text", "bank.tsv")
log = logging.getLogger("studio")
# Held while the startup preload builds an index; requests that use one wait
# for it instead of building the same index alongside.
WARM = threading.Lock()
SLOW = ("/api/atlas", "/api/codemap", "/api/search/", "/api/rommap", "/api/romtree", "/api/cutscene/", "/api/actors", "/api/procs", "/api/proc/",
        "/api/code/", "/api/learned", "/api/stages", "/api/areas", "/api/text/uses/")
# polled by the live view several times a second; logged only when slow or failing
POLLED = ("/api/live", "/api/live/player")


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


GUIDE_TYPES = {"items": "item", "weapons": "weapon", "accessories": "accessory", "gunframes": "gun frame",
               "gunlenses": "gun lens", "bikeparts": "bike part", "dolls": "doll", "photos": "photo", "valuables": "valuable"}


def guide_entries(trans):
    """(icon, type, id, name) from DarthNemesis's Shin Bokura no Taiyou guide in
    the translation project (../boktai3trans/shinbok guide), when present."""
    import html as _html
    import re
    d = os.path.join(trans, "shinbok guide")
    out = []
    for page, kind in GUIDE_TYPES.items():
        f = os.path.join(d, page + ".html")
        if not os.path.exists(f):
            continue
        s = open(f, encoding="utf-8", errors="replace").read()
        for icon, rest in re.findall(r'<img[^>]*src="images/([a-z]+[0-9a-f]+)\.png"[^>]*>(.*?)</tr>', s, re.S):
            text = re.sub(r"\s+", " ", _html.unescape(re.sub(r"<[^>]+>", " ", rest))).strip()
            text = text.split(" Desc:")[0].split(" Found:")[0].strip()
            m = re.match(r"([0-9A-Fa-f]{2}) (.*)", text)
            out.append({"icon": icon, "type": kind, "id": m.group(1).upper() if m else "", "name": m.group(2) if m else text})
    return out


_stages = None


def stages():
    """Stage loads in the scripts (tools/assets/tilesets.py)."""
    global _stages
    if _stages is None:
        import tilesets
        _stages = tilesets.script_stages()
    return _stages


def learned():
    """Captured stage and sprite palettes, and the large-sprite palette variants."""
    import bigpalettes  # large sprites: default from the data, variants the game selects
    return {"stages": live.stage_palettes(), "sprites": live.sprite_palettes(),
            "bigpools": {f"{h:04X}": bigpalettes.variants(h) for h in bigpalettes.defaults()},
            "bigsources": {f"{h:04X}": {str(p): src for p, src in t["variants"].items()} for h, t in bigpalettes.table().items()},
            "poolfuncs": {p: [[f, actors.fname(int(f[4:], 16)) if f.startswith("sub_") else f] for f in fs]
                          for p, fs in bigpalettes.code_selections().items()}}


def preload():
    """Build the slow indexes once at startup, so the first visit to each page is quick."""
    import atlas
    import gclview
    steps = [("atlas index", atlas.load), ("actors", actors.actors),
             ("script procedures", lambda: (actors.proc_list(), gclview.callee_names(), gclview.names())),
             ("disassembly", actors.code), ("code explorer", codemap_warm), ("function history", lambda: __import__("codemap").function_history()), ("ROM map", atlas.rom_tree), ("sprite palettes", learned),
             ("stage loads", stages)]
    start = time.time()
    for name, fn in steps:
        t = time.time()
        with WARM:
            try:
                fn()
            except Exception:
                log.exception("preload: %s failed", name)
                continue
        log.info("preload: %s (%.1fs)", name, time.time() - t)
    log.info("preload: done in %.1fs", time.time() - start)


def codemap_warm():
    import codemap
    codemap.load()
    codemap._shared_helpers()
    codemap.layouts()  # struct layouts measure in the background


def size_text(n):
    return f"{n} B" if n < 1024 else f"{n / 1024:.0f} KB" if n < 1 << 20 else f"{n / (1 << 20):.1f} MB"


STAGEMETA = os.path.join(ROOT, "build", "assets", "stages.json")


def stage_meta():
    """Per-scene doors, entrances, items and enemies (tools/assets/stages.py)."""
    tools = [os.path.join(ROOT, "tools", "assets", n) for n in ("stages.py", "gcx.py", "hp.py")]
    if not os.path.exists(STAGEMETA) or os.path.getmtime(STAGEMETA) < max(map(os.path.getmtime, tools)):
        subprocess.run([sys.executable, os.path.join(ROOT, "tools", "assets", "stages.py"), "json", STAGEMETA],
                       cwd=ROOT)
    return open(STAGEMETA, "rb").read()


def world():
    """Scenes grouped into areas by their doors (tools/assets/areas.py), with
    which scenes the stage survey captured, and the areas grouped into places
    (tools/studio/places.py)."""
    import areas
    import gclfiles
    w = areas.build(json.loads(stage_meta()))
    file_of = gclfiles.file_of()
    for i, s in w["scenes"].items():
        s["file"] = file_of.get(i)  # scene_NNNN, or a shared file for scenes only doors reach
    for i, info in scene_info().items():
        if i in w["scenes"]:
            w["scenes"][i]["shot"] = info["shot"]
            w["scenes"][i]["map"] = w["scenes"][i]["map"] or info["map"]
    import places
    rows = csv.DictReader(open(os.path.join(ROOT, "data", "labels", "areas.csv"), encoding="utf-8"))
    order = {int(r["area"]): int(r["story_order"] or 0) for r in rows}
    for a in w["areas"]:
        a["story_order"] = order.get(a["area"], 0)
    w["places"] = places.assign(w)
    return w


_TEXT_USES = {}


def text_uses(n):
    """The script procedures that show text-bank string N (their text#N operands)."""
    if not _TEXT_USES:
        import gclview
        names = {int(p): r.get("name") for p, r in gclview.names().procs.items()}
        uses = {}
        for cid, p in actors.procs().items():
            for t in set(p["texts"]):
                uses.setdefault(t, []).append({"id": cid, "name": names.get(cid) or ""})
        _TEXT_USES.update(uses or {-1: []})
    procs = [dict(p) for p in _TEXT_USES.get(n, [])]
    if procs:
        # where each one runs: its scene file's entry scene, placed in the world
        import gclfiles
        entry = {c: f["entry"] for f in gclfiles.files() if f["kind"] == "scene" for c in f["procs"]}
        w = world()
        for p in procs:
            sc = entry.get(p["id"])
            a = w["areas"][w["scene_area"][sc]] if sc is not None and sc in w["scene_area"] else None
            label = (w["scenes"].get(sc) or {}).get("label") if sc is not None else None
            place = a["place"] if a else None
            if label:  # the scene's own label, led by its place unless it already names it
                where = label if not place or label.startswith(place) else f"{place} · {label}"
            else:
                where = " · ".join(x for x in (place, a["detail"]) if x) if place else None
            p["scene"], p["place"] = sc, where
            p["area"] = a["area"] if a else None
    return {"procs": procs}


def hp_data(h):
    """A decoded HP (terrain) resource, tools/assets/hp.py."""
    f = resource_file("tAE6C", h)
    if not f:
        return None
    sys.path.insert(0, os.path.join(ROOT, "tools", "assets"))
    import hp
    return hp.decode(open(f, "rb").read())


def display_strings(trans):
    """Plain Japanese and English per string ID, for the live view."""
    sys.path.insert(0, os.path.join(ROOT, "tools", "mgba"))
    sys.path.insert(0, os.path.join(ROOT, "tools", "trans"))
    import gen_centaur_data as g
    import english
    jp = {}
    for ln in open(BANK, encoding="utf-8"):
        p = ln.rstrip("\n").split("\t")
        if len(p) > 2 and p[1] == "T":
            jp[int(p[0])] = g.display(p[2])
    en = {}
    script = os.path.join(trans, "scriptEd", "script.sjs")
    if os.path.exists(script):
        for sid, (j, e) in english.fan_script(script).items():
            if j and e and e != j:
                en[sid] = g.display(e)
    return jp, en


def scene_list():
    """Scene entries (tools/mgba/scenes.py) with survey results when present."""
    path = os.path.join(ROOT, "build", "centaur", "scenes.json")
    if not os.path.exists(path) or "entrance" not in (json.load(open(path)) or [{}])[0]:
        subprocess.run([sys.executable, os.path.join(ROOT, "tools", "mgba", "scenes.py")], cwd=ROOT)
    sc = json.load(open(path))
    pals = {r["map"]: r["palette"] for r in live.stage_palettes()}
    for s in sc:
        cap = os.path.join(live.OUT, "captures", f"scene_{s['scene']:04d}")
        s["shot"] = os.path.exists(os.path.join(cap, "screen.png"))
        a = os.path.join(cap, "analysis.json")
        if os.path.exists(a):
            try:
                an = json.load(open(a))
                s["shown"] = an.get("stage_map")
                s["became"] = an.get("meta", {}).get("scene")
            except ValueError:
                pass
        s["palette"] = pals.get(s.get("shown") or s.get("map") or "")
    return sc


def scene_info():
    """{scene: {map, shot}}: the stage each scene shows and whether the stage
    survey captured it (captures/scene_NNNN)."""
    return {s["scene"]: {"map": s.get("shown") or s.get("map"), "shot": s["shot"]} for s in scene_list()}


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
    raw = text_bank.encode_string(text, {c: g for g, c in text_bank.charmap().items()})
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
    return raw



# a page the Studio doesn't have: the header and search still work
NOT_FOUND_PAGE = """<!doctype html>
<html lang="en"><head><meta charset="utf-8"><title>Boktai 3 Studio</title>
<link rel="stylesheet" href="/static/common.css">
<style>body { margin: 0; background: var(--bg); color: var(--ink); font: 14px/1.5 system-ui, -apple-system, sans-serif; }
main { padding: 40px 32px; max-width: 640px; } a { color: var(--accent); }</style></head>
<body><header id="top"></header>
<main><h1>No page at {path}</h1><p class="empty">Pick a view above, or press ⌘K to search everything.
<a href="/atlas">Browse</a> is the way in.</p></main>
<script src="/static/common.js"></script><script>studioHeader();</script></body></html>
"""

class Handler(http.server.BaseHTTPRequestHandler):
    def send(self, code, body, ctype="application/json", etag=None, max_age=600):
        """etag: the body is a file that rarely changes; the browser may keep it
        max_age seconds (0: always ask) and then revalidate it (304) instead of
        downloading it again."""
        if etag and self.headers.get("If-None-Match") == etag:
            self.status, self.size = 304, 0
            self.send_response(304)
            self.send_header("ETag", etag)
            self.end_headers()
            return
        if isinstance(body, (dict, list)):
            body = json.dumps(body, ensure_ascii=False)
        if isinstance(body, str):
            body = body.encode("utf-8")
        self.status, self.size = code, len(body)
        self.send_response(code)
        self.send_header("Content-Type", ctype)
        self.send_header("Content-Length", str(len(body)))
        if etag:
            self.send_header("Cache-Control", f"max-age={max_age}" if max_age else "no-cache")
            self.send_header("ETag", etag)
        else:
            self.send_header("Cache-Control", "no-store")
        self.end_headers()
        self.wfile.write(body)

    def log_message(self, fmt, *args):
        pass

    def timed(self, method, route):
        start, waited = time.time(), 0.0
        self.status, self.size = None, 0
        try:
            if self.path.startswith(SLOW):
                with WARM:
                    waited = time.time() - start
                    route()
            else:
                route()
        finally:
            took = time.time() - start
            if self.path not in POLLED or took > 0.5 or self.status != 200:
                wait = f" (waited {waited:.1f}s for preload)" if waited > 0.1 else ""
                log.info("%s %s %s %s %.2fs%s", method, self.path, self.status or "failed", size_text(self.size), took, wait)

    def do_GET(self):
        self.path = self.path.split("?")[0]
        self.timed("GET", self.get)

    def do_POST(self):
        self.timed("POST", self.post)

    def get(self):
        if self.path in ("/", "/index.html", "/text"):
            self.send(200, open(os.path.join(HERE, "index.html"), "rb").read(), "text/html; charset=utf-8")
        elif self.path == "/api/strings":
            orig = self.server.original
            self.send(200, [dict(s, edited=orig.get(s["id"]) != s["text"]) for s in read_strings()])
        elif self.path.startswith("/api/text/uses/"):
            self.send(200, text_uses(int(self.path.rsplit("/", 1)[1])))
        elif self.path == "/api/original":
            self.send(200, self.server.original)
        elif self.path == "/api/english":
            self.send(200, self.server.display_en)
        elif self.path == "/api/charmap":
            self.send(200, {f"{g:04X}": c for g, c in text_bank.charmap().items()})
        elif self.path == "/api/font":
            self.send(200, open(font_path(), "rb").read(), "application/octet-stream")
        elif self.path in ("/gfx", "/gfx.html"):
            self.send(200, open(os.path.join(HERE, "gfx.html"), "rb").read(), "text/html; charset=utf-8")
        elif self.path in ("/live", "/live.html"):
            self.send(200, open(os.path.join(HERE, "live.html"), "rb").read(), "text/html; charset=utf-8")
        elif self.path in ("/atlas", "/atlas.html"):
            self.send(200, open(os.path.join(HERE, "atlas.html"), "rb").read(), "text/html; charset=utf-8")
        elif self.path == "/api/atlas":
            import atlas
            ix = atlas.load()  # rebuilt when an input changes; the tag is the cache file's
            st = os.stat(atlas.CACHE) if os.path.exists(atlas.CACHE) else None
            tag = f'"{st.st_mtime_ns:x}-{st.st_size:x}"' if st else None
            self.send(200, b"" if tag and self.headers.get("If-None-Match") == tag else {"tree": ix["tree"], "entities": ix["entities"]},
                      etag=tag, max_age=0)
        elif self.path in ("/rom", "/rom.html"):
            self.send(200, open(os.path.join(HERE, "rom.html"), "rb").read(), "text/html; charset=utf-8")
        elif self.path == "/api/rommap":
            import atlas
            self.send(200, atlas.rom_load())
        elif self.path == "/api/romtree":
            import atlas
            self.send(200, atlas.rom_tree())
        elif self.path.startswith("/api/cutscene/"):
            import cutscenes
            n = int(self.path.rsplit("/", 1)[1])
            st, rows = cutscenes.timeline(n)
            self.send(200, {"start": st, "steps": rows})
        elif self.path == "/api/atlas/funcs":
            import atlas
            self.send(200, atlas.load()["funcs"])
        elif self.path in ("/actors", "/scripts", "/code", "/world"):
            self.send(200, open(os.path.join(HERE, self.path[1:] + ".html"), "rb").read(), "text/html; charset=utf-8")
        elif self.path == "/api/actors":
            import player_cue
            self.send(200, {"actors": actors.actors(), "enemies": actors.name_list(*actors.ENEMY_NAMES),
                            "characters": actors.name_list(*actors.CHARACTER_NAMES), "scenes": scene_info(),
                            "command_tables": actors.command_tables(),
                            "player_animations": player_cue.animation_variants(),
                            "numeric_tables": actors.numeric_tables()})
        elif self.path == "/api/procs":
            import gclview
            self.send(200, {"procs": actors.proc_list(), "scenes": scene_info(), "callees": gclview.callee_names(),
                            "procnames": {r["name"]: int(p) for p, r in gclview.names().procs.items()}})
        elif self.path.startswith("/api/proc/"):
            d = actors.proc_detail(int(os.path.basename(self.path)))
            if d:
                import gclfiles
                d["file"] = gclfiles.file_of().get(d["id"])
            self.send(200 if d else 404, d or {"error": "no such procedure"})
        elif self.path == "/api/files":
            import gclfiles
            self.send(200, {"files": gclfiles.files()})
        elif self.path.startswith("/api/file/"):
            import gclfiles
            fid, *opts = self.path[len("/api/file/"):].split("/")  # /api/file/ID[/inline][/raw][/en]
            if "en" in opts:
                import gclview
                gclview.ENGLISH = self.server.display_en
            lines = gclfiles.render(fid, inline="inline" in opts, readable="raw" not in opts, english="en" in opts)
            self.send(200 if lines is not None else 404,
                      {"lines": lines, "inlined": sorted(gclfiles.inlinable(fid))} if lines is not None else {"error": "no such file"})
        elif self.path.startswith("/api/search/"):
            import search  # the ⌘K palette on every page (common.js)
            q = urllib.parse.unquote(self.path[len("/api/search/"):])
            self.send(200, search.search(q, self.server.strings(), self.server.display_en))
        elif self.path == "/api/codemap/stats":
            import codemap  # the home page's numbers, without the tree
            sm = codemap.summary()
            self.send(200, {k: sm[k] for k in ("measures", "concession_kinds", "missing", "updating")} |
                      {"ram": len(sm["ram"]), "functions_with_concessions": sum(
                          1 for c in sm["categories"] for u in c["units"] for f in u["functions"] if f[4]),
                       "drafts": sum(1 for c in sm["categories"] if c["name"] != codemap.INTENTIONAL_ASM
                                     for u in c["units"] for f in u["functions"] if f[3] < 100 and not f[4])})
        elif self.path == "/api/codemap/history":
            import codemap
            self.send(200, codemap.history())
        elif self.path == "/api/codemap/changes":
            import codemap
            self.send(200, codemap.changes())
        elif self.path.startswith("/api/codemap/fnhistory/"):
            import codemap
            self.send(200, codemap.function_history().get(self.path.rsplit("/", 1)[1].upper(), {}))
        elif self.path == "/api/codemap":
            import codemap
            self.send(200, codemap.summary())
        elif self.path.startswith(("/api/codemap/fn/", "/api/codemap/ram/", "/api/codemap/type/")):
            import codemap  # /api/codemap/fn/NAME|ADDR, /api/codemap/ram/NAME|ADDR, /api/codemap/type/NAME
            what, arg = self.path.split("/")[3:5]
            d = {"fn": codemap.function, "ram": codemap.ram_object, "type": codemap.type_info}[what](urllib.parse.unquote(arg))
            self.send(200 if d else 404, d or {"error": "not found"})
        elif self.path.startswith("/api/code/"):
            try:
                d = actors.code_info(int(os.path.basename(self.path), 16))
            except ValueError:
                d = None
            self.send(200 if d else 404, d or {"error": "no function there"})
        elif self.path in ("/captures", "/captures.html"):
            self.send(200, open(os.path.join(HERE, "captures.html"), "rb").read(), "text/html; charset=utf-8")
        elif self.path == "/api/guide":
            self.send(200, guide_entries(self.server.trans))
        elif self.path.startswith("/api/guide/"):
            f = os.path.join(self.server.trans, "shinbok guide", "images", os.path.basename(self.path))
            if os.path.isfile(f):
                self.send(200, open(f, "rb").read(), "image/png" if f.endswith(".png") else "image/gif")
            else:
                self.send(404, {"error": "not found"})
        elif self.path == "/api/display":
            self.send(200, {"jp": self.server.display_jp, "en": self.server.display_en})
        elif self.path in ("/sound", "/sound.html"):
            self.send(200, open(os.path.join(HERE, "sound.html"), "rb").read(), "text/html; charset=utf-8")
        elif self.path == "/api/sound":
            import sound
            self.send(200, sound.load())
        elif self.path.startswith(("/api/sound/song/", "/api/sound/vg/", "/api/sound/sample/")):
            import sound  # /api/sound/song/N, /api/sound/vg/ADDR, /api/sound/sample/ADDR
            what, arg = self.path.split("/")[3:5]
            try:
                d = {"song": lambda: sound.song_data(int(arg)), "vg": lambda: sound.voicegroup_data(int(arg, 16)),
                     "sample": lambda: sound.sample_data(int(arg, 16))}[what]()
            except ValueError:
                d = None
            self.send(200 if d else 404, d or {"error": "no such " + what})
        elif self.path in ("/static/common.js", "/static/common.css", "/static/inspector.js", "/static/mp2k.js"):
            ctype = "text/javascript" if self.path.endswith(".js") else "text/css"
            self.send(200, open(os.path.join(HERE, os.path.basename(self.path)), "rb").read(), ctype + "; charset=utf-8")
        elif self.path == "/api/notes":
            self.send(200, live.notes())
        elif self.path == "/api/items":
            f = os.path.join(ROOT, "data", "items.csv")
            self.send(200, list(csv.DictReader(open(f, encoding="utf-8"))) if os.path.exists(f) else [])
        elif self.path == "/api/captures":
            self.send(200, live.list_captures(limit=None))
        elif self.path == "/api/live":
            self.send(200, self.server.live.snapshot(self.server.display_jp, self.server.display_en))
        elif self.path.startswith("/api/live/file/"):
            # bookmarks/NAME.png or captures/NAME/FILE
            rel = os.path.normpath(self.path[len("/api/live/file/"):])
            f = os.path.join(live.OUT, rel)
            if rel.startswith("..") or not os.path.isfile(f):
                return self.send(404, {"error": "not found"})
            ctype = "image/png" if f.endswith(".png") else "application/json" if f.endswith(".json") else "application/octet-stream"
            self.send(200, open(f, "rb").read(), ctype)
        elif self.path.startswith("/api/live/analysis/"):
            name = os.path.basename(self.path)
            try:
                self.send(200, live.analyse_capture(name))
            except FileNotFoundError:
                self.send(404, {"error": "no such capture"})
        elif self.path == "/api/scenes":
            self.send(200, scene_list())
        elif self.path == "/api/learned":
            self.send(200, learned())
        elif self.path == "/api/tags":
            self.send(200, live.tags())
        elif self.path == "/api/stages":
            self.send(200, stages())
        elif self.path == "/api/areas":
            self.send(200, world())
        elif self.path == "/api/tilesets":
            import tilesets
            self.send(200, tilesets.load())
        elif self.path == "/api/stagemeta":
            self.send(200, stage_meta(), "application/json")
        elif self.path.startswith("/api/hp/"):
            d = hp_data(os.path.basename(self.path))
            if d:
                self.send(200, d)
            else:
                self.send(404, {"error": "no such hp resource"})
        elif self.path == "/api/live/player":
            self.send(200, self.server.live.player or {})
        elif self.path == "/api/res":
            self.send(200, resource_index())
        elif self.path.startswith("/api/res/"):
            f = resource_file(*self.path[len("/api/res/"):].split("/")[:2])
            if f:
                st = os.stat(f)
                tag = f'"{st.st_mtime_ns:x}-{st.st_size:x}"'
                self.send(200, b"" if self.headers.get("If-None-Match") == tag else open(f, "rb").read(),
                          "application/octet-stream", etag=tag)
            else:
                self.send(404, {"error": "no such resource"})
        elif not self.path.startswith("/api/"):
            self.send(404, NOT_FOUND_PAGE.replace("{path}", html.escape(self.path)), "text/html; charset=utf-8")
        else:
            self.send(404, {"error": "not found"})

    def post(self):
        n = int(self.headers.get("Content-Length", 0))
        try:
            req = json.loads(self.rfile.read(n) or b"{}")
        except ValueError:
            return self.send(400, {"error": "bad json"})
        if self.path == "/api/string":
            try:
                raw = write_string(int(req["id"]), req["text"])
            except (ValueError, KeyError) as e:
                return self.send(400, {"error": str(e)})
            # a game running with Centaur gets the edit at once (centaur.lua settext)
            live_sent = self.server.live.command({"c": "settext", "id": int(req["id"]), "hex": raw.hex()})
            self.send(200, {"ok": True, "live": live_sent})
        elif self.path == "/api/live/cmd":
            self.send(200, {"ok": self.server.live.command(req)})
        elif self.path == "/api/tags":
            self.send(200, live.save_tag(req))
        elif self.path == "/api/notes":
            self.send(200, live.save_note(req))
        elif self.path.startswith("/api/captures/"):
            try:
                self.send(200, live.set_capture_info(os.path.basename(self.path), req))
            except FileNotFoundError:
                self.send(404, {"error": "no such capture"})
        elif self.path == "/api/build":
            p = subprocess.run(["make", "mod"], cwd=ROOT, capture_output=True, text=True)
            self.send(200, {"ok": p.returncode == 0, "log": (p.stdout + p.stderr)[-4000:]})
        else:
            self.send(404, {"error": "not found"})


def default_trans():
    """../boktai3trans beside the checkout, or beside the main checkout when this is a worktree."""
    here = os.path.join(os.path.dirname(ROOT), "boktai3trans")
    if os.path.isdir(here):
        return here
    try:
        common = subprocess.run(["git", "rev-parse", "--path-format=absolute", "--git-common-dir"], cwd=ROOT,
                                capture_output=True, text=True, check=True).stdout.strip()
    except (OSError, subprocess.CalledProcessError):
        return here
    main = os.path.join(os.path.dirname(os.path.dirname(common)), "boktai3trans")
    return main if os.path.isdir(main) else here


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--port", type=int, default=8642)
    ap.add_argument("--link-port", type=int, default=8643, help="port Centaur connects to (its CENTAUR_LINK_PORT)")
    ap.add_argument("--trans", default=default_trans())
    ap.add_argument("--no-preload", action="store_true", help="build each index on first use instead of at startup")
    a = ap.parse_args()
    logging.basicConfig(level=logging.INFO, format="%(asctime)s %(message)s", datefmt="%H:%M:%S")
    ensure_bank()
    srv = http.server.ThreadingHTTPServer(("127.0.0.1", a.port), Handler)
    srv.original = original_strings()
    srv.live = live.Live(a.link_port)
    srv.trans = a.trans
    srv.display_jp, srv.display_en = display_strings(a.trans)
    srv.strings = read_strings
    log.info("studio: http://localhost:%d (Centaur link on port %d)", a.port, a.link_port)
    if not a.no_preload:
        threading.Thread(target=preload, daemon=True).start()
    srv.serve_forever()


if __name__ == "__main__":
    main()
