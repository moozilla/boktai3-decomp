#!/usr/bin/env python3
"""How much of the Codex weekly quota is left, from Codex's own session logs.

  codex_quota.py                 print remaining % and the reset time
  codex_quota.py --floor 15      exit 1 when remaining is below 15% (for runners:
                                 check before starting each batch, stop if it fails)

Every `codex exec` run writes ~/.codex/sessions/YYYY/MM/DD/rollout-*.jsonl, whose
token_count events carry the account's rate_limits (used_percent of the
10080-minute weekly window). This reads the newest one, so it costs nothing;
if no session ran in the last --max-age minutes it runs one tiny gpt-6-luna
prompt first to refresh the reading.
"""
import argparse
import glob
import json
import os
import subprocess
import sys
import time

SESSIONS = os.path.expanduser("~/.codex/sessions")


def latest(max_age):
    files = sorted(glob.glob(os.path.join(SESSIONS, "*", "*", "*", "*.jsonl")), key=os.path.getmtime, reverse=True)
    for f in files[:20]:
        if time.time() - os.path.getmtime(f) > max_age * 60:
            break
        found = None
        for line in open(f, errors="replace"):
            if '"rate_limits"' in line:
                try:
                    e = json.loads(line)
                except ValueError:
                    continue
                rl = (e.get("payload") or {}).get("rate_limits") or e.get("rate_limits")
                if rl and rl.get("primary"):
                    found = rl
        if found:
            return found
    return None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--floor", type=float, help="exit 1 if remaining %% is below this")
    ap.add_argument("--max-age", type=float, default=30, help="minutes a reading stays fresh")
    a = ap.parse_args()
    rl = latest(a.max_age)
    if not rl:
        subprocess.run(["codex", "exec", "--skip-git-repo-check", "-m", "gpt-6-luna", "-s", "read-only", "Reply OK"],
                       cwd="/tmp", capture_output=True, stdin=subprocess.DEVNULL, timeout=300)
        rl = latest(a.max_age)
    if not rl:
        sys.exit("no Codex rate-limit reading found")
    p = rl["primary"]
    left = 100 - p["used_percent"]
    reset = time.strftime("%a %H:%M", time.localtime(p["resets_at"])) if p.get("resets_at") else "?"
    print(f"Codex weekly quota: {left:.0f}% left (resets {reset})")
    if a.floor is not None and left < a.floor:
        sys.exit(1)


if __name__ == "__main__":
    main()
