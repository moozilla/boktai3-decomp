#!/bin/sh
# Land campaign matches as one batch integration PR, and merge it only if the whole ROM matches.
#   tools/scope/batch.sh N TREE FUNC...
# Each FUNC gets its own commit on claude/night-N (from origin/main) in the worktree TREE. A FUNC whose
# unit does not match is skipped (campaign.py land restores the file). The PR is opened and merged
# only after `make` prints exactly "build/boktai3.gba: OK".
set -eu
n=$1
tree=$2
shift 2
here=$(cd "$(dirname "$0")/../.." && pwd)
cd "$tree"
git fetch -q origin
git checkout -q -B "claude/night-$n" origin/main
landed=""
for f in "$@"; do
    if ! out=$(python3 "$here/tools/scope/campaign.py" land "$f" "$tree" 2>&1); then
        echo "$out"
        echo "skip $f"
        git checkout -q -- src
        continue
    fi
    echo "$out"
    pins=$(echo "$out" | sed -n 's/.*(\([0-9]*\) pins, \([0-9]*\) barriers in the file).*/\1 \2/p' | tail -1)
    p=${pins% *}
    b=${pins#* }
    src=$(git status --porcelain src | awk '{print $2}' | head -1)
    if [ "$p" = 0 ] && [ "$b" = 0 ]; then
        note="Matched by a gpt-6.1-sol scope worker with no concessions."
    else
        note="Matched by a gpt-6.1-sol scope worker; the file has $p register pin(s) and $b barrier(s)
(marked MATCHING_CONCESSION: register-pinning)."
    fi
    git add src
    git commit -q -m "Match $f (in $(basename "$src"))

$note

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
    landed="$landed $f"
done
[ -n "$landed" ] || { echo "nothing landed"; exit 1; }
make -j8 > build/batch.log 2>&1 || true
if ! grep -qx "build/boktai3.gba: OK" build/batch.log; then
    grep "boktai3.gba" build/batch.log || tail -20 build/batch.log
    echo "ROM does not match: not opening a PR"
    exit 1
fi
echo "build/boktai3.gba: OK"
python3 tools/progress.py --markdown PROGRESS.md | grep -i "nonmatching drafts"
git add PROGRESS.md PROGRESS.json
git commit -q -m "Regenerate PROGRESS.md and PROGRESS.json

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
python3 tools/pii_check.py --range origin/main..HEAD
git push -q -u origin "claude/night-$n"
list=$(for f in $landed; do echo "- $f"; done)
url=$(gh pr create --base main --head "claude/night-$n" --title "Night batch $n:$landed" --body "Batch integration of scope-campaign matches, one commit per function:

$list

- combined ROM build: \`build/boktai3.gba: OK\`
- PROGRESS.md / PROGRESS.json regenerated

🤖 Generated with [Claude Code](https://claude.com/claude-code)" | tail -1)
echo "$url"
gh pr merge "$url" --merge
gh pr view "$url" --json state -q .state
