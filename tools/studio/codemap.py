#!/usr/bin/env python3
"""The Studio's code explorer index (tools/studio/code.html): every function in
the report's category -> unit -> function tree, with its matching status, the
concessions it still holds, and what it touches.

  python3 tools/studio/codemap.py              build and print a summary
  python3 tools/studio/codemap.py NAME|ADDR    print one function's page data

The tree is the one PROGRESS.json carries (tools/progress.py, the same as
decomp.dev's report). Concession kinds per function come from
`progress.py --list-pinned` (tools/concessions.py's kinds, plus `helper` for a
function that holds one only through a static helper or macro). Cross-references
come from the generated disassembly (gen/code.s): calls, RAM objects
(symbols/ram.csv) and ROM data (symbols/data.csv) loaded from literal pools, and
functions whose address is taken (callbacks). Cached in build/cache/codemap.json
until an input changes; a function's C and its concession lines are read on
request, so they follow edits at once.
"""
import bisect
import collections
import csv
import json
import os
import pickle
import re
import subprocess
import sys
import tempfile
import threading
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
for p in (os.path.join(ROOT, "tools", "assets"), os.path.join(ROOT, "tools"), HERE):
    if p not in sys.path:
        sys.path.insert(0, p)

CACHE = os.path.join(ROOT, "build", "cache", "codemap.json")
INPUTS = ["symbols/functions.csv", "symbols/ram.csv", "symbols/data.csv", "symbols/progress_functions.csv",
          "symbols/progress_subsystems.csv", "symbols/progress_categories.csv", "symbols/progress_libraries.csv",
          "symbols/rom_data.csv", "data/manifest.csv",
          "tools/progress.py", "tools/concessions.py", "tools/studio/codemap.py", "gen/code.s"]
SOURCE_DIRS = ["src", "include"]
RAM_RANGES = ((0x02000000, 0x02040000, "EWRAM"), (0x03000000, 0x03008000, "IWRAM"))
_LIT = re.compile(r"@ =(0x[0-9A-Fa-f]+|[A-Za-z_]\w*)")
_BL = re.compile(r"^\s*bl (\w+)")


def _newest():
    newest = max(os.path.getmtime(os.path.join(ROOT, p)) for p in INPUTS if os.path.exists(os.path.join(ROOT, p)))
    for d in SOURCE_DIRS:
        for dirpath, _, files in os.walk(os.path.join(ROOT, d)):
            for f in files:
                if f.endswith((".c", ".h")):
                    newest = max(newest, os.path.getmtime(os.path.join(dirpath, f)))
    return newest


def _progress():
    """(tree, report measures, {name: [kinds]}) from tools/progress.py, run as the
    integration does, so the explorer shows what PROGRESS.json would."""
    prog = os.path.join(ROOT, "tools", "progress.py")
    with tempfile.TemporaryDirectory() as d:
        md, rep = os.path.join(d, "progress.md"), os.path.join(d, "report.json")
        runs = [subprocess.Popen([sys.executable, prog, "--markdown", md, "--json", rep], cwd=ROOT,
                                 stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL),
                subprocess.Popen([sys.executable, prog, "--list-pinned"], cwd=ROOT,
                                 stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True)]
        pinned_out, _ = runs[1].communicate()
        runs[0].wait()
        if runs[0].returncode or runs[1].returncode:
            return _committed_progress()
        tree = _read_json(os.path.join(d, "progress.json"))
        report = _read_json(rep)
    kinds = {}
    for row in csv.DictReader(pinned_out.splitlines(), delimiter="\t"):
        kinds[row["name"]] = row["kinds"].split(",") if row["kinds"] else ["helper"]
    return tree, report, kinds


def _committed_progress():
    """The committed PROGRESS.json, when tools/progress.py can't run here (it needs the
    built tree): the same tree as of the last integration, without concession kinds."""
    tree = _read_json(os.path.join(ROOT, "PROGRESS.json"))

    def measures(fns):
        total = sum(f[2] for f in fns)
        clean = sum(f[2] for f in fns if f[3] == 100)
        return {"total_code": str(total), "complete_code": str(clean), "total_functions": len(fns),
                "matched_functions": sum(1 for f in fns if f[3] == 100),
                "complete_code_percent": 100 * clean / total if total else 0}
    every = [f for c in tree["categories"] for u in c["units"] for f in u["functions"]]
    report = {"measures": measures(every), "stale": True,
              "categories": [{"name": c["name"], "measures": measures([f for u in c["units"] for f in u["functions"]])}
                             for c in tree["categories"]]}
    kinds = {f[0]: ["concession"] for f in every if f[4]}
    return tree, report, kinds


def _ram():
    rows = []
    for r in csv.DictReader(open(os.path.join(ROOT, "symbols", "ram.csv"), encoding="utf-8")):
        rows.append({"addr": r["addr"].upper(), "name": r["name"], "size": int(r["size"] or "0", 16),
                     "decl": r["decl"], "header": r["header"], "confidence": r["confidence"], "notes": r["notes"]})
    rows.sort(key=lambda r: int(r["addr"], 16))
    return rows


def _data():
    """ROM data in three layers, most specific first: named symbols (symbols/data.csv), the
    decoded assets (data/manifest.csv) and the ROM's data regions (symbols/rom_data.csv)."""
    def rows_of(path, conv):
        out = []
        for r in csv.DictReader((ln for ln in open(os.path.join(ROOT, path), encoding="utf-8") if not ln.startswith("#"))):
            try:
                out.append(conv(r))
            except (ValueError, KeyError):
                continue
        return sorted(out, key=lambda r: int(r["addr"], 16))
    span = lambda r: {"addr": f"{int(r['start'], 16):08X}", "size": int(r["end"], 16) - int(r["start"], 16)}
    return [rows_of("symbols/data.csv", lambda r: {"addr": f"{int(r['addr'], 16):08X}", "name": r["name"],
                                                   "size": int(r["size"] or "0", 16), "kind": r["kind"], "notes": r["notes"]}),
            rows_of("data/manifest.csv", lambda r: span(r) | {"name": r["name"], "kind": r["format"],
                                                              "notes": r["description"], "file": r["file"]}),
            rows_of("symbols/rom_data.csv", lambda r: span(r) | {"name": r["unit"], "kind": r["kind"], "notes": r["evidence"]})]


class _Ranges:
    """Find the object (from a sorted list of {addr, size}) that holds an address."""

    def __init__(self, rows):
        self.rows = rows
        self.starts = [int(r["addr"], 16) for r in rows]

    def find(self, a):
        i = bisect.bisect_right(self.starts, a) - 1
        if i < 0:
            return None
        r = self.rows[i]
        return r if a < self.starts[i] + max(r["size"], 1) else None


def xrefs(funcs, names, ram_rows, data_layers):
    """{func addr: {"ram": {obj: [offsets]}, "data": [names], "ptrs": [func addrs]}} from the
    literal pools of gen/code.s (funcs and names as actors.code() returns them)."""
    ram, layers = _Ranges(ram_rows), [_Ranges(rows) for rows in data_layers]
    data_names = {r["name"] for rows in data_layers for r in rows}
    out = {}
    for a, f in funcs.items():
        rr, dd, pp = collections.defaultdict(set), set(), set()
        for ln in f["lines"]:
            m = _LIT.search(ln)
            if not m:
                continue
            t = m.group(1)
            if t.startswith("0x"):
                v = int(t, 16)
                if any(lo <= v < hi for lo, hi, _ in RAM_RANGES):
                    r = ram.find(v)
                    if r:
                        rr[r["name"]].add(v - int(r["addr"], 16))
                    else:
                        rr[f"{v:08X}"].add(0)
                elif 0x08000000 <= v < 0x0A000000:
                    if v & 1 and (v & ~1) in funcs:
                        pp.add(v & ~1)
                    else:
                        r = next((r for layer in layers if (r := layer.find(v))), None)
                        if r:
                            dd.add(r["name"])
            elif t in names and names[t] != a:
                pp.add(names[t])
            elif t in data_names:
                dd.add(t)
        if rr or dd or pp:
            out[a] = {"ram": {k: sorted(v) for k, v in rr.items()}, "data": sorted(dd), "ptrs": sorted(pp)}
    return out


INTENTIONAL_ASM = "Intentional assembly"


def _with_intentional_asm(tree, report):
    """The compact tree leaves out the SDK's hand-written assembly (crt0, the
    MP2K mixer, libgcc's division; tools/progress.py load_asm_ranges); the
    full report keeps those units. Add them as their own category so every
    function in the ROM has a place."""
    have = {row[1] for c in tree["categories"] for u in c["units"] for row in u["functions"]}
    units = []
    for u in report.get("units", []):
        if "intentional_asm" not in u.get("metadata", {}).get("progress_categories", []):
            continue
        rows = [[f["name"], f"{int(f['metadata']['virtual_address']):08X}", int(f["size"]),
                 f.get("fuzzy_match_percent", 0), 0, -2] for f in u["functions"]]
        rows = [r for r in rows if r[1] not in have]
        if rows:
            units.append({"name": u["name"].split("/", 1)[-1], "functions": sorted(rows, key=lambda r: r[1])})
    if not units:
        return tree
    return dict(tree, categories=tree["categories"] + [{"name": INTENTIONAL_ASM, "units": units}])


def build():
    import actors
    tree, report, kinds = _progress()
    try:
        funcs, names = actors.code()  # gen/code.s, from `make disasm`
    except OSError:
        funcs, names = {}, {}
    ram_rows, data_layers = _ram(), _data()
    x = xrefs(funcs, names, ram_rows, data_layers)
    data_by_name = {}
    for rows in reversed(data_layers):
        data_by_name.update({r["name"]: r for r in rows})
    calls = {a: sorted({names[m.group(1)] for ln in f["lines"] if (m := _BL.match(ln)) and m.group(1) in names} - {a})
             for a, f in funcs.items()}
    tree = _with_intentional_asm(tree, report)
    # functions in report order, with their place in the tree
    fns = {}
    for ci, c in enumerate(tree["categories"]):
        for ui, u in enumerate(c["units"]):
            for row in u["functions"]:
                # --list-pinned names a function as its C does (row[6], when that differs)
                name, addr, size, pct, pinned, file = row[:6]
                fns[addr] = {"name": name, "addr": addr, "size": size, "pct": pct, "cat": ci, "unit": ui,
                             "file": f"src/fn/sub_{addr}.c" if file == -1 else None if file == -2 else tree["files"][file],
                             "defined": row[6] if len(row) > 6 else None, "kinds": (kinds.get(name) or kinds.get(row[6] if len(row) > 6 else None)
                                                 or ["concession"]) if pinned else []}
    ram_users, data_users, ptr_users = collections.defaultdict(list), collections.defaultdict(list), collections.defaultdict(list)
    for a, d in x.items():
        h = f"{a:08X}"
        for k in d["ram"]:
            ram_users[k].append(h)
        for k in d["data"]:
            data_users[k].append(h)
        for p in d["ptrs"]:
            ptr_users[f"{p:08X}"].append(h)
    return {
        "measures": report["measures"],
        "missing": [what for what, ok in (("live progress (tools/progress.py failed; showing PROGRESS.json)",
                                           not report.get("stale")),
                                          ("disassembly and cross-references (run make disasm for gen/code.s)", bool(funcs)))
                    if not ok],
        "categories": [{"name": c["name"], "units": [u["name"] for u in c["units"]],
                        "measures": next((r["measures"] for r in report["categories"] if r["name"] == c["name"]), None)}
                       for c in tree["categories"]],
        "functions": fns,
        "calls": {f"{a:08X}": [f"{c:08X}" for c in cs] for a, cs in calls.items() if cs},
        "xrefs": {f"{a:08X}": {"ram": d["ram"], "data": d["data"], "ptrs": [f"{p:08X}" for p in d["ptrs"]]}
                  for a, d in x.items()},
        "ram": ram_rows, "ram_users": ram_users,
        "data": {k: r for k, r in data_by_name.items() if k in data_users}, "data_users": data_users,
        "ptr_users": ptr_users,
        "concession_kinds": collections.Counter(k for f in fns.values() for k in f["kinds"]),
    }


_INDEX = {"mtime": None, "data": None}
_LOCK = threading.Lock()
_LAYOUT_LOCK = threading.Lock()


def _read_json(path):
    with open(path, encoding="utf-8") as f:
        return json.load(f)


def _rebuild(newest):
    data = build()
    os.makedirs(os.path.dirname(CACHE), exist_ok=True)
    tmp = CACHE + ".tmp"
    with open(tmp, "w") as f:
        json.dump(data, f, separators=(",", ":"))
    os.replace(tmp, CACHE)
    data = _read_json(CACHE)  # the same shape the cached path returns (str keys)
    _INDEX.update(mtime=newest, data=data)
    return data


def _rebuild_in_background(newest):
    def run():
        try:
            _rebuild(newest)
        finally:
            _LOCK.release()
    if _LOCK.acquire(blocking=False):
        threading.Thread(target=run, daemon=True).start()


def updating():
    """True while a rebuild runs behind the index being served."""
    return _LOCK.locked()


def load(wait=False):
    """The index. When an input is newer than it (a source edit, a merge), the old one is
    served while a new one builds in the background (about half a minute, tools/progress.py);
    only the first build, or WAIT, blocks."""
    now = time.time()
    if _INDEX["data"] is not None and now - _INDEX.get("checked", 0) < 2:
        return _INDEX["data"]
    _INDEX["checked"] = now
    newest = _newest()
    if _INDEX["data"] is not None and _INDEX["mtime"] >= newest:
        return _INDEX["data"]
    if _INDEX["data"] is None and os.path.exists(CACHE):
        data = _read_json(CACHE)
        if not data.get("missing"):
            _INDEX.update(mtime=os.path.getmtime(CACHE), data=data)
            if _INDEX["mtime"] >= newest:
                return data
    if _INDEX["data"] is not None and not wait:
        _rebuild_in_background(newest)
        return _INDEX["data"]
    with _LOCK:
        if _INDEX["data"] is not None and _INDEX["mtime"] >= newest:
            return _INDEX["data"]
        return _rebuild(newest)


def progress_by_address():
    """{address: (size, matched, name, "Category/Unit")}, the shape of
    tools/assets/script_targets.progress(), from this index instead of another
    run of tools/progress.py (Browse and the ROM map use it)."""
    ix = load(wait=True)
    cats = [c["name"] for c in ix["categories"]]
    out = {}
    for addr, f in ix["functions"].items():
        c = cats[f["cat"]]
        top = "Libraries" if c == INTENTIONAL_ASM else c  # where the report files them
        out[int(addr, 16)] = (f["size"], f["pct"] == 100, f["name"], f"{top}/{ix['categories'][f['cat']]['units'][f['unit']]}")
    return out


def summary():
    """The tree without per-function cross-references, for the explorer's sidebar,
    overview and search: [category -> [unit -> [function rows]]]."""
    ix = load()
    if _INDEX.get("summary_of") is ix:
        return dict(_INDEX["summary"], updating=updating())
    cats = [{"name": c["name"], "measures": c["measures"], "units": [{"name": u, "functions": []} for u in c["units"]]}
            for c in ix["categories"]]
    for f in ix["functions"].values():
        cats[f["cat"]]["units"][f["unit"]]["functions"].append(
            [f["name"], f["addr"], f["size"], f["pct"], f["kinds"], f["file"]])
    for c in cats:
        for u in c["units"]:
            u["functions"].sort(key=lambda r: r[1])
    t = types()
    out = {"measures": ix["measures"], "missing": ix.get("missing", []), "updating": False,
           "types": sorted(set(t["defs"]) | {a for a, tag in t["aliases"].items() if tag in t["defs"]}), "concession_kinds": ix["concession_kinds"],
           "categories": cats,
           "ram": [{k: r.get(k) for k in ("addr", "name", "size", "header")} | {"users": len(ix["ram_users"].get(r["name"], []))}
                   for r in ix["ram"]]}
    _INDEX.update(summary=out, summary_of=ix)
    return dict(out, updating=updating())


HEADLINE = re.compile(r"Code matched in C: ([\d.]+)%.*?(\d+) / (\d+) functions")
CLEAN = re.compile(r"Without (concessions|register pinning): \*\*([\d.]+)%")


def history():
    """Progress over time, from every committed PROGRESS.md (decomp.dev's
    history graph): [{t, matched, clean, functions, total}], oldest first.
    matched is the share of code bytes in C; clean, from when PROGRESS.md
    started reporting it, the share without concessions (clean_of "pins":
    without register pins, before PROGRESS.md counted the other kinds)."""
    try:
        log = subprocess.run(["git", "log", "--format=%H %ct", "--", "PROGRESS.md"], cwd=ROOT,
                             capture_output=True, text=True, timeout=30).stdout.split()
    except (OSError, subprocess.SubprocessError):
        return []
    commits = list(zip(log[::2], map(int, log[1::2])))
    if not commits:
        return []
    key = commits[0][0]
    if _INDEX.get("history_of") == key:
        return _INDEX["history"]
    path = os.path.join(ROOT, "build", "cache", "progress_history.json")
    cached = _read_json(path) if os.path.exists(path) else None
    if cached and cached.get("head") == key:
        out = cached["points"]
    else:
        blobs = "".join(f"{h}:PROGRESS.md\n" for h, _ in commits)
        raw = subprocess.run(["git", "cat-file", "--batch"], cwd=ROOT, input=blobs.encode(),
                             capture_output=True, timeout=120).stdout
        out, pos = [], 0
        for h, t in commits:
            nl = raw.index(b"\n", pos)
            head = raw[pos:nl].split()
            if len(head) < 3 or head[1] != b"blob":
                pos = nl + 1
                continue
            size = int(head[2])
            text = raw[nl + 1:nl + 1 + min(size, 4000)].decode("utf-8", "replace")
            pos = nl + 1 + size + 1
            m = HEADLINE.search(text)
            if not m:
                continue
            c = CLEAN.search(text)
            out.append({"t": t, "matched": float(m.group(1)), "clean": float(c.group(2)) if c else None,
                        "clean_of": ("pins" if c.group(1) == "register pinning" else "all") if c else None,
                        "functions": int(m.group(2)), "total": int(m.group(3))})
        out.sort(key=lambda p: p["t"])
        try:
            os.makedirs(os.path.dirname(path), exist_ok=True)
            with open(path + ".tmp", "w") as f:
                json.dump({"head": key, "points": out}, f)
            os.replace(path + ".tmp", path)
        except OSError:
            pass
    _INDEX.update(history=out, history_of=key)
    return out


def _source(f):
    """(path, first line, text, {line in text: [kinds]}) of a function's C, or None."""
    import concessions
    import progress
    from split import split_nonmatching
    if not f["file"] or not os.path.exists(os.path.join(ROOT, f["file"])):
        return None
    raw = open(os.path.join(ROOT, f["file"]), encoding="utf-8", errors="replace").read()
    text, _ = split_nonmatching(raw)
    bodies = progress._bodies(text)
    body = bodies.get(f["defined"] or f["name"]) or bodies.get(f"sub_{f['addr']}")
    if not body:
        return None
    body = body[1]
    pos = raw.find(body)
    line = raw.count("\n", 0, pos) + 1 if pos >= 0 else None
    marks = collections.defaultdict(list)
    if f["kinds"]:
        for s in concessions.counted(concessions.sites(body, f["file"])):
            marks[s.line].append(s.kind)
        helpers = progress.unit_helpers(text, f["file"], _shared_helpers())
        for ln in progress._calls(body, helpers):
            if ln not in marks:
                marks[ln].append("helper")
    return f["file"], line, body, dict(marks)


_HELPERS = {}


def _symbols():
    if "sym" not in _HELPERS:
        _HELPERS["sym"] = {r["addr"].upper(): r for r in csv.DictReader(open(os.path.join(ROOT, "symbols", "functions.csv"),
                                                                              encoding="utf-8"))}
    return _HELPERS["sym"]


def _shared_helpers():
    if "h" not in _HELPERS:
        import progress
        _HELPERS["h"] = frozenset(progress.header_helpers())
    return _HELPERS["h"]


def _game_links(name):
    """Atlas entities (objects, script routines, monsters ...) whose code includes this function."""
    try:
        import atlas
        E = atlas.load()["entities"]
    except Exception:
        return []
    out = []
    for e in E.values():
        if e.get("fname") == name or name in (e.get("code") or []):
            out.append({"id": e["id"], "kind": e["kind"], "title": e.get("title") or e["id"],
                        "direct": e.get("fname") == name})
    out.sort(key=lambda e: (not e["direct"], e["kind"], e["title"]))
    return out


def find(q):
    """A function's address (8 hex digits) for a name, sub_ name or address in it."""
    ix = load()
    q = q.strip()
    by_name = {f["name"]: a for a, f in ix["functions"].items()}
    if q in by_name:
        return by_name[q]
    m = re.fullmatch(r"(?:sub_|0x)?([0-9A-Fa-f]{7,8})", q)
    if m:
        a = int(m.group(1), 16) & ~1
        starts = sorted(int(k, 16) for k in ix["functions"])
        i = bisect.bisect_right(starts, a) - 1
        if i >= 0:
            k = f"{starts[i]:08X}"
            if a < starts[i] + ix["functions"][k]["size"]:
                return k
    return None


def function(q):
    """Everything the explorer's function page shows."""
    import actors
    ix = load()
    a = find(q)
    if a is None:
        return None
    f = ix["functions"][a]
    cat = ix["categories"][f["cat"]]
    try:
        asm = actors.code()[0].get(int(a, 16))
    except OSError:
        asm = None
    fn_name = lambda k: ix["functions"].get(k, {}).get("name") or k
    ref = lambda k: {"addr": k, "name": fn_name(k)}
    x = ix["xrefs"].get(a, {"ram": {}, "data": [], "ptrs": []})
    callers = sorted({c for c, cs in ix["calls"].items() if a in cs})
    src = _source(f)
    ram_by_name = {r["name"]: r for r in ix["ram"]}
    sym = _symbols().get(a, {})
    unit_fns = sorted((g for g in ix["functions"].values() if g["cat"] == f["cat"] and g["unit"] == f["unit"]),
                      key=lambda g: g["addr"])
    i = next(j for j, g in enumerate(unit_fns) if g["addr"] == a)
    return {
        "name": f["name"], "addr": a, "size": f["size"], "pct": f["pct"], "kinds": f["kinds"],
        "category": cat["name"], "unit": cat["units"][f["unit"]],
        "prev": ref(unit_fns[i - 1]["addr"]) if i else None,
        "next": ref(unit_fns[i + 1]["addr"]) if i + 1 < len(unit_fns) else None,
        "confidence": sym.get("confidence"), "notes": sym.get("notes"),
        "mode": asm["mode"] if asm else None, "asm": "\n".join(asm["lines"]).rstrip() if asm else "",
        "c": src and {"file": src[0], "line": src[1], "text": src[2], "marks": src[3]},
        "calls": [ref(k) for k in ix["calls"].get(a, [])],
        "callers": [ref(k) for k in callers],
        "ptrs": [ref(k) for k in x["ptrs"]],
        "ptr_users": [ref(k) for k in ix["ptr_users"].get(a, [])],
        "ram": [{"name": k, "offsets": v, "addr": ram_by_name[k]["addr"] if k in ram_by_name else k,
                 "decl": ram_by_name.get(k, {}).get("decl"), "named": k in ram_by_name} for k, v in sorted(x["ram"].items())],
        "data": [ix["data"].get(k) or {"name": k} for k in x["data"]],
        "game": _game_links(f["name"]),
    }


def ram_object(name):
    """A RAM object and every function that loads its address, by unit."""
    ix = load()
    r = next((r for r in ix["ram"] if r["name"] == name or r["addr"] == name.upper()), None)
    key = r["name"] if r else name.upper()
    users = ix["ram_users"].get(key, [])
    if not r and not users:
        return None
    rows = []
    for a in users:
        f = ix["functions"].get(a)
        if f:
            rows.append({"addr": a, "name": f["name"], "category": ix["categories"][f["cat"]]["name"],
                         "unit": ix["categories"][f["cat"]]["units"][f["unit"]], "kinds": f["kinds"],
                         "offsets": ix["xrefs"][a]["ram"].get(key, [])})
    rows.sort(key=lambda r: r["addr"])
    region = next((n for lo, hi, n in RAM_RANGES if r and lo <= int(r["addr"], 16) < hi), None)
    return {"object": r or {"name": key, "addr": key}, "region": region, "users": rows}


# ---------- types: struct and union definitions, with their layout as agbcc measures it ----------

_TYPE_DEF = re.compile(r"^(?:typedef\s+)?(struct|union)\s+(\w+)\s*\{", re.M)
_TYPEDEF = re.compile(r"^typedef\s+(struct|union)\s+(\w+)\s+(\w+)\s*;", re.M)
_TYPEDEF_BODY = re.compile(r"\}\s*(\w+)\s*;")


def _headers():
    import glob
    return sorted(glob.glob(os.path.join(ROOT, "include", "**", "*.h"), recursive=True))


def types():
    """{"defs": {tag: {kind, file, line, text}}, "aliases": {typedef name: tag}} for the shared
    headers (include/); cached per process until a header changes."""
    heads = _headers()
    key = max((os.path.getmtime(h) for h in heads), default=0)
    if _HELPERS.get("types_key") == key:
        return _HELPERS["types"]
    defs, aliases = {}, {}
    for h in heads:
        rel = os.path.relpath(h, ROOT)
        with open(h, encoding="utf-8", errors="replace") as f:
            text = f.read()
        for m in _TYPE_DEF.finditer(text):
            i, depth = m.end(), 1
            while i < len(text) and depth:
                depth += (text[i] == "{") - (text[i] == "}")
                i += 1
            end = text.find(";", i)
            body = text[m.start():end + 1 if end >= 0 else i]
            start = m.start()
            while True:  # the comment block above the definition belongs to it
                prev = text.rfind("\n", 0, start - 1)
                ln = text[prev + 1:start].strip()
                if start and ln.startswith(("//", "/*", "*")):
                    start = prev + 1
                else:
                    break
            defs.setdefault(m.group(2), {"kind": m.group(1), "file": rel, "line": text.count("\n", 0, m.start()) + 1,
                                         "text": text[start:m.start()] + body})
            t = _TYPEDEF_BODY.match(text, i - 1) if m.group(0).startswith("typedef") else None
            if t:
                aliases[t.group(1)] = m.group(2)
        for m in _TYPEDEF.finditer(text):
            aliases[m.group(3)] = m.group(2)
    out = {"defs": defs, "aliases": aliases}
    _HELPERS.update(types=out, types_key=key)
    return out


def layouts(wait=False):
    """{tag: {size, header, fields: [(offset, size, member)]}} measured by compiling offsetof tables
    with agbcc (tools/scope/structure.py, in scope's own cache, so `scope.py fields` shares it). The
    first measure takes a while; until it lands this returns None and measures in the background."""
    if "layouts" in _HELPERS and _HELPERS.get("layouts_key") == max(map(os.path.getmtime, _headers()), default=0):
        return _HELPERS["layouts"]
    if not wait:
        if _LAYOUT_LOCK.acquire(blocking=False):
            def run():
                try:
                    _measure_layouts()
                except Exception:
                    _HELPERS["layouts"] = {}
                finally:
                    _LAYOUT_LOCK.release()
            threading.Thread(target=run, daemon=True).start()
        return _HELPERS.get("layouts")
    with _LAYOUT_LOCK:
        return _measure_layouts()


def _measure_layouts():
    # not tools/scope/scope.py itself: without capstone it re-executes the
    # whole process under build/venv, which would restart the Studio
    sys.path.insert(0, os.path.join(ROOT, "tools", "scope"))
    import build as buildtool
    import structure
    heads = _headers()

    def compile_c(text):  # as scope.py fields compiles its tables
        r = subprocess.run(buildtool.CPP + ["-nostdinc", "-undef"] + buildtool.DEFINES + ["-I", "include", "-"],
                           input=text, capture_output=True, text=True, cwd=ROOT)
        if r.returncode:
            return None
        a = subprocess.run([buildtool.AGBCC] + buildtool.CFLAGS + ["-o", "-"], input=r.stdout, capture_output=True,
                           text=True, cwd=ROOT)
        return a.stdout if a.returncode == 0 and "error" not in a.stderr else None
    key = "|".join(f"{p}:{os.stat(p).st_mtime_ns}:{os.stat(p).st_size}" for p in heads)
    path = os.path.join(os.path.dirname(CACHE), "struct_layouts.pickle")
    try:
        with open(path, "rb") as f:
            k, lay = pickle.load(f)
        if k != key:
            raise ValueError
    except (OSError, ValueError, EOFError, pickle.UnpicklingError):
        lay = structure.layouts(ROOT, compile_c)
        os.makedirs(os.path.dirname(path), exist_ok=True)
        with open(path + ".tmp", "wb") as f:
            pickle.dump((key, lay), f)
        os.replace(path + ".tmp", path)
    _HELPERS.update(layouts=lay, layouts_key=max(map(os.path.getmtime, heads), default=0))
    return lay


def type_info(name):
    """A struct or union: its definition, its layout and the functions whose C names it."""
    t = types()
    tag = t["aliases"].get(name, name)
    d = t["defs"].get(tag)
    if not d:
        return None
    lay = (layouts() or {}).get(tag)
    names = {tag} | {a for a, x in t["aliases"].items() if x == tag}
    rx = re.compile(r"\b(?:struct|union)\s+" + re.escape(tag) + r"\b|\b(?:" + "|".join(map(re.escape, sorted(names))) + r")\b")
    ix = load()
    by_file = collections.defaultdict(list)
    for f in ix["functions"].values():
        if f["file"]:
            by_file[f["file"]].append(f)
    users = []
    import progress
    from split import split_nonmatching
    for path, fs in by_file.items():
        try:
            with open(os.path.join(ROOT, path), encoding="utf-8", errors="replace") as fh:
                raw = fh.read()
        except OSError:
            continue
        if not rx.search(raw):
            continue
        bodies = progress._bodies(split_nonmatching(raw)[0]) if len(fs) > 1 else None
        for f in fs:
            body = bodies.get(f["defined"] or f["name"], (0, ""))[1] if bodies is not None else raw
            if rx.search(body):
                users.append({"addr": f["addr"], "name": f["name"], "kinds": f["kinds"],
                              "unit": ix["categories"][f["cat"]]["units"][f["unit"]],
                              "category": ix["categories"][f["cat"]]["name"]})
    users.sort(key=lambda u: u["addr"])
    return {"name": tag, "kind": d["kind"], "file": d["file"], "line": d["line"], "text": d["text"],
            "aliases": sorted(names - {tag}), "size": lay and lay["size"], "fields": lay and lay["fields"],
            "measuring": lay is None and _LAYOUT_LOCK.locked(), "users": users}


def main():
    if len(sys.argv) > 1:
        d = function(sys.argv[1]) or ram_object(sys.argv[1])
        print(json.dumps(d, indent=1, ensure_ascii=False)[:6000] if d else "not found")
        return
    ix = load()
    m = ix["measures"]
    print(f"{m['total_functions']} functions, {len(ix['categories'])} categories, "
          f"{sum(len(c['units']) for c in ix['categories'])} units")
    print(f"without concessions: {m['complete_code_percent']:.2f}% of code; concession kinds: "
          + ", ".join(f"{k} {n}" for k, n in sorted(ix["concession_kinds"].items(), key=lambda kv: -kv[1])))
    print(f"RAM objects referenced: {len(ix['ram_users'])}; ROM data referenced: {len(ix['data_users'])}")


if __name__ == "__main__":
    main()
