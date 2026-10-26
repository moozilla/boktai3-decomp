#!/bin/bash
# integrate.sh N: cherry-pick every "Match FUNC" commit not yet integrated from the codex/sc-* branches onto
# claude/sc-batch-N (from origin/main) in the worktree $WT/sc-integ. Then, by hand: review the diff (no
# hand-written asm, concession markers present), `make` (must print build/boktai3.gba: OK),
# `python3 tools/progress.py --markdown PROGRESS.md`, commit both PROGRESS files, open the PR, merge,
# and append the picked subjects ($STATE/integrated.pending) to $STATE/integrated.txt.
set -e
. "$(dirname "$0")/common.sh"
N=$1; I=$WT/sc-integ
git -C "$MAIN" fetch -q origin
[ -d "$I" ] || (cd "$MAIN" && BRANCH_PREFIX=claude tools/worktree.sh sc-integ >/dev/null)
cd "$I"; git checkout -q -B "claude/sc-batch-$N" origin/main
touch "$STATE/integrated.txt"; : > "$STATE/integrated.pending"
for b in $(git for-each-ref --format='%(refname:short)' 'refs/heads/codex/sc-*'); do
  for c in $(git rev-list --reverse origin/main.."$b"); do
    subj=$(git log -1 --format=%s "$c")
    case "$subj" in Match\ *) ;; *) continue ;; esac
    grep -qxF "$subj" "$STATE/integrated.txt" && continue
    if out=$(git cherry-pick "$c" 2>&1); then echo "$subj" >> "$STATE/integrated.pending"; echo "picked $b: $subj"
    else echo "CONFLICT $b: $subj: $(echo "$out" | tail -2)"; git cherry-pick --abort 2>/dev/null || git reset -q --hard; fi
  done
done
