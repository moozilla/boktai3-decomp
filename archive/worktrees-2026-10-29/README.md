# Worktree archive, 2026-10-29

Work from local worktrees, stashes and branches that was not on `main` when the local
worktrees were cleaned up. Nothing here is built or checked; it is kept so no work is lost.
Patches are plain `git diff` / `git format-patch` output; apply with `git apply -3` or
`git am -3` and re-check each unit.

* `<worktree>/uncommitted.patch`: uncommitted and untracked changes, against the commit in
  that directory's README.txt.
* `stashes/`: every `git stash` entry (README.txt lists them).
* `branches/<branch>/NNN.patch`: commits on local branches whose subject is not on main,
  one patch per commit, only the branch's own commits (no history from before the
  rewrite). SUBJECTS.txt lists them.

Left out on purpose: the `claude/fixsearch-bounded` worktree's 7,347 staged files (a stale
index from before the history rewrite; the branch's commits are on main), and the running
matchsearch workers (`codex/sc-s4-*`, `codex/sc-s5-*`), which are integrated in batches.
Sol's search engine is on GitHub as `codex/sc-search-sol`.
