#!/bin/bash
# watch.sh FLOOR [FINISHED]: keep the run draining. Resumes workers that died on a transient API error
# (capacity, 429, 5xx; up to 5 times each); marks the others finished. Exits when FINISHED workers have
# finished (default: never), or stops every worker (stopall.sh) once the Codex weekly quota left <= FLOOR %,
# or at DEADLINE=HHMM (local time) when that is set.
. "$(dirname "$0")/common.sh"
floor=${1:?quota floor in percent}; want=${2:-999}
while true; do
  for W in $(workers); do
    alive "$W" && continue
    [ -e "$W/build/finished" ] && continue
    last=$(tail -3 "$W/build/run.jsonl")
    if echo "$last" | grep -q '"turn.failed"' && echo "$last" | grep -qiE 'capacity|rate limit|429|Too Many|overloaded|stream|timeout|internal|5[0-9][0-9]' \
       && [ "$(cat "$W/build/retries" 2>/dev/null || echo 0)" -lt 5 ]; then
      "$HERE/resume.sh" "$(basename "$W")"; continue
    fi
    left_q=$(python3 -c "import json,os,sys;C=sys.argv[1];print(sum(not os.path.exists(os.path.join(C,r['func'],'done')) for r in json.load(open(os.path.join(C,'queue.json')))))" "$W/build/scope/campaign")
    if echo "$last" | grep -q '"turn.completed"' && [ "$left_q" -gt 0 ] && [ "$(cat "$W/build/retries" 2>/dev/null || echo 0)" -lt 5 ]; then
      # ended its turn with work left (Terra does this): push it on
      "$HERE/resume.sh" "$(basename "$W")" "Do not end your turn yet: the task is the whole queue, not a progress report. $left_q functions in build/scope/campaign/queue.json have no done file. Continue from where you stopped, following the original instructions, and end your turn only when every function is done and the second pass is finished."; continue
    fi
    touch "$W/build/finished"; echo "$(date +%T) $(basename "$W") exited: $(echo "$last" | tail -1 | cut -c1-200)"
  done
  nf=$(ls "$WT"/sc-*/build/finished 2>/dev/null | wc -l); [ "$nf" -ge "$want" ] && { echo "$(date +%T) $nf finished"; exit 0; }
  [ -n "$DEADLINE" ] && [ "$(date +%H%M)" -ge "$DEADLINE" ] && { echo "$(date +%T) deadline $DEADLINE: stopping all workers"; "$HERE/stopall.sh"; exit 0; }
  left=$(python3 "$MAIN/tools/codex_quota.py" | sed -n 's/.*: \([0-9]*\)% left.*/\1/p')
  [ "${left:-100}" -le "$floor" ] && { echo "$(date +%T) quota left $left%: stopping all workers"; "$HERE/stopall.sh"; exit 0; }
  sleep 60
done
