# Boktai 3 agent entry point

Read `README.md`, `CLAUDE.md`, `docs/HANDOFF.md`, then `docs/WORKER.md`.
The matching and evidence requirements in those files apply to every model.

Use `build/venv/bin/python` (or activate `build/venv`) for local tooling.
Worker checkouts must have their own build outputs; only `gen/`, the ROM,
the Python environment and built third-party tools may be shared read-only.
Use `codex/` branches for Codex work and keep benchmark workers isolated so
one model cannot see another model's candidate solutions.

Fast per-function checks are sufficient for working-branch matching commits.
Run one complete combined build before merging a batch into main; it must print
`build/boktai3.gba: OK`. Do not rebuild for every function or documentation edit.
Never commit the ROM, generated assembly or emulator dumps.
Never commit nonmatching C on `main`. Dedicated WIP branches may preserve
nonmatching drafts and alternate candidates; keep them outside production
`src/`, and do not merge them into `main` or count them as exact progress.
Name functions `Subsystem_VerbNoun` (e.g. `SceneIndicator_CreateFromScript`).
Workers add a file-local `#define Name sub_XXXXXXXX` alias; the integrator runs
`tools/promote_names.py` before merging a batch, which moves aliases into
`symbols/functions.csv` and rewrites all references (then `make format`, `make`).
Provisional descriptive names are welcome: mark uncertain interpretations as
tentative and correct them as understanding improves. Do not block matching on
a naming investigation. Coverage tags describe observed execution.
Report code progress as percentage and percentage-point gain, with bytes secondary.

The user prefers batched integration: workers commit each match separately, and
the orchestrator reviews and validates their combined work before merging a
batch into `main`. Such integration PRs may contain multiple translation units;
individual matching commits should still contain only one. Progress Actions run
only on pushes to `main` (or explicit manual dispatch), never per worker commit.

Local register pinning is user-authorized as a temporary matching concession.
Mark affected translation units `// MATCHING_CONCESSION: register-pinning`;
count exact bytes normally and report them in the Register-pinned category.
Retain readable C, fast exact checks, and batch validation; defer removing the
hints. This does not authorize handwritten instruction bodies.
