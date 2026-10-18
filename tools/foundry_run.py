#!/usr/bin/env python3
"""Run the foundry unattended: headless `claude -p` workers, one subsystem batch each.

  foundry_run.py [--workers 4] [--batch 8] [--backend codex|claude] [--model M]
                 [--bucket <50|50-150] [--range START END] [--max-batches N]

Workers are `codex exec` (default; bills to the OpenAI plan) or headless
`claude -p`. Keeps WORKERS batches in flight. Each batch claims a contiguous run of queued
functions in one subsystem (tools/foundry.py batch: one active batch per
subsystem, so header edits don't collide), runs a worker on it, records its
tokens and cost (build/foundry/scale/usage.tsv), and frees the subsystem. The
machine stages run separately (build/foundry/scale/finisher.sh). Several runners
can share the queue (e.g. one per bucket or range). Stop with
`touch build/foundry/scale/STOP`; running batches finish first.

Workers may read anything, write only under wip/, build/foundry/ and include/,
and run only the project's Python tools.
"""
import argparse
import json
import os
import subprocess
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import foundry  # noqa: E402
from romlib import ROOT  # noqa: E402

BACKEND = "claude"
TOOLS = ["Read", "Glob", "Grep", "Edit(wip/**)", "Write(wip/**)", "Edit(build/foundry/**)",
         "Write(build/foundry/**)", "Edit(include/**)", "Write(include/**)", "Bash(build/venv/bin/python tools/*)"]


def quota_ok(floor):
    """False when the Codex weekly quota is below floor percent (tools/codex_quota.py)."""
    q = os.path.join(ROOT, "tools", "codex_quota.py")
    if not os.path.exists(q):
        return True
    r = subprocess.run([sys.executable, q, "--floor", str(floor)], cwd=ROOT, capture_output=True, text=True)
    if r.returncode != 0:
        print(f"quota gate: {r.stdout.strip()} (floor {floor}%): no new batches", flush=True)
    return r.returncode == 0


def claim(n, bucket, rng=None):
    r = subprocess.run([foundry.PY, "tools/foundry.py", "batch", str(n)] + (["--bucket", bucket] if bucket else []) +
                       (["--range", *rng] if rng else []),
                       cwd=ROOT, capture_output=True, text=True)
    path = r.stdout.strip()
    return path if r.returncode == 0 and path.endswith(".txt") else None


def launch(path, model, backend, effort):
    log = path.replace(".txt", ".json")
    fh = open(log, "w")
    if backend == "codex":
        # bills to the OpenAI plan; workspace-write lets it write anywhere in this worktree, so the
        # brief's rules (wip/, build/foundry/, include/ only) are what keep it in bounds
        cmd = ["codex", "exec", "-m", model, "-s", "workspace-write", "-C", ROOT, "-c",
               f"model_reasoning_effort={effort}", "--json", "-o", path.replace(".txt", ".last.txt"),
               open(path).read()]
    else:
        cmd = ["claude", "-p", open(path).read(), "--model", model, "--output-format", "json",
               "--allowedTools", *TOOLS]
    p = subprocess.Popen(cmd, cwd=ROOT, stdout=fh, stderr=subprocess.DEVNULL, stdin=subprocess.DEVNULL)
    return p, log, time.time()


def record(path, log, t0):
    k = os.path.basename(path)[6:9]
    try:
        lines = [json.loads(x) for x in open(log).read().strip().split("\n") if x.startswith("{")]
        turns = [x for x in lines if x.get("type") == "turn.completed"]
        if turns:  # codex: per-turn usage, cached input reported separately
            u = {"input_tokens": sum(t["usage"]["input_tokens"] - t["usage"].get("cached_input_tokens", 0)
                                     for t in turns),
                 "output_tokens": sum(t["usage"]["output_tokens"] for t in turns)}
            d = {"usage": u, "total_cost_usd": 0, "is_error": False}
        else:
            d = lines[-1]
            u = d.get("usage", {})
        # non-cached tokens: cache reads are ~99% of the raw total and cost a tenth as much
        tokens = u.get("input_tokens", 0) + u.get("output_tokens", 0) + u.get("cache_creation_input_tokens", 0)
        cost = d.get("total_cost_usd", 0)
        err = d.get("is_error")
    except Exception as e:
        tokens, cost, err = 0, 0, repr(e)
    with open(os.path.join(foundry.SCALE, "usage.tsv"), "a") as fh:
        fh.write(f"{k}\t{tokens}\t{int(time.time() - t0)}\t{time.time():.0f}\t{cost}\t{BACKEND}\n")
    foundry.release(path)
    print(f"batch {k} done: {tokens} tokens, ${cost:.2f}, {int(time.time() - t0)} s" + (f", error {err}" if err else ""),
          flush=True)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--workers", type=int, default=4)
    ap.add_argument("--batch", type=int, default=8)
    ap.add_argument("--backend", choices=["codex", "claude"], default="codex",
                    help="codex bills to the OpenAI plan (default); claude to Claude usage")
    ap.add_argument("--model", help="default: gpt-6.1-sol for codex, sonnet for claude")
    ap.add_argument("--effort", default="medium", help="codex reasoning effort")
    ap.add_argument("--bucket")
    ap.add_argument("--range", nargs=2, help="START END (hex): only functions in this address range")
    ap.add_argument("--quota-floor", type=int, default=15,
                    help="codex: start no batch once the weekly quota is below this percent")
    ap.add_argument("--stop", default="STOP", help="stop file name under build/foundry/scale/")
    ap.add_argument("--max-batches", type=int, default=10 ** 9)
    a = ap.parse_args()
    global BACKEND
    BACKEND = a.backend
    model = a.model or ("gpt-6.1-sol" if a.backend == "codex" else "sonnet")
    stop = os.path.join(foundry.SCALE, a.stop)
    running, started = [], 0
    while True:
        for item in running[:]:
            p, log, t0, path = item
            if p.poll() is not None:
                record(path, log, t0)
                running.remove(item)
        if not os.path.exists(stop) and started < a.max_batches:
            while len(running) < a.workers:
                if a.backend == "codex" and not quota_ok(a.quota_floor):
                    started = a.max_batches  # stop for good; running batches finish
                    break
                path = claim(a.batch, a.bucket, a.range)
                if not path:
                    break
                p, log, t0 = launch(path, model, a.backend, a.effort)
                running.append((p, log, t0, path))
                started += 1
                print(f"started {os.path.basename(path)}", flush=True)
        if not running:
            break
        time.sleep(20)


if __name__ == "__main__":
    main()
