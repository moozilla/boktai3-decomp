# Boktai 3 agent entry point

Read `README.md`, `CLAUDE.md`, `docs/HANDOFF.md` (the four phases, current state,
the owner's preferences), then `docs/WORKER.md` and `tools/scope/WORKER.md` (the
matching loop). Look everything else up in `docs/INDEX.md`
(`python3 tools/docindex.py ADDR`). The matching and evidence requirements in those
files apply to every model.

Aim for the final form: C that matches with no register pins or barriers and reads
like the original authors wrote it. Work in ROM order through a subsystem and stay on
a function until it matches or the blocking compiler decision is proved with a scope
knob; concessions are a last resort. Codex subsystem workers get
`tools/scope/subsystem/BRIEF.md` (see that directory's README).

Use `build/venv/bin/python` (or activate `build/venv`) for local tooling.
Worker checkouts must have their own build outputs; only `gen/`, the ROM,
the Python environment and built third-party tools may be shared read-only.
Use `codex/` branches for Codex work and keep benchmark workers isolated so
one model cannot see another model's candidate solutions.

Fast per-function checks are sufficient for working-branch matching commits.
Run one complete combined build before merging a batch into main; it must print
`build/boktai3.gba: OK`. Do not rebuild for every function or documentation edit.
Never commit the ROM, generated assembly or emulator dumps.
Never commit personal information: no real names, usernames or home paths
(`/Users/<name>`; write `~` or a repo-relative path). `tools/pii_check.py` enforces
this in the git hooks (`.githooks`) and in CI.
Nonmatching drafts are committed on `main` inside `#ifdef NONMATCHING ... #else
INCLUDE_ASM(...) #endif`, with a fuzzy score recorded by
`tools/check.py --nonmatching --record`. They never count as exact progress.
Do not create new WIP branches for drafts.
Name functions `Subsystem_VerbNoun` (e.g. `SceneIndicator_CreateFromScript`):
`scope.py rename FUNC NEW` or `scope.py land FUNC --name NEW` writes
`symbols/functions.csv` and rewrites every reference. Older drafts may carry a
file-local `#define Name sub_XXXXXXXX` alias; `tools/promote_names.py` moves those
into `symbols/functions.csv` (then `make format`, `make`).
Provisional descriptive names are welcome: mark uncertain interpretations as
tentative and correct them as understanding improves. Do not block matching on
a naming investigation. Coverage tags describe observed execution.
Report code progress as percentage and percentage-point gain, with bytes secondary.
Totals live only in `PROGRESS.md` (regenerated once per batch integration) and
the README badges; don't restate them in README, HANDOFF or subsystem docs.
A history entry may state its own batch's gain.
Record work as `docs/HANDOFF.md` describes ("Recording your work"): notes and
history entries with `Tags:` lines, then `python3 tools/docindex.py`.

The user prefers batched integration: workers commit each match separately, and
the orchestrator reviews and validates their combined work before merging a
batch into `main`. Such integration PRs may contain multiple translation units;
individual matching commits should still contain only one. Progress Actions run
only on pushes to `main` (or explicit manual dispatch), never per worker commit.
Every batch integration PR regenerates the checked-in report in the same commit:
`python3 tools/progress.py --markdown PROGRESS.md` writes `PROGRESS.md` and
`PROGRESS.json` (the tree that the progress mod reads); commit both. CI does not
commit them.

Local register pinning is user-authorized as a temporary matching concession.
Mark affected translation units `// MATCHING_CONCESSION: register-pinning`;
count exact bytes normally and report them in the Register-pinned category.
Retain readable C, fast exact checks, and batch validation; defer removing the
hints. This does not authorize handwritten instruction bodies.
