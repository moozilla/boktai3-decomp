# Matching model pilot

This is a small paired experiment, not a definitive ranking of model families.
Models attempt the same functions in isolated checkouts of the same baseline.
The ROM, generated assembly and compiler are shared read-only; candidates,
check logs and build outputs are private to each worker.

## Protocol

The first pilot compares `gpt-6-luna`, `gpt-6.1-sol` and `gpt-6-astra` with
the same worker brief. A retrospective audit of local session records found
that Sol used **low** reasoning while Luna and Astra used **medium**; the
original claim of a common reasoning setting was incorrect. Six unmatched targets
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

No per-worker billing or token meter was exposed by the orchestration tools.
The account usage meter combines this chat, workers and any other active work,
so it cannot supply a per-model price. Do not call wall time a monetary cost or
claim a measured plan-usage Pareto frontier from these observations. A later
[retrospective](RETROSPECTIVE_2026-10-08.md) recovered per-response token records
from local session logs and computed a published-rate cost proxy. Exact
per-model subscription allowance attribution remains unavailable. The pilot
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

## Production round 1 (2026-10-08)

Workers started from `540daab` on disjoint ranges. Existing matched source,
template adaptation and clone porting were allowed. These results test the
orchestration workflow; they are not another paired model benchmark.

| Worker | Address range | New matches | Emitted bytes | Automatic ports |
|---|---|---:|---:|---:|
| Luna A | `08002000–080A0000` | 15 | 960 | 0 |
| Luna B, including continuation | `080A0000–08170000` | 11 | 640 | 1 |
| Sol 6.1, including thunk follow-up | `08170000–0822F248` | 26 | 2,032 | 10 |
| Total | | **52** | **3,632** | **11** |

The progress-span increase is **3,704 bytes**, including retained intervening
bytes, bringing the repository to **4,203 / 11,028 functions** and
**224,688 / 2,415,354 code bytes (9.302%)**. One Luna B rewrite of an already
matched function was rejected during integration and earns no credit.

Luna A logged 42 candidate checks over 22 targets and finished in 11m41s,
including builds. Sol's initial 25-function round logged 251 checks over 54
targets, including automated template trials, and finished in 11m39s. Its thunk
follow-up added another match. Luna B's first-pass elapsed estimate was not
reliable; its separately timestamped five-function continuation took 3m09s.
Do not compare these heterogeneous times as model speed or plan efficiency.

The Luna results justify continuing bounded production batches. Require an
unmatched-target check before drafting, explicit retry/stop rules, and retained
near-matches. Family templates were effective for all workers. Sol remains a
useful default for harder functions and diagnosing toolchain blockers; no Astra
worker was needed in this round. Per-model usage cost remains unmeasured.

Compiler references led to concrete permuter fixes, described in
`docs/COMPILER_REFERENCES.md`. The corrected tool preserved a difficult loop
candidate and scored it appropriately, but a 60-second trial found no new
match; none of this round's matching credit comes from permutation search.

## Production round 2 (2026-10-08 Pacific)

Baseline `e2546f6`. Workers continued disjoint production ranges, with bounded
permutation search permitted. Review excluded three already-matched rewrites.

| Worker | New functions | Emitted bytes | Progress-span bytes | Automatic ports | Matching interval |
|---|---:|---:|---:|---:|---|
| Luna A | 5 | 328 | 328 | 2 | 18m46s |
| Luna B | 21 | 944 | 980 | 0 | 19m58s |
| Sol 6.1 | 40 | 3,736 | 3,736 | 12 | 23m34s |
| Main-thread counter family | 7 | 4,088 | 4,088 | 0 | interleaved with orchestration |
| Main-thread libgcc identification | 40 | 5,864 | 5,864 | 0 | interleaved with orchestration |
| Total | **113** | **14,960** | **14,996** | **14** | |

Sol solved three targets in five 60-second permuter trials; manual family
adaptation propagated one result to two siblings. One null-guard pattern solved
the longstanding allocator-wrapper backlog. Root solved a seven-member 584-byte
counter family using a static inline clamp with assignments and one return.
The 40 library functions came from identifying and adapting known upstream C,
not 40 independently reconstructed functions. See `docs/LIBGCC.md`.

Root's remaining 1,060-byte motion-family seed differs in four register-choice
instructions. A three-minute permutation trial found no improvement and logged
an upstream AST assertion; the candidate stays uncommitted. Luna A's six-member
family requiring explicit assembly register bindings was rejected and remains
WIP. These failures earn no matched-byte credit.

The library pass exposed 18 previously unlisted function boundaries. The
reporting denominator grows from 11,028 to 11,046 functions; the code-byte
denominator stays 2,415,354. This batch reaches **4,316 functions and 239,684
code bytes (9.923%)**. Matched progress uses next-function spans; it includes
36 bytes retained beyond Luna B's emitted objects.

The intervals include build waiting and vary by target mix; root time includes
integration/tooling. Account-wide usage includes all concurrent work. These
observations do not establish per-model price or a measured cost frontier.
Luna remains useful on bounded work, but pre-draft checks must exclude existing
matches; Sol was more productive in this heterogeneous round. No Astra matcher
was used. A separately requested Astra process/SolDec review began afterward.

## Production batch 3 and resource-allocation change

Base `6834838`. This batch combines the completed third production rounds,
two MGS-focused Sol rounds, and main-thread library/tracing work.

| Worker | New functions | Emitted bytes | Progress-span bytes | Matching interval |
|---|---:|---:|---:|---|
| Luna A round 3 | 6 | 788 | 788 | 19m27s |
| Luna B round 3 | 4 | 264 | 264 | 20m09s |
| Sol 6.1 round 3 | 40 | 6,736 | 6,736 | 24m49s |
| Sol 6.1 MGS rounds 1–2 | 25 | 3,272 | 3,428 | separate targeted assignments |
| Main-thread fdlibm source identification | 22 | 11,584 | 11,584 | interleaved with orchestration |
| Total | **97** | **22,644** | **22,800** | |

Sol round 3 made 71 checks: 42 successful checks, 29 mismatches, and no
check-tool errors. Only 40 successes are new functions; a baseline recheck
and duplicate successful check do not count. Sixteen manual seeds, sixteen
manual family ports and eight automatic ports contributed the 40. One bounded
permuter run found the pointer-base source pattern; a separate bitfield AST
failure was worked around manually. Two state mappings contributed 968 bytes.
See `notes/round3-sol.md` for all attempts and retained candidates.

Luna B then attempted another round for 9m04s and produced **zero** new matches.
The user requested a stronger emphasis on progress, so the parent stopped that
round and reassigned the slot to Sol 6.1, preserving its near-matches and
`notes/round4-luna-b.md`. Sol promptly solved the two retained family seeds;
their eventual gains belong to the following batch and include Luna's draft
work, so they are not independent model trials.

Continuing both Luna workers had become a poor allocation for the user's
immediate progress objective. All current matching slots now use Sol 6.1,
including one permanently assigned to the MGS lead while the usage budget
permits. This is a practical decision based on recent output, **not measured
Pareto efficiency**. There is still no per-model usage attribution, the target
sets differ, and inherited drafts/source matches change difficulty. The library
gain also demonstrates that choosing reusable source can matter more than
scaling individual assembly reconstruction.

The main-thread tracing work adds no native-byte credit. It records validated
script/actor call chains and corrects an execution-sampling error; see
`docs/SCRIPT_TRACING.md`. MGS packet formats, callbacks and comparison limits
are fully recorded in `docs/MGS_GCL_MATCHING.md`. The math units and their
source/license evidence are in `docs/LIBM.md`.

## Production batch 4: targeted Sol rounds and SDK identification

Base `cfd3731`. All matching workers in this batch were Sol 6.1. The two
family rounds reuse earlier Luna drafts and therefore do not measure an
independent model comparison.

| Work | New functions | Emitted bytes | Matching interval |
|---|---:|---:|---|
| Sol family rounds 1–2 | 13 | 1,448 | about 19 + 7.5 minutes |
| Sol round 4 | 8 | 1,912 | 20.10 minutes |
| Sol MGS round 3 | 4 | 536 | bounded 15-minute assignment |
| Root SIIRTC reconstruction | 14 | 1,784 | interleaved with orchestration |
| Root libc memory-source matches | 2 | 180 | interleaved with orchestration |
| Root clone ports from Sol seeds | 2 | 284 | interleaved with orchestration |
| Total | **43** | **6,144** | |

Sol round 4 recorded 47 checks: eight matches, 34 mismatches and five compile
errors; four manual seeds, three manual ports, one automated port. Moving state
counter increments into individual switch cases solved a pair of 520-byte
functions. All attempt details remain in worker notes.

Progress grows by 6,142 bytes because the final memset object's two alignment
bytes fall beyond the reporting code boundary. A newly verified but unintegrated
RTC boundary excludes 464 retained assembly bytes from its preceding function's
credit. The result is 4,456 / 11,047 functions and 268,626 / 2,415,354 bytes
(11.122%). No speed or cost frontier can be inferred from these mixed workloads.
The user extended the stopping threshold to below 40% weekly allowance remaining;
finish in-flight work after crossing it and start no further rounds.

## Production batch 5: source reuse and mixed instruction sets

Base `4852e86`. Reviewed combined output is **135 new functions and 23,220
progress-span bytes**, reaching 4,591 / 11,065 functions and 12.083% code bytes.

| Work | New functions | Emitted bytes | Progress-span gain | Matching interval |
|---|---:|---:|---:|---|
| Sol round 5 | 15 | 2,124 | 2,124 | 18.41 min matching; 20.44 min including notes |
| Sol family round 3 | 8 | 1,688 | 1,688 | 19m20s |
| Sol family follow-up 4 | 2 | 348 | 348 | 3m47s |
| Sol MGS round 4 | 14 | 1,980 | 1,980 | bounded targeted assignment |
| Sol MGS round 5 | 0 | 0 | 0 | bounded 10-minute assignment |
| Root EEPROM source comparison | 5 | 808 | 808 | interleaved with orchestration |
| Root RFU_V1024 source reuse and ARM support | 91 | 18,056 total unit bytes, including 54 existing functions | 16,272 | interleaved with orchestration |

Do not compare the RFU total unit size with incremental worker output. Fifty-four
old C functions were consolidated, not newly solved. New reviewed boundaries
also refine previous spans. Seven ARM ISR routines and the final three retained
assembly stubs receive distinct inventory entries. Source/compiler version
identification produced more bytes than repeated register-only searches here;
this observation does not establish model-specific cost or Pareto efficiency.

The user extended the allowance floor to below **35% weekly remaining**. Three
Sol matching rounds were authorized at 41% remaining. Keep checking before new
assignments and finish only the in-flight work once below that floor. Live
browser verification of decomp.dev is no longer requested.

## Production batch 6 and allowance stop

Base `ed9805e`. All workers are Sol 6.1; no further model-comparison experiment
was started. This batch adds **34 functions / 4,812 code bytes**.

| Work | New functions | Emitted / progress bytes | Matching interval |
|---|---:|---:|---|
| Sol round 6 | 9 | 932 | 14.10 min |
| Sol short follow-up | 1 | 96 | 1.41 min |
| Sol family round 5 | 9 | 1,608 | 13m02s |
| Sol family final round 6 | 4 | 328 | 3m41s |
| Sol MGS round 6 | 7 | 1,236 | about 12 min |
| Sol MGS round 7 | 0 | 0 | 5m23s |
| Root solar timer/IRQ plus teardown | 4 | 612 | interleaved with orchestration |

Three reviewed unmatched Thumb boundaries remove 340 trailing assembly bytes
from these functions' spans. This is an accounting correction, not new C.
The result is **4,625 / 11,068 functions; 296,658 / 2,415,354 bytes (12.282%)**.
The final short follow-ups were assigned at 37%/36% weekly allowance remaining.
When the meter reached 35%, no new assignments or targets were started; existing
work was drained and integrated. It read 34% during final integration. No Luna
or Astra matching workers were resumed, and no reset credit was consumed.

Across the sustained goal from `e2546f6`, 422 functions and 71,970 progress bytes
were added (9.302% to 12.282%). Account allowance went from 73% to 34% during the
run, including concurrent research, tooling, documentation and integration.
There is still no defensible per-model usage attribution or Pareto frontier.
The largest recent gains came from exact historical library/source reuse.
All dedicated MGS rounds together yielded 50 matches / 7,024 emitted bytes.

The solar experiment adds architectural evidence beyond matching: four light
inputs, eight memory snapshots, result-store probes and a verified outdoor
screenshot. The standard local harness now reproduces the corrected probe
behavior. See `docs/SOLAR_SENSOR.md` and `notes/root-batch6.md`.
