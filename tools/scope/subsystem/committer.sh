#!/bin/bash
# The Codex sandbox cannot write a worktree's .git: workers write build/scope/campaign/FUNC/commit after
# `campaign.py land FUNC .` prints unit MATCH, and this loop (outside the sandbox) makes one commit per
# function. Run it in the background for the whole run.
. "$(dirname "$0")/common.sh"
while true; do
  for W in $(workers); do
    for c in "$W"/build/scope/campaign/*/commit; do
      [ -e "$c" ] || continue
      d=$(dirname "$c"); f=$(basename "$d")
      [ -e "$d/committed" ] && continue
      src=$(python3 -c "import json,sys;print({r['func']:r['src'] for r in json.load(open(sys.argv[1]))}[sys.argv[2]])" "$W/build/scope/campaign/queue.json" "$f")
      if git -C "$W" diff --quiet -- "$src"; then touch "$d/committed"; continue; fi
      git -C "$W" add -- "$src" && git -C "$W" commit -q -m "$(cat "$c")

Matched by a $MODEL scope worker (subsystem run $(basename "$W"))." && touch "$d/committed" \
        && echo "$(date +%T) $(basename "$W") committed $f"
    done
  done
  sleep 45
done
