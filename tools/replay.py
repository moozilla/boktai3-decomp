#!/usr/bin/env python3
"""The spelling library's test: replay real draft-to-match fixes with fix.py.

  replay.py [--only FUNC ...] [--limit N] [--budget S] [--depth N] [-j N] [--pinned]
  replay.py report [FILE]

Every case from tools/harvest.py is a draft whose matching C is known. For
each one, the draft's function is put back into today's source in place of
the matched one (an isolated copy, tools/isolate.py), and tools/fix.py runs
on it with the library alone: tools/spellings.py and tools/rewrites.py, no
permuter seeds, so every step is a named, deterministic rewrite. A case is

  solved     fix.py reaches matching code (the path names the rewrites used);
  closer     it removes some of the difference but not all;
  stuck      no rewrite in the library helps: a technique to add;
  as-is      the draft already matches today (the fix was outside the function);
  stale      the draft no longer compiles against today's headers.

Results go to build/harvest/replay.jsonl (one line per case, appended, cases
already there are skipped); `report` summarizes them by outcome, by the
decision step.py named, and by the rewrites that solved. Matches with register
pins are skipped unless --pinned: pins are not something fix.py writes.
"""
import argparse
import collections
import json
import os
import re
import sys
import tempfile
import time
from concurrent.futures import ProcessPoolExecutor

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import harvest  # noqa: E402
from romlib import ROOT  # noqa: E402

OUT = os.path.join(ROOT, "build", "harvest", "replay.jsonl")
WORK = os.path.join(ROOT, "build", "harvest", "work")


def current_homes():
    """{key: (path, name)} for every function defined as plain C in today's src/."""
    import glob
    homes = {}
    for path in glob.glob(os.path.join(ROOT, "src", "**", "*.c"), recursive=True):
        _, matched = harvest.file_facts(open(path).read())
        for key, (name, _) in matched.items():
            homes.setdefault(key, (os.path.relpath(path, ROOT), name))
            homes.setdefault(name, (os.path.relpath(path, ROOT), name))
    return homes


def rom_label(text, name):
    alias = re.search(r"^#define\s+" + re.escape(name) + r"\s+(sub_[0-9A-F]{8})\s*$", text, re.M)
    return alias.group(1) if alias else name


def build_case(case, homes):
    """(isolated source with the draft in place, ROM label, today's name) or a reason string."""
    import isolate
    home = homes.get(case["key"]) or homes.get(case["name"])
    if not home:
        return "gone"
    path, name = home
    text = open(os.path.join(ROOT, path)).read()
    os.makedirs(WORK, exist_ok=True)
    iso = os.path.join(WORK, f"{case['key']}.c")
    isolate.isolate(text, name, iso, False)
    iso_text = open(iso).read()
    span = harvest.definitions(iso_text).get(name)
    if span is None:
        return "no definition"
    start = iso_text.index(span)
    draft = case["draft"]
    dname = harvest.DEF.match(draft.lstrip())
    if dname and dname.group(1) != name:
        draft = re.sub(r"\b" + re.escape(dname.group(1)) + r"(\s*\()", name + r"\1", draft, count=1)
    out = iso_text[:start] + draft + iso_text[start + len(span):]
    open(iso, "w").write(out)
    return iso, rom_label(iso_text, name), name


def replay_one(job):
    case, homes, depth, budget = job
    import fix
    t0 = time.time()
    built = build_case(case, homes)
    row = {"key": case["key"], "name": case["name"], "match_commit": case["match_commit"][:10]}
    if isinstance(built, str):
        row.update(outcome="stale", why=built)
        return row
    iso, label, name = built
    lines = []
    log = lines.append
    with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, "build")) as wd:
        try:
            d = fix.Draft(open(iso).read(), label, wd)
            start = d.score()
        except (Exception, SystemExit) as e:
            row.update(outcome="stale", why=f"{type(e).__name__}: {str(e)[:200]}")
            return row
    if start is None:
        row.update(outcome="stale", why="draft does not compile")
        return row
    row["start"] = start[0]
    if start[0] == 0:
        row.update(outcome="as-is", seconds=round(time.time() - t0, 1))
        return row
    try:
        rc, path, compiles = fix.run(iso, label, depth=depth, seeds=0, log=log, budget=budget)
    except (Exception, SystemExit) as e:
        row.update(outcome="stale", why=f"fix.py: {type(e).__name__}: {str(e)[:200]}")
        return row
    best = re.search(r"best (\d+) differing", lines[-1] if lines else "")
    decisions = [ln.strip()[len("decision "):].split(" (line")[0] for ln in lines if ln.strip().startswith("decision ")]
    row.update(outcome="solved" if rc == 0 else ("closer" if best and int(best.group(1)) < start[0] else "stuck"),
               best=0 if rc == 0 else (int(best.group(1)) if best else start[0]),
               path=path, compiles=compiles, seconds=round(time.time() - t0, 1),
               decision=decisions[0] if decisions else None)
    return row


def report(path):
    rows = [json.loads(ln) for ln in open(path)]
    by = collections.Counter(r["outcome"] for r in rows)
    print(f"{len(rows)} cases: " + ", ".join(f"{k} {v}" for k, v in by.most_common()))
    tried = [r for r in rows if r["outcome"] in ("solved", "closer", "stuck")]
    if tried:
        cpu = sum(r.get("seconds", 0) for r in tried) / 3600
        print(f"solved {by['solved']} of {len(tried)} that fix.py ran on ({cpu:.2f} CPU hours)")
    fam = collections.Counter()
    for r in rows:
        if r["outcome"] == "solved":
            for p in r["path"]:
                fam[p.split("]")[0].lstrip("[")] += 1
    if fam:
        print("rewrites on solved paths: " + ", ".join(f"{k} {v}" for k, v in fam.most_common()))
    dec = collections.defaultdict(collections.Counter)
    for r in tried:
        dec[(r.get("decision") or "?").split(",")[0]][r["outcome"]] += 1
    print("by first decision:")
    for k, c in sorted(dec.items(), key=lambda kv: -sum(kv[1].values())):
        print(f"  {k:24} " + ", ".join(f"{o} {n}" for o, n in c.most_common()))


def main():
    if sys.argv[1:2] == ["report"]:
        report(sys.argv[2] if len(sys.argv) > 2 else OUT)
        return
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--only", nargs="*")
    ap.add_argument("--limit", type=int, default=0)
    ap.add_argument("--budget", type=float, default=120)
    ap.add_argument("--depth", type=int, default=4)
    ap.add_argument("-j", type=int, default=max(1, (os.cpu_count() or 2) - 1))
    ap.add_argument("--pinned", action="store_true")
    ap.add_argument("--out", default=OUT)
    ap.add_argument("--redo", action="store_true", help="replay cases already in --out again")
    a = ap.parse_args()
    cases = harvest.load()
    if not a.pinned:
        cases = [c for c in cases if not c["pinned"]]
    if a.only:
        cases = [c for c in cases if c["key"] in a.only or c["name"] in a.only]
    done = set()
    if os.path.exists(a.out) and not a.redo:
        done = {json.loads(ln)["key"] for ln in open(a.out)}
    cases = [c for c in cases if c["key"] not in done]
    if a.limit:
        cases = cases[:a.limit]
    homes = current_homes()
    print(f"{len(cases)} cases to replay", flush=True)
    os.makedirs(os.path.dirname(a.out), exist_ok=True)
    with ProcessPoolExecutor(a.j) as pool, open(a.out, "a") as fh:
        for row in pool.map(replay_one, [(c, homes, a.depth, a.budget) for c in cases]):
            fh.write(json.dumps(row) + "\n")
            fh.flush()
            print(f"{row['key']:40} {row['outcome']:7} {row.get('start', '')}->{row.get('best', '')} "
                  f"{' '.join(row.get('path', []))[:120]}", flush=True)
    report(a.out)


if __name__ == "__main__":
    main()
