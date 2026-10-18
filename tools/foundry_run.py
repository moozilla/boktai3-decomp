#!/usr/bin/env python3
"""Run the foundry unattended: headless `claude -p` workers, one subsystem batch each.

  foundry_run.py [--workers 4] [--batch 8] [--model sonnet] [--bucket <50|50-150]
                 [--max-batches N]

Keeps WORKERS batches in flight. Each batch claims a contiguous run of queued
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

TOOLS = ["Read", "Glob", "Grep", "Edit(wip/**)", "Write(wip/**)", "Edit(build/foundry/**)",
         "Write(build/foundry/**)", "Edit(include/**)", "Bash(build/venv/bin/python tools/*)"]


def claim(n, bucket, rng=None):
    r = subprocess.run([foundry.PY, "tools/foundry.py", "batch", str(n)] + (["--bucket", bucket] if bucket else []) +
                       (["--range", *rng] if rng else []),
                       cwd=ROOT, capture_output=True, text=True)
    path = r.stdout.strip()
    return path if r.returncode == 0 and path.endswith(".txt") else None


def launch(path, model):
    log = path.replace(".txt", ".json")
    fh = open(log, "w")
    p = subprocess.Popen(["claude", "-p", open(path).read(), "--model", model, "--output-format", "json",
                          "--allowedTools", *TOOLS], cwd=ROOT, stdout=fh, stderr=subprocess.STDOUT,
                         stdin=subprocess.DEVNULL)
    return p, log, time.time()


def record(path, log, t0):
    k = os.path.basename(path)[6:9]
    try:
        d = json.loads(open(log).read().strip().split("\n")[-1])
        u = d.get("usage", {})
        # non-cached tokens: cache reads are ~99% of the raw total and cost a tenth as much
        tokens = u.get("input_tokens", 0) + u.get("output_tokens", 0) + u.get("cache_creation_input_tokens", 0)
        cost = d.get("total_cost_usd", 0)
        err = d.get("is_error")
    except Exception as e:
        tokens, cost, err = 0, 0, repr(e)
    with open(os.path.join(foundry.SCALE, "usage.tsv"), "a") as fh:
        fh.write(f"{k}\t{tokens}\t{int(time.time() - t0)}\t{time.time():.0f}\t{cost}\theadless\n")
    foundry.release(path)
    print(f"batch {k} done: {tokens} tokens, ${cost:.2f}, {int(time.time() - t0)} s" + (f", error {err}" if err else ""),
          flush=True)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--workers", type=int, default=4)
    ap.add_argument("--batch", type=int, default=8)
    ap.add_argument("--model", default="sonnet")
    ap.add_argument("--bucket")
    ap.add_argument("--range", nargs=2, help="START END (hex): only functions in this address range")
    ap.add_argument("--max-batches", type=int, default=10 ** 9)
    a = ap.parse_args()
    stop = os.path.join(foundry.SCALE, "STOP")
    running, started = [], 0
    while True:
        for item in running[:]:
            p, log, t0, path = item
            if p.poll() is not None:
                record(path, log, t0)
                running.remove(item)
        if not os.path.exists(stop) and started < a.max_batches:
            while len(running) < a.workers:
                path = claim(a.batch, a.bucket, a.range)
                if not path:
                    break
                p, log, t0 = launch(path, a.model)
                running.append((p, log, t0, path))
                started += 1
                print(f"started {os.path.basename(path)}", flush=True)
        if not running:
            break
        time.sleep(20)


if __name__ == "__main__":
    main()
