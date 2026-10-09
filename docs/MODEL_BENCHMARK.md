# Matching model pilot

This is a small paired experiment, not a definitive ranking of model families.
Models attempt the same functions in isolated checkouts of the same baseline.
The ROM, generated assembly and compiler are shared read-only; candidates,
check logs and build outputs are private to each worker.

## Protocol

The first pilot compares `gpt-6-luna`, `gpt-6.1-sol` and `gpt-6-astra` with
the same inherited reasoning setting and worker brief. Six unmatched targets
span short functions, medium functions and two documented difficult cases.
Every worker reads the repository matching playbook and may use existing
matched source and notes. Workers may not inspect another worker's solutions,
delegate, run clone porting or use automated permutation search during the pilot.
Those techniques are useful for production, but would obscure this comparison.

Each worker attempts every target in the fixed order, with at most ten
candidate checks per target and a target of fifteen minutes of matching work.
An existing matched function is a separate toolchain control and earns no
credit. All candidate checks go through:

```sh
build/venv/bin/python tools/benchmark_check.py wip/sub_XXXXXXXX.c
```

The wrapper saves source hashes, exit status, compiler-check time and output
under `build/benchmark/`. A compile error is an error, never a match. A positive
per-function check is provisional until the complete ROM build prints
`build/boktai3.gba: OK`. Only verified files enter `src/fn/`. Parent integration
also runs the shiftability regression.

## Measurements and limits

Report solved targets, newly matched bytes, attempts to first match, failures,
and worker elapsed time. Count duplicate discoveries only once in repository
progress, while crediting each model's independent success in the comparison.
The check's emitted object size and the progress tracker's next-function span
are different measures; label them explicitly.

No per-worker billing or token meter is exposed by the orchestration tools.
The account usage meter combines this chat, workers and any other active work,
so it cannot supply a per-model price. Do not call wall time a monetary cost or
claim a measured plan-usage Pareto frontier from these observations. The pilot
can identify promising candidates for longer, disjoint production rounds.

Use the cheapest model that demonstrably handles a class, retain its best
failed candidates, and escalate those to another model with a fresh budget.
Benchmark trials stay isolated; production workers should exchange discovered
compiler patterns after review. This session permits three workers beside the
orchestrator, rather than five concurrent background workers.

Results and the final target list will be recorded here after verification.
