#!/usr/bin/env python3
"""Actors and script procedures, cross-referenced for the studio.

  actors.py actors [KEY...]     registry entries: callback, loader, sprites, scenes
  actors.py procs [PROC...]     script procedures: calls, spawns, strings, scenes
  actors.py code ADDR           a function's disassembly and, if matched, its C

Sources (all ROM-derived or in the repository):
  * data/actor_registry.csv: the 722 `chara` keys (16-bit hashes of the
    original names, docs/DATA.md) and their callbacks.
  * data/monster_commands/*.csv: reviewed, ordered script-handler tables
    declared in data/manifest.csv. Literal references in a registry callback
    or its loader link these tables to actors; a later assignment may replace
    the referenced table.
  * gen/code.s (the generated disassembly): a callback's calls. Most callbacks
    allocate the actor (Heap_Alloc, Memory_Clear) and then call one function
    with it: that is the *loader* (enemy families: docs/ASSETS.md). Sprite
    resources are the 16-bit literals in the callback, the loader and the
    loader's nearby callees (within LOCAL bytes, the family's own code) that
    name a sprite package (CB05) or a large sprite (98F5 record), which the
    code requests by name through ScriptProc_Start / AnimData_Load.
  * the gcx script image (tools/assets/gcx.py): `chara KEY NAME` creates a
    registry entry's actor and `command KEY` (B745) calls its routine and keeps
    the result (every operand is a registry key). Recovered GCL names of keys
    are in gcl/names/chara.csv (tools/assets/gclnames.py). Procedures are numbered by the IDs calls use, which are one
    above gcx.py's printed headers. A scene is an entry procedure that starts
    `proc_861 FLAGS; proc_864 MODE` (tools/mgba/scenes.py); a scene uses every
    procedure it reaches through calls (not through `load`).
  * the text bank: `text#N` operands and debug `print` strings (EUC-JP).

Enemy names: the text bank's monster list starts at string 9838 (ENEMY_NAMES)
and characters at 9818. No code or script operand has been found that adds
9838 to an enemy kind: string IDs are reached as a script-supplied base plus
an offset (the item menus use base + 0x37 and similar), no script operand
supplies a base in 9790-9838, and the late-game save's pause menus never look
up a string in that range (Text_LookupString probe, 2026-10-15). The names per
sprite below therefore rest on the visual identifications in docs/ASSETS.md
("Enemy families and their sprites"), confidence low.
"""
import csv
import json
import os
import re
import struct
import sys
from collections import defaultdict, deque

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(ROOT, "tools"))
import archive  # noqa: E402
import gclnames  # noqa: E402
import gcx  # noqa: E402
from datafmt import load_manifest  # noqa: E402
from romlib import load_rom  # noqa: E402

ALLOC = {0x08219C40, 0x08219DD8, 0x08219D38}  # Heap_Alloc, Memory_Clear, Heap_Free
LOCAL = 0x3000
CHARACTER_NAMES = (9818, 16)   # ジャンゴ, サバタ, おてんこ, ... (first string, count)
ENEMY_NAMES = (9838, 84)       # ポイズナー ... ヴァナルガンド
# sprite -> monster-list string IDs, from the visual identifications in
# docs/ASSETS.md (renders in captured colors); candidates, not facts.
SPRITE_NAMES = {
    "DA6D": [9847, 9848],               # bat, vampire bat
    "9247": [9858, 9859, 9860],         # ghoul, zombie, ghoul+
    "B96C": [9909, 9910, 9911, 9912],   # red/blue/purple/green slime (sprite hash = "slime")
    "9506": [9844, 9845, 9846],         # bee, poison bee, killer bee (sprite hash = "bee")
    "D637": [9914],                     # wraith?
    "528F": [9842, 9843],               # spider, poison spider
    "5A6E": [9849, 9850],               # crow, raven
    "D636": [9857],                     # octopus?
    "5290": [9882, 9883, 9884],         # clay/stone golem, golem+
    "48DE": [9861, 9863],               # mummy, mummy+
    "D635": [9852, 9853, 9854, 9855],   # hound, hot dog, ice dog, hound+
    "3876": [9885],                     # mimic
    "D291": [9907],                     # moeruru?
    "D638": [9906],                     # chlorohorurun
    "5291": [9886, 9887],               # sword, sword+
    "AE52": [9875, 9876, 9877, 9878, 9879, 9880, 9881],  # skeletons
    "3DB6": [9847, 9848],               # black winged creature (small sprites)
}

_CACHE = {}


def cached(fn):
    def wrap():
        if fn.__name__ not in _CACHE:
            _CACHE[fn.__name__] = fn()
        return _CACHE[fn.__name__]
    wrap.__name__ = fn.__name__
    return wrap


# ---------- code ----------
@cached
def code():
    """{addr: {"name", "mode", "lines"}} from gen/code.s, plus a name index."""
    funcs, names, cur = {}, {}, None
    mode = "thumb"
    for ln in open(os.path.join(ROOT, "gen", "code.s")).read().split("\n"):
        m = re.match(r"\s*(arm|thumb)_func_start (\w+)", ln)
        if m:
            mode, cur = m.group(1), None
            continue
        m = re.match(r"^(\w+): @ 0x([0-9A-F]{8})", ln)
        if m and cur is None:
            a = int(m.group(2), 16)
            cur = funcs[a] = {"name": m.group(1), "mode": mode, "lines": []}
            names[m.group(1)] = a
        if cur is not None:
            cur["lines"].append(ln)
    return funcs, names


def func_at(addr):
    """The function containing addr (start address), or None."""
    funcs, _ = code()
    starts = _starts()
    import bisect
    i = bisect.bisect_right(starts, addr) - 1
    return starts[i] if i >= 0 else None


@cached
def _starts():
    return sorted(code()[0])


def fname(addr):
    """The project's name for a function (symbols/functions.csv), else gen/code.s's."""
    s = symbols().get(addr)
    return s["name"] if s else code()[0].get(addr, {}).get("name", f"sub_{addr:08X}")


def calls(addr):
    funcs, names = code()
    out = []
    for ln in funcs.get(addr, {}).get("lines", []):
        m = re.match(r"\s*bl (\w+)", ln)
        if m and m.group(1) in names:
            out.append(m.group(1))
    return out


def literals(addr):
    return [int(v, 16) for v in re.findall(r"@ =0x([0-9A-F]{8})", "\n".join(code()[0].get(addr, {}).get("lines", [])))]


@cached
def symbols():
    out = {}
    with open(os.path.join(ROOT, "symbols", "functions.csv")) as f:
        for r in csv.DictReader(f):
            out[int(r["addr"], 16)] = r
    return out


@cached
def sources():
    """{function name: (path, line, text, nonmatching)} for C definitions in src/."""
    out = {}
    for d, _, files in os.walk(os.path.join(ROOT, "src")):
        for fn in files:
            if not fn.endswith(".c"):
                continue
            path = os.path.join(d, fn)
            lines = open(path, encoding="utf-8", errors="replace").read().split("\n")
            depth = []  # preprocessor stack: True inside #ifdef NONMATCHING
            i = 0
            while i < len(lines):
                ln = lines[i]
                if ln.startswith("#if"):
                    depth.append("NONMATCHING" in ln)
                elif ln.startswith("#else") and depth:
                    depth[-1] = False if depth[-1] else depth[-1]
                elif ln.startswith("#endif") and depth:
                    depth.pop()
                m = re.match(r"^(?:static\s+)?[A-Za-z_][\w\s\*]*?\b(\w+)\s*\([^;]*$", ln)
                if m and not ln.startswith(("#", " ", "\t", "INCLUDE_ASM", "typedef", "return")):
                    j = i
                    while j < len(lines) and lines[j] != "}" and not (j > i and lines[j].endswith(";") and "{" not in "".join(lines[i:j + 1])):
                        j += 1
                    if j < len(lines) and lines[j] == "}":
                        start = i
                        while start > 0 and lines[start - 1].startswith(("//", "/*", " *", " */")):
                            start -= 1
                        out.setdefault(m.group(1), (os.path.relpath(path, ROOT), i + 1, "\n".join(lines[start:j + 1]), any(depth)))
                        i = j
                i += 1
    return out


def code_info(addr):
    """Disassembly and C (when present) of the function containing addr."""
    a = func_at(addr)
    if a is None:
        return None
    f = code()[0][a]
    sym = symbols().get(a, {})
    src = sources().get(sym.get("name")) or sources().get(f["name"])
    names = code()[1]
    return {"addr": f"{a:08X}", "name": fname(a), "gen_name": f["name"], "mode": f["mode"], "asm": "\n".join(f["lines"]).rstrip(),
            "confidence": sym.get("confidence"), "notes": sym.get("notes"),
            "c": src and {"file": src[0], "line": src[1], "text": src[2], "nonmatching": src[3]},
            "calls": [{"addr": f"{names[c]:08X}", "name": fname(names[c])} for c in sorted(set(calls(a)))],
            "callers": callers().get(f"{a:08X}", [])}


@cached
def callers():
    out = defaultdict(set)
    funcs, _ = code()
    for a, f in funcs.items():
        for c in calls(a):
            out[c].add(f["name"])
    names = code()[1]
    return {f"{names[k]:08X}": [{"addr": f"{names[c]:08X}", "name": fname(names[c])} for c in sorted(v)] for k, v in out.items()}


# ---------- sprites ----------
@cached
def sprite_hashes():
    rom = load_rom()
    out = {}
    for label, _, _, ents in archive.archives(rom):
        if label == "tCB05":
            for h, _, _ in ents:
                out.setdefault(h, "spr")
        if label == "t98F5":
            _, a, _ = ents[0]
            o = a - 0x08000000
            count, = struct.unpack_from("<I", rom, o + 4)
            for i in range(count):
                h, = struct.unpack_from("<I", rom, o + 0x18 + i * 12)
                out[h] = "big"
    for t in archive.TYPE_KEYS:  # type hashes (0xCB05 ...) are not names
        out.pop(t, None)
    return out


# ---------- text ----------
@cached
def bank():
    sys.path.insert(0, os.path.join(ROOT, "tools", "mgba"))
    import gen_centaur_data as g
    from datafmt import load_manifest
    out = {}
    for e in load_manifest():
        if e.name == "text_bank":
            if not os.path.exists(e.file):
                e.extract()
            for ln in open(e.file, encoding="utf-8"):
                p = ln.rstrip("\n").split("\t")
                if len(p) > 2 and p[1] == "T":
                    out[int(p[0])] = g.display(p[2])
                elif len(p) > 2 and p[1] == "B":  # a data record in the bank, not text
                    out[int(p[0])] = None
    return out


def name_list(first, count):
    b = bank()
    return [{"id": first + i, "jp": b.get(first + i) or ""} for i in range(count)]


# ---------- scripts ----------
@cached
def image():
    return gcx.Image(load_rom())


@cached
def procs():
    """{call id: {"lines", "calls", "spawns", "texts", "prints", "loads", "scene"}}."""
    img = gcx.Image(load_rom())
    out = {}
    for pid in range(len(img.procs)):
        lines = gcx.disassemble(img, pid)  # keys in hex, for parsing
        if not lines:
            continue
        cid = pid + 1
        shown = gcx.disassemble(img, pid, names=True)
        p = out[cid] = {"flag": img.procs[pid] >> 24, "lines": ["    " * d + t for d, t in shown],
                        "calls": [], "spawns": [], "texts": [], "prints": [], "loads": []}
        for _, t in lines:
            p["calls"] += [int(x) for x in re.findall(r"\bproc_(\d+)", t)]
            for k, n in re.findall(r"\bchara (0x[0-9a-f]+|\d+)(?: (\S+))?", t):
                p["spawns"].append({"key": f"{int(k, 0):04X}", "cmd": "chara", "arg": n})
            for k in re.findall(r"\bcommand (0x[0-9a-f]+|\d+)", t):
                p["spawns"].append({"key": f"{int(k, 0):04X}", "cmd": "command"})
            p["texts"] += [int(x) for x in re.findall(r"text#(\d+)", t)]
            p["prints"] += re.findall(r"\bprint '((?:[^'\\]|\\.)*)'", t)
            p["loads"] += [int(x, 0) for x in re.findall(r"\bload (0x[0-9a-f]+|\d+)", t)]
        p["scene"] = (len(lines) >= 2 and bool(re.match(r"proc_861 \S+ <end>$", lines[0][1]))
                      and bool(re.match(r"proc_864 \S+ <end>$", lines[1][1])))
    for cid, p in out.items():
        p["calls"] = sorted(set(p["calls"]))
    return out


@cached
def proc_scenes():
    """{proc: sorted scenes reaching it}, through calls and procedure references
    (callbacks); see gclfiles.owners."""
    import gclfiles
    return {k: sorted(v) for k, v in gclfiles.owners().items() if v}


@cached
def proc_callers():
    out = defaultdict(set)
    for cid, p in procs().items():
        for c in p["calls"]:
            out[c].add(cid)
    return {k: sorted(v) for k, v in out.items()}


@cached
def scene_stages():
    """{scene: map hash} from build/centaur/scenes.json when present."""
    f = os.path.join(ROOT, "build", "centaur", "scenes.json")
    try:
        return {s["scene"]: s.get("map") for s in json.load(open(f))}
    except (OSError, ValueError):
        return {}


def proc_list():
    """A compact row per procedure for the scripts view."""
    sc, cl = proc_scenes(), proc_callers()
    return [{"id": cid, "flag": p["flag"], "n": len(p["lines"]), "scene": p["scene"], "calls": len(p["calls"]),
             "callers": len(cl.get(cid, [])), "scenes": len(sc.get(cid, [])), "spawns": len(p["spawns"]),
             "texts": len(p["texts"]), "prints": len(p["prints"]), "keys": sorted({s["key"] for s in p["spawns"]})}
            for cid, p in sorted(procs().items())]


def proc_detail(cid):
    p = procs().get(cid)
    if p is None:
        return None
    b, reg = bank(), registry()
    st = scene_stages()
    scenes = proc_scenes().get(cid, [])
    import gclview
    en = gclnames.prints()
    readable = gclview.render(image(), cid) or []
    return dict(p, id=cid, callers=proc_callers().get(cid, []), prints=[{"text": t, "en": en.get(t, "")} for t in p["prints"]], readable=readable, scenes=[{"scene": s, "map": st.get(s)} for s in scenes],
                texts=[{"id": t, "jp": b.get(t) or "", "data": t in b and b[t] is None} for t in sorted(set(p["texts"]))],
                spawns=[dict(s, callback=reg.get(s["key"], {}).get("callback"), name=reg.get(s["key"], {}).get("name"))
                        for s in p["spawns"]])


# ---------- actors ----------
@cached
def command_tables():
    """Reviewed actor command arrays, keyed by their original ROM address.

    The encoder validates indices, callback values and table extents. Names
    come from current function symbols, not the CSV's informational column.
    """
    out = {}
    for entry in load_manifest():
        source = os.path.relpath(entry.file, ROOT).replace(os.sep, "/")
        if entry.format != "callback_array" or not source.startswith("data/monster_commands/"):
            continue
        handlers = []
        for index, (_, callback, _) in enumerate(entry.encode()):
            addr = callback & ~1
            handlers.append({"index": index, "callback": f"{callback:08X}",
                             "addr": f"{addr:08X}" if callback else None,
                             "name": fname(addr) if callback else None})
        key = f"{entry.start:08X}"
        out[key] = {"addr": key, "end": f"{entry.end:08X}", "name": entry.name,
                    "description": entry.description, "source": source,
                    "size": entry.end - entry.start, "entries": handlers}
    return out


def command_tables_of(funcs):
    """Decoded table literals in these exact functions, with their provenance.

    This is a reference index, not an assertion that the table remains active
    after initialization. Nearby callees are deliberately not searched.
    """
    tables, refs = command_tables(), defaultdict(list)
    for addr in sorted(set(funcs)):
        for value in sorted(set(literals(addr))):
            key = f"{value:08X}"
            if key in tables:
                refs[key].append({"addr": f"{addr:08X}", "name": fname(addr)})
    return [{"table": key, "in": refs[key]} for key in sorted(refs)]


@cached
def registry():
    out = {}
    with open(os.path.join(ROOT, "data", "actor_registry.csv")) as f:
        for r in csv.DictReader(f):
            out[r["key"].upper()] = r
    return out


def loader_of(cb):
    """The function a callback hands its new actor to (after Heap_Alloc and
    Memory_Clear), or None."""
    names = code()[1]
    cs = [names[c] for c in calls(cb)]
    if 0x08219DD8 in cs:
        for c in cs[cs.index(0x08219DD8) + 1:]:
            if c not in ALLOC:
                return c
    return None


def sprites_of(funcs):
    hs, out = sprite_hashes(), []
    for a in funcs:
        for v in literals(a):
            if v in hs and f"{v:04X}" not in [o["hash"] for o in out]:
                out.append({"hash": f"{v:04X}", "kind": hs[v], "in": code()[0][a]["name"]})
    return out


@cached
def actors():
    _, names = code()
    reg = registry()
    spawned = defaultdict(list)
    for cid, p in procs().items():
        for s in p["spawns"]:
            spawned[s["key"]].append(cid)
    sc = proc_scenes()
    b = bank()
    gcl = gclnames.load("chara")
    out = []
    for key, r in sorted(reg.items()):
        cb = int(r["callback"], 16) & ~1
        cbname = fname(cb)
        lda = loader_of(cb)
        ld = fname(lda) if lda else None
        funcs = [cb] + ([lda] if lda else [])
        commands = command_tables_of(funcs)
        for f in list(funcs):
            funcs += [names[c] for c in sorted(set(calls(f))) if c in names and abs(names[c] - f) < LOCAL
                      and names[c] not in funcs and names[c] not in ALLOC]
        spr = sprites_of(funcs)
        ps = sorted(set(spawned.get(key, [])))
        scenes = sorted(set(s for p in ps for s in sc.get(p, [])))
        cand = []
        for s in spr:
            for i in SPRITE_NAMES.get(s["hash"], []):
                if i not in [c["id"] for c in cand]:
                    cand.append({"id": i, "jp": b.get(i) or "", "via": s["hash"]})
        out.append({"key": key, "callback": f"{cb:08X}", "callback_name": cbname,
                    "loader": f"{lda:08X}" if lda else None, "loader_name": ld,
                    "command_tables": commands,
                    "sprites": spr, "procs": ps, "scenes": scenes, "spawns": sum(1 for p in ps for s in procs()[p]["spawns"] if s["key"] == key),
                    "names": cand, "gcl": gcl.get(int(key, 16), [])})
    return out


def main():
    cmd = sys.argv[1] if len(sys.argv) > 1 else "actors"
    args = sys.argv[2:]
    if cmd == "actors":
        for a in actors():
            if args and a["key"] not in [x.upper() for x in args]:
                continue
            print("\t".join([a["key"], a["callback_name"], a["loader_name"] or "-",
                             ",".join(s["hash"] for s in a["sprites"]) or "-", f"{len(a['procs'])} procs",
                             f"{len(a['scenes'])} scenes", "/".join(n["jp"] for n in a["names"])]))
    elif cmd == "procs":
        for cid in [int(x) for x in args] or sorted(procs()):
            d = proc_detail(cid)
            print(f"proc {cid}: calls {d['calls']} callers {d['callers']} scenes {[s['scene'] for s in d['scenes']][:20]}")
            for ln in d["lines"]:
                print("    " + ln)
    elif cmd == "code":
        d = code_info(int(args[0], 16))
        print(json.dumps(d, ensure_ascii=False, indent=1) if d else "no function")


if __name__ == "__main__":
    main()
