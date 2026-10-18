# The foundry: asm to a committed match, measured as a funnel

Tags: #matching #tooling

One pipeline from an unmatched function to a committed match, with every stage
logged so we know where functions die and what each match costs. The driver is
`tools/foundry.py` (see its docstring); state for each function lives in ignored
`build/foundry/FUNC/` (context pack, every check attempt, stages, usage).

## Stages

1. **Context pack** (`foundry.py context FUNC`). The ROM's code, declarations
   for everything it calls or loads (from `include/` and matched `src/`),
   how matched code calls it, and the C of matched relatives that explain at
   least 40% of it (`tools/relatives.py`; weaker ones are only a shared
   prologue and were noise).
2. **Draft and converge** (a model worker). The worker gets `foundry.py brief
   FUNC`, writes `wip/foundry/FUNC.c`, and checks only through `foundry.py
   check`, which logs each attempt's fuzzy score and keeps a copy. The budget
   is 12 checks under 150 instructions and 20 above. The brief lists the plain-C
   levers for register copies; workers otherwise conclude that a pin is
   needed and stop.
3. **Machine stages** (`foundry.py finish FUNC`) on the worker's best attempt:
   - `tools/repair.py` puts the ROM's constants and symbols into the C;
   - when only registers differ, `tools/autosolve.py` runs the compiler-space
     solve and the C search.
4. **Commit**. Each MATCH goes to `src/fn/FUNC.c`, one commit per function,
   through the batch PR flow.

Workers here were Sonnet subagents of the orchestrating session. The headless
`claude -p` CLI would make this unattended, but its login had expired on this
machine.

## First sample (2026-10-17)

The sample was 40 never-drafted functions, 10 per size bucket (`foundry.py
sample`, seed `foundry-2026-10-17`, functions up to 1,500 instructions). It took
about 25 minutes of wall time with 8 workers at once and 3.4M worker tokens in
total.

| bucket | n | first draft fuzzy (median) | worker best (median) | matched | shape off 1–3 | shape off 16+ | checks (mean) | tokens (mean) | tokens per match |
|---|---|---|---|---|---|---|---|---|---|
| <50 | 10 | 85.7 | 100.0 | **8** (8 worker) | 1 | 1 | 5.1 | 63k | 79k |
| 50–150 | 10 | 75.0 | 98.4 | **5** (4 worker, 1 register solver) | 2 | 3 | 7.9 | 70k | 141k |
| 150–400 | 10 | 74.8 | 96.6 | **3** (3 worker) | 2 | 5 | 12.9 | 85k | 284k |
| 400+ | 10 | 62.4 | 74.6 | **1** (1 worker) | 0 | 9 | 17.8 | 122k | 1.1M |

"Shape off" is the aligned instruction distance with registers and immediates
abstracted, after repair: what a C change still has to fix. The 50–150
"shape off 1–3" row counts two register-only residues the solver did not
close (sub_081AA330 at 98.4%, sub_08130F60 at 95.5%).

By instructions, the matched share is 78% for <50, 43% for 50–150, 29% for
150–400 and 9% for 400+. All 17 matches are committed, including
sub_0818C0BC (546 instructions, a jump table, 17 checks) and sub_0812DC28
(235 instructions, 6 checks).

## Where the funnel loses functions

* **Drafting is not the bottleneck.** The first compiled draft is at 62–86%
  fuzzy in every bucket, and no worker failed to compile a draft. Workers
  converge on small functions within about 5 checks.
* **Convergence on large functions is.** In 400+ the median worker ends at
  75% after 18 checks. Every non-matching worker reported the same thing:
  "the structure is right, the register allocation and stack layout are
  not". The shape distance agrees only partly. Once allocation is wrong, the
  extra copies and spills are themselves instructions, so the score has no
  slope: a correct structural fix in one region can lower the fuzzy % because
  registers shift everywhere else. Workers cannot tell a good edit from a bad
  one.
* **One 876-instruction function** got no further than a partial draft. A
  single context is not enough for it; it needs to be worked region by region.
* **repair.py did not help fresh drafts, and hurt five of them.** Its byte
  distance accepted nonsense edits in code whose registers differ (0x144 to 1,
  a 0xFFFF mask to 5). Fixes:
  - an edit must now improve both the shape distance and the count of aligned
    instructions whose values differ;
  - values guessed from nearby code must have the same magnitude;
  - masks are left alone;
  - drafts whose shape is far off are refused (`--force` overrides).

  Its value is in ports from relatives (the 15 matches in #231), not in fresh
  drafts.
* **The register solver** converted 1 of 3 register-only residues.

**Link names.** check.py resolves `sub_XXXXXXXX` and `D_XXXXXXXX` from the
address, but the ROM link does not when a function has a real name. Seven
matches failed the combined build that way. The context pack now shows real
names, and `foundry.py check` fails with a `LINK NAMES` line until they are
fixed.

## Next: a score with a slope for large functions

The bottleneck is a structural signal that survives allocation differences:

1. **Allocation-free score.** The instrumented compiler can force each pseudo
   to the register the ROM uses (`AGBCC_FORCE`, as tools/rasolve.py does). The
   distance that remains under the best forced allocation is the structural
   distance, and it moves monotonically as the structure improves.
   `foundry.py check --alloc` will report it per region of basic blocks, so a
   worker sees "regions 1–4 locked, region 5 differs here" instead of a fuzzy %
   that jumps around.
2. **Region-wise work.** With per-region locking, a 400–1000 instruction
   function becomes a sequence of 30–80 instruction tasks, which is the size
   where workers already succeed (78%). Several workers can take separate
   regions of one function.
3. **Allocation last, by machine.** Once the allocation-free score is 0, the
   remaining work is the class the compiler-space solver handles. It matched 6
   of 40 pinned functions in #230 and 1 of 3 here, and it gets more budget for
   large functions.
4. **Types.** sub_0818C0BC went from 64% to 88% when the worker introduced a
   struct. Sharing recovered struct layouts across a family (callers and
   callees of the same object) in the context pack should move first drafts up.

Run the <50 bucket at scale now: 8 of 10 match at about 80k tokens each, and
1,075 such functions remain.

## Re-measure with the region score (2026-10-18)

`tools/regions.py` gives the structural distance with allocation ignored:
register copies, stack spills and the frame are dropped, registers are
abstracted, and each basic-block region is reported as locked or not. Every
`foundry.py check` prints it, and the brief tells workers to steer by it.
The 16 functions of 150+ instructions that failed in the first sample were
re-run from scratch (`FOUNDRY_RUN=foundry2`, same budgets and model).

* **The match rate did not move: 0 of 16.**
* **Structure did move.**
  - sub_0818B2A0: 2 core instructions off of 494 (97.8%).
  - sub_0823F148: 6 of 393 (96%).
  - sub_0823B0D4: 2 of 132 (99.5%).
  - sub_08142194 (1,001 instructions): 73% to 91%, 29 of 893 off.
  - sub_0812FA0C (876 instructions): 429 to 145 core off.
* **What is left is not structure.** Three kinds of residue remain:
  - register choice;
  - instruction order (one `movs` a few slots away);
  - CSE and rematerialization: agbcc keeps a constant in a callee-saved
    register where the ROM rebuilds it at each use.

  At a 150-second budget the register solver closed none of these (0 of 4).
  Some of the residue is instruction order, which allocation search cannot
  change.

## At scale (2026-10-18, running)

Batches of 6–8 contiguous functions per worker (`foundry.py queue`,
`foundry.py batch`), headless `claude -p` workers (`tools/foundry_run.py`),
and a finisher loop for the machine stages. Each match is committed named
(`#define Name sub_X`, family in `symbols/foundry_families.csv`) and placed in
its unit when the unit holds its `INCLUDE_ASM` (`foundry.py integrate`).

| bucket | finished | matched | tokens per match |
|---|---|---|---|
| <50 | 19 | 16 (84%) | ~45k (batched) |
| 50–150 | 16 | 1 (6%) | — |

The 50–150 functions so far are all from the early ROM (0800D–0802xxxx).
Every failure has the residue above, and no compiler or flag variant explains
it: -O1, old_agbcc, -fno-expensive-optimizations and -fno-builtin were each
worse or equal on 18 drafts. Batching shares context: about 20–50k tokens per
match on <50, against about 80k for one worker per function.

Next: a last-mile search over C edits (statement order, temporaries, scope,
types), scored by core distance and then bytes, run with long budgets on the
drafts that are within 3 core instructions. The first measurement is running
(`build/lastmile/`).

## Codex vs Sonnet (2026-10-18, overnight run)

From 2026-10-18 the at-scale workers run on Codex (`tools/foundry_run.py --backend codex`,
`gpt-6.1-sol`, medium reasoning), which bills to the OpenAI plan. Batches of 6–8
contiguous functions run per worker. These are worker-reported results; integration re-checks
every file before committing it.

| backend | bucket | functions | matched | named | non-cached tokens per match |
|---|---|---|---|---|---|
| Codex gpt-6.1-sol | <50 | 773 | 634 (82%) | 761 | ~39k (both buckets) |
| Codex gpt-6.1-sol | 50–150 (08080000–08180000) | 799 | 550 (69%) | 799 | |
| Sonnet, headless `claude -p` | <50 | 31 | 25 (81%) | 31 | ~$0.8 per match |
| Sonnet subagents | <50 / 50–150 (early ROM) | 24 / 18 | 14 / 1 | | |

Codex matches Sonnet on <50, and on 50–150 in the barely-matched regions it reaches 69%.
The 1-of-18 Sonnet result came from early-ROM leftovers that past workers had already given
up on, not from the bucket itself. At about 39k non-cached tokens per match, Codex is the
default; Claude coordinates.

Disk filled once overnight. The finisher's solver stage writes per-run work directories,
and the volume was at 97% from other sessions. The finisher now runs as several locked
loops (`build/foundry/scale/finisher.sh`), and the stale `build/rasolve-*` directories from
killed runs are left for the user to clear.

