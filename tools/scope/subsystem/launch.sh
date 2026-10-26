#!/bin/bash
# launch.sh NAME QUEUE.json "Category"   worktree $WT/NAME (branch codex/NAME), one Codex worker in the background
set -e
. "$(dirname "$0")/common.sh"
NAME=$1; Q=$2; CAT=$3; W=$WT/$NAME
[ -d "$W" ] || "$MAIN/tools/worktree.sh" "$NAME" >/dev/null
mkdir -p "$W/build/scope/campaign"
[ -e "$W/build/scope/agbcc" ] || ln -s "$MAIN/build/scope/agbcc" "$W/build/scope/agbcc"   # scope's instrumented agbcc
[ -e "$W/build/scope/campaign/queue.json" ] || cp "$Q" "$W/build/scope/campaign/queue.json"
N=$(python3 -c "import json,sys;print(len(json.load(open(sys.argv[1]))))" "$W/build/scope/campaign/queue.json")
python3 - "$HERE/BRIEF.md" "$W" "$NAME" "$CAT" "$N" > "$W/build/brief.txt" <<'PY'
import os, sys
b, w, name, cat, n = sys.argv[1:]
print(open(b).read().format(wt=w.replace(os.path.expanduser("~"), "~"), branch="codex/" + name,
      category=cat, n=n, run=name))
PY
cd "$W"
nohup codex exec -m "$MODEL" -s workspace-write -C "$W" -c model_reasoning_effort=high --json \
  -o "$W/build/last.txt" "$(cat build/brief.txt)" > "$W/build/run.jsonl" 2>&1 < /dev/null &
echo "$NAME pid $! ($N drafts)"
