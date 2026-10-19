#!/usr/bin/env python3
"""Integrate verified unitize.py units into main in batches: one commit per unit, a
combined ROM build per batch, then a PR that is merged when the build is OK.

  unit_integrate.py [--batch 10] [--loop MINUTES] [--state DIR] [--dry-run]

Run it from its own worktree (with its own gen/): it builds, and the unit
worktrees share the gen/ of the checkout unitize.py runs from. --state is that
checkout's build/unitize/.

A unit's own changes are taken from its worktree commit (unit/unit-SLUG), limited
to src/SLUG.c, include/ (its own header and fields it added to shared ones), the
src/fn/ files it removed, symbols/ and notes/ (shared docs would collide).
They are applied on a fresh branch from origin/main; a unit that no longer
applies is marked "conflict" for a later re-run. If the combined build fails,
the batch is split in half and each half is tried on its own. State goes in
build/unitize/SLUG.json ("integrated": PR number). With --loop the script
repeats every MINUTES until no verified units are left and unitize.py is idle.
"""
import argparse
import glob
import json
import os
import subprocess
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from romlib import ROOT  # noqa: E402

OUT = os.path.join(ROOT, "build", "unitize")
JOBS = 3          # build parallelism: the machine has 10 cores and 16 GB shared with workers


def git(*a, check=False, inp=None):
    r = subprocess.run(["git", *a], cwd=ROOT, capture_output=True, text=True, input=inp)
    if check and r.returncode:
        raise RuntimeError(f"git {' '.join(a)}: {r.stderr.strip()}")
    return r


def pending():
    out = []
    for f in sorted(os.listdir(OUT)) if os.path.isdir(OUT) else []:
        if f.endswith(".json"):
            st = json.load(open(os.path.join(OUT, f)))
            if st.get("state") == "verified" and not st.get("integrated") and not st.get("conflict"):
                out.append(st)
    return out


def save(st):
    json.dump(st, open(os.path.join(OUT, st["slug"] + ".json"), "w"), indent=1)


def own_patch(st):
    """The unit's own changes, without its src/fn/ deletions (returned separately: main may have
    edited those files since, e.g. renamed callees, and the unit replaces them whatever they hold)."""
    b, s = st["branch"], st["slug"]
    status = [l.split("\t") for l in git("diff", "--name-status", f"{b}~1", b).stdout.splitlines()]
    deleted = [f for k, f, *_ in status if k == "D" and f.startswith("src/fn/")]
    keep = [f for k, f, *_ in status if f not in deleted and
            (f == f"src/{s}.c" or f.startswith(("include/", "src/fn/", "symbols/", "notes/")))]
    return (git("diff", "--binary", f"{b}~1", b, "--", *keep).stdout if keep else ""), deleted


def current_names():
    """sub_XXXXXXXX -> name, from main's symbols/functions.csv."""
    import csv
    return {f"sub_{r['addr'].upper()}": r["name"] for r in csv.DictReader(open(os.path.join(ROOT, "symbols", "functions.csv")))}


def refresh(st):
    """Bring the unit up to date with main: renamed callees, and functions main has matched since
    the unit's base (their C moves from src/fn/ into the unit, replacing its INCLUDE_ASM)."""
    import re
    s = st["slug"]
    names = current_names()
    paths = [os.path.join(ROOT, p) for p in (f"src/{s}.c", f"include/{s}.h") if os.path.exists(os.path.join(ROOT, p))]
    for p in paths:
        text = open(p).read()
        text = re.sub(r"\bsub_[0-9A-F]{8}\b", lambda m: names.get(m.group(0), m.group(0)), text)
        open(p, "w").write(text)
    unit = os.path.join(ROOT, "src", f"{s}.c")
    if not os.path.exists(unit):
        return
    text = open(unit).read()
    for m in list(re.finditer(r'^INCLUDE_ASM\("[^"]*",\s*(\w+)\);\n', text, re.M)):
        fn = m.group(1)
        hits = [p for p in glob.glob(os.path.join(ROOT, "src", "fn", "*.c"))
                if re.search(r"^[^\n;]*\b" + re.escape(fn) + r"\s*\([^;]*\)\s*\{", open(p).read(), re.M)]
        if len(hits) == 1:
            body = open(hits[0]).read()
            body = "\n".join(l for l in body.split("\n") if not l.startswith("#include"))
            text = text.replace(m.group(0), body.strip() + "\n", 1)
            git("rm", "-q", os.path.relpath(hits[0], ROOT))
    open(unit, "w").write(text)


def build_ok():
    subprocess.run(["make", "format"], cwd=ROOT, capture_output=True)
    r = subprocess.run(["make", f"-j{JOBS}"], cwd=ROOT, capture_output=True, text=True)
    open(os.path.join(ROOT, "build", "integrate-build.log"), "w").write(r.stdout + r.stderr)
    return "build/boktai3.gba: OK" in r.stdout + r.stderr


def attempt(batch, dry):
    """Apply BATCH on a fresh branch from origin/main; build; return the units that applied, or None."""
    git("fetch", "-q", "origin")
    git("reset", "-q", "--hard")                          # leftovers of a failed attempt
    git("clean", "-fdq", "src", "include", "notes")
    name = f"claude/units-{int(time.time())}"
    git("checkout", "-q", "-B", name, "origin/main", check=True)
    applied = []
    for st in batch:
        p, deleted = own_patch(st)
        r = git("apply", "-3", "--whitespace=nowarn", inp=p) if p else None
        if p and not r.returncode:
            for f in deleted:
                if os.path.exists(os.path.join(ROOT, f)):
                    git("rm", "-q", f)
            refresh(st)
        if not p or r.returncode or git("diff", "--name-only", "--diff-filter=U").stdout.strip():
            git("reset", "-q", "--hard")                  # drop this unit's partial apply only
            st["conflict"] = True
            save(st)
            print(f"  {st['slug']}: does not apply on main, marked conflict")
            continue
        git("add", "-A", "src", "include", "symbols", "notes")
        git("commit", "-q", "-m", f"Unit {st['slug']}: merge into src/{st['slug']}.c with shared types "
            f"(tools/unitize.py, {st.get('backend', 'codex')})\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>")
        applied.append(st)
    if not applied:
        return [], name
    if dry:
        print(f"  dry run: {len(applied)} units apply")
        return applied, name
    if not build_ok():
        tail = open(os.path.join(ROOT, "build", "integrate-build.log")).read()[-1500:]
        print("  build failed:\n" + "\n".join("    " + l for l in tail.splitlines()[-12:]))
        return None, name
    if git("status", "--porcelain", "--", "src", "include").stdout.strip():      # make format changes
        git("add", "-A", "src", "include")
        git("commit", "-q", "-m", "make format\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>")
    subprocess.run([sys.executable, "tools/progress.py", "--markdown", "PROGRESS.md"], cwd=ROOT, capture_output=True)
    git("add", "PROGRESS.md")
    git("commit", "-q", "-m", "Regenerate PROGRESS.md\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>")
    git("-c", "http.postBuffer=524288000", "push", "-q", "-u", "origin", name, check=True)
    body = ("Subsystem units from `tools/unitize.py`. Each merges its `src/fn/` files into one unit with "
            "shared types, and each was verified in its own worktree and again here in a combined build "
            "(`build/boktai3.gba: OK`).\n\n" + "\n".join(f"- {s['name']} (`{s['slug']}`)" for s in applied)
            + "\n\n🤖 Generated with [Claude Code](https://claude.com/claude-code)")
    r = subprocess.run(["gh", "pr", "create", "--base", "main", "--head", name, "--title",
                        f"{len(applied)} subsystem units: {', '.join(s['slug'] for s in applied[:3])}"
                        + ("..." if len(applied) > 3 else ""), "--body", body], cwd=ROOT, capture_output=True, text=True)
    url = r.stdout.strip().split("\n")[-1]
    m = subprocess.run(["gh", "pr", "merge", "--merge", url], cwd=ROOT, capture_output=True, text=True)
    if m.returncode:
        print(f"  merge failed: {m.stderr.strip()}")
        return None, name
    for st in applied:
        st["integrated"] = url
        save(st)
    print(f"  merged {url}: {len(applied)} units")
    return applied, name


def integrate(batch, dry):
    got, _ = attempt(batch, dry)
    if got is None and len(batch) > 1:                    # build failed: split
        h = len(batch) // 2
        integrate(batch[:h], dry)
        integrate(batch[h:], dry)
    elif got is None:
        batch[0]["conflict"] = True
        save(batch[0])
        print(f"  {batch[0]['slug']}: fails the combined build alone, marked conflict")


def unitize_running():
    return subprocess.run(["pgrep", "-f", "unitize.py run"], capture_output=True).returncode == 0


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--batch", type=int, default=10)
    ap.add_argument("--loop", type=float)
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--state", help="unitize.py's state folder (default build/unitize here)")
    a = ap.parse_args()
    global OUT
    OUT = a.state or OUT
    while True:
        todo = pending()
        while len(todo) >= (a.batch if a.loop and unitize_running() else 1):
            print(f"batch of {min(a.batch, len(todo))} ({len(todo)} pending)", flush=True)
            integrate(todo[:a.batch], a.dry_run)
            if a.dry_run:
                return
            todo = pending()
        if not a.loop or (not unitize_running() and not pending()):
            break
        time.sleep(a.loop * 60)


if __name__ == "__main__":
    main()
