# Retained candidate source

This branch preserves 519 WIP C files from seven local checkouts, including
alternate attempts and targets already matched. The audited target inventory
contains 48 accepted matches and 65 unresolved targets; file count is not
unfinished-function count. `targets.csv` maps the best selected unresolved
candidate to its preserved source. `manifest.csv` records every source path
and SHA-256 hash. Some candidates are incorrect or unsafe; retention is not
acceptance. Consult the existing draft inventory and local worker notes.

The user explicitly authorized committing nonmatching C on this separate
branch on October 9, 2026. This is a narrow exception to AGENTS.md: retain
drafts here, outside `src/`, without merging them into main or counting them
as exact code. ROMs, generated assembly, dumps, binary outputs and assets
remain excluded. No build inputs or production sources are changed.

Restore from a fresh clone:

```sh
git fetch origin codex/wip-drafts-2026-10-09
git switch --track origin/codex/wip-drafts-2026-10-09
```

Inspect a candidate under `drafts/retained/`, use `build/venv/bin/python
tools/check.py PATH` after setting up the local build, and promote only a
reviewed exact candidate to production on a separate matching branch.
The production full-ROM and shift checks still apply to any promotion.
