#!/usr/bin/env python3
"""A work queue for cloud workers, coordinated through git branches (no server).

  cloud_task.py list [--min-insns 150]      open tasks, biggest first
  cloud_task.py claim [--min-insns 150]     claim the next open task: prints its brief
  cloud_task.py claim --task ID             claim that open task (e.g. one with smaller functions)
  cloud_task.py list --smallest             order by the largest function, smallest first
  cloud_task.py list --from-end             order by address, end of the ROM first
  cloud_task.py list --max-insns 150        only functions below 150 instructions
  cloud_task.py list --fresh                most functions without a NONMATCHING draft first
  cloud_task.py show TASK                   a task's functions

A task is the unmatched functions of at least --min-insns instructions inside one
subsystem range (symbols/progress_subsystems.csv), or a run of up to 6 such
functions where no range is recorded. Its id is its first address. Claiming pushes
a commit unique to this worker to refs/heads/cloud/TASK; a second worker's push to
the same name is rejected (not a fast-forward), so each task has one owner. The
claim commit lists the task's functions, and a task overlapping any function of an
open (unmerged) claim is skipped: the loose runs of 6 shift as origin/main changes,
so task ids alone would let two workers take the same function. A task whose
functions are all matched on origin/main drops out of the list. Work happens
on that branch; finish with a PR (docs/CLOUD.md).
"""
import argparse
import csv
import os
import re
import signal
import subprocess
import sys
import uuid

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from romlib import ROOT  # noqa: E402
import worklist  # noqa: E402


def git(*a):
    return subprocess.run(["git", *a], cwd=ROOT, capture_output=True, text=True)


def drafted():
    """Names (sub_ or csv) of functions that already have a NONMATCHING draft in src/."""
    out = set()
    for d, _, files in os.walk(os.path.join(ROOT, "src")):
        for fn in files:
            if not fn.endswith(".c"):
                continue
            s = open(os.path.join(d, fn), errors="ignore").read()
            if "NONMATCHING" in s:
                out.update(re.findall(r'INCLUDE_ASM\("asm/nonmatching",\s*(\w+)\)', s))
    names = {r[1]: r[0].upper() for r in csv.reader(open(os.path.join(ROOT, "symbols", "functions.csv")))
             if len(r) > 1 and re.fullmatch(r"[0-9A-Fa-f]{8}", r[0])}
    return out | {f"sub_{names[n]}" for n in out if n in names}


def tasks(min_insns, max_insns=None):
    done = worklist.done_names()
    # By address too: gen/code_sym.s in a fresh worktree can predate names promoted on main.
    done_addrs = {int(n[4:], 16) for n in done if re.fullmatch(r"sub_[0-9A-F]{8}", n)}
    done_addrs |= {int(r[0], 16) for r in csv.reader(open(os.path.join(ROOT, "symbols", "functions.csv")))
                   if len(r) > 1 and r[1] in done and re.fullmatch(r"[0-9A-Fa-f]{8}", r[0])}
    fs = [f for f in worklist.functions() if f["name"] not in done and f["addr"] not in done_addrs and f["insns"] >= min_insns
          and (max_insns is None or f["insns"] < max_insns)
          and not worklist.in_library(f["addr"]) and f["addr"] >= 0x08000350]
    subs = [(int(r["start"], 16), int(r["end"], 16), r["name"])
            for r in csv.DictReader(open(os.path.join(ROOT, "symbols", "progress_subsystems.csv")))]
    groups = {}
    loose = []
    for f in fs:
        s = next((s for s in subs if s[0] <= f["addr"] < s[1]), None)
        if s:
            groups.setdefault(s, []).append(f)
        else:
            loose.append(f)
    out = [{"id": f"{fl[0]['addr']:08X}", "name": s[2], "functions": fl} for s, fl in groups.items()]
    for i in range(0, len(loose), 6):
        fl = loose[i:i + 6]
        out.append({"id": f"{fl[0]['addr']:08X}", "name": "unassigned functions", "functions": fl})
    return sorted(out, key=lambda t: -sum(f["insns"] for f in t["functions"]))


def claimed():
    r = git("ls-remote", "--heads", "origin", "refs/heads/cloud/*")
    return {l.split("refs/heads/cloud/")[1] for l in r.stdout.splitlines() if "refs/heads/cloud/" in l}


def claimed_functions():
    """Function addresses listed in claim commits of cloud/* branches not yet merged into main."""
    git("fetch", "-q", "origin", "+refs/heads/cloud/*:refs/remotes/origin/cloud/*")
    r = git("log", "--format=%B", "--grep=^Claim cloud task", "--remotes=origin/cloud/*", "--not", "origin/main")
    return {a for l in r.stdout.splitlines() if l.startswith("Functions:") for a in l.split()[1:]}


def brief(t):
    lines = [f"Task {t['id']}: {t['name']}, {len(t['functions'])} unmatched functions "
             f"({sum(f['insns'] for f in t['functions'])} instructions). Branch: cloud/{t['id']}.", ""]
    lines += [f"  {f['addr']:08X} {f['name']} ({f['insns']} instructions)" for f in t["functions"]]
    return "\n".join(lines)


def main():
    signal.signal(signal.SIGPIPE, signal.SIG_DFL)  # `list | head` exits quietly
    ap = argparse.ArgumentParser()
    sp = ap.add_subparsers(dest="cmd", required=True)
    for c in ("list", "claim"):
        p = sp.add_parser(c)
        p.add_argument("--min-insns", type=int, default=150)
        p.add_argument("--unassigned", action="store_true",
                       help="only functions outside subsystem ranges (local workers take the ranges)")
        p.add_argument("--task", help="claim/list only this task id (ids depend on --min-insns/--max-insns: pass the same filters as the list that showed it)")
        p.add_argument("--smallest", action="store_true",
                       help="order tasks by their largest function, smallest first")
        p.add_argument("--max-insns", type=int, help="only functions below this size")
        p.add_argument("--fresh", action="store_true",
                       help="order by functions without an existing NONMATCHING draft, most first")
        p.add_argument("--from-end", action="store_true",
                       help="order tasks by address, highest first (stays clear of workers going forwards)")
    p = sp.add_parser("show")
    p.add_argument("task")
    a = ap.parse_args()
    git("fetch", "-q", "origin")
    if a.cmd == "show":
        t = next((t for t in tasks(0) if t["id"] == a.task.upper()), None)
        print(brief(t) if t else "no such open task")
        return
    taken = claimed()
    busy = claimed_functions()
    open_ = [t for t in tasks(a.min_insns, a.max_insns) if t["id"] not in taken
             and not busy & {f"{f['addr']:08X}" for f in t["functions"]}
             and (not a.unassigned or t["name"] == "unassigned functions")
             and (not a.task or t["id"] == a.task.upper())]
    if a.smallest:
        open_.sort(key=lambda t: max(f["insns"] for f in t["functions"]))
    if a.from_end:
        open_.sort(key=lambda t: -int(t["id"], 16))
    if a.fresh:
        dr = drafted()
        for t in open_:
            t["fresh"] = sum(f["name"] not in dr and f"sub_{f['addr']:08X}" not in dr for f in t["functions"])
        open_.sort(key=lambda t: -t["fresh"])  # stable: keeps --from-end order within a count
    if a.cmd == "list":
        for t in open_:
            fresh = f"  {t['fresh']} fresh" if "fresh" in t else ""
            print(f"{t['id']}  {len(t['functions']):3d} functions {sum(f['insns'] for f in t['functions']):6d} insns  {t['name']}{fresh}")
        print(f"{len(open_)} open, {len(taken)} claimed")
        return
    for t in open_:
        git("checkout", "-q", "-B", f"cloud/{t['id']}", "origin/main")
        git("commit", "-q", "--allow-empty", "-m", f"Claim cloud task {t['id']} ({uuid.uuid4().hex[:8]})",
            "-m", "Functions: " + " ".join(f"{f['addr']:08X}" for f in t["functions"]))
        if git("push", "-q", "origin", f"HEAD:refs/heads/cloud/{t['id']}").returncode == 0:
            print(brief(t))
            return
        git("checkout", "-q", "--detach", "origin/main")      # someone else got it first
    sys.exit("no open tasks" + (" (task ids depend on --min-insns/--max-insns; pass the filters used with list)" if a.task else ""))


if __name__ == "__main__":
    main()
