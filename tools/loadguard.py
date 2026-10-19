#!/usr/bin/env python3
"""Keep overnight runs from hanging the machine: pause workers under memory or CPU pressure.

  loadguard.py [--min-free 15] [--resume-free 30] [--max-load 40] [--resume-load 28] [--pace FILE]

With --pace FILE (default ~/git/b3-logs/pace.txt) it also paces plan usage: at most
the number in FILE of headless `claude -p` workers run; the youngest beyond that are
paused and resumed in turn as the number goes up or workers finish. Edit the file
to speed up or slow down; delete it for no limit.

Every few seconds this reads the system's free-memory percentage (memory_pressure -Q)
and the 1-minute load. While free memory is below --min-free or the load is above
--max-load it pauses (SIGSTOP) the newest worker, with its whole process tree; when
both are back under the resume levels it continues (SIGCONT) the oldest paused one.
Workers are `codex exec`, `claude -p`, and this repo's runner scripts. Nothing is
killed, and a paused worker resumes where it was. The machine (16 GB, 10 cores) hung
once when four unit workers, four foundry workers, the integrator and the depin farm
all built at the same time (2026-10-18 watchdog panic).
"""
import argparse
import os
import re
import signal
import subprocess
import time

WORKER = re.compile(r"(codex exec|claude -p|tools/foundry_run\.py|tools/autosolve\.py|tools/rasolve\.py|"
                    r"tools/translate\.py|tools/depin\.py|tools/pin_farm|tools/fixsearch\.py)")


def free_percent():
    out = subprocess.run(["memory_pressure", "-Q"], capture_output=True, text=True).stdout
    m = re.search(r"free percentage: (\d+)%", out)
    return int(m.group(1)) if m else 100


def processes():
    out = subprocess.run(["ps", "-axo", "pid=,ppid=,etime=,command="], capture_output=True, text=True).stdout
    rows = []
    for line in out.splitlines():
        parts = line.split(None, 3)
        if len(parts) == 4:
            rows.append((int(parts[0]), int(parts[1]), seconds(parts[2]), parts[3]))
    return rows


def seconds(etime):
    """ps etime ([[dd-]hh:]mm:ss) in seconds."""
    days, _, rest = etime.rpartition("-")
    f = [int(x) for x in rest.split(":")]
    while len(f) < 3:
        f.insert(0, 0)
    return (int(days) if days else 0) * 86400 + f[0] * 3600 + f[1] * 60 + f[2]


def tree(pid, rows):
    kids = {}
    for p, pp, _, _ in rows:
        kids.setdefault(pp, []).append(p)
    out, todo = [], [pid]
    while todo:
        p = todo.pop()
        out.append(p)
        todo += kids.get(p, [])
    return out


SHELLS = {"sh", "zsh", "bash", "-zsh", "-bash", "login", "env", "nohup", "time", "timeout"}


def is_worker(cmd):
    """A real worker process: never a shell whose command line merely mentions one."""
    exe = os.path.basename(cmd.split()[0]) if cmd.split() else ""
    return exe not in SHELLS and bool(WORKER.search(cmd))


def roots(rows):
    """Worker processes whose parent is not itself a worker (one per job)."""
    by = {p: c for p, _, _, c in rows}
    return [(p, age) for p, pp, age, c in rows if is_worker(c) and not is_worker(by.get(pp, ""))
            and p != os.getpid()]


def signal_tree(pid, sig, rows):
    for p in tree(pid, rows):
        try:
            os.kill(p, sig)
        except ProcessLookupError:
            pass


START = 90      # seconds: a worker younger than this has not done real work yet


def priority(cmd):
    """Matching (decomp progress) first; consolidation-only unit workers (the old brief) second."""
    return 0 if cmd.startswith("claude -p Follow docs/UNIT_WORKER.md") else 1


def pace_limit(path):
    """The number in the pace file (max concurrent `claude -p` workers), or None for no limit."""
    try:
        return int(open(path).read().split()[0])
    except (OSError, ValueError, IndexError):
        return None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--min-free", type=int, default=15)
    ap.add_argument("--resume-free", type=int, default=30)
    ap.add_argument("--max-load", type=float, default=40)
    ap.add_argument("--resume-load", type=float, default=28)
    ap.add_argument("--interval", type=float, default=5)
    ap.add_argument("--pace", default=os.path.expanduser("~/git/b3-logs/pace.txt"),
                    help="file holding the max number of running `claude -p` workers (usage pacing)")
    a = ap.parse_args()
    paused, paced = [], []
    while True:
        free, load = free_percent(), os.getloadavg()[0]
        rows = processes()
        alive = {p for p, _, _, _ in rows}
        paused = [p for p in paused if p in alive]
        paced = [p for p in paced if p in alive]
        limit = pace_limit(a.pace)
        if limit is not None:
            # Usage pacing that wastes nothing: a worker already working is never paused; only one
            # that has just started (younger than START seconds) is held, and held workers are
            # released in priority order (matching before unit consolidation) as others finish.
            by = {p: c for p, _, _, c in rows}
            claude = [(p, age, c) for p, pp, age, c in rows
                      if is_worker(c) and "claude -p" in c and "claude -p" not in by.get(pp, "")]
            running = [x for x in claude if x[0] not in paced and x[0] not in paused]
            if len(running) > limit:
                fresh = sorted((x for x in running if x[1] < START), key=lambda x: (-priority(x[2]), x[1]))
                for p, _, _ in fresh[:len(running) - limit]:
                    signal_tree(p, signal.SIGSTOP, rows)
                    paced.append(p)
                    print(f"{time.strftime('%H:%M:%S')} pace {limit}: holding new worker {p}", flush=True)
            elif len(running) < limit and paced:
                held = sorted(paced, key=lambda p: priority(by.get(p, "")), reverse=True)
                for p in held[:limit - len(running)]:
                    signal_tree(p, signal.SIGCONT, rows)
                    paced.remove(p)
                    print(f"{time.strftime('%H:%M:%S')} pace {limit}: released {p}", flush=True)
        if free < a.min_free or load > a.max_load:
            running = sorted((age, p) for p, age in roots(rows) if p not in paused)
            if running:
                p = running[0][1]                     # the youngest job
                signal_tree(p, signal.SIGSTOP, rows)
                paused.append(p)
                print(f"{time.strftime('%H:%M:%S')} free {free}% load {load:.1f}: paused {p}", flush=True)
                time.sleep(a.interval * 3)            # give the system time to recover
                continue
        elif paused and free >= a.resume_free and load <= a.resume_load:
            p = paused.pop(0)
            signal_tree(p, signal.SIGCONT, rows)
            print(f"{time.strftime('%H:%M:%S')} free {free}% load {load:.1f}: resumed {p}", flush=True)
            time.sleep(a.interval * 3)
            continue
        time.sleep(a.interval)


if __name__ == "__main__":
    main()
