#!/usr/bin/env python3
"""Apply named spellings to every NONMATCHING draft, one step, deterministically.

  sweep.py [SPELLING ...] [--files SRC ...] [--out FILE] [-j N]
  sweep.py --follow [-j N] [--budget S]

For each draft (tools/split.py's NONMATCHING blocks in src/), the draft is
isolated (tools/isolate.py) and every output of each spelling (default: all of
tools/spellings.py) is compiled and scored against the ROM like tools/fix.py
does. It reports the drafts a spelling matches outright and the ones it brings
closer, and writes each match as a whole isolated source to build/sweep/FUNC.c.
A spelling is a named technique, so a sweep is the library applied everywhere
at once, with no search: run it after adding a spelling. Follow a "closer"
result with tools/fix.py for the remaining divergences.
"""
import argparse
import glob
import json
import os
import sys
import tempfile
import time
from concurrent.futures import ProcessPoolExecutor

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fix  # noqa: E402  (puts the permuter on sys.path)
import isolate  # noqa: E402
import spellings  # noqa: E402
from romlib import ROOT  # noqa: E402
from split import nonmatching_functions  # noqa: E402

OUT = os.path.join(ROOT, "build", "sweep")


def drafts(files):
    for path in files:
        for name, fuzzy in nonmatching_functions(path):
            yield os.path.relpath(path, ROOT), name, fuzzy


def sweep_one(job):
    src, func, fuzzy, names = job
    sps = [sp for sp in spellings.SPELLINGS + spellings.HELPER_SPELLINGS if not names or sp.__name__ in names]
    t0 = time.time()
    row = {"src": src, "func": func, "fuzzy": fuzzy}
    try:
        iso = isolate.ensure(src, func)
        text = open(os.path.join(ROOT, iso)).read()
        with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, "build")) as wd:
            d = fix.Draft(text, func, wd)
            base = d.score()
            row["start"] = base and base[0]
            best = None
            for sp in sps:
                # the function, and (a plain spelling) each inline helper it calls, as fix.py does
                targets = [(None, d.fn)] + ([] if sp in spellings.HELPER_SPELLINGS else list(d.helpers.items()))
                outs = []
                for name, node in targets:
                    try:
                        got = list(sp(d.fn, d.helpers) if sp in spellings.HELPER_SPELLINGS else sp(node))
                    except Exception:
                        continue
                    for desc, n2 in got:
                        if name:  # an edited helper (and any helpers the spelling added)
                            n2 = (d.fn, [n2[0]] + list(n2[1])) if isinstance(n2, tuple) else (d.fn, [n2])
                        outs.append((desc + (f" in {name}" if name else ""), n2))
                for desc, f2 in outs:
                    helpers = {}
                    if isinstance(f2, tuple):
                        f2, extra = f2
                        helpers = {e.decl.name: e for e in extra}
                    sc = d.score(f2, helpers)
                    if sc is None or base is None:
                        continue
                    if sc[0] < base[0] and (best is None or sc[0] < best[0]):
                        best = (sc[0], sp.__name__, desc, d.materialize(f2, helpers))
                    if sc[0] == 0:
                        break
                if best and best[0] == 0:
                    break
            if best:
                row.update(best=best[0], spelling=best[1], desc=best[2])
                name = func + (".c" if best[0] == 0 else ".closer.c")
                open(os.path.join(OUT, name), "w").write(best[3])
    except (Exception, SystemExit) as e:
        row["error"] = f"{type(e).__name__}: {str(e)[:160]}"
    row["seconds"] = round(time.time() - t0, 1)
    return row


def follow_one(job):
    """semcheck the sweep's closer result, then tools/fix.py (library only) from it."""
    row, depth, budget = job
    import subprocess
    out = {"func": row["func"], "src": row["src"], "sweep": row.get("best")}
    path = os.path.join(OUT, row["func"] + ".closer.c")
    venv = os.path.join(ROOT, "build", "venv", "bin", "python")

    def check(src, *flags):
        r = subprocess.run([venv if os.path.exists(venv) else sys.executable, os.path.join(ROOT, "tools", "semcheck.py"),
                            os.path.relpath(src, ROOT), row["func"], *flags], capture_output=True, text=True, cwd=ROOT)
        return (r.stdout.strip().splitlines() or ["?"])[-1]

    verdict = check(path)
    out["semcheck"] = verdict.split(": ")[1].split(" ")[0] if ": " in verdict else verdict
    if "DIFFERENT" in verdict:
        out["semcheck_detail"] = verdict[:300]
        # a spelling must not break a draft that was right; a draft that was
        # already wrong cannot judge it, so carry on and say so
        if "DIFFERENT" not in check(os.path.join(ROOT, row["src"]), "--nonmatching"):
            return out
        out["semcheck"] = "DIFFERENT (as the draft)"
    t0 = time.time()
    lines = []
    try:
        rc, steps, compiles = fix.run(path, row["func"], depth=depth, seeds=0, log=lines.append, budget=budget)
    except (Exception, SystemExit) as e:
        out["error"] = f"{type(e).__name__}: {str(e)[:160]}"
        return out
    out.update(match=rc == 0, path=steps, compiles=compiles, seconds=round(time.time() - t0, 1),
               last=lines[-1][:200] if lines else "")
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("names", nargs="*")
    ap.add_argument("--files", nargs="*")
    ap.add_argument("--out", default=os.path.join(OUT, "sweep.jsonl"))
    ap.add_argument("-j", type=int, default=1)
    ap.add_argument("--follow", action="store_true",
                    help="for each closer result in --out: semcheck it, then tools/fix.py from it "
                         "(-> build/sweep/follow.jsonl; matches in build/fix/FUNC.c)")
    ap.add_argument("--budget", type=float, default=120)
    a = ap.parse_args()
    if a.follow:
        rows = [json.loads(ln) for ln in open(a.out)]
        rows = [r for r in rows if r.get("best")]
        fo = os.path.join(OUT, "follow.jsonl")
        seen = {json.loads(ln)["func"] for ln in open(fo)} if os.path.exists(fo) else set()
        rows = [r for r in rows if r["func"] not in seen]
        rows.sort(key=lambda r: r["best"])
        print(f"{len(rows)} closer drafts to follow", flush=True)
        with ProcessPoolExecutor(a.j) as pool, open(fo, "a") as fh:
            for out in pool.map(follow_one, [(r, 3, a.budget) for r in rows]):
                fh.write(json.dumps(out) + "\n")
                fh.flush()
                print(f"{out['func']:44} {out.get('semcheck', ''):11} "
                      f"{'MATCH' if out.get('match') else out.get('last', out.get('error', ''))[:90]}", flush=True)
        return
    files = a.files or sorted(glob.glob(os.path.join(ROOT, "src", "**", "*.c"), recursive=True))
    os.makedirs(OUT, exist_ok=True)
    done = set()
    if os.path.exists(a.out):
        done = {json.loads(ln)["func"] for ln in open(a.out)}
    jobs = [(src, func, fz, a.names) for src, func, fz in drafts(files) if func not in done]
    print(f"{len(jobs)} drafts", flush=True)
    with ProcessPoolExecutor(a.j) as pool, open(a.out, "a") as fh:
        for row in pool.map(sweep_one, jobs):
            fh.write(json.dumps(row) + "\n")
            fh.flush()
            tag = "MATCH" if row.get("best") == 0 else (f"{row['start']}->{row['best']}" if "best" in row else "")
            print(f"{row['func']:44} {tag:10} {row.get('spelling', '')} {row.get('desc', '')[:70]} "
                  f"{row.get('error', '')[:80]}", flush=True)


if __name__ == "__main__":
    main()
