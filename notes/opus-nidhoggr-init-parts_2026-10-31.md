# Nidhoggr_InitParts (081940F8): holdout

Tags: #matching #nidhoggr #compiler #cse #combine #local-alloc #loop #scope-gap

**Not matched; holdout.** `Nidhoggr_InitParts` is 904 bytes. The guarded draft
in `src/large_scene_animation_controller.c` has been replaced. The new draft
differs from the ROM in 12 bytes (`tools/check.py --nonmatching`: fuzzy 99.9%,
6 differing instructions after realignment). In scope that is 3 ROM / 3 draft
instructions, one order difference. The previous draft differed in 54 bytes
(2 / 2 in scope, 8 after realignment), with a `lowerBound` variable standing
in for the ROM's zero. The forked store `build/matchsearch/opus_081940F8`
(from `build/handoff/matchsearch/resume_081940F8_agent`) has the unit draft as
node `eeeba637` (label `unit-g7k`), plus the probes `base`, `r5`, `g1`, `g7`,
`g7k` and `g9`. Scratch variants are in `build/scope/drafts/nid/` (worktree
scratch, not committed).

## What the ROM shows

The last collider loop (colliders 18 and 19) has a dead zero between the flag
OR and its store:

```
ldrh r1, [r4, #6]
mov  r0, r8          @ flags 0x1008
orrs r0, r1
movs r6, #0          @ dead: r6 is next written by a reload at func+0x2D2
strh r0, [r4, #6]
```

It is the field-store zero of `Nidhoggr_OrPartFlags` (`p->flags |= mask`).
store_fixed_bit_field clears the field with `(and:HI (mem) (const_int 0))`, an
HImode pseudo that cse folds to `(const_int 0)`. That zero's register pushes
`flags` from r6 (the previous loop's register) to r8. The two earlier loops
lose the same zero. Armor_UpdateWalk (081826C8) keeps a zero like this one
(`build/handoff/frontiers/damage-and-orbit/NIDHOGGR.md`).

## Mechanism

Three passes all have to keep the zero:

1. **cse1 must reuse it for a later field store.** The palette index write
   `t->rows[21].widget.paletteIndex = 0xf1a3` is also a field store. cse makes
   its mask zero the loop's zero: make_regs_eqv keeps the older register
   canonical. That store's IOR now reads the loop's zero. combine folds the IOR
   to the constant, since the zero has no nonzero bits. distribute_notes then
   puts `(use (reg:HI zero))` with the death note after the last jump before
   the store, which is the loop's back branch. The USE keeps the zero live
   through flow2 and emits nothing.

   For that, the loop body and the palette store have to be in one cse1
   block. Before the loop pass, cse stops at NOTE_INSN_LOOP_END (cse.c, `if (!
   after_loop && ... NOTE_INSN_LOOP_END) break;`). Every C loop has that note
   right after its exit test, so the last loop has to be a goto loop.

   The kind of store matters too. A store through a `u16 *` has no mask zero.
   A store through a cast pointer
   (`((struct DisplayKeyframeState *)&t->rows[21])->paletteIndex`) has one, but
   it addresses the field as row + 0x3A (`ldr r5, =0x5C1C; strh r0, [r1,
   #0x3a]`, g7w). The ROM's `t + 0x5C56` needs a row element type that contains
   the field. `rows` is now a local `struct NidhoggrPartRow` whose first 0x3C
   bytes are the shared `struct DisplayKeyframeState`. Only this function uses
   `struct NidhoggrPartInit`. The unused pointer helpers and
   `struct NidhoggrPaletteRow` are gone.
2. **update_equiv_regs must not replace it.** local-alloc.c:1196 marks a
   register for replacement when three things hold: its REG_N_REFS is 2, it
   spans blocks, and its REG_EQUIV note equals its source. update_equiv_regs
   then puts the constant into the one use and deletes the set. The `.lreg`
   dump shows `(use (const_int 0))`. recompute_reg_usage (flow.c:4365, called
   right before local_alloc) weights those refs by loop notes. In a plain goto
   loop the set and the USE count 1 each, and the zero is deleted.

   Two later field stores don't help (g6). Only the last use has a death note,
   so combine leaves one USE. Armor_UpdateWalk survives because its two
   consumers are on different branches, and combine leaves two USEs. A loop
   note pair around both the set and the USE doubles their counts, to 4.
   `do { ... } while (0)` around the goto loop and the palette writes does
   that, and the zero survives in r6. This is the new draft.
3. **loop.c must place the argument constants.** This is the draft's remaining
   difference. In the last loop, the 0x120 argument and the
   `Nidhoggr_ReceivePartContact` address are loaded before r0 and r1.
   expand_call copies costly constant arguments into pseudos (calls.c: thumb
   has SMALL_REGISTER_CLASSES, and preserve_subexpressions_p returns 1 at -O2).
   Outside loops, the shared 0x120 pseudo gets no hard register, so reload
   loads the constant in place. In loops 1 and 2, loop.c puts
   each single-use constant into its argument move (scan_loop,
   `reg_single_usage`), which gives the ROM's order. combine won't do it:
   combinable_i3pat refuses to set an argument register right after another
   argument register is set. loop.c skips the do-while (0) loop ("Loop from 756
   to 907 is phony" in the `.loop` dump), because the loop doesn't start at a
   label.

   Starting the do-while at the goto label lets loop.c scan it (g9: 21 / 21,
   132 bytes). That fixes the argument order. But loop.c also hoists the zero
   itself out of the loop ("Insn 808: regno 237 (life 25) ... moved to 929"),
   and the palette store's address and 0xF1A3 with it.

So the ROM needs three things together:

* one cse1 block from the last loop body to the palette store, with no
  loop-end note between them;
* the zero's set or its USE inside a loop note pair;
* loop.c scanning the last loop without hoisting the zero or the palette code.

No source I tried gives all three.

## Ruled out (this session)

Numbers are scope's ROM / draft differing instructions, and matchsearch's byte
mismatches where given.

* Real loops for the last collider pair, with the member palette store (n0 to
  n3, r1 to r5). cse1 stops at the loop end and the zero is deleted. The old
  draft is 2 / 2 (54 bytes); r5 is also 54 bytes.
* Goto loops (g1 to g6). cse1 reuses the zero, combine leaves the USE, and
  update_equiv_regs deletes the zero. g1 is 23 / 21 (246 bytes), and it has
  the argument-order difference too. g5 writes the palette index twice and g6
  adds a second constant field store. Both still leave one USE.
* `do { } while (0)` around the goto loop only (g8, 23 / 21): cse1 stops at
  its loop end.
* `do { } while (0)` starting at the goto label (g9, see point 3).
* The palette index written through a `DisplayKeyframeState *` cast (g7w,
  6 / 6).
* The 0x120 and callback arguments as variables set before the loop (g7c1 to
  g7c3, 19 / 21 to 20 / 22).
* The earlier rounds' attempts (NIDHOGGR.md): 1,213 compilations of values
  carried out of the loop (last collider index, relative index, next index),
  single-row palette loops, native halfword vector records. The frontier's
  zero-offset witness (`nidhoggr-zero-witness.patch`) reproduces all 904
  bytes, but the code has no reason for it: a union bitfield read as a row
  offset that is always zero.

## Scope gaps

* For a ROM-only constant that nothing reads, `why` could check whether the
  draft's matching register was replaced by update_equiv_regs. The signs are a
  `(use (const_int N))` in the `.lreg` dump and weighted refs of 2. Here that
  was found by hand from the dumps.
* `why 1215` on the out-of-order 0x120 load says "cse swapped equal values".
  The actual cause is a local pseudo that loop.c never put into its use.
  scope could say whether the insn lies in a loop that loop.c scanned, or in
  one it skipped as phony (the `.loop` dump has both).
