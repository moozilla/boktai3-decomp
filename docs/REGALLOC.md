# Register allocation in agbcc, and how to steer it from C

agbcc is gcc 2.95. Its source is in `build/tools/agbcc/gcc/` after a build
(`local-alloc.c`, `global.c`, `reload1.c`, `cse.c`). This page records the
rules that decide registers, how to see which rule fired, and the C changes
that moved them. Use it before reaching for register pins.

## Tools

* `tools/regdiag.py FILE.c [FUNC] [--nonmatching] [--asm] [--lines]` compiles the file
  with `-g -dp -dl -dg`, aligns the instructions with the ROM and, for each
  register that differs, reports which allocator chose it, the pseudo's
  statistics and the source lines that use it. `--asm` prints the aligned
  listing with insn numbers and source lines.
* `tools/unpin.py [FILE.c...]` strips register pins and `asm("")` barriers
  (outside NONMATCHING drafts) and reports which files still match.
  `--write` keeps the result.
* `tools/regsolve.py FILE.c [FUNC]` replays global allocation from the dumps
  (it says how many registers the replay reproduces) and reports which
  pseudo must outrank which, by how many uses or instructions of lifetime,
  with the C edit; `tools/litsweep.py FILE.c --pairs` tries RAM globals as
  literal addresses for reload rotation. Workflow and measurements:
  [PERMUTER.md](PERMUTER.md).
* `tools/isolate.py UNIT.c FUNC OUT.c [--nonmatching]` copies one function
  out of a unit, so `check.py`, `regdiag.py` and `permute.py` see a file
  with one ROM function. It also splits `int a, b;` declarations, which
  decomp-permuter otherwise crashes on ("nodes should only appear once").
* `tools/reloaddbg.py FILE.c [FUNC] [--nonmatching]`: for each reload register
  that differs from the ROM, the rotation state and why reload skipped the
  ROM's register. `python3 tools/agbccsrc.py reload` and `reload-mode` give the
  rules (a high register copied straight back means a mode change).
* `tools/helperswap.py [--files SRC...]` tries the matched corpus's spellings
  of each inline helper a draft calls. A helper's types often decide the
  allocation (fix.py tries them as `sp_corpus_helper`).
* `tools/promote_draft.py FILE.c FUNC` drops a draft's NONMATCHING guard once
  it matches.
* Raw dumps: `agbcc u.i -O2 -mthumb-interwork -da` writes one file per pass
  (`.rtl .jump .cse .addressof .gcse .loop .cse2 .flow .combine .regmove
  .lreg .greg .mach`). Grep an insn number through them to find the pass
  that changed it. `-dp` puts each insn's number after its instruction in the
  `.s`.

Note: `check.py FILE` without `--nonmatching` compiles a draft's
`INCLUDE_ASM` fallback, so it always "matches". Check drafts with
`--nonmatching`, or isolate them first.

## Which allocator chose the register

`regdiag.py` tags each differing register:

* **local** — a pseudo used in only one basic block (`local-alloc.c`).
  Quantities are allocated in priority order,
  `floor_log2(refs) * refs / (death - birth)` (shorter life and more uses
  first; a value used once gets 0). Ties go to the lower quantity number,
  which is the order values are born in the block. Quantities with a
  *suggested* hard register, from a copy to or from a hard register such as
  a call argument or return value, are placed first, in that register. Each
  takes the first free register over its lifetime.
* **global** — a pseudo live across blocks (`global.c`). The `.greg` dump
  lists them in priority order ("N regs to allocate"); the same priority
  formula uses whole-function use counts and live lengths, which the `.lreg`
  dump prints per register ("used N times across M insns").
* **reload** — insns numbered above every insn in `.lreg` were created by
  reload. That covers constants folded into an address or a large add
  operand (`p + 0x58C`, a literal address), stack-slot accesses, and the
  scratch registers of patterns like `ldrsh rD, [rB, rI]`. **Reload rotates
  through the spill registers in instruction order** (`allocate_reload_reg`,
  `last_spill_reg`): each new reload starts after the register the previous
  one used. One reload more or fewer early in a function shifts every later
  reload register.

## Fixes that worked

* **Reload rotation.** `sub_08160EA4` differed only in r3/r4. Writing the
  random table as the literal `0x0203B400` instead of
  `(u32)gUnk_0203B400` turned its load into a reload, which advanced the
  rotation and fixed both registers. Three other random-table users lost
  their pins the same way (of 40 tried).
  Write such literals through a named macro in `include/global.h`
  (`#define RANDOM_TABLE_ADDR 0x0203B400 /* gUnk_0203B400 */`).
  `make` runs `tools/check_ram_literals.py`, which fails if the address no
  longer equals the symbol in the link map. These are RAM addresses, so the
  ROM shift test can't move them; the check keeps them tied to the symbol if
  RAM layout ever changes.
* **Statement order.** regdiag's line numbers show where the original
  computes a value. `sub_081DC470` matched after moving `half = s + 0xC4`
  below the stores the original emits first, then storing to `s + 0xC4` and
  `s + 0xC6` directly.
* **Load before constant.** `f(s, *p <= 7 ? 0 : 1)` emitted the load after
  the constant. `u8 v = *p; u32 r = 1; if (v <= 7) r = 0; f(s, r);` matched
  (`sub_08172AF4`).
* **Value-first temporaries.** `u8 *sample = (u8 *)(offset + 0x0203B400);
  s.v[10] = *sample;` put `mov r1, sp` after the load, as in the original
  (`sub_08015CF0`).
* **Two shifts.** The permuter found `(c->height >> 2) >> 2` for `>> 4`
  (`sub_08013458`).

* **Loop depth weights uses.** `flow.c` adds the loop depth to a register's
  use count for every reference inside a loop, so an empty
  `do { ... } while (0)` around an assignment raises that value's priority
  without changing the program. `sub_08010514` needed the random table
  pointer to outrank two short-lived constants; four nested empty loops
  around `table = gUnk_0203B400;` did it (found by `tools/translate.py`).
* **Pins can be the difference.** `sub_0802D44C` was a 96.5% draft with a pin
  and an r9-clobbering barrier; without them it matches. Try the draft
  unpinned (`tools/unpin.py`'s `strip_code`) before solving.
* **Natural r7 allocation.** `local-alloc.c` excludes every eliminable register
  in `find_free_reg` (lines1917–1918 in the pinned source), including the Thumb
  frame pointer. Global allocation can still place a pseudo there and relocate
  it if reload needs a frame pointer. In `StackedActor_InitRisingParticle`
  (`080521DC`), reusing one ordinary u32 scratch for768, the loaded Y coordinate
  and the final zero Z velocity gave it multiple lifetimes and moved it into
  global allocation. The compiler then selected r7 naturally, closing the match
  without binding r7 explicitly. Keep independent particle fields typed; the
  reused local is a compiler-lifetime concession rather than shared game state.

## Grid reload scratch registers

The inventory grids provide another reload example. In `080600DC`, manually
binding the large constant289 to r3 produced the desired immediate instruction
but did not advance reload's spill-register rotation. Leaving `(kind + 289)`
ordinary and keeping already-defined r0/r1 temporaries live made reload choose
r3 naturally. Later signed halfword loads then selected the original scratch
registers. The complete 616-byte extent matched; `depin.py` reduced104 trial
hints to44. The Thumb `*extendhisi2_insn` pattern emits both a scratch-constant
move and `ldrsh` from one RTL instruction, so inspect `.greg` and the final
assembly together when diagnosing its scratch choice. Lifetime constraints must
use defined values, and this does not justify binding r7 or writing instructions.

## Known patterns not solved yet

* **Commutative operand order** (`adds r1, r1, r0` vs `adds r1, r0, r1`).
  CSE (`fold_rtx` in `cse.c`) moves an operand with a known constant value
  to the second position. When the original keeps such a value (a pointer
  loaded from a literal) first, CSE didn't know it was constant there; a pin
  has the same effect, because CSE doesn't track hard registers.
  `GameState_SelectRamBase` (`sub_0821B084`) still needs its pin for this.
* **Reused shifts across an inlined test.** GCSE/CSE2 merge `one << i`
  computed in an inline test with the same expression later
  (`sub_08125B68`).

## Empty barriers can change branch length before emitting any instructions

agbcc includes empty `asm("")` statements in its estimated instruction lengths.
With enough lifetime barriers, it can choose a long conditional-branch sequence
even when the final machine-code distance fits a short Thumb branch. The extra
branch can also shift literal pools and padding. Adding more pins can therefore
make a near match worse even though the barriers emit no physical instructions.

Three independently reviewed October 15 examples demonstrate this effect:

* `0806E690` matched all body instructions but had a long failure branch;
  removing seven redundant barriers gave the exact 316-byte extent.
* `0806E7CC` had the same issue in its bounds check; removing four barriers
  gave the exact 308-byte extent.
* `080706DC` was 276 bytes against a 272-byte target at 97.794% fuzzy match.
  A greedy pass allowing equal scores removed ten barriers; the twelfth trial
  shortened the branch and aligned its pools, producing an exact match.

The normal depin pass accepts only exact candidates, so it cannot begin from
these mismatching bases. First remove barriers while accepting unchanged or
improved scores; review the resulting source, then require a complete byte
match and run normal hint minimization. Keep a checkpoint and check the full
reviewed extent, including pools and padding. Do not remove a barrier that
provides the only definition of a value retained by another asm input.

The compiler source confirms the mechanism: `gcc/final.c` in the bundled
agbcc source initializes `asm_insn_count` to one, then adds entries for newline
or separator characters. Even an empty template therefore contributes one
estimated instruction when branch shortening computes its length. Check the
actual emitted branch distance before attributing the extra branch to C flow.

## Replacing pins with plain C (depin worker, 2026-10-21)

A Sonnet worker took the 60 smallest pinned functions (`progress.py
--list-pinned`) and made 28 of them clean C with no pins or asm, plus 5 with
fewer hints. Per-function edits and the blockers on the rest are in
`notes/depin-manual_2026-10-21.md`. The edits that replaced pins most often:

1. Many pins only made up for an earlier mistake: a parameter copy
   (`T *x = input`), or a missing `return 0;` or early return. Use the
   parameters directly and add the early returns first; the pin often goes away.
2. `do { stmt; } while (0);` around a use raises its loop-depth weight and so
   its register priority. It replaces a pin that forces a register order, and
   it can be nested (three levels in ScriptUtility_SetFlag800).
3. `(*(&x))++` or `(*(&x)) <<= n` instead of `asm("" : "+r"(x))`: the value
   becomes opaque without asm, which blocks biv reversal, early increments
   and CSE reuse.
4. Small type and order changes: a `u8` constant temp, one int temp reused
   for two constants, swapped `+` operands so the right load comes first,
   storing `a + b` straight into the destination. Falling off the end of a
   non-void function leaves the incoming r0, which removes a pinned result.
5. For an unpinned copy under about 20 bytes off, `tools/permute.py FILE
   RealName --run N -j 2` found 5 of the 28 (use the real name, not sub_X).

Second batch (19 of the next 60 made clean; `notes/depin-manual-2_2026-10-21.md`):

6. An `asm("" ::: "rN")` clobber often stands in for an early return: a body
   wrapped in `if (x) { loop }` matched once written `if (!x) return 0;`
   before the loop. Splitting a compound final condition into two early
   returns works the same way.
7. A `static inline` wrapper that takes the arguments in another order
   (`Wrap(size, p) { Fn(p, size); }`) changes which temporary gets the low
   register; it replaced r0 pins (the permuter finds these with its
   inline-wrapper mutation).
8. Declaring a pointer temporary inside the loop body removed `"+r"` barriers
   on loop-carried pointers.
9. A permuter score of 0 is not a match: it ignores branch offsets. Port the
   result by hand and run check.py.

Third batch (14 of the next 50; `notes/depin-manual-3_2026-10-21.md`):

10. Set a loop constant twice (`u32 one = 1;` before the loop and `one = 1;`
    inside it): with two sets loop.c no longer hoists it. Replaced an
    `asm volatile("" : "+r"(c))`.
11. Reuse an existing local for a second value instead of a new temporary, or
    re-derive `&actor->field` in some uses next to a local copy.
12. A named int for a constant call argument (`int id = 0xB; Task_Create(id, ...)`)
    and `(*(&off))` on a constant's first use both change which register the
    constant gets.
13. The permuter's dummy statements (`x++; x--;`, `s->q = s->q;`) are not
    acceptable depins; leave the pin instead.

## Are the pins a sign of the wrong compiler or flags? (2026-10-21)

No. `tools/flagprobe.py` took the 150 smallest pinned functions, stripped their
hints, and compiled each under 12 variants: agbcc -O2 (the project's), old_agbcc
-O2 and -O1, agbcc -O1 and -O3, and agbcc -O2 with -fno-regmove,
-fno-expensive-optimizations, -fno-strength-reduce, -fno-gcse,
-fno-caller-saves, -fno-rerun-loop-opt/-fno-rerun-cse-after-loop, and
-fno-cse-follow-jumps/-fno-cse-skip-blocks.

| Variant | New exact matches | Closer than -O2 | Further |
|---|---:|---:|---:|
| old_agbcc -O2 | 0 | 33 | 45 |
| agbcc -O1 | 2 | 41 | 67 |
| -fno-gcse | 2 | 8 | 11 |
| -fno-cse-follow-jumps/-skip-blocks | 2 | 15 | 26 |
| every other variant | 0 | ≤22 | ≥ as many |

Six scattered single matches, no group, and every variant is further from the
ROM on more functions than it is closer: -O2 agbcc stays the best fit, as the
~9,900 exact matches already suggest. Pins are a C-spelling problem.

`do { } while (0)` in particular is the standard statement-macro idiom, and
inline helpers and macro arguments are where re-reads such as `*(&x)` and
extra temporaries come from. The original source very likely had macros and
inline functions (flag tests, list walks, fixed-point helpers) that expand to
exactly these shapes, so a do/while(0) depin is best rewritten as the macro it
probably was.

## Which compiler pass a pin acts on: the idiom families (2026-10-21)

`tools/passdiff.py FILE.c FUNC` compiles a pinned function and its
hint-stripped copy with `agbcc -da` and compares the RTL after every pass
(rtl jump cse addressof gcse loop cse2 flow combine regmove lreg greg jump2
mach) on an operation skeleton: registers anonymous, low-part subregs folded,
plain register copies dropped, hard register numbers kept only after reload.
The first pass that differs, and the shape of the difference, name what the
pin really steers. `--batch LIST OUT -j 4` runs it over `progress.py
--list-pinned` (930 functions in about 15 minutes):

| Family | Functions | What the pin changes | Natural levers (translate.py `--natural` passes) |
|---|---:|---|---|
| const-in-reg | 249 | A constant stays in a register instead of being folded into an instruction or re-materialized (mostly at cse, some at expand) | One named constant shared by two uses; set the constant again inside a loop (`reset_const`); a `u8`/`u16` constant temp; field access vs. a precomputed pointer for large struct offsets (`struct_ref`, `const_var`) |
| alloc | 197 | Nothing until allocation: the RTL is identical through lreg; only the hard register differs | Declaration order (pseudo numbers break ties), use parameters directly (`param_copy`), split or merge a variable (`split_var`, `remove_var`), inline-wrapper argument order, early returns. If `rasolve.py` says "no priority change helps", the ROM's register comes from a conflict: the original had another value live there |
| branch | 105 | Jump threading or block order | Early returns (`early_return`), split or combined conditions (`condition`, `reverse_cond`), loop form |
| order | 95 | Same instructions, different order | Statement order (`swap_stmts`, `reorder_stmts`), call argument order, commutative operands, declaration placement (`move_to_use`) |
| expr-shape | 91 | Different arithmetic or address form | `struct_ref`, `compound_assignment`, `add_sub`, index vs. pointer |
| combine | 55 | Instruction fusion at combine/regmove | Split or chain an assignment, add or remove a temporary (`temp_for_expr`) |
| extend | 35 | A zero/sign extension appears or disappears | The declared type of the variable (`randomize_internal_type`), a cast |
| spill | 34 | Reload or spill code at greg | Shorter live ranges: `move_to_use`, `split_var` |
| load-reuse | 28 | CSE reuses a loaded value instead of loading again (or the reverse) | Re-read the field instead of a local copy, or keep a copy; a store through a pointer or a call between the uses forces the reload |
| header | 23 | The hint is in a header helper shared by other units | Unpin in the header and check every caller |
| hoist | 11 | Loop-invariant motion | `reset_const`, declare inside the loop |
| none | 7 | The stripped copy compiles to the same RTL | (stripping should already match) |

The first run of this classification found two tooling gaps, both fixed:
`unpin.py` never stripped function-pointer pins (`register void (*f)(T *)
asm("r1")`; 36 pins in 15 files the pin farm had never really tried), and the
farm skips a function whose hint lives in a header helper.

Every lever above is now a natural pass in `translate.py --natural` (26
passes; the ones that can change meaning are allowed there because only exact
matches are kept), and `tools/natfarm.py` runs that search deterministically
over the pin farm's list. The families show why a mechanical inversion is
hard: only alloc (21%) is pure allocation, which `rasolve.py` can solve in
compiler space. Everything else is a different decision in an earlier pass,
and the C spellings that produce that decision are many-to-one. The most
likely original spellings that no pass yet generates are macros and inline
helpers (see the flag section above).

### do/while(0) is a macro: write the macro

A `do { ... } while (0)` that a match needs is not a trick to avoid; it is the
expansion of a statement macro, which the original source almost certainly
had. The rule is that it must be written as a named macro, not left bare:

* An empty `do { } while (0);` is `DEBUG_NOP();` (include/global.h): the shape
  a debug check compiled out of a release build takes. Its loop notes still
  split the code around it, which is what the match needs.
* fdlibm code uses newlib's own word macros (`EXTRACT_WORDS`,
  `GET_HIGH_WORD`, ...; include/libm_compat.h).
* A body that repeats across functions goes in that object family's header
  as one macro; the enemy action reset at 0x2B0/0x2B5/0x21C/0x2C8 (Golem,
  Mummy, Ghoul, Dog, Vorn, three `SetControl`s, sub_08130F60) is the largest
  such group still written out.
* A one-off wrapped body: name the macro for what it does, local to the unit.

`translate.py --natural` may now insert `do { } while (0)` blocks
(perm_ins_block); natfarm turns an empty one into `DEBUG_NOP()` and rejects
`if (1)` blocks.
