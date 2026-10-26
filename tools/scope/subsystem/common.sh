# Paths shared by the subsystem-run scripts (sourced).
HERE=$(cd "$(dirname "$0")" && pwd)
MAIN=$(git -C "$HERE" worktree list --porcelain | sed -n '1s/^worktree //p')   # the main checkout
WT=$(dirname "$MAIN")/wt                     # worker worktrees: $WT/sc-NAME (tools/worktree.sh)
STATE=$MAIN/build/subsystem                  # queues, logs, integrated.txt
MODEL=${MODEL:-gpt-6.1-sol}
mkdir -p "$STATE"
workers() { for W in "$WT"/sc-*; do [ -f "$W/build/scope/campaign/queue.json" ] && echo "$W"; done; }
thread_id() { grep -o '"thread_id":"[0-9a-f-]*"' "$1/build/run.jsonl" 2>/dev/null | head -1 | cut -d'"' -f4; }
alive() { pgrep -f "codex exec -m $MODEL -s workspace-write -C $1 " >/dev/null && return 0
          id=$(thread_id "$1"); [ -n "$id" ] && pgrep -f "codex exec resume.*$id" >/dev/null; }
