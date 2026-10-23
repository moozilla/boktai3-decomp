# Faster depin search: measurements and ideas (2026-10-23)

Question: can the natural-only depin farm (`tools/natfarm.py`) find more
pin-free rewrites per CPU-hour? This note covers what was measured, what
shipped, what failed, and the ideas worth building next.

## Samples and baseline

Two disjoint random samples of 48 functions each from `pinfarm.py list`
(870 hinted functions): A (seed 23) and B (seed 24). Each function ran on
one core.

| Run | Matches | CPU s | Matches per CPU-hour |
|---|---:|---:|---:|
| A, natfarm 300 s (baseline) | 8 / 48 | 10,640 | 2.7 |
| B, natfarm 300 s (baseline) | 5 / 48 | 10,251 | 1.8 |
| B, lazy + learned order, 120 s | 5 / 48 (the same 5) | 4,792 | 3.8 |
| A rows 1-31, baseline | 4 | 6,958 | 2.1 |
| A rows 1-31, lazy + learned order, 120 s | 3 (missed a 3-edit match at 278 s) | 3,390 | 3.2 |

Across both samples, lazy search gives about 1.9 times the matches per
CPU-hour. Two of A's eight matches needed no edit at all: stripping the pin
already matched. The yield on these samples (10-17%) is higher than the farm's
running 4%, because the farm's later functions are larger.

## What shipped

`translate.py --lazy` (natfarm's default now, with `--seconds 120`):

* The old step generated every neighbor of every beam state up front (about
  10 s per state, so 40-70 s per step before anything compiled) and then
  compiled state by state. A 300 s run rarely got past step 2, and the later
  states and passes got no time.
* Lazy mode generates neighbors one (state, pass) pair at a time. Pairs are
  ordered by pass priority first, then by state. It stops at the first match.
  One-edit matches drop from 17-20 s to about 4 s.
* `LEARNED_ORDER` is the pass priority. It counts which passes' edits ended in
  natfarm's best states on sample A. The most productive passes are
  `expand_expr`, `ins_block` (statement macro), `cast_simple`,
  `randomize_internal_type` and `const_var`, and the old order ran almost all
  of them last.
* Evaluation skips the reload-anchor bookkeeping when there are no targets.
  That bookkeeping was 18% of each 31 ms evaluation.
* Every result prints `edits:` (the passes on the path) and each step prints
  `best shape` (see below). Future logs can retrain the order.

## What did not work (negative results)

* **Pinned RTL as a per-pass target.** Natural depins that already merged
  differ from the pinned build's RTL skeleton from the first pass and
  converge only at register allocation. So "make pass P's output equal the
  pinned build's" is the wrong target.
* **Decision-space solve at CSE.** The prototype numbered every CSE
  set-source choice in instrumented agbcc (`AGBCC_CSE_FLIP=k`, `AGBCC_CSE_LOG`;
  not committed) and tried every single flip and the best pairs. No flip of 1
  or 2 choices solved any of 14 sample functions, and single flips almost never
  reduced the differing bytes. Some flips hang agbcc (use a timeout). Pins act
  on register identity, not on which source form CSE picks.
* **Steering by rasolve's targets.** Rasolve finds an exact compiler-space
  solution (one priority or reload change) for 4 of 15 allocation-only
  functions. `translate.py --natural` steered by those targets, 240 s and 4
  steps, matched none of the 4. The compiler-space answer exists, but none of
  the natural edits within reach realizes it.
* **Shared action-reset idiom.** Rewriting the written-out
  state2A8/phase2A9/behavior2AA/counter2AC/field2B1 block as one
  helper/macro applies to only 4 of the 38 pinned functions that contain the
  block, and improves none of them.

## Shape vs allocation

`translate.py` now computes a **shape distance**: halfwords that differ from
the ROM once Thumb register fields are masked. 20 of 87 unmatched sample
functions differ only in allocation (shape 0). The natural search matched 5
of those 20 (25%), against 4 of the 67 with shape differences (6%). Rasolve
solves 4 more of the 20 in compiler space. Triage could put allocation-only
functions first.

## Tooling gaps found

* About 1 in 10 listed functions are never searched. The permuter's
  pycparser round trip breaks their source (GNU statement expressions
  `({ ... })`, among others), so translate's base evaluation fails and natfarm
  logs "no natural edit" in 1 s. Fixing the printer, or rewriting those
  constructs before search, gives about 10% more of the list a chance.
* A sweep that strips the pin and checks the result (1-2 s each) finds the
  free depins. Run it over the whole list before any search.

Both are fixed now. The permuter patch parses a statement expression
anywhere an expression can go and prints it with its parentheses
(`tools/setup.sh` rebuilds the parser tables). That rescued 6 of the 8
failing sample functions. natfarm's triage pass records the free depins
and orders the search: allocation-only first, then fewest differing bytes.
A tools checkout set up before the patch gained param_copy, early_return and
reset_const silently lacked those passes; `tools/setup.sh` re-applies a
changed patch.

## Ideas with the biggest payoff (literature pass)

Related work: Schulte et al., "Evolving Exact Decompilation" (genetic
programming over code fragments from a big-code corpus, scored by byte
distance to the target). Decaf (2026: best-of-N neural sampling with
byte-distance reranking, 39-71% exact matches at N=32). Echo (2026:
rule-based AST mutators, a neural refiner from the asm diff and a reasoning
LLM, in a compiler-feedback loop; 2.4x more exact matches than baselines).
STOKE (MCMC over programs with a shaped cost). Learned edit selection in
genetic improvement and program repair. Constraint models of register
allocation (Unison).

1. **Corpus-mined idioms (Schulte's "big code", applied to this codebase).**
   About 9,000 matched, pin-free functions were written by the same
   programmers with the same macros and inline helpers. Index them by
   register-masked instruction n-grams together with the C lines that
   produced them, using `-g` line info. For a pinned function's differing
   window, retrieve C spellings that produced the same shape elsewhere and
   splice them in. The natural passes only perturb the decompiler's current
   spelling. Retrieval would propose spellings already proven to produce
   this exact code in this codebase. The pinned functions hold contorted
   spellings (zero8/byteZero temporaries), and that is why local edits stall.
2. **Learned edit proposal.** Every farm run labels thousands of (edit,
   context) pairs with a byte delta. A small model trained on these (pass, AST
   context, family, differing window, shape) can rank neighbors before
   compiling them. A global pass order already gave about 2x, and a
   context-aware ranker is the obvious next step. It runs locally, so it costs
   no plan usage. An LLM mutator, as in Echo, is the expensive variant.
3. **Shape-then-allocation pipeline with MCMC.** Search shape distance first
   (`--shape` keeps those states). Accept worse states with Metropolis
   probability so the search can cross plateaus; the current beam is greedy
   with 4 states and 3 steps. Hand shape-0 states to the allocation
   tools.
4. **Inverse allocation as constraints.** Rasolve searches priorities and
   reloads. It fails when the ROM register needs a different conflict set
   (another value live), as in Vorn_UpdateScriptedAction. An ILP/SMT model of
   local-alloc and global-alloc could solve for the minimal change to
   references, live ranges, pseudo order and conflicts. Its unknowns would be
   restricted to deltas that known source levers produce (an extra use, a
   moved declaration, a split variable).
5. **Decision journals for the other passes.** CSE source choice was the wrong
   decision. The pins steer register identity, so the right vocabulary is
   probably "value V lives in hard register R over range L". A journal at
   local-alloc and global-alloc that diffs the pinned and unpinned builds on
   that vocabulary would give a gradient for idea 4.

The order of ideas 1-5 is the recommended build order. Idea 1 is the only one
that adds spellings the current search cannot reach. The other four make the
existing search cheaper.
