#!/bin/bash
# One line per worker: running or not, queue size, functions done, matches, commits; then the quota.
. "$(dirname "$0")/common.sh"
for W in $(workers); do
  C=$W/build/scope/campaign
  q=$(python3 -c "import json,sys;print(len(json.load(open(sys.argv[1]))))" "$C/queue.json")
  printf "%-14s %-6s queue %3s done %3s matched %3s commits %3s\n" "$(basename "$W")" "$(alive "$W" && echo run || echo exited)" \
    "$q" "$(ls "$C"/*/done 2>/dev/null | wc -l | tr -d ' ')" "$(ls "$C"/*/commit 2>/dev/null | wc -l | tr -d ' ')" \
    "$(git -C "$W" rev-list --count HEAD ^origin/main 2>/dev/null)"
done
python3 "$MAIN/tools/codex_quota.py"
