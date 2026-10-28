#!/bin/bash
# Stop every subsystem worker (first sessions and resumed ones); other Codex runs are left alone.
. "$(dirname "$0")/common.sh"
for sig in TERM KILL; do
  for W in $(workers); do
    pkill -$sig -f "codex exec -m [^ ]* -s workspace-write -C $W "
    id=$(thread_id "$W"); [ -n "$id" ] && pkill -$sig -f "codex exec resume.*$id"
    touch "$W/build/finished"
  done
  sleep 5
done
echo "$(date +%T) stopped all workers"
