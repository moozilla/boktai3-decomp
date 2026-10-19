#!/usr/bin/env python3
"""A work queue for cloud workers, coordinated through git branches (no server).

  cloud_task.py list [--min-insns 150]      open tasks, biggest first
  cloud_task.py claim [--min-insns 150]     claim the next open task: prints its brief
  cloud_task.py claim --task ID             claim that open task (e.g. one with smaller functions)
  cloud_task.py list --smallest             order by the largest function, smallest first
  cloud_task.py show TASK                   a task's functions

A task is the unmatched functions of at least --min-insns instructions inside one
subsystem range (symbols/progress_subsystems.csv), or a run of up to 6 such
functions where no range is recorded. Its id is its first address. Claiming pushes
a commit unique to this worker to refs/heads/cloud/TASK; a second worker's push to
the same name is rejected (not a fast-forward), so each task has one owner. A task
whose functions are all matched on origin/main drops out of the list. Work happens
on that branch; finish with a PR (docs/CLOUD.md).
"""
import argparse
import csv
import os
import subprocess
import sys
import uuid

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from romlib import ROOT  # noqa: E402
import worklist  # noqa: E402


def git(*a):
    return subprocess.run(["git", *a], cwd=ROOT, capture_output=True, text=True)


def tasks(min_insns):
    done = worklist.done_names()
    fs = [f for f in worklist.functions() if f["name"] not in done and f["insns"] >= min_insns
          and not worklist.in_library(f["addr"]) and f["addr"] >= 0x0800145C]
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


def brief(t):
    lines = [f"Task {t['id']}: {t['name']}, {len(t['functions'])} unmatched functions "
             f"({sum(f['insns'] for f in t['functions'])} instructions). Branch: cloud/{t['id']}.", ""]
    lines += [f"  {f['addr']:08X} {f['name']} ({f['insns']} instructions)" for f in t["functions"]]
    return "\n".join(lines)


def main():
    ap = argparse.ArgumentParser()
    sp = ap.add_subparsers(dest="cmd", required=True)
    for c in ("list", "claim"):
        p = sp.add_parser(c)
        p.add_argument("--min-insns", type=int, default=150)
        p.add_argument("--unassigned", action="store_true",
                       help="only functions outside subsystem ranges (local workers take the ranges)")
        p.add_argument("--task", help="claim/list only this task id")
        p.add_argument("--smallest", action="store_true",
                       help="order tasks by their largest function, smallest first")
    p = sp.add_parser("show")
    p.add_argument("task")
    a = ap.parse_args()
    git("fetch", "-q", "origin")
    if a.cmd == "show":
        t = next((t for t in tasks(0) if t["id"] == a.task.upper()), None)
        print(brief(t) if t else "no such open task")
        return
    taken = claimed()
    open_ = [t for t in tasks(a.min_insns) if t["id"] not in taken
             and (not a.unassigned or t["name"] == "unassigned functions")
             and (not a.task or t["id"] == a.task.upper())]
    if a.smallest:
        open_.sort(key=lambda t: max(f["insns"] for f in t["functions"]))
    if a.cmd == "list":
        for t in open_:
            print(f"{t['id']}  {len(t['functions']):3d} functions {sum(f['insns'] for f in t['functions']):6d} insns  {t['name']}")
        print(f"{len(open_)} open, {len(taken)} claimed")
        return
    for t in open_:
        git("checkout", "-q", "-B", f"cloud/{t['id']}", "origin/main")
        git("commit", "-q", "--allow-empty", "-m", f"Claim cloud task {t['id']} ({uuid.uuid4().hex[:8]})")
        if git("push", "-q", "origin", f"HEAD:refs/heads/cloud/{t['id']}").returncode == 0:
            print(brief(t))
            return
        git("checkout", "-q", "--detach", "origin/main")      # someone else got it first
    sys.exit("no open tasks")


if __name__ == "__main__":
    main()
