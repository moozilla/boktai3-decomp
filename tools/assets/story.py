#!/usr/bin/env python3
"""The main quest as story progress values.

  story.py                 one line per value: where it is set, what happens, what it opens
  story.py N               one value in detail
  story.py --json          everything

`r708[0x3C]` is the story progress (`story` in the readable view, `progress`
in tools/assets/stages.py). Scripts raise it with `advance_story(value=N)`
(proc 875: `story = max(story, N)`) as the quest moves on, and test it
everywhere: a door leads to a room's first-visit entrance while progress is
below some value, an event plays once, a character says something else. Each
value is one beat of the quest; this lists, for each value, the procedures
that set it (and the scene and area they belong to), the cutscene around it,
and the conditions that compare progress with it.

A setter is tied to a cutscene when it is the cutscene's `onfinish`, its
starter, or called from either (a cutscene's end usually runs `advance_story`
and then restores the HUD).
"""
import collections
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(ROOT, "tools"))
import gcx  # noqa: E402

VAR = "r708[0x3c]:t1"
_cache = {}


def procs():
    if "procs" not in _cache:
        img = gcx.Image(gcx.load_rom())
        out = {}
        for pid in range(1, 16993):
            try:
                L = gcx.disassemble(img, pid - 1, names=False)
            except Exception:
                continue
            if L:
                out[pid] = L
        _cache["procs"] = out
    return _cache["procs"]


def scan():
    """({value: [setter proc]}, {value: [(proc, condition text)]}, {proc: callers})."""
    if "scan" in _cache:
        return _cache["scan"]
    sets, tests, callers = collections.defaultdict(list), collections.defaultdict(list), collections.defaultdict(set)
    for pid, L in procs().items():
        for _, t in L:
            m = re.match(r"proc_875 (\d+)", t)
            if m:
                sets[int(m.group(1))].append(pid)
            for m in re.finditer(r"proc_(\d+)", t):
                callers[int(m.group(1))].add(pid)
            if VAR in t and not t.startswith(VAR + " ="):
                for m in re.finditer(re.escape(VAR) + r"\)? ?(<|<=|==|>=|>|!=) ?(\d+)", t):
                    tests[int(m.group(2))].append((pid, t))
    _cache["scan"] = (sets, tests, callers)
    return _cache["scan"]


def places():
    """{proc: (file id, scene entry or None, area or None)}."""
    if "places" in _cache:
        return _cache["places"]
    import gclfiles
    import areas
    fof = gclfiles.file_of()
    fk = {f["id"]: f for f in gclfiles.files()}
    sa = {int(k): v for k, v in areas.build()["scene_area"].items()}
    out = {}
    for p, fid in fof.items():
        f = fk.get(fid, {})
        e = f.get("entry") if f.get("kind") == "scene" else None
        out[p] = (fid, e, sa.get(e) if e is not None else None)
    _cache["places"] = out
    return out


def cutscene_of():
    """{proc: cutscene index} for starters, onfinish procedures and what they call."""
    import cutscenes
    out = {}
    for s in cutscenes.scan()["starts"]:
        for p in (s["proc"], s["onfinish"]):
            if p:
                out.setdefault(p, s["event"])
    return out


def describe_test(t):
    t = t.replace(VAR, "story")
    return re.sub(r"\s+", " ", t)[:140]


def beats():
    import cutscenes
    sets, tests, callers = scan()
    pl = places()
    cut = cutscene_of()
    tx = gcx.text_excerpts()
    out = []
    for v in sorted(set(sets) | set(tests)):
        setters = []
        for p in sorted(set(sets.get(v, []))):
            fid, scene, area = pl.get(p, (None, None, None))
            # the cutscene: this procedure ends or starts one, or is called by one that does
            c = cut.get(p)
            if c is None:
                c = next((cut[q] for q in sorted(callers.get(p, ())) if q in cut), None)
            line = None
            if c is not None:
                st, rows = cutscenes.timeline(c)
                said = [m for _, row in rows for m in row if "text" in m]
                if said:
                    line = said[0]["text"]
            if line is None:
                for _, t in procs().get(p, []):
                    m = re.search(r"text#(\d+)", t)
                    if m:
                        line = int(m.group(1))
                        break
            setters.append({"proc": p, "file": fid, "scene": scene, "area": area, "cutscene": c,
                            "text": line, "excerpt": tx.get(line) if line is not None else None})
        uses = []
        for p, t in tests.get(v, []):
            fid, scene, area = pl.get(p, (None, None, None))
            uses.append({"proc": p, "file": fid, "scene": scene, "area": area, "test": describe_test(t)})
        out.append({"value": v, "setters": setters, "tests": uses})
    return out


def main():
    a = sys.argv[1:]
    bs = beats()
    if "--json" in a:
        print(json.dumps(bs, ensure_ascii=False, indent=1))
        return
    if a:
        v = int(a[0])
        for b in bs:
            if b["value"] == v:
                print(json.dumps(b, ensure_ascii=False, indent=1))
        return
    for b in bs:
        s = b["setters"][0] if b["setters"] else {}
        where = f"scene {s['scene']} (area {s['area']})" if s.get("scene") else (s.get("file") or "-")
        what = f"cutscene {s['cutscene']}" if s.get("cutscene") is not None else ""
        ex = f" \"{s['excerpt']}\"" if s.get("excerpt") else ""
        print(f"{b['value']:4d}  set {len(b['setters'])}x in {where:24s} {what:13s}{ex[:60]:60s} tested {len(b['tests'])}x")


if __name__ == "__main__":
    main()
