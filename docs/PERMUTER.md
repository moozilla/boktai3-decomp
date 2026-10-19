# Solving register allocation in the compiler, then in C

Tags: #matching #tooling

Research from 2026-10-17 on why large functions stall on register allocation,
and the tools that replaced blind search. Read [REGALLOC.md](REGALLOC.md) first
for how agbcc allocates. Per-run evidence lives in ignored `build/autosolve/`,
`build/translate/`, `build/cmp/` and `build/chunk/`.

## Workflow

FILE.c holds one ROM function (`tools/isolate.py UNIT.c FUNC OUT.c
--nonmatching`). Use the venv Python (`build/venv/bin/python`, for capstone).
`tools/agbcc-instr.sh` builds the instrumented compiler once (20 seconds).

0. `tools/repair.py FILE.c FUNC` first: most near-misses are wrong values,
   not wrong allocation (see Findings). It substitutes the ROM's constants and
   symbols into the C in seconds and lists the shape differences left.
1. `tools/autosolve.py LIST.tsv` runs everything below over many functions
   (rows `FUNC<TAB>FILE.c`), on the draft and on a copy without register
   pins, and writes `build/autosolve/results.tsv` with any matching source.
2. `tools/rasolve.py FILE.c` says what the compiler would have to do
   differently: which value must outrank which in allocation, or where one
   more reload is needed. It answers in seconds and says when the difference
   is not allocation at all.
3. `tools/translate.py FILE.c` searches C for that change: every edit a
   permuter pass can make to the lines involved, scored by how far the
   change has got and whether anything else broke.
4. Reload registers alone: `tools/litsweep.py FILE.c --pairs` tries RAM
   globals as literal addresses (the commonest reload fix) deterministically.
5. `tools/permute.py FILE.c --run 600 --focus auto` is the blind fallback,
   now fast and crash-free.

Big functions (500+ instructions), or a difference rasolve calls "not
allocation" (CSE, PRE, loop hoisting, instruction selection): use the
measure / variant-batch / pass-dump loop in [BIG_FUNCTIONS.md](BIG_FUNCTIONS.md)
(`tools/anchordiff.py`, `tools/variants.py`, `tools/rtlprobe.py`).

Every result is reviewed like permuter output and confirmed with
`tools/check.py`; upstream passes do not always keep the program's meaning.

## Why blind search stalled

The permuter's score is flat for register problems: a candidate either
swaps the two registers or it does not, so thousands of mutations score the
same and nothing points the walk towards the fix. In a side-by-side run
(7 near matches, 15 minutes each, `-j 3`), upstream, the fork and the
focused fork improved only one function each beyond the first match:

| Function | Base | Upstream | Fork, blind | Fork, `--focus auto` |
|---|---:|---:|---:|---:|
| ScenePool_StateLaunchShortArc | 10 | 10 | **0** (9 s) | 10 |
| SceneActorControl_TickRandomMotion | 20 | 20 | 20 | 20 |
| sub_081CC64C | 10 | 10 | 10 | 10 |
| sub_0802D44C | 25 | 25 | 25 | 25 |
| ContactSequenceActor_UpdateDying | 30 | 30 | 30 | 30 |
| Anim_SetSequence | 30 | 20 | 20 | 20 |
| sub_08010514 | 65 | 65 | 65 | 65 |

Scores are the permuter's (lower is better). The one match came from the
fork's literal-address pass. Two of the stalled functions are now matched by
the compiler-space method below.

## Solving in the compiler (rasolve.py)

`tools/agbcc-instr.sh` builds `build/agbcc-instr` from the fetched agbcc
plus `tools/patches/agbcc-instr.patch`. Without its environment variables it
emits exactly what agbcc emits (checked on all 6,422 files in `src/` and the
101 drafts). With them:

| Variable | Effect |
|---|---|
| `AGBCC_PRI=reg:delta` | add to a pseudo's allocation priority (local and global) |
| `AGBCC_PRI_LABEL=label:delta` | the same for the pseudos labelled LABEL |
| `AGBCC_FORCE=reg:hard` | allocate a pseudo to a hard register |
| `AGBCC_SPILL=k:d` | advance reload's spill rotation by D before the K-th reload choice |
| `AGBCC_SPILL_LABEL=label#n:d` | the same before the N-th reload of a value labelled LABEL |
| `AGBCC_DUMPPRI=file` | log every pseudo's priority, lifetime, uses and register before reload, and every reload with its source line |
| `AGBCC_FORCE_LABEL=label:hard` | `AGBCC_FORCE` by label |
| `AGBCC_SERVER=rfd:wfd` | fork server (below) |

Labels name a value across different sources: a C variable (followed
through inlining), or a constant (`=65535`, `=gUnk_0203B400`), with `#N`
for the N-th later pseudo with the same label (`=0#3`). Reloads are
labelled by the constant they load. The search also tries changing a
single reload's register (shift at one reload, undo at the next), since a
plain shift moves every later reload too.

`rasolve.py` compiles the draft, then searches these knobs with real
compiles until the bytes match, and reports the smallest set of changes:

```
sub_08010514: 16 differing bytes
EXACT MATCH in compiler space with 1 change(s):
  table (pseudo 28): raise priority 0.182 -> 1.273 (global, live 44, 4 uses; now r5)
    above pseudo 70 (priority 1.000, 2 uses, life 2) ...
    source: 62: table = gUnk_0203B400;
```

or `reload #1 (sub_0802D44C.c:90 size.y = 128;) should come 1 spill
register later: add 1 reload before it`. When nothing helps it says the
difference is instruction selection or CSE, not allocation. Forcing the
ROM's registers onto the differing pseudos (`AGBCC_FORCE`) was tried first
and is rarely enough: it breaks the conflicts the other pseudos rely on.
Searching priorities instead keeps the allocator's own rules.

## Translating to C (translate.py)

The compiler-space answer becomes measurable targets (`rasolve.py
--json-out`): the labelled value's priority, or the number of reloads before
the labelled reload. `translate.py` then:

* enumerates neighbours instead of sampling them: each permuter pass runs
  under a recording random source that walks all of its choices like an
  odometer, restricted to the lines the targets point at (the state's own
  dump says where they are now);
* compiles every candidate twice through the fork server: as written, and
  with the instrumented compiler told to close whatever remains of the
  target gap. The second count, the residual, is what still differs once
  the target is met: 0 means the edit broke nothing on the way;
* keeps a beam of the best on-path states (residual, then gap), the best gap
  closers and the fewest bytes, and tries each distinct compiled code once;
* also edits the static inline helpers the function calls (their code is
  compiled into it), and anchors a reload without a constant label to its
  statement's text;
* uses only passes that keep the program's meaning unless `--unsafe` is
  given (A4FC's best "improvement" with statement moves read an
  uninitialized pointer). Two passes were added for this: `perm_call_order`
  (side-effect-free arguments through an inline wrapper in another order,
  so they load in that order) and `perm_swap_stmts` (adjacent independent
  statements).

`sub_08010514` matched in four steps (6.5 minutes): residual 0 throughout
while the gap fell from 0.857 to 0.571 and the bytes from 16 to 0. The edit
was four empty `do { } while (0)` loops around the table load. gcc 2.95
counts a register's uses weighted by loop depth (`flow.c`), so the loops
raise the pointer's priority exactly as the solver asked. That idiom is now
in [REGALLOC.md](REGALLOC.md).

Without targets (`--targets none`), the same search runs on the lines
regdiag reports as different, by bytes alone.

## Unpinned drafts

Register pins can be the problem rather than the fix. `sub_0802D44C`
(96.5%, a pin to r2 and a barrier clobbering r9) matches exactly with both
removed. Of the 25 drafts with pins, it is the only one; the others get worse
without theirs. `autosolve.py` tries both versions.

## The pin farm: removing pins from matched code

Units marked `// MATCHING_CONCESSION: register-pinning` already match, and
the C is right apart from the hints. With the hints removed only register
allocation differs, so they are the solver's best input. `tools/pinfarm.py`
turns the chain into a background job.

Measured on 2026-10-17 (all 677 marked units, then a random 40 of the 596
functions left):

| Step | Result |
|---|---|
| `depin.py`, one pass | 324 units improved, 5,575 -> 4,265 hints, 64 units clean |
| `depin.py`, repeated to a fixed point | 39 more units; 23 functions only lost their last pins this way |
| autosolve sample, 40 functions | 6 matched: 3 unpinned copies already matched (depin's old single pass missed them), 3 translated by the solver |
| near misses | 5 functions within 1-2 bytes after the C search; 8 more within 10 |
| failures | 2 functions `rasolve.py` cannot assemble (gas rejects its line-info output); 1 renamed by `#define` and 1 isolated with a helper's body (both fixed) |

So about 8% of the remaining functions translate, 15% counting what the
fixed-point depin now catches first. Time: 323 s of wall time per function
per worker on average (120 s compiler search, 300 s C search, less when
the solver finds nothing), with four workers under `nice -n 10` on a
machine shared with another solver run. All 596 take about 13 hours with
four workers.

The translations seen so far are small: empty `do { } while (0)` blocks
around two or three statements (they raise the uses' loop-depth weight), a
re-read `*(&offset)`, a pointer alias re-declared inside a loop.

### The overnight run (2026-10-18)

All 604 functions still holding pins after depin ran through `autosolve.py
--fast` with 2 workers (`-j 2` or `-j 3`, `nice -n 10`). It ran about 7
hours of wall time, including a restart after a machine crash; the workers
resumed from `results.tsv`. The load average was often 25-40 from other
sessions.

| Verdict | Functions |
|---|---:|
| translated to a match | 83 |
| partly solved in the compiler; C search fell short | 163 |
| not allocation; C search fell short | 139 |
| no compiler solution and too far for the C search | 111 |
| skipped by triage (more than 64 halfwords off) | 88 |
| solved in the compiler; C search fell short | 19 |
| partly solved, too far for the C search | 1 |

`pinfarm.py apply` committed 82 of them, one commit per function, in
batches #247, #251, #263 and this one. 7 matched in isolation but not once
spliced back into units that main had since merged or reorganized, so they
were left pinned. Register-pinned code went from 8.627% to 8.003% of total
code; other merges during the night also move this number. The yield,
13.7% of pinned functions, is about twice the sample's 8%. Most of the gain
is the step budget, which no longer depends on load.

What is left: 163 + 19 partly or fully solved in the compiler, where the C
search (2 steps, beam 2) did not find the edit. A longer `translate.py`
run on just those, `--steps 4 --beam 4`, is the next farm pass.

### Running it

On a 16 GB machine use four workers at most (one crashed at `-j10`), each
`autosolve.py -j 1`, under `nice -n 10`. Build `build/agbcc-instr` first
(`tools/agbcc-instr.sh`) and a full `make` so `build/asm/` exists.

```sh
python3 tools/depin.py -j 4 --write $(grep -rl 'MATCHING_CONCESSION: register-pinning' src)
build/venv/bin/python tools/pinfarm.py list --shards 4        # wip/farm/list.tsv, list-K.tsv, iso/
for k in 0 1 2 3; do
  nohup nice -n 10 build/venv/bin/python tools/autosolve.py wip/farm/list-$k.tsv \
    -j 2 --fast --out wip/farm/out/s$k > wip/farm/shard$k.log 2>&1 &
done
```

`--fast` (2026-10-18) is what makes the farm practical. It checks each
function with one compile and skips it if it is more than 64 halfwords away;
in the sample those never reached the C search. Its budgets are counts, not
seconds: rasolve stops after 5,000 compiles, translate after 2 steps with
beam 2. Time budgets lost matches whenever other jobs loaded the machine.
Every sample match came from the top beam states within two steps. On the
40-function sample it keeps all 3 translations and runs at about 97 s per
solved function per worker, at load 25-36. translate.py's neighbor
generation is also 15-30 times faster: it copies only the edited function
(a slot clone), not the whole preprocessed file, and reuses the printed
text of the other top-level items. It produces the same neighbors, checked
on 12 functions. autosolve also passes `-j` to rasolve, which otherwise
used every core in every worker, and kills a timed-out run's process group,
which left agbcc servers behind.

Each worker appends to `wip/farm/out/sK/results.tsv` and skips functions
already there, so a stopped worker restarts with the same command. Re-run
`pinfarm.py list` only between runs: it rewrites `iso/`.

Whenever results come in (they can be applied while the workers run):

```sh
cat wip/farm/out/s*/results.tsv > wip/farm/results.tsv
build/venv/bin/python tools/pinfarm.py apply wip/farm/results.tsv --list wip/farm/list.tsv --commit
```

`apply` splices each `MATCHES` or `TRANSLATED` function into its unit,
undoes every hunk of the translation the match does not need (the permuter
re-spells lines as it goes), drops the marker when the unit has no hints
left, runs clang-format and `check.py`, and makes one commit per function.
Review the commits like permuter output, then run the combined ROM build
before integrating. Rows other than matches are the next work: `C search
at 1 bytes` is worth a longer `translate.py --seconds` run by hand.

## Compile speed

Per candidate on an idle machine (A4FC-sized function, 694-line base):

| | Before | Now |
|---|---:|---:|
| agbcc | 8.5-24.5 ms | 3.2-13 ms: built with `-O2` (agbcc's Makefile uses `-g`), identical output |
| shared declarations | parsed every time | parsed once: fork server |
| `compile.sh` | 31 ms (shell, mktemp, perl, as, ld) | `as` only; bytes resolved in Python |
| objdump for scoring | 3 ms | none |

The compile server: with `AGBCC_SERVER`, a marker byte after the declarations
stops the parser; each request forks a child that reads only the function
and writes its assembly. It gives the same assembly as a normal compile for
every permuter variant tested (140 variants of five drafts), and
`agbserve.Resolver` turns the unlinked object into ROM bytes equal to the
linked bytes. `rasolve.py` now does 355 compiles in 4 seconds (12-22 s
before), and `translate.py` evaluates about 200 candidates per second on
nine workers.

The permuter itself (`tools/permute.py`) also got faster: its `compile.sh`
no longer starts Python per candidate (2.5 times the candidates under load),
and `-j` defaults to all cores. Earlier "slow" runs were mostly a machine at
load 40-70 from other searches. On an idle machine the permuter does about
130 candidates per second at `-j 6`. For the ROM build an `-O2` agbcc saves
only about 20% (small functions are dominated by process start), so the
build keeps the stock compiler.

## The decomp-permuter fork

`tools/setup.sh` applies `tools/patches/decomp-permuter-agbcc.patch` to the
pinned upstream in `build/tools/decomp-permuter` (replacing an older version
of the patch). `tools/permute.py` refuses an unpatched checkout.

Crash fixes. On the A4FC and 82184 drafts, upstream crashed in 10 of 592
pass applications (8 of 16 for `perm_temp_for_expr`, its most used pass);
the fork crashed in none of 688 (`build/ptest/stress.py`, which applies each
pass directly):

| Crash | Cause | Fix |
|---|---|---|
| `Anonymous declarations cannot happen in functions` | a local `struct {...};` or union definition | such declarations are skipped |
| `KeyError` in `perm_temp_for_expr` | a pass failed after changing the tree, so the next pass saw nodes missing from the index; also names the type map lacks | every pass is rolled back if it fails or crashes after a change; unknown names type as `int` |
| `nodes should only appear once in AST` | `struct T *a, *b;` shares one type node | shared nodes are copied when the source is parsed |
| `dereferencing non-pointer` | two locals with one name in different blocks | shadowing locals are renamed (`i_2`) before the search |

New passes (`src/agbcc_passes.py` in the patch), each an edit that has
closed matches by hand here; weights are in the `[gcc]` section of
`default_weights.toml`, and `permute.py --weight PASS=W` overrides them:

| Pass | Edit | Why it moves registers |
|---|---|---|
| `perm_literal_address` | `gUnk_0203B400` -> `(*(T *)0x0203B400)` (RAM only; ROM data would break the shift test) | the constant becomes a reload and advances the spill rotation |
| `perm_move_to_use` | moves `x = e;` down to its first use, or up to just after what it reads | lifetime is the divisor in both allocators' priority |
| `perm_split_var` | a reused variable gets a second variable from one assignment to the next | separate pseudos, often local instead of global |
| `perm_const_var` | a constant becomes a typed local (`u16 one = 1`), at the top, before the statement, or in the expression (`f(x, zero = 0)`) | which copies agbcc shares |
| `perm_loop_form` | `for` <-> `while` <-> `if (c) do {} while (c)` | loop reversal, exit test place |
| `perm_addr_deref` | `x` -> `*&x` | stops CSE treating an address as a constant (the `trap` fix) |
| `perm_call_order` | `f(a, b, K)` -> `f_2(K, a, b)` through a static inline wrapper | argument load order (the `CpuSet` fill idiom) |
| `perm_swap_stmts` | swaps two adjacent independent statements | instruction order without changing meaning |

Declaration order and adding or removing a temporary already existed
upstream; their gcc weights are raised. `--focus auto|LINES` restricts all
passes to source lines (`focus_lines` in `settings.toml`); for a differing
reload it includes the lines from two agreeing reloads before it, because a
reload register is set by earlier reloads, often one that is missing.

## Other tools

* `tools/regsolve.py` replays global allocation in Python from the
  `.greg`/`.lreg` dumps (95.7% of global registers in the 101 drafts; reload
  evictions are not modelled) and gives priority arithmetic for each
  differing pseudo. `rasolve.py` supersedes it where the instrumented
  compiler is built.
* `tools/chunk.py` extracts statements as a function of their live-ins and
  searches them alone. On `SceneInput_RebuildGrid` the extracted region
  started 13 instructions worse than in place (CSE across the region
  boundary is lost) and the search's result made the whole function worse
  when spliced back (10 -> 34 differing shapes). `--inplace` (mutating the
  region inside the function) is the usable form; `--map` scores each
  statement's shapes against the ROM.

## Results

Matched in this work (1,892 bytes):

| Function | Was | How |
|---|---:|---|
| `ScenePool_StateLaunchShortArc` (0803E7B4, 484 B) | 99.1% | the fork's `perm_literal_address` in 9 s; `litsweep.py` finds it deterministically |
| `sub_0802D44C` (292 B) | 96.5% | `rasolve.py` said "one reload of rotation"; the draft matched with its pin and barrier removed |
| `sub_08010514` (376 B) | 93.0% | `rasolve.py` + `translate.py`: four empty loops around the table load (above) |
| `TaggedSceneNpc_Create` (081D2A38, 740 B) | 96.0% | `autosolve.py`, unattended: the model id in one local (`perm_const_var`) |

`autosolve.py` over the drafts, best first (first 18 functions of 98; the
run continues in `build/autosolve/results.tsv`):

| Verdict | Functions |
|---|---:|
| translated to a match | 1 |
| solved partly in the compiler (residue 1-10 halfwords as is) | 5 |
| not allocation (instruction order, CSE, call-argument order) | 11 |
| no draft function by that name | 1 |

Distances are halfwords after alignment. Most of the near matches end 1-10
halfwords away. What is left there:

* Scratch and reload registers that rotation cannot reach: in the ROM a
  register was free that ours holds (often a call argument loaded too early,
  `sub_08010C10`, `ContactSequenceActor_UpdateDying`). `perm_call_order`
  fixed the argument order in `sub_08010C10` (8 -> 2 bytes); its last
  scratch register needs one more reload before the function's first one,
  which no meaning-preserving edit found produces.
* CSE choices the compiler knobs do not cover (`sub_081CC64C`: each zero
  store uses the other zero's register; `sub_08125B68`: a shift merged
  across an inlined test, 1 halfword).

A4FC (`NamedObject_CreateFromScript`, 66%): in the compiler, lowering the
object pointer's priority just below one pseudo (0.518 -> 0.510; ties go by
definition order) and raising one zero constant halves the distance
(477 -> 240). The C search's best result (84.6%) read an uninitialized
pointer and was rejected; that is why the translator now uses only
meaning-preserving passes by default.

## Findings

* Search speed was not the main problem; the score was. A flat score cannot
  guide a random walk to a register swap. Solving in the compiler gives an
  exact target in seconds, and the residual (bytes left once the compiler
  closes the remaining gap) gives the C search a slope.
* Reload rotation and allocation order are small, enumerable spaces inside
  the compiler. Most of the drafts' register differences are one or two
  changes there.
* Pins and barriers are worth removing before solving: they can be the
  difference themselves (`sub_0802D44C`).
* agbcc counts uses by loop depth. Empty `do { } while (0)` loops raise a
  value's priority without changing the program (`sub_08010514`).
* Extracting regions as separate functions loses the context that decides
  their code; restrict changes in place instead.

### What the near-misses actually are (2026-10-17)

Of 118 near-miss drafts and ports, compared with registers abstracted and then
also immediates abstracted, 10% differ only in registers, 6% only in
registers and constants, 18% in one to three instructions of shape and 66% in
more. The allocation solver above addresses the first group only, which is
why it matched 7 of about 145 functions. On the 21 relatives.py ports at 90%
or more, every one was "not allocation": wrong constants, callees or a
missing call or guard copied from the relative. Hand fixes matched 10 and
`tools/repair.py` matched the next 3 on its own in under 6 seconds each,
where autosolve had spent several minutes each and found nothing. Allocation
search is the right tool for register-pinned code (structurally right by
construction), not for drafts.

