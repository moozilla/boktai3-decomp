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
import re
import subprocess
import sys
import tempfile

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
    # functions in report order, with their place in the tree
    fns = {}
    for ci, c in enumerate(tree["categories"]):
        for ui, u in enumerate(c["units"]):
            for row in u["functions"]:
                name, addr, size, pct, pinned, file = row[:6]
                fns[addr] = {"name": name, "addr": addr, "size": size, "pct": pct, "cat": ci, "unit": ui,
                             "file": f"src/fn/sub_{addr}.c" if file == -1 else None if file == -2 else tree["files"][file],
                             "defined": row[6] if len(row) > 6 else None, "kinds": kinds.get(name, []) if pinned else []}
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


def _read_json(path):
    with open(path, encoding="utf-8") as f:
        return json.load(f)


def load():
    """The index, rebuilt when an input is newer than the cache (or the cache was built
    without some of its inputs)."""
    newest = _newest()
    if _INDEX["data"] is not None and _INDEX["mtime"] >= newest:
        return _INDEX["data"]
    data = None
    if os.path.exists(CACHE) and os.path.getmtime(CACHE) >= newest:
        data = _read_json(CACHE)
        if data.get("missing"):
            data = None
    if data is None:
        data = build()
        os.makedirs(os.path.dirname(CACHE), exist_ok=True)
        tmp = CACHE + ".tmp"
        with open(tmp, "w") as f:
            json.dump(data, f, separators=(",", ":"))
        os.replace(tmp, CACHE)
        data = _read_json(CACHE)  # the same shape the cached path returns (str keys)
    _INDEX.update(mtime=newest, data=data)
    return data


def summary():
    """The tree without per-function cross-references, for the explorer's sidebar,
    overview and search: [category -> [unit -> [function rows]]]."""
    ix = load()
    if _INDEX.get("summary_of") is ix:
        return _INDEX["summary"]
    cats = [{"name": c["name"], "measures": c["measures"], "units": [{"name": u, "functions": []} for u in c["units"]]}
            for c in ix["categories"]]
    for f in ix["functions"].values():
        cats[f["cat"]]["units"][f["unit"]]["functions"].append(
            [f["name"], f["addr"], f["size"], f["pct"], f["kinds"], f["file"]])
    for c in cats:
        for u in c["units"]:
            u["functions"].sort(key=lambda r: r[1])
    out = {"measures": ix["measures"], "missing": ix.get("missing", []), "concession_kinds": ix["concession_kinds"],
           "categories": cats,
           "ram": [{k: r.get(k) for k in ("addr", "name", "size", "header")} | {"users": len(ix["ram_users"].get(r["name"], []))}
                   for r in ix["ram"]]}
    _INDEX.update(summary=out, summary_of=ix)
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
