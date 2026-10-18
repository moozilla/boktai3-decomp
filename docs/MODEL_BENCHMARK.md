# Small-model matching benchmark: explain.py vs check.py

Tags: #matching #tooling

October 17, 2026. Can Haiku and Sonnet workers match near-miss drafts, and does
the per-line report from `tools/explain.py` (with idioms from
`tools/idioms.py`) help them more than `tools/check.py`'s aligned listing?

## Method

* **Functions.** 15 unmatched near misses from the 118-draft shape survey
  (permuter-research drafts, isolated one per file), chosen for 1–10
  instructions of structural difference after register and constant
  abstraction, 17–81 instructions long. One survey row (Pool1B8_FindByName) had
  been matched since; sub_0816AC64 replaced it.
* **Conditions.** Each function × {Haiku, Sonnet} × {explain, plain}: 60 runs,
  one fresh subagent each, same task text apart from the tool paragraph. Each
  run had a private copy of the draft and a `check` wrapper that compiled it,
  counted and logged each use, and refused after 10. *explain* runs got
  `tools/explain.py` output plus free `tools/idioms.py` lookups; *plain* runs got
  `tools/check.py` output (aligned ROM | yours listing, `!` on differing lines).
  Both could read `src/`, `include/`, `tools/asmat.py` and `tools/relatives.py`.
  Register pins and empty `asm("")` barriers were allowed (the project's
  concession); hand-written instructions were not.
* **Scoring.** A run counts as matched only if `tools/check.py` on the run's
  final file says MATCH. "@N" is the check (out of 10) on which the wrapper log
  first recorded exit 0.

## Results

| function | insns | explain Haiku | plain Haiku | explain Sonnet | plain Sonnet |
|---|---:|---|---|---|---|
| sub_08125B68 | 40 | MATCH@9 | no (2 B) | no (33 B) | MATCH@9 |
| Collider_Refresh | 19 | MATCH@2 | MATCH@1 | MATCH@1 | MATCH@1 |
| sub_0816AC64 | 80 | MATCH@5 | no (94 B) | no (94 B) | MATCH@6 |
| Registry3C8D_RunFromScript | 27 | no (51 B) | MATCH@10 | no (48 B) | MATCH@6 |
| Route_Start | 39 | no (4 B) | no (8 B) | MATCH@6 | MATCH@7 |
| ScriptActor_GetField534FromScript | 23 | no (26 B) | no (26 B) | no (20 B) | MATCH@7 |
| StageIconDisplay_SelectUpdate | 15 | no (2 B) | no (6 B) | MATCH@5 | MATCH@6 |
| sub_08126000 | 52 | no (4 B) | no (89 B) | no (7 B) | no (89 B) |
| sub_08229440 | 16 | no (16 B) | no (29 B) | MATCH@7 | MATCH@7 |
| sub_082285C4 | 81 | no (12 B) | no (12 B) | MATCH@8 | MATCH@4 |
| sub_08234770 | 36 | MATCH@8 | MATCH@7 | MATCH@5 | MATCH@5 |
| sub_0823652C | 55 | no (10 B) | MATCH@4 | no (65 B) | no (63 B) |
| sub_081CC4CC | 48 | no (57 B) | no (6 B) | no (6 B) | no (8 B) |
| sub_0822B968 | 54 | MATCH@3 | MATCH@5 | MATCH@5 | MATCH@4 |
| sub_08239CDC | 38 | MATCH@2 | no (48 B) | MATCH@2 | no (15 B) |
| **matched** | | **6/15** | **5/15** | **8/15** | **11/15** |
| checks to match (mean) | | 4.8 | 5.4 | 4.9 | 5.6 |
| subagent tokens (mean) | | 73k | 75k | 61k | 61k |

"no (N B)": differing bytes in the final file (not always the best attempt;
some runs ended on an unchecked or worse edit).

The first explain report averaged 1.1 KB (max 2.3 KB); the first check.py output
averaged 2.2 KB (max 4.0 KB).

## Reading

* **The tool did not raise the match rate on this set.** Haiku: 6 vs 5. Sonnet:
  8 vs 11. With 15 functions the per-function outcome is mostly noise. Discordant
  pairs: Haiku 3 explain-only vs 2 plain-only, Sonnet 1 vs 4. Neither difference
  is significant (a sign test on 1 vs 4 gives p ≈ 0.38), but there is no
  evidence of a benefit, and Sonnet leans the other way.
* **Small models can match near misses.** 13 of the 15 functions matched in at
  least one run, and 12 of those 13 are now committed (below). Sonnet matched
  8–11 within 10 checks, Haiku 5–6. Matches took about 5 checks in all four
  conditions, and token use was the same with and without the tool.
* **Where explain won: a structural hint pointing at a type.** sub_08239CDC
  matched in 2 checks for both explain runs and in neither plain run. The idiom
  list pointed to `src/fn/sub_0813D34C.c`, whose struct field at 0xAB0 gave the
  ROM's `ldrh` and register use. Collider_Refresh and StageIconDisplay ("extra:
  movs R, #0" at the return, so the function is void) were quick in both
  conditions.
* **Where explain lost: a single register copy.** Registry3C8D,
  ScriptActor_GetField534 and sub_08125B68 each came down to one extra or missing
  `adds rX, rY, #0`. explain labels this "register copy only" with a generic
  hint. Its idioms for these regions were weak matches (~0.75–0.86 similarity)
  and gave no lever. The plain runs that matched found it in the full listing:
  a second pointer variable (ScriptActor), an early `return -1` plus a
  `goto fail` layout (Registry), or a pinned constant (sub_0816AC64, which
  `tools/depin.py` later showed works as a scoped local without the pin). The
  report shows only the differing span plus one neighbour, which may hide the
  context the plain listing gives.
* **Register-only remnants stay hard.** sub_08126000, sub_081CC4CC and
  sub_0823652C ended within 4–10 bytes in several runs. That is the
  regdiag/rasolve stage, which neither condition was told to use.
* **Task-rule artifacts.** StageIconDisplay_SelectUpdate is really `void`, but
  its header declares `s32`. The two Haiku runs stopped rather than conflict
  with the header. The Sonnet runs matched by renaming around the include or
  with an `asm("name")` label. Neither is committable as is.

## What to change next (not yet measured)

1. For copy-only regions, print the whole basic block on both sides, not just
   the bracketed span. Suggest the concrete levers that worked here, in order: a
   second variable holding the same value; an early return or `goto` that
   changes block layout; a scoped constant local (`{ u32 v = K; *p = v; }`).
2. Show idioms only at ≥ 0.9 similarity, or when the shape is exact.
   Low-similarity hits cost context and were never what solved a run.
3. Add `--full` to explain.py (the aligned listing under the per-line report),
   so a worker gets both views from one check.
4. Re-run this benchmark (`build/bench/`, recreated by the steps above) on a
   fresh set of near misses after those changes, and use 30+ functions so that
   a 2–3 function difference means something.

## Matches committed from the benchmark

One commit per function on the tools branch, checked with one combined ROM
build: sub_0821FE40 (Collider_Refresh), sub_08125B68 (pinned), sub_0816AC64,
sub_0822BFC8 (Registry3C8D_RunFromScript), sub_0821E5C4 (Route_Start, pinned),
sub_0815A838 (ScriptActor_GetField534FromScript), sub_08229440 (pinned),
sub_082285C4 (pinned), sub_08234770, sub_0823652C, sub_0822B968 and
sub_08239CDC. Each takes the cleanest matching run, preferring plain C, with
pins reduced by `tools/depin.py`.
