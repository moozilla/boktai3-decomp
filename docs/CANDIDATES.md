# Compact checking and candidate checkpoints

Use `build/venv/bin/python`. `tools/check.py FILE` now displays only aligned
mismatch hunks (three context rows). `--full` shows the entire decoded listing;
`--context N` changes context; `--quiet` prints only the summary. Every check
keeps `full.diff`, `comparison.json`, and compiler/link outputs in its own
ignored `build/check/run-*` directory. `--json` emits only the structured result
on successful compilation; compilation/link failures remain nonzero errors.
The final human MATCH/MISMATCH line remains compatible with benchmark_check.py.

Supply `--target-size BYTES` after reviewing the function body and literal pool/
alignment extent. Without it the checker retains legacy candidate-length
comparison, explicitly labeled as such: an equal prefix is not proof of a
complete function. With it a shorter or longer candidate cannot pass equality.
Do not blindly use the next symbol's distance: retained tails may contain other
functions or data. Full-ROM validation remains mandatory before commits.

The alignment compares decoded text without erasing registers, constants or
branch targets. Both original and candidate addresses remain visible after
insertions. Literal pools are still decoded as possible instructions; this
view does not claim to distinguish all code and data. Resolved-byte equality
and different-byte addresses in JSON are authoritative even when decoded text
looks identical. No fuzzy percentage is presented as correctness.

## Preserve an attempt

```
build/venv/bin/python tools/candidate.py wip/sub_XXXXXXXX.c \
  --target-size 364 --diff --category types --validity unreviewed \
  --hypothesis 'Use the observed owner layout' \
  --next 'Investigate status-store ordering against the exact sibling'
```

This wraps checking and retains every attempt under ignored build/candidates/:
a source snapshot, preprocessed input, source/preprocessing hashes, compiler
identity/hash and flags, Git base, reviewed extent, structured comparison,
stdout/stderr, elapsed compiler-wrapper time, and user-supplied hypotheses and
validity status. `index.jsonl` lists searchable target addresses, function names, source hashes,
size/difference summaries, categories and hypotheses; e.g.
`rg 0818DB10 build/candidates/index.jsonl`. Category and validity are supplied
judgments, not inferred facts. Match status still grants zero progress credit
until source review and complete-ROM integration. Errors are distinct from
mismatches and return exit code 2; mismatches return 1; matches return 0.

These local checkpoints are not durable Git retention. Before leaving a session,
preserve selected authored candidate sources and concise metadata on a dedicated
NONMATCHING draft in src/ (see docs/WORKER.md); older drafts remain on WIP branches. Never copy whole
build/candidates directories into Git: they include preprocessed third-party
material, assembly comparisons and ROM-derived outputs. Keep useful alternates
and failed hypotheses; do not choose solely by a scalar similarity score.

Use `--diff` to print compact hunks from the same checkpointed compilation.
The October 12 diagnostic pilot compiled eight candidates twice to obtain both
a record and a visible diff; this option removes that redundant second check.
