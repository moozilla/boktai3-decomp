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

## First pilot results (2026-10-08)

Baseline: `b6a7c2972d85b2c08b2bf6033ece5cda3e56e359`. All models passed the
unscored `08033568` toolchain control. The first four targets were selected
deterministically from unmatched 15–40 and 41–80 instruction functions (two
per tier), excluding previously mentioned functions. Candidates were ordered
by SHA-256 of `boktai3-pilot-2026-10-08:` plus function name. The final two were
documented difficult cases. No worker saw another model's solutions.

| Target, in assigned order | Instructions | Luna checks | Sol 6.1 checks | Astra checks |
|---|---:|---:|---:|---:|
| `08139298` | 33 | 10, mismatch | 6, mismatch | 1, match |
| `0805B2CC` | 31 | 2, match | 1, match | 1, match |
| `0806FD5C` | 43 | 2, match | 1, match | 2, match |
| `08074E38` | 53 | 10, mismatch | 3, match | 2, match |
| `081A6EF8` | 27 | 5, match | 2, match | 2, match |
| `081FB788` | 30 | 5, match | 1, match | 1, match |

| Model | Verified matches | Emitted object bytes | Progress-span bytes | Active matching elapsed |
|---|---:|---:|---:|---:|
| `gpt-6-luna` | 4/6 | 312 | 396 | 7m 55s, two passes |
| `gpt-6.1-sol` | 5/6 | 432 | 516 | 2m 00s |
| `gpt-6-astra` | 6/6 | 500 | 584 | 3m 34s |

Each model's final full ROM build passed. The parent integrated Sol's five
matches plus Astra's unique `08139298`, then verified the combined ROM and
all three shiftability scenarios (35 identical screenshots). Duplicate
solutions count once: **six new functions, 500 emitted bytes, 584 progress-span
bytes**. The span for `081FB788` includes 84 bytes retained after its emitted
object; this is why the two byte measures differ.

### Deviations and interpretation

Luna's first pass stopped after about 68 seconds with no matches, despite
unused checks, and reordered the last three targets. The parent then explicitly
asked it to continue revising in the assigned order until a match or the
ten-check cumulative cap, within about eight more minutes. It received no
solution hints. The second pass took about 406 seconds; the table sums both
active intervals and excludes the pause between them. Sol also stopped short
of its cap on the first target. These are observed agent behaviors, not a
controlled comparison of equal effort. Sol and Astra times exclude their final
build/reporting; Luna's continuation endpoint was recorded after its build,
so its time includes some verification overhead.

This six-function pilot supports **Sol as the default production matcher**,
with Astra for difficult escalations. Luna demonstrated useful matching ability,
including both historically difficult cases, but needed a more explicit
persistence instruction. Try it next on disjoint, bounded batches with clear
stop rules, reviewed templates and retained failure candidates. Do not scale
to many Luna workers based on this sample alone. The pilot measures observed
success and elapsed time, not plan cost, long-run throughput or statistical
superiority; it cannot establish a monetary Pareto curve.

The shared compiler lesson is to use real destination/array structs and explicit
source-value temporaries before stores when related offsets must reuse a
register. The allocation wrapper matched with a cached-pointer early return
and a guarded allocation/init block. These patterns may now be shared in
production rounds.

Local audit evidence lives in each managed `match-{luna,sol,astra}-pilot`
worktree under `build/benchmark/`: `results.json`, `timing.json`, `checks.jsonl`
and per-check transcripts. Candidate source and assembly transcripts remain
local; they are not progress artifacts uploaded by CI.
