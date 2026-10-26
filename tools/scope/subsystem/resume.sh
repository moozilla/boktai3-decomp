#!/bin/bash
# resume.sh NAME: resume worker NAME's Codex session after a transient API error
. "$(dirname "$0")/common.sh"
W=$WT/$1; id=$(thread_id "$W")
n=$(cat "$W/build/retries" 2>/dev/null || echo 0); echo $((n+1)) > "$W/build/retries"
cd "$W"
nohup codex exec resume -m "$MODEL" -c sandbox_mode=\"workspace-write\" -c model_reasoning_effort=high --json \
  -o "$W/build/last.txt" "$id" "You were interrupted by a transient API error. Continue the same task from where you stopped (your files are in build/scope/campaign/), following the original instructions." >> "$W/build/run.jsonl" 2>&1 < /dev/null &
echo "$(date +%T) resumed $1 ($id, attempt $((n+1)))"
