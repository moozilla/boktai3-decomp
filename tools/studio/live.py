"""Live link between the studio and tools/mgba/centaur.lua running in mGBA.

Centaur connects to 127.0.0.1:8643 and sends newline-delimited JSON events
(hello, state, text, res, tileset, bookmarks, capture, eval, error); the studio
sends commands back ({"c": "capture"} and so on, see COMMANDS in centaur.lua).

Also here: scene-capture analysis (which palette resources the BG palette RAM
came from; which sprite resource each OAM entry draws, with its real palette)
and the store of graphics tagged as containing text (data/gfx_text.csv).
"""
import collections
import csv
import json
import logging
import os
import socket
import struct
import threading
import time

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
OUT = os.path.join(ROOT, "build", "centaur")
TAGS = os.path.join(ROOT, "data", "gfx_text.csv")
NOTES = os.path.join(ROOT, "data", "notes.csv")
STAGE_PALS = os.path.join(ROOT, "data", "stage_palettes.csv")
SPRITE_PALS = os.path.join(OUT, "sprite_palettes.json")
log = logging.getLogger("studio")
SIZES = {0: [(8, 8), (16, 16), (32, 32), (64, 64)], 1: [(16, 8), (32, 8), (32, 16), (64, 32)],
         2: [(8, 16), (8, 32), (16, 32), (32, 64)], 3: [(8, 8)] * 4}


class Live:
    def __init__(self, port=8643):
        self.lock = threading.Lock()
        self.conn = None
        self.hello = None
        self.state = {}
        self.player = None  # latest {"t": "player"}: world position and BG2 camera
        self.story = None  # latest {"t": "story"}: progress, entrance, event flags, watched variables
        self.texts = collections.deque(maxlen=300)
        self.res = collections.deque(maxlen=300)
        self.events = collections.deque(maxlen=100)
        self.bookmarks = []
        self.last_seen = 0
        self.port = port
        threading.Thread(target=self._serve, daemon=True).start()

    # -- socket ---------------------------------------------------------------
    def _serve(self):
        srv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        srv.bind(("127.0.0.1", self.port))
        srv.listen(1)
        while True:
            conn, _ = srv.accept()
            log.info("live: Centaur connected")
            with self.lock:
                if self.conn:
                    try:
                        self.conn.close()
                    except OSError:
                        pass
                self.conn = conn
            threading.Thread(target=self._read, args=(conn,), daemon=True).start()
            self.command({"c": "bookmarks"})

    def _read(self, conn):
        buf = b""
        while True:
            try:
                chunk = conn.recv(65536)
            except OSError:
                break
            if not chunk:
                break
            buf += chunk
            while b"\n" in buf:
                line, buf = buf.split(b"\n", 1)
                try:
                    self._event(json.loads(line.decode("utf-8", "replace")))
                except ValueError:
                    pass
        with self.lock:
            if self.conn is conn:
                self.conn = None
        log.info("live: Centaur disconnected")

    def _event(self, e):
        with self.lock:
            self.last_seen = time.time()
            t = e.get("t")
            if t == "hello":
                self.hello = e
            elif t == "state":
                self.state = e
            elif t == "player":
                self.player = e
            elif t == "story":
                self.story = e
            elif t == "text":
                self.texts.append(e)
            elif t in ("res", "tileset"):
                self.res.append(e)
            elif t == "bookmarks":
                self.bookmarks = e.get("list", [])
            else:
                e["time"] = time.time()
                self.events.append(e)
                if t == "capture":
                    try:
                        analyse_capture(e["name"])
                    except Exception as ex:  # keep the link alive on analysis errors
                        self.events.append({"t": "error", "error": f"analysis: {ex}"})

    def command(self, cmd):
        with self.lock:
            conn = self.conn
        if not conn:
            return False
        try:
            conn.sendall((json.dumps(cmd) + "\n").encode())
            return True
        except OSError:
            return False

    def snapshot(self, strings_jp, strings_en):
        with self.lock:
            texts = list(self.texts)[-40:]
            return {
                "connected": self.conn is not None,
                "hello": self.hello,
                "state": self.state,
                "player": self.player,
                "story": self.story,
                "texts": [dict(x, jp=strings_jp.get(x["id"]), en=strings_en.get(x["id"])) for x in reversed(texts)],
                "res": list(self.res)[-60:][::-1],
                "events": list(self.events)[-20:][::-1],
                "bookmarks": self.bookmarks,
                "captures": list_captures(),
            }


def _json(path, default):
    try:
        return json.load(open(path))
    except (OSError, ValueError):
        return default


def list_captures(limit=40):
    """Captures, newest first, with their label/group (info.json) and the stage
    map the analysis found."""
    d = os.path.join(OUT, "captures")
    if not os.path.isdir(d):
        return []
    out = []
    for name in os.listdir(d):
        m = os.path.join(d, name, "meta.json")
        if os.path.exists(m):
            meta = _json(m, {})
            info = _json(os.path.join(d, name, "info.json"), {})
            an = os.path.join(d, name, "analysis.json")
            stage = _json(an, {}).get("stage_map") if limit is None and os.path.exists(an) else None
            out.append(dict(meta, name=name, mtime=os.path.getmtime(m), label=info.get("label", ""),
                            group=info.get("group", ""), notes=info.get("notes", ""), stage_map=stage))
    out.sort(key=lambda x: -x["mtime"])
    return out[:limit] if limit else out


def set_capture_info(name, req):
    """Label, group and notes of a capture (build/centaur/captures/NAME/info.json)."""
    d = os.path.join(OUT, "captures", os.path.basename(name))
    if not os.path.isdir(d):
        raise FileNotFoundError(name)
    p = os.path.join(d, "info.json")
    info = _json(p, {})
    for k in ("label", "group", "notes"):
        if k in req:
            info[k] = str(req[k]).strip()
    json.dump(info, open(p, "w"), ensure_ascii=False, indent=1)
    return info


# -- capture analysis ----------------------------------------------------------
_tile_index = None


def _archive(label):
    from datafmt import load_manifest
    for e in load_manifest():
        if e.name == "res_" + label:
            if not os.path.exists(e.file):
                e.extract()
            d = os.path.dirname(e.file)
            return [(ln.split()[0], os.path.join(d, ln.split()[1])) for ln in open(e.file)
                    if ln[:1] not in "#h" and ln.strip()]
    return []


_pkg_frames = {}


def package_frames(h):
    """CB05 package -> [(set of pool tile numbers, set of part palette banks)] per
    frame (tools/assets/sprites.py)."""
    if h in _pkg_frames:
        return _pkg_frames[h]
    f = dict(_archive("tCB05")).get(h)
    out = []
    if f:
        b = open(f, "rb").read()
        nframes = struct.unpack_from("<H", b, 2)[0]
        offs = struct.unpack_from("<5I", b, 0xC)
        for i in range(nframes):
            n, = struct.unpack_from("<H", b, offs[0] + i * 20 + 2)
            po, = struct.unpack_from("<I", b, offs[0] + i * 20 + 16)
            tiles, banks = set(), set()
            for k in range(n):
                o = offs[2] + po + k * 8
                attr, a2 = struct.unpack_from("<H", b, o)[0], struct.unpack_from("<H", b, o + 6)[0]
                w, hh = SIZES[(attr >> 8) & 3][(attr >> 10) & 3]
                t = a2 & 0xFFF  # 12-bit tile number in packages with more than 1024 tiles
                banks.add(a2 >> 12)
                for ty in range(hh // 8):
                    for tx in range(w // 8):
                        tiles.add(t + ty * 16 + tx)
            out.append((tiles, banks))
    _pkg_frames[h] = out
    return out


def tile_index():
    """32-byte 4bpp tile -> (kind, hash, n) for the sprite resources: CB05
    packages (n = tile number in the package's pool) and 98F5 large sprites
    (n = frame)."""
    global _tile_index
    if _tile_index is not None:
        return _tile_index
    idx = {}
    for h, f in _archive("tCB05"):
        d = open(f, "rb").read()
        tiles = struct.unpack_from("<I", d, 0x1C)[0]
        for o in range(tiles, len(d) - 31, 32):
            t = d[o:o + 32]
            if any(t):
                idx.setdefault(t, ("spr", h, (o - tiles) // 32))
    for h, f in _archive("t98F5"):
        d = open(f, "rb").read()
        count, = struct.unpack_from("<I", d, 4)
        pixels, frames_at, parts_at = struct.unpack_from("<3I", d, 0xC)
        recs = [struct.unpack_from("<IBBBBI", d, 0x18 + i * 12) for i in range(count)]
        for i, r in enumerate(recs):
            end = recs[i + 1][5] if i + 1 < count else parts_at - frames_at
            for fi, o in enumerate(range(r[5], end, 12)):
                n_parts, n_tiles, _, pix, _ = struct.unpack_from("<BBHII", d, frames_at + o)
                for k in range(n_tiles):
                    t = d[pixels + pix + k * 32:pixels + pix + k * 32 + 32]
                    if any(t):
                        idx.setdefault(t, ("big", f"{r[0] & 0xFFFF:04X}", fi))
    _tile_index = idx
    return idx


_map_heads = None


def map_heads():
    """First 64 bytes of each map resource as it sits in RAM (decompressed) -> hash."""
    global _map_heads
    if _map_heads is not None:
        return _map_heads
    import lz77
    out = {}
    for h, f in _archive("map"):
        d = open(f, "rb").read()
        if d[0] == 0x10:
            r = lz77.decompress(d, 0)
            if not r:
                continue
            d = r[0][4:]
        out[bytes(d[:64])] = h
    _map_heads = out
    return out


def maps_in_ram(ewram):
    """Map resources present (decompressed) in EWRAM: [(address, hash)]."""
    heads = map_heads()
    found, i = [], 0
    while True:
        i = ewram.find(b"MP\0\0", i)
        if i < 0:
            return found
        h = heads.get(bytes(ewram[i:i + 64]))
        if h:
            found.append((0x02000000 + i, h))
        i += 4


def _palettes():
    out = {}
    for h, f in _archive("t92B3"):
        out[h] = open(f, "rb").read()[20:532]
    return out


def analyse_capture(name):
    d = os.path.join(OUT, "captures", name)
    pal = open(os.path.join(d, "pal.bin"), "rb").read()
    vram = open(os.path.join(d, "vram.bin"), "rb").read()
    oam = open(os.path.join(d, "oam.bin"), "rb").read()
    io = open(os.path.join(d, "io.bin"), "rb").read()
    meta = json.load(open(os.path.join(d, "meta.json")))
    res = _palettes()
    ram_maps = []
    if os.path.exists(os.path.join(d, "ewram.bin")):
        ram_maps = maps_in_ram(open(os.path.join(d, "ewram.bin"), "rb").read())
    # BG palette banks -> palette resources containing them at the same bank
    bg = []
    votes = collections.Counter()
    for bank in range(16):
        chunk = pal[bank * 32:bank * 32 + 32]
        if not any(chunk):
            continue
        hits = [h for h, p in res.items() if p[bank * 32:bank * 32 + 32] == chunk]
        bg.append({"bank": bank, "resources": hits[:8]})
        for h in hits:
            votes[h] += 1
    bg_best = votes.most_common(1)[0][0] if votes else None
    # sprites on screen
    idx = tile_index()
    obj1d = bool(struct.unpack_from("<H", io, 0)[0] & 0x40)
    sprites = []
    learned = {}
    for i in range(128):
        a0, a1, a2 = struct.unpack_from("<3H", oam, i * 8)
        if a0 & 0x300 == 0x200:  # hidden
            continue
        shape, size = a0 >> 14, a1 >> 14
        w, h = SIZES[shape][size]
        tile = a2 & 0x3FF
        bank = a2 >> 12
        y, x = a0 & 0xFF, a1 & 0x1FF
        if y >= 160 and y < 228:
            continue
        found = collections.Counter()
        nums = collections.defaultdict(list)
        tw = w // 8
        for k in range((w // 8) * (h // 8)):
            tx, ty = k % tw, k // tw
            t = tile + (ty * tw + tx if obj1d else ty * 32 + tx)
            o = 0x10000 + (t & 0x3FF) * 32
            hit = idx.get(vram[o:o + 32])
            if hit:
                found[hit[:2]] += 1
                nums[hit[:2]].append(hit[2])
        match = found.most_common(1)[0][0] if found else None
        frame = None
        if match and match[0] == "big":
            frame = collections.Counter(nums[match]).most_common(1)[0][0]
        elif match:
            got = set(nums[match])
            best = max(enumerate(package_frames(match[1])), key=lambda f: len(f[1][0] & got), default=None)
            frame = best[0] if best and best[1][0] & got else None
        colors = list(struct.unpack_from("<16H", pal, 0x200 + bank * 32))
        sprites.append({"oam": i, "x": x if x < 256 else x - 512, "y": y if y < 160 else y - 256, "w": w, "h": h,
                        "tile": tile, "bank": bank, "match": list(match) if match else None, "frame": frame,
                        "tiles": (w // 8) * (h // 8), "matched": found[match] if match else 0, "colors": colors,
                        "nums": sorted(set(nums[match])) if match and match[0] == "spr" else None})
        if match:
            learned.setdefault(f"{match[0]}:{match[1]}", []).append((bank, frame))
    # remember what was learned: the whole OBJ palette RAM, and which bank the
    # resource's palette bank 0 landed in (package parts carry their own bank)
    objpal = list(struct.unpack_from("<256H", pal, 0x200))
    # the palette manager's pool entry (resource 9B1B) per OBJ bank
    objpool = None
    if os.path.exists(os.path.join(d, "iwram.bin")):
        iw = open(os.path.join(d, "iwram.bin"), "rb").read()
        if len(iw) >= 0x5210:
            objpool = list(struct.unpack_from("<16H", iw, 0x51F0))
    for key, seen in list(learned.items()):
        kind, h = key.split(":")
        part_min = 0
        if kind == "spr":
            frames = package_frames(h)
            got = [frames[f][1] for _, f in seen if f is not None and f < len(frames)]
            part_min = min((min(b) for b in got if b), default=0)
        learned[key] = {"pal": objpal, "base": max(0, min(b for b, _ in seen) - part_min)}
        if kind == "big" and objpool:  # one bank: remember its pool entry, unfaded
            p = objpool[min(b for b, _ in seen)]
            if p < 0x8000:
                learned[key]["pool"] = p
    if learned:
        known = {}
        if os.path.exists(SPRITE_PALS):
            try:
                known = json.load(open(SPRITE_PALS))
            except ValueError:
                known = {}
        known.update(learned)
        json.dump(known, open(SPRITE_PALS, "w"), indent=0)
    # the stage on screen: a stage map decompressed in RAM (menus use raw maps in ROM)
    stage_map = ram_maps[0][1] if ram_maps else meta.get("map")
    if bg_best and stage_map:
        remember_stage_palette(stage_map, meta.get("tileset") or "", bg_best)
    result = {"bg": bg, "bg_best": bg_best, "sprites": sprites, "meta": meta, "objpool": objpool,
              "ram_maps": [[f"{a:08X}", h] for a, h in ram_maps], "stage_map": stage_map}
    json.dump(result, open(os.path.join(d, "analysis.json"), "w"))
    return result


def remember_stage_palette(map_hash, tileset, pal):
    rows = {}
    if os.path.exists(STAGE_PALS):
        for r in csv.DictReader(open(STAGE_PALS)):
            rows[r["map"]] = {"map": r["map"], "palette": r["palette"], "evidence": r.get("evidence", "")}
    rows[map_hash] = {"map": map_hash, "palette": pal,
                      "evidence": "BG palette RAM matched in a centaur capture"}
    with open(STAGE_PALS, "w", newline="") as f:
        w = csv.DictWriter(f, ["map", "palette", "evidence"])
        w.writeheader()
        for k in sorted(rows):
            w.writerow(rows[k])


def stage_palettes():
    if not os.path.exists(STAGE_PALS):
        return []
    return list(csv.DictReader(open(STAGE_PALS)))


def sprite_palettes():
    if not os.path.exists(SPRITE_PALS):
        return {}
    try:
        return json.load(open(SPRITE_PALS))
    except ValueError:
        return {}


# -- notes on anything: maps, sprites, palettes, scenes, strings ---------------
NOTE_FIELDS = ["kind", "key", "label", "notes"]


def notes():
    if not os.path.exists(NOTES):
        return []
    return list(csv.DictReader(open(NOTES, encoding="utf-8")))


def save_note(n):
    rows = {(r["kind"], r["key"]): r for r in notes()}
    key = (str(n.get("kind", "")), str(n.get("key", "")).upper())
    label, text = str(n.get("label", "")).strip(), str(n.get("notes", "")).strip()
    if not label and not text:
        rows.pop(key, None)
    else:
        rows[key] = {"kind": key[0], "key": key[1], "label": label, "notes": text}
    with open(NOTES, "w", newline="", encoding="utf-8") as f:
        w = csv.DictWriter(f, NOTE_FIELDS)
        w.writeheader()
        for k in sorted(rows):
            w.writerow(rows[k])
    return notes()


# -- graphics containing text ---------------------------------------------------
TAG_FIELDS = ["kind", "hash", "part", "note", "english", "status"]


def tags():
    if not os.path.exists(TAGS):
        return []
    return list(csv.DictReader(open(TAGS, encoding="utf-8")))


def save_tag(t):
    rows = {(r["kind"], r["hash"], r["part"]): r for r in tags()}
    key = (t.get("kind", ""), t.get("hash", "").upper(), str(t.get("part", "")))
    if t.get("delete"):
        rows.pop(key, None)
    else:
        rows[key] = {"kind": key[0], "hash": key[1], "part": key[2], "note": t.get("note", ""),
                     "english": t.get("english", ""), "status": t.get("status", "todo")}
    with open(TAGS, "w", newline="", encoding="utf-8") as f:
        w = csv.DictWriter(f, TAG_FIELDS)
        w.writeheader()
        for k in sorted(rows):
            w.writerow(rows[k])
    return tags()


if __name__ == "__main__":
    import sys
    sys.path.insert(0, os.path.join(ROOT, "tools"))
    if len(sys.argv) > 1 and sys.argv[1] == "analyse-all":
        caps = sorted(os.listdir(os.path.join(OUT, "captures")))
        for k, name in enumerate(caps):
            try:
                analyse_capture(name)
            except Exception as ex:
                print(name, "failed:", ex)
            if k % 50 == 0:
                print(f"{k}/{len(caps)}")
        print(f"analysed {len(caps)} captures; stage palettes in {os.path.relpath(STAGE_PALS, ROOT)}")
    else:
        print("usage: python3 tools/studio/live.py analyse-all")
