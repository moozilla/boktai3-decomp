# Research handoff: making matching cheaper (2026-10-23)

This is a factual record of one research thread ("Faster depin search"), for
a new session that starts fresh. It lists what exists, what was measured and
what is still open. It does not recommend a direction.

## The problem as measured

* About 994 matched functions still carry register pins or empty `asm("")`
  barriers (`python3 tools/progress.py --list-pinned`). Removing the hints
  and getting the same bytes from plain C is "depinning". The same skills
  apply to unmatched drafts.
* `tools/passdiff.py` sorts pins by the compiler pass they act on
  (`docs/REGALLOC.md`, "idiom families"): constant kept in a register 27%,
  allocation 21%, branch 11%, the rest smaller.
* The Mac pin farm had made about 30 natural depins in 8 CPU-hours before
  this work.

## Tools that exist

| Tool | What it does |
|---|---|
| `tools/check.py`, `tools/build.py` | Exact unit check; full ROM build |
| `tools/regdiag.py FILE FUNC` | Aligns your code with the ROM. For each register difference, names the allocator (local, global or reload), with each pseudo's uses, live length and priority order |
| `tools/rasolve.py FILE FUNC` | Searches allocation priorities and reload rotation *inside* the compiler (instrumented agbcc), and prints the solution as inequalities between source values |
| `build/agbcc-instr` (`tools/agbcc-instr.sh`, `tools/patches/agbcc-instr.patch`) | agbcc with environment knobs (below). With none set, it emits the same code as agbcc |
| `tools/rtlprobe.py` | Per-pass RTL dumps (`dump`); loop invariant decisions (`loop`); gcse hash buckets (`gcse`); sweeps of the knobs (`try-t`, `try-env`); per-line instruction surplus (`perline`) |
| `tools/translate.py --natural`, `tools/natfarm.py` | Beam search over decomp-permuter passes that excludes pins and asm. natfarm runs it over the pin list (`docs/PERMUTER.md`) |
| `tools/pinfarm.py` | Lists pinned functions with pin-free isolated copies; applies results |
| `tools/explain.py`, `tools/idioms.py` | Per-line mismatch classes, plus matched C elsewhere with the same shape (older; see `docs/MODEL_BENCHMARK.md`) |
| `tools/bigcode.py mine` / `query FUNC FILE` | Index of 10,232 pin-free functions by register-masked instruction 4-grams, with the C lines that produced them; for a draft, the clean spellings of the ROM's shape |
| `tools/why.py FILE FUNC` | For each differing instruction run: the source lines, the insn's trail through the passes (`-da` dumps), what the creating pass does, and one counterfactual probe (`AGBCC_NOTIE`) |
| `docs/AGBCC_RULES.md` | Compiler-decision-to-source rules, verified only (two so far) |

Instrumented agbcc knobs:

* `AGBCC_PRI` / `AGBCC_FORCE`: priority delta, or a forced hard register, per pseudo.
* `AGBCC_SPILL`: reload rotation.
* `AGBCC_GCSE_T`: the gcse hash table size.
* `CSE_KEEP` / `CSE_NOCANON`: CSE choices.
* `AGBCC_DUMPPRI`: dump allocation data.
* `AGBCC_NOTIE=uid,...`: no output/input tie at these insns, in regmove and local-alloc.
* `AGBCC_SERVER`: fork server, used by `tools/agbserve.py`.

## Measured results

Samples A and B are 48 random pinned functions each (`notes/depin-search-2026-10-23.md`).

| Change | Result |
|---|---|
| natfarm lazy search with a learned pass order (#569) | About 1.9x matches per CPU-hour, with the same matches |
| Parsing GNU statement expressions in the permuter (#570) | About 10% of the list had never been searched; 6 of 8 sample failures could now be searched |
| Triage: free depins first, allocation-only (shape 0) next (#570) | Natural search matches 25% of shape-0 functions and 6% of the rest |
| Corpus retrieval (#572) | 30% of differing ROM windows appear verbatim in clean code. One depin came from it (SceneActor_TickRandomReaction) |
| Retrieval trial (10 farm failures, 2 Sonnet agents, about 10 compiles each) | 0 matches with retrieval, 0 without. With it, 8 of 10 got smaller; without it, 3 of 10 |
| Reading the compiler instead of searching (this branch) | Modifier_ApplyToStat matched on the first compile (below). The farm and both trial agents had failed on it |

Earlier, outside this thread: `docs/MODEL_BENCHMARK.md` found that
explain.py's per-line "what differs" report did not beat check.py's plain
listing for Haiku or Sonnet.

## Negative results

* **Pinned RTL as a per-pass target.** Natural depins that already merged
  diverge from the pinned build's RTL at pass 1 and converge only at
  allocation.
* **Flipping CSE source choices** (1 or 2 at a time; not committed): 0 of 14
  functions.
* **natfarm steered by rasolve's compiler-space solution:** 0 of 4.
* **A shared action-reset macro:** fits only 4 of 38 functions and improves
  none.
* **Corpus retrieval:** not a step change. The remaining near misses need
  spellings that clean code never uses.

## The one clear positive: reasoning from the compiler source

Modifier_ApplyToStat differed only in where `movs r1, #0xff` sits, before or
after the loop's pointer and counter inits. loop.c emits hoisted invariants
before the loop start first, and strength reduction's inits after them. So
the ROM order means the original was `for (i = 0; i < 8; i++)
record[i].order = -1;`, not a hand-written pointer loop. It matched on the
first try and is committed on this branch. The rule is in
`docs/AGBCC_RULES.md`.

## Open patterns, with what is known

* **Mask-copy group:**
  * Functions: EffectPart_UpdateAnimation, EffectPart_UpdateLoopAnimation,
    Trigger_UpdateIdleAnimation and AnimatedHazard_AnimateHold.
  * ROM: `movs r3,#1; adds r1,r3,#0; ands r1,r0`. Drafts get
    `movs r3,#1; ands r1,r3`.
  * Cause: regmove (forward and backward) and local-alloc tie the AND's
    output to the dying input. `AGBCC_NOTIE` on that insn reproduces the
    copy, so the decision is known. No spelling found yet stops all three
    ties.
  * Conditions: `docs/AGBCC_RULES.md`. The only corpus hits are
    ScenePanelSubject_Tick*, which pin the same thing.
* **SceneHud_UpdateActionSwapSelection, Vorn_UpdateScriptedAction:**
  2-byte register swaps that rasolve cannot solve in compiler space.
  Forcing the ROM's registers makes it worse, so allocation alone does not
  explain them.
* **Untried ideas from the literature pass:**
  * a learned ranker for permuter edits, trained on farm logs;
  * shape-then-allocation MCMC;
  * an ILP/SMT model of local- and global-alloc, to solve for the source
    delta;
  * allocation decision journals.
  * Literature: Schulte "Evolving Exact Decompilation"; Decaf; Echo; STOKE;
    Unison.
* **Not finished:** a trial of why.py with AGBCC_RULES.md against plain
  diagnostics. It was stopped before any result. Its 15-function set
  (`build/why/`, local only) was the 9 open functions above plus
  SceneHud_UpdateActionSwapSelection, Vorn_UpdateScriptedAction,
  ActorDirection_ComputeRelativeCardinal, CircleWindow_GenerateScanlines,
  sub_082356E4 and MotionPool_Update.

## Practical notes

* Isolated pin-free copies: `python3 tools/pinfarm.py list OUT_DIR`
  (`OUT_DIR/iso/`).
* For per-pass questions: `tools/rtlprobe.py dump` on an isolated copy, then
  grep the insn number (`@ N` in `u.s`) through the `u.i.*` dumps. The
  compiler source is in `build/agbcc-instr/gcc/`.
* Don't use `pkill -f` or `pgrep -f` with patterns that can match your own
  shell command; that killed this thread's shell three times.
