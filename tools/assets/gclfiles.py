#!/usr/bin/env python3
"""The GCL scripts grouped into source files, as Konami's compiler laid them out.

  gclfiles.py                 list the files
  gclfiles.py FILE [--inline] print one file in the readable view

Procedure ids are table slots assigned in source order, and a scene's private
procedures sit right before and after its entry procedure (287 of 492 scene files
in one contiguous run). So a file is:

* a scene file: a scene entry procedure and every procedure only it reaches,
  through calls and through procedure references (callbacks passed as values,
  GCL value type 8);
* a library file: an id range in gcl/names/files.csv, with a category;
* a shared file: a contiguous run of the remaining procedures, used by
  several scenes.

Each procedure keeps its id (`def name(...):  # proc N`), so a file maps back
onto the table and could be recompiled in order. --inline expands calls to
procedures that are private to the file and called once, for reading only:
the ROM has a real call there.
"""
import collections
import csv
import functools
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.dirname(HERE))
import actors  # noqa: E402
import gclview  # noqa: E402
import gcx  # noqa: E402

LIBRARY = os.path.join(gcx.ROOT, "gcl", "names", "files.csv")


def _walk(stmts, out):
    for s in stmts:
        if s[0] == "call":
            out.add(s[1])
        for x in s[1:]:
            _value(x, out)


def _value(x, out):
    if isinstance(x, tuple):
        if x and x[0] == "proc":
            out.add(x[1])
        elif x and x[0] == "block":
            _walk(x[1], out)
        elif x and x[0] == "call":
            _walk([x], out)
        else:
            for y in x:
                _value(y, out)
    elif isinstance(x, list):
        for y in x:
            _value(y, out)


@functools.lru_cache(None)
def refs():
    """{proc: (calls, references)}: called procedures and every procedure it names."""
    img = gclview._image()
    out = {}
    for cid in actors.procs():
        stmts = gclview.procedure(img, cid) or []
        calls, every = collections.Counter(), set()
        _count_calls(stmts, calls)
        _walk(stmts, every)
        out[cid] = (calls, every)
    return out


def _count_calls(stmts, c):
    for s in stmts:
        if s[0] == "call":
            c[s[1]] += 1
        for x in s[1:]:
            _calls_in(x, c)


def _calls_in(x, c):
    if isinstance(x, tuple):
        if x and x[0] == "block":
            _count_calls(x[1], c)
        elif x and x[0] == "call":
            _count_calls([x], c)
        else:
            for y in x:
                _calls_in(y, c)
    elif isinstance(x, list):
        for y in x:
            _calls_in(y, c)


@functools.lru_cache(None)
def owners():
    """{proc: frozenset of scene entries that reach it}."""
    ps, rf = actors.procs(), refs()
    reach = collections.defaultdict(set)
    for s, p in ps.items():
        if not p["scene"]:
            continue
        seen, q = {s}, [s]
        while q:
            c = q.pop()
            reach[c].add(s)
            for n in rf.get(c, ((), ()))[1]:
                if n in ps and n not in seen and not ps[n]["scene"]:
                    seen.add(n)
                    q.append(n)
    return {c: frozenset(reach.get(c, ())) for c in ps}


def library():
    with open(LIBRARY, encoding="utf-8") as f:
        return [dict(r, first=int(r["first"]), last=int(r["last"])) for r in csv.DictReader(f)]


@functools.lru_cache(None)
def files():
    """[{"id", "kind", "category", "title", "description", "entry", "procs", "scenes"}] in id order."""
    ps, own = actors.procs(), owners()
    stages = actors.scene_stages()
    lib = library()
    of, out = {}, {}

    def add(fid, **kw):
        if fid not in out:
            out[fid] = dict(id=fid, procs=[], entry=None, **kw)
        return out[fid]

    for c in sorted(ps):
        r = next((r for r in lib if r["first"] <= c <= r["last"]), None)
        if r:
            of[c] = add(r["file"], kind="library", category=r["category"], title=r["file"], description=r["description"])
        elif ps[c]["scene"] or len(own[c]) == 1:
            s = c if ps[c]["scene"] else next(iter(own[c]))
            m = stages.get(s)
            of[c] = add(f"scene_{s}", kind="scene", category="Scenes", title=f"scene {s}",
                        description=f"map {m}" if m else "")
            if c == s:
                of[c]["entry"] = s
    # what is left (shared by several scenes, or reached by none): contiguous runs
    run = None
    for c in sorted(ps):
        if c in of:
            run = None
            continue
        if run is None:
            run = add(f"shared_{c}", kind="shared", category="Shared", title=f"shared {c}", description="")
        of[c] = run
    for c, f in of.items():
        f["procs"].append(c)
    for f in out.values():
        f["procs"].sort()
        sc = set()
        for c in f["procs"]:
            sc |= own[c]
        f["scenes"] = len(sc)
        if f["kind"] == "shared":
            f["title"] = f"shared {f['procs'][0]}-{f['procs'][-1]}"
            f["description"] = f"used by {len(sc)} scenes" if sc else "reached by no scene"
    return sorted(out.values(), key=lambda f: f["procs"][0])


@functools.lru_cache(None)
def file_of():
    return {c: f["id"] for f in files() for c in f["procs"]}


@functools.lru_cache(None)
def call_counts():
    """{proc: number of call sites across all scripts}."""
    c = collections.Counter()
    for calls, _ in refs().values():
        c.update(calls)
    return c


def inlinable(fid):
    """Procedures of this file that are called exactly once, from inside it."""
    fo, cc, rf = file_of(), call_counts(), refs()
    mine = {c for c, f in fo.items() if f == fid}
    out = set()
    for c in mine:
        for n, k in rf[c][0].items():
            if n in mine and cc[n] == 1 and not actors.procs()[n]["scene"]:
                out.add(n)
    return out


def header(cid):
    n = gclview.names()
    r = n.procs.get(cid)
    name = r["name"] if r else f"proc_{cid}"
    params = r["params"] if r and r["params"] else ""
    return f"def {name}({params.replace(',', ', ')}):  # proc {cid}"


def render(fid, inline=False, readable=True, english=False):
    """[(proc id or None, line)] for one file: each procedure's def line and body
    (readable view), or Konami-style GCL as compiled when not readable."""
    f = next((f for f in files() if f["id"] == fid), None)
    if f is None:
        return None
    ok = inlinable(fid) if inline and readable else set()
    img, out = gclview._image(), []
    if not readable:
        for c in f["procs"]:
            out.append((c, f"# proc {c}"))
            out += [(c, ln) for ln in actors.procs()[c]["lines"]]
            out.append((None, ""))
        return out
    for c in f["procs"]:
        if c in ok:
            continue
        out.append((c, header(c)))
        lines = gclview.render(img, c, inline=ok.__contains__ if ok else None, english=english) or ["pass"]
        out += [(c, "    " + ln) for ln in lines]
        out.append((None, ""))
    return out


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    if not args:
        fs = files()
        kinds = collections.Counter(f["kind"] for f in fs)
        print(f"{len(fs)} files: {dict(kinds)}")
        for f in fs:
            print(f"{f['id']:20} {f['category']:22} {len(f['procs']):5} procs  {f['procs'][0]}-{f['procs'][-1]}  {f['description']}")
        return
    for c, ln in render(args[0], "--inline" in sys.argv) or []:
        print(ln)


if __name__ == "__main__":
    main()
