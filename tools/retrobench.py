#!/usr/bin/env python3
"""A matching benchmark built from functions that already match, so every answer is known.

  retrobench.py make NAME [--n 24] [--seed S] [--max-edits 3] [--min-insns 20] [--max-insns 400]
                                  tasks: matched, pin-free functions the causebook was not built from,
                                  each broken by 1..max-edits random single edits that still compile
                                  -> build/retrobench/NAME/FUNC/{draft.c,meta.json,answer.c}
  retrobench.py sandbox NAME DIR  a copy of the repo for the agents: no .git, no answers, and each
                                  task's function removed from its unit in src/
  retrobench.py check TASKDIR [--hint]
                                  the agent's only compile: tools/check.py (or tools/hint.py) on
                                  TASKDIR/draft.c, logged to TASKDIR/checks.log, refused after the cap
  retrobench.py score NAME [--arm ARM]
                                  per task: matched?, checks used, differing bytes; and the totals

The edits are the causebook's own vocabulary (tools/causebook.py edits), applied in
sequence. Run the tasks of one arm in one sandbox, copy task directories per
arm (`cp -r build/retrobench/NAME DIR/bench/ARM`) so arms never share drafts.
"""
import argparse
import json
import os
import random
import re
import shutil
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import causebook as atlas  # noqa: E402
from romlib import ROOT  # noqa: E402

BENCH = os.path.join(ROOT, "build", "retrobench")
CAP = 10


def replace_definition(text, func, new):
    import pinfarm
    for name, item in pinfarm.definitions(text):
        if name == func:
            return text.replace(item, new, 1)
    return None


def make_task(job):
    unit, func, k, seed, out_dir = job
    import fixsearch
    import isolate
    from src import ast_util
    try:
        rel = isolate.ensure(unit, func)
        iso_text = open(os.path.join(ROOT, rel)).read()
        rel, ast, fn, prefix = fixsearch.prepare(rel, func)
        rom = atlas.init_compiler(rel, func)
        if atlas.texts_of(prefix + ast_util.to_c(fn)) != rom:
            return func, "base does not match"
        rng = random.Random(f"{seed}:{func}")
        cur, applied = fn, []
        for _ in range(k):
            ast.ext[-1] = cur
            cands = list(atlas.edits(cur, ast, func, seeds=2).items())
            rng.shuffle(cands)
            for t, (etype, desc, f2) in cands:
                mine = atlas.texts_of(prefix + t)
                if mine is not None and mine != rom:
                    cur = f2
                    applied.append(dict(edit=etype, desc=desc))
                    break
        if not applied:
            return func, "no breaking edit"
        mine = atlas.texts_of(prefix + ast_util.to_c(cur))
        orig_def = next(item for name, item in __import__("pinfarm").definitions(iso_text) if name == func)
        draft = replace_definition(iso_text, func, ast_util.to_c(cur).strip() + "\n")
        d = os.path.join(out_dir, func)
        os.makedirs(d, exist_ok=True)
        open(os.path.join(d, "draft.c"), "w").write(draft)
        open(os.path.join(d, "answer.c"), "w").write(orig_def)
        json.dump(dict(func=func, unit=unit, edits=applied, insns=len(rom),
                       regions=len(atlas.regions(rom, mine))), open(os.path.join(d, "meta.json"), "w"), indent=1)
        return func, None
    except Exception as e:
        return func, f"{type(e).__name__}: {str(e)[:80]}"


def cmd_make(a):
    import multiprocessing
    mined = {r["func"] for r in atlas.load()} if os.path.exists(atlas.PAIRS) else set()
    funcs = [x for x in atlas.clean_functions(a.min_insns, a.max_insns) if x[1] not in mined]
    rng = random.Random(a.seed)
    rng.shuffle(funcs)
    out_dir = os.path.join(BENCH, a.name)
    os.makedirs(out_dir, exist_ok=True)
    made = 0
    jobs = [(u, f, 1 + i % a.max_edits, a.seed, out_dir) for i, (u, f, _) in enumerate(funcs[:a.n * 2])]
    with multiprocessing.get_context("fork").Pool(os.cpu_count(), maxtasksperchild=1) as pool:
        for func, err in pool.imap(make_task, jobs):
            if err:
                print(f"  skip {func}: {err}")
                continue
            made += 1
            if made >= a.n:
                pool.terminate()
                break
    tasks = sorted(os.listdir(out_dir))
    for t in tasks[a.n:]:
        shutil.rmtree(os.path.join(out_dir, t))
    print(f"{min(made, a.n)} tasks in {os.path.relpath(out_dir, ROOT)}")


def cmd_sandbox(a):
    tasks = [json.load(open(os.path.join(BENCH, a.name, t, "meta.json"))) for t in sorted(os.listdir(os.path.join(BENCH, a.name)))]
    dst = os.path.abspath(a.dir)
    if os.path.exists(dst):
        sys.exit(f"{dst} exists")
    skip = {".git", "causebook", "retrobench", "isolated", "hint", "fixsearch", "candidates", "foundry", "why",
            "rtlprobe", "bigcode"}
    shutil.copytree(ROOT, dst, symlinks=True, ignore=lambda d, names: [n for n in names if n in skip])
    for m in tasks:
        path = os.path.join(dst, m["unit"])
        text = open(path).read()
        new = replace_definition(text, m["func"], f"/* {m['func']}: removed for the benchmark */\n")
        if new is None:
            sys.exit(f"{m['func']} not found in {m['unit']}")
        open(path, "w").write(new)
        # matched C can be quoted in notes and docs
        for d, _, fs in os.walk(dst):
            if "/build" in d or "/src" in d:
                continue
            for f in fs:
                if f.endswith(".md"):
                    p = os.path.join(d, f)
                    try:
                        s = open(p).read()
                    except Exception:
                        continue
                    if m["func"] + "(" in s:
                        open(p, "w").write(s.replace(m["func"], "<removed>"))
    print(f"sandbox {dst}: {len(tasks)} functions removed")


def cmd_check(a):
    d = os.path.abspath(a.taskdir)
    meta = json.load(open(os.path.join(d, "meta.json")))
    log = os.path.join(d, "checks.log")
    used = sum(1 for _ in open(log)) if os.path.exists(log) else 0
    if used >= CAP:
        print(f"refused: the cap of {CAP} checks is used up")
        return 3
    tool = ["tools/hint.py", os.path.join(d, "draft.c"), meta["func"]] if a.hint else \
        ["tools/check.py", os.path.join(d, "draft.c")]
    r = subprocess.run([sys.executable] + tool, cwd=os.getcwd(), capture_output=True, text=True)
    out = (r.stdout + r.stderr)[-6000:]
    with open(log, "a") as f:
        f.write(json.dumps(dict(n=used + 1, tool="hint" if a.hint else "check", rc=r.returncode)) + "\n")
    print(out)
    print(f"[check {used + 1} of {CAP}]")
    return r.returncode


def cmd_score(a):
    root = os.path.join(BENCH, a.name) if a.arm is None else a.arm
    rows = []
    for t in sorted(os.listdir(root)):
        d = os.path.join(root, t)
        if not os.path.exists(os.path.join(d, "meta.json")):
            continue
        meta = json.load(open(os.path.join(d, "meta.json")))
        r = subprocess.run([sys.executable, "tools/check.py", os.path.join(d, "draft.c"), "--quiet"],
                           cwd=ROOT, capture_output=True, text=True)
        log = os.path.join(d, "checks.log")
        used = sum(1 for _ in open(log)) if os.path.exists(log) else 0
        m = re.search(r"(\d+) (?:bytes|halfwords) differ", r.stdout + r.stderr)
        rows.append((t, r.returncode == 0, used, m.group(1) if m else "", meta["insns"], len(meta["edits"])))
    for t, ok, used, diff, n, k in rows:
        print(f"{t:40} {'MATCH' if ok else 'no':5} checks {used:2}  insns {n:4}  edits {k}  {diff}")
    print(f"matched {sum(r[1] for r in rows)}/{len(rows)}")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    m = sub.add_parser("make")
    m.add_argument("name")
    m.add_argument("--n", type=int, default=24)
    m.add_argument("--seed", type=int, default=11)
    m.add_argument("--max-edits", type=int, default=3)
    m.add_argument("--min-insns", type=int, default=20)
    m.add_argument("--max-insns", type=int, default=400)
    s = sub.add_parser("sandbox")
    s.add_argument("name")
    s.add_argument("dir")
    c = sub.add_parser("check")
    c.add_argument("taskdir")
    c.add_argument("--hint", action="store_true")
    sc = sub.add_parser("score")
    sc.add_argument("name")
    sc.add_argument("--arm")
    a = ap.parse_args()
    if a.cmd in ("make", "score"):
        os.chdir(ROOT)
    import permute
    sys.path.insert(0, permute.PERMUTER)
    return {"make": cmd_make, "sandbox": cmd_sandbox, "check": cmd_check, "score": cmd_score}[a.cmd](a)


if __name__ == "__main__":
    sys.exit(main())
