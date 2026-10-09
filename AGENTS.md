# Boktai 3 agent entry point

Read `README.md`, `CLAUDE.md`, `docs/HANDOFF.md`, then `docs/WORKER.md`.
The matching and evidence requirements in those files apply to every model.

Use `build/venv/bin/python` (or activate `build/venv`) for local tooling.
Worker checkouts must have their own build outputs; only `gen/`, the ROM,
the Python environment and built third-party tools may be shared read-only.
Use `codex/` branches for Codex work and keep benchmark workers isolated so
one model cannot see another model's candidate solutions.

Before committing, a complete build must print `build/boktai3.gba: OK`.
Never commit the ROM, generated assembly, emulator dumps or nonmatching C.
Function and subsystem names require evidence; a coverage tag means observed
execution in a segment, not proof of exclusive ownership or a semantic name.
