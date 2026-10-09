# Retained candidate source

This branch preserves 656 WIP C files from seven local checkouts, including
alternate attempts and targets already matched. The audited target inventory
contains 48 accepted matches and 65 unresolved targets; file count is not
unfinished-function count. `targets.csv` maps the best selected unresolved
candidate to its preserved source. `manifest.csv` records every source path
and SHA-256 hash. Some candidates are incorrect or unsafe; retention is not
acceptance. Consult the existing draft inventory and local worker notes.

The user approved the main/WIP policy on October 9, 2026: AGENTS.md
allows nonmatching drafts on dedicated WIP branches while prohibiting them
on main. Retain drafts here, outside `src/`, without merging them into main
or counting them as exact code. ROMs, generated assembly, dumps, binary outputs and assets
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


The inherited ignore rule also matches archived `wip/` directories. When
saving a new candidate here, stage its specific C path explicitly, for example
`git add -f drafts/retained/wt/push20-integrate/wip/example.c`, and update the
manifest. Preserve source rather than generated or binary scratch output.


## Solo family tranche checkpoint (October 9)

The 137 newly retained C files cover 68 targets, including variants of sources
already exact on main. The solo tranche merged 46 functions / 7,424 bytes in
PR #16. `solo-family_2026-10-09.csv` records every new candidate's preserved
path, original path, hash, target and status. Exact-source status means its
hash equals accepted production source; other alternates are unaccepted and
receive no credit. See `notes/solo-family_2026-10-09.md` for the selected
near-matches, successful recipes and failed hypotheses. Production state is
synchronized with main; candidate C remains confined to this WIP branch.
