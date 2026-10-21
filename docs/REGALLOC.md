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
