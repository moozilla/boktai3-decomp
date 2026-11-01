# ChargedBurst_EmitSpark (082335F0): matched (2026-11-01)

Tags: #matching #charged-burst #random-table #compiler #local-alloc #reload #combine #global-alloc

**Matched, pin-free** (commit `d7da7d897`, merged in PR 965). Two ordinary-C
changes to the holdout draft did it:

1. **The table pointer is a union assembled from its two halves** (the
   081EEECC idiom): `union ChargedBurstTableSource { const u16 *table; u16
   halves[2]; }`, `source.halves[0] = (u16)RANDOM_TABLE_ADDR; source.halves[1] =
   RANDOM_TABLE_ADDR >> 16;`, then `table = ChargedBurst_ReadTable(&source)`
   (an inline returning `source->table`). cse cannot fold the two half
   insertions, and combine folds them into 0x203B400 without a REG_EQUAL note.
   So update_equiv_regs has nothing to make a REG_EQUIV from, and the spilled
   table gets the ROM's slot at sp+4 (shape 36 to 4; 63 / 59 differing).
2. **Each sample address is index first**: `(const u16 *)(index *
   sizeof(*table) + (u32)table)`. `&table[index]` expands to `(plus table
   (index*2))`. expand_binop does not swap it, because flag_force_mem has
   already loaded any memory operand into a register (o1, o3 below). At the z
   sample both operands die, and local-alloc ties the sum to the first, the
   table. That puts table and z sample in r6 at 6 refs over 58 insns, just
   denser than p (11 over 108). So global.c's takeover of a sparse local
   register skips r6, and p takes r5. Index first, the sum ties to the shifted
   index, the table sits alone in r6 (4 over 50), and p evicts it. The table
   moves to r5, mask to r4, and the 0x2c constant is rematerialized, as in the
   ROM. Then the reload rotation matches as well.

`tools/scope/README.md` facts 48 and 49 describe both. `frame` now suggests the
union when it finds the spilled-constant pattern.

| Draft (`build/scope/drafts/cb2/`) | Change | Differing |
|---|---|---|
| base | the holdout draft | 50 / 40 (shape 36) |
| u1, u4, u5, u7 | halves union (read per sample, or once into `table`) | 63 / 59 (shape 4) |
| u2 | `source.table = gRandomTable` | 50 / 40, as base |
| u3, u6 | halves with `&source.table[index]` / `table = source.table` (address never taken) | 54 / 62, 54 / 62 (shape 38) |
| o1 | helper returning `&source->table[index]` | 63 / 59 |
| o2 | halves stored through a pointer helper: the union stays in memory (two `strh`) | 55 / 57 (shape 28) |
| o3 | `ChargedBurst_ReadTable(&source) + index` | 63 / 59 |
| i5 | `(const u16 *)((u32)table + index * 2)` (pointer first) | 63 / 59 |
| i4 | `(const u16 *)((u8 *)table + index * 2)` | 63 / 59 |
| b1 | index first without the union | 50 / 40 (shape 36) |
| i1, i2, i3, i6 (landed) | union + index first (`* 2`, `<< 1`, `(u32)index * 2`, `* sizeof(*table)`) | **exact** |

The rest of this note is the holdout record as it stood before the match.

## What the ROM shows

```
ldr  r5, =0x203B400    @ gRandomTable
str  r5, [sp, #4]      @ stored at once
...
adds r3, r3, r5        @ x sample: the register is still live
...
ldr  r5, [sp, #4]      @ y sample: reloaded from the stack
...
ldr  r1, [sp, #4]      @ z sample
```

The table pseudo has a hard register, is spilled by reload, and has no
equivalence, so reload gives it a stack slot. In every draft the table's set is
a pool load, `(set (reg 36) (mem/u (symbol_ref .LC0)))`. cse adds
`REG_EQUAL (symbol_ref gRandomTable)`, and update_equiv_regs turns that into
`REG_EQUIV` because the pseudo is set once. When reload spills it, it reloads
the constant (`ldr rN, =0x203B400`) at each use and deletes the set.

## What the draft gets right

`if ((u8)(delay & byteMask) > 1)` with `byteMask = 255` set first. The ROM
keeps 255 in sl (`movs r3, #0xff; mov sl, r3`), which needs byteMask to have no
equivalence. combine merges `byteMask = 255`, `t = delay & byteMask` and the
QImode truncation as a three-insn combination. byteMask is still used at the
`nextSpark` wrap, so its set comes back as `newi2pat`. distribute_notes keeps a
REG_EQUAL note only when it was on i3, so the re-emitted set has none, and
update_equiv_regs finds no note. With `(u8)delay > 1` (the earlier unit draft),
255 keeps its equivalence and is reloaded as a constant. The table needs the
same missing note. Its uses are additions that need a register operand, so no
combination can absorb its pool load.

The rest of the draft is unchanged: the x, y and z jitter blocks, `*seed =
index` before z, and the zero in sb (`movs r3, #0; mov sb, r3` before the x
store), which the timer and both resets reuse.

## The table across the ROM

A scan of `build/asm/nonmatching/` lists every function that loads a pool
address, stores the register straight to a stack slot, never stores to that
slot again, and reloads from it later. Outside loops, the only such constant
is the random table, in four functions:

* 082335F0 (this one);
* 081EEECC and 0807D2E0, both still `INCLUDE_ASM`;
* WirelessGameDisplay_UpdateRoundAnnouncement, matched only with a
  `u16 *volatile table` and register pins. A volatile reads the slot at
  every use, which is wrong here, because the ROM's x use reads r5.

Inside loops, 21 matched functions get it in ordinary C, with no pins. All
but one are monsters and scene actors using
`SceneActorChase_RandomDirectionExcept`. There, the
`SceneActorChaseRandomTable` struct is initialized from a `randomSamples`
variable set many blocks earlier. The struct is read inside the loop, and
cse1 does not record stores through `MEM (ADDRESSOF)`. After purge_addressof
the read is a register copy, and loop.c hoists it. cse2 does not know the
source's constant in that block, so the copy has no note and no equivalence
(insn 2954 in Mummy_ResumePlayerPursuit). That needs the constant's set to be
outside the block that holds the copy. This function is one block from entry
to the last table use, and cse1 and cse2 both follow it (entry, then the `if`
body), so every copy of the table gets the note.

The other one, SparkParticle_Spawn (`src/fn/sub_0806A160.c`), keeps the table
in a union with the angle parameter. The union's first set copies the
parameter, and the parameter is then reassigned, so cse can't substitute the
later read of the angle. The union pseudo has two sets, and the first has no
note, so update_equiv_regs calls no_equiv before it records anything: no
equivalence and no doubled live length. Here a union with delay or next (u1
to u3) fails, because a union member store is a copy from a temporary that
stays valid. cse replaces the member's reads with the temporary, flow deletes
the dead first set, and the table is again the only set.

## Ruled out

Numbers are scope's ROM / draft differing instructions.

| Draft | Change | Differing |
|---|---|---|
| unit draft | `(u8)delay > 1` | 49 / 37 (shape 34) |
| v3 (landed) | `(u8)(delay & byteMask) > 1` | 50 / 40 (shape 36) |
| v3, `AGBCC_NOEQUIV=36` | knob, diagnostic | exact (build and `-g`) |
| v1, v2 | `(delay & byteMask) > 1`, or `& 0xFF` twice | 52 / 38 |
| v4 | v3 with a `u8` count | 50 / 40 |
| t1 | table at function scope | 53 / 43 |
| t3 | table assigned again before each sample (cse deletes the sets) | as v3 |
| w1, w2 | `table += index` or `table = &table[index]` for z | 48 / 50 (shape 8) |
| u1 to u3 | unions of the table with delay or next | as v3 |
| r1 to r3 | the table in a one-member struct, read directly or through a pointer | as v3 |
| s1, s2 | one u32 `work` for delay (or next) and the table | 33 / 31 (shape 26) |
| s3 | `work` for delay, next and the table | 58 / 56 |
| d1 | the `if` body in `do { } while (0)` | 56 / 46 |
| d2 | each sample in `do { } while (0)` | 73 / 61 |
| d3 | struct table set before the test, read in the body | 49 / 39 |
| d4 | the table set before the test | 49 / 39 |
| lb1, lb2 | the table as an integer address in a `u32` variable | as v3 |
| lb3 | the integer address written in each sample, no variable | 20 / 18 (shape 14) |
| la1 to la4 | one `u32 work` for the `nextSpark` offset 0x204, then the table | as v3, or 51 / 47 |

Why the near misses fail:

* **lb3** has fewer differences but the wrong mechanism. The table is set at its
  first use, keeps r4, and the 1023 mask takes the spill. The mask has an
  equivalence, so it is rematerialized (`ldr r5, =0x3FF` at each use). The ROM
  spills the table and keeps the mask in r4.
* **la1 to la4** tried a variant of fact 20's lever: another value first in the
  table's pseudo, as in Dog_UpdateTurnSequence (there the first value is not a
  constant; here it is the offset 0x204, and the second, different constant
  would call no_equiv). r5's
  `movs r5, #0x81; lsls r5, r5, #2` before the table load is not a pseudo.
  It is reload's register for the constant in `(plus p (const_int 516))`.
  cse folds a `work = 0x204` variable into that add, so the variable's first set
  dies, and the table is again set once.

* **w1** gives the table a second set, so the second set calls no_equiv. But
  update_equiv_regs records the equivalence at the first set and doubles the
  live length before that happens. The table then outranks the x sample
  address for r5, and the address spills.
* **s1 and s2** make the shared pseudo span blocks. It is no longer a local
  quantity, and local-alloc swaps seed and the zero between r8 and r9.
* **d1** ends cse1's block at `NOTE_INSN_LOOP_END`, but cse2 ignores loop
  notes, so the table keeps its note. byteMask takes the stack slot instead.
* **d3** relies on cse1 not recording stores through `MEM (ADDRESSOF)`. After
  purge_addressof, cse2 sees the copy in the same block and adds REG_EQUAL.

What would give the table no equivalence, from update_equiv_regs
(`build/scope/agbcc/gcc/local-alloc.c`):

* a second set whose note is missing or differs, coming first in insn order
  (otherwise the doubling of w1);
* a set through a SUBREG, as of a DImode struct, which would need a register
  pair;
* a register copy with no REG_EQUAL note;
* a set that combine or regmove re-emits after cse2.

The ROM rules out a pair: the table is in r5 and seed in r8. No copy escapes
cse in one block. regmove only rewrites two-address patterns such as `ands`
and large-immediate `adds`, and the table's uses are three-register adds.

## Round 8, 2026-11-01: the union lever and reload's arithmetic

Still not matched. Two things are new.

**The union table view (PR 944) does nothing here.** With `union
RandomTableView table = {gRandomTable};` opened after the position copy and
`table.samples[index]` for the three samples (un1), the score is v3's (50 / 40,
shape 36), and the table pseudo still has `REG_EQUIV` (insn 101). The
initializer expands to `(clobber (reg 69))` and then the pool load. In a loop,
loop.c counts the clobber as a second set and keeps the load in the loop (fact
21). Here there is no loop, and the clobber is dead because the set follows it
at once. Flow deletes it before update_equiv_regs runs, and the table is again
set once from the pool.

**The table may have only four refs.** At the x jitter's add (insn 178)
reload needs two low registers. r3 is already a spill register; for the second
it compares refs times the number of usable registers at or above each one (a
quirk of order_regs_for_reload, README fact 32): r5 (the table, 4 refs) 28, r4
(the 0x3FF mask, 4 refs) 32, r1 (the x position address, 3 refs) 33. The table
loses r5 only while 7 x refs < 32, so its pseudo can have one set and its three
uses, nothing more. That is why every lever that kills the equivalence with a
second set fails:

* w1 (the table reused as the z sample pointer) and w_next1 (a `work`
  variable that holds `nextSpark` first): 6 refs, 42. Reload keeps the table
  and evicts the position address or the mask.
* a live CLOBBER or any other extra set: 5 refs, 35, above the mask's 32.

REG_N_REFS is recounted after combine (recompute_reg_usage; README fact 17 is
corrected), so a ref that combine removes doesn't lower the table's count, and
one it removes from the mask doesn't raise the mask's.

With one set and no equivalence, update_equiv_regs (local-alloc.c) leaves two
cases:

* the set is a register copy with no REG_EQUAL note. cse2 puts one on every copy
  here, because the function is one extended block from entry to the last
  sample. A second variable for the y and z samples (cp1) is merged by cse,
  as before;
* the source is in a single-register class (`CLASS_LIKELY_SPILLED_P`, which
  for Thumb means STACK_REG). The table's uses are three-register adds, so its
  class is LO_REGS.

A pool load with no note would still get `REG_EQUIV (mem)`, because the table
is local to one block. So no C form found yet gives the ROM's allocation.

**Scope, from this session:**

* `frame` now finds ROM slots that hold a pool constant: the slot is stored
  once, right after its `ldr rN, =C`. It names the draft pseudo whose
  REG_EQUIV keeps the constant out of memory, and scores `AGBCC_NOEQUIV` on it.
  Here it reports MATCH. It reports MATCH for 08234868 as well, and finds the
  same slot in 081EEECC and 0807D2E0.
* Knobs set in the environment now reach the build's compile too (see the
  README's paper cuts).

## Scope gaps

* `AGBCC_NOEQUIV=REG,...` is new (commit `2d1a19e0f`). The proof above needed
  it, because `AGBCC_NOREG` leaves the equivalence in place.
* scope could flag the pattern itself: a ROM `ldr rN, =CONST; str rN, [sp, #K]`
  with later loads from `[sp, #K]`, where the draft's pseudo for CONST has a
  `REG_EQUIV` note. `frame` shows only that the ROM has slots the draft lacks.
* update_equiv_regs doubles a pseudo's live length at its first set even when a
  later set removes the equivalence. `why` doesn't show this. It decided w1.
