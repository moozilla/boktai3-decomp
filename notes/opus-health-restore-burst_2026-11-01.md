# HealthRestoreEffect_StartParticleBurst (08234868): holdout, one equivalence short

Tags: #matching #health-restore #random-table #compiler #loop #local-alloc #reload #holdout

**Not matched.** `HealthRestoreEffect_StartParticleBurst` stays `INCLUDE_ASM`.
The best ordinary-C draft is in the unit's `#ifdef NONMATCHING` block: fuzzy
84.4% (the unit's earlier draft was 82.8%), scope 17 ROM / 13 draft
instructions differing (was 20 / 16), shape 10 (was 12). It has no register
pins, barriers, inline assembly or compiler overrides. With the scope knob
`AGBCC_NOEQUIV=29`, which gives pseudo 29 (the random table) no equivalence,
the build's compile of this draft is byte-identical to the ROM (the `-g` one
too; this note first said only `-g`, an artifact of a scope bug fixed on
2026-11-01: a knob set in the environment didn't reach the build's compile).
That is the same
single blocker as ChargedBurst_EmitSpark
(`notes/opus-charged-burst-spark_2026-10-31.md`). The matchsearch store is
`build/matchsearch/opus_08234868` (root = the landed draft, nine ruled-out
nodes with their hypotheses). The scope change from this session is commit
`59d2b1077`.

## What the ROM shows

```
mov  r4, ip
adds r4, #0xa4          @ motion
ldr  r7, =0x203B400     @ gRandomTable, the last pre-loop set
str  r7, [sp]
...                     @ loop
lsls r0, r1, #1
ldr  r7, [sp]           @ first sample
adds r0, r0, r7
...
lsls r1, r1, #1
ldr  r0, [sp]           @ second sample
adds r1, r1, r0
...
movs r0, #0xff          @ cosine mask, inside the loop
...
movs r0, #0xff          @ sine mask, after the cosine's branch
```

In the knob compile, global allocation gives the table r7. Reload then spills
r7 for insn 143, the scratch zero of the cosine's `ldrsh`. The table goes to
r7's spill slot, sp+0. With an equivalence, reload would have reloaded the
constant (`ldr rN, =0x203B400`) at each use and deleted the set.

## What the draft gets right

1. **The twin's shape.** FlickerParticle_StartSecondaryBurst
   (`src/fn/sub_08235174.c`, matched) steps the motion pointer by
   `sizeof(struct FlickerParticle)`, keeps an `offset` BIV, and writes the
   flags and update stores through block-local base pointers. This draft does
   the same.
2. **Declaration order** `i, index, mask, sine, offset, motion, table`. That
   gives the pre-header order and sl, sb, r8 for the index pointer, the mask
   and the sine table.
3. **The sine's mask is a variable set twice.** It is `u32 byteMask = 255;`
   before the loop and `byteMask = 255;` again right before the sine lookup.
   With two plain `& 255`, loop.c's combine_movables folds the sine's 255
   (insn 183) into the cosine's (insn 132), summing savings and lifetime. The
   pair moves in pass 1 (26 x 2 x 2 >= 77 insns) and takes r7, and sine, mask
   and index each move up one high register. Alone, each scores
   26 x 1 x 1 < 77. The variable is no movable: its in-loop set follows the
   cosine's branch (maybe_never), and its first appearance is outside the loop
   (reg_in_basic_block_p fails). So the cosine's 255 is tested alone and
   stays, and both masks are `movs r0, #0xff`, as in the ROM. Flow deletes
   the dead set before the loop. On the cosine side instead (h12), the
   variable's set comes before the branch, so it moves (25 / 21).

## The remaining difference

cse1 puts `REG_EQUAL (symbol_ref gRandomTable)` on the table's pool load
(insn 27). The pseudo is set once, so update_equiv_regs makes that a
`REG_EQUIV`, and reload rematerializes the constant. The routes in the
ChargedBurst note fail here for the same reasons:

* The pre-loop code is straight-line. cse2 sees it as one extended block from
  entry to the loop label, so every copy or union member that carries the
  table gets the note. A copy hoisted out of the loop (the
  SceneActorChase_RandomDirectionExcept trick) lands in that block too. That
  trick works only when the source's set is in an earlier block.
* A second set kills the equivalence only if it survives to local-alloc. A
  dead set doesn't: flow deletes it, and the table is again set once. A live
  second set needs a second store to the slot, and the ROM has one.
* `const u16 *volatile table` (h4, h5) gives the right slot. But the pool
  load is a local pseudo in r0, not reload's r7, and the reads come before the
  index arithmetic.

Not tried (reasoning only). The second set could be an insn that generates no
code: a CLOBBER kept alive by a later use (reload deletes CLOBBERs), or a store
that reload_cse_regs drops as a no-op. A union initialized with `{}` emits a
lone CLOBBER, and every other C form tried sets the register for real. None of
these is plausible original code.

## Ruled out

Numbers are scope's ROM / draft differing instructions.

| Draft | Change | Differing |
|---|---|---|
| The unit's earlier draft | table first, `particles[i]` stores | 20 / 16 (shape 12) |
| g0 | the table read inside the helper, no offset BIV | 28 / 22 |
| g1 | g0 with a per-iteration motion pointer | 31 / 25 |
| g2 | per-iteration motion, `particles[i]` flags and update | 24 / 20 |
| h1 | h2 with the base pointers written into the expressions | 38 / 26 |
| h2 | offset BIV, block-local bases, the table last | 25 / 19 (shape 32) |
| h2, `AGBCC_NOMOVE=132` | knob: the 255 pair stays | 17 / 13 |
| h2, `AGBCC_NOEQUIV=29` | knob: no table equivalence | 16 / 16 |
| h2, both knobs | diagnostic | exact (build and `-g`) |
| h3 | the natural form, the globals read in the helper | 28 / 22 |
| h4 | `const u16 *volatile table` | 11 / 11 (shape 4) |
| h5 | the volatile read inside the helper | 15 / 15 (shape 4) |
| h6 | the twin's `polar` union for the cosine mask | 40 / 34 |
| h7 | in-loop locals, the table read inside a random macro | 33 / 32 |
| h8 | `byteMask` set once, before the loop, for the cosine | 17 / 13 |
| h8, `AGBCC_NOEQUIV=29` | knob: byteMask spans the loop, refilled into r7 | 2 / 2 |
| h9 | a do-while(0) random macro to raise the insn count (78) | 33 / 32 |
| h10 (landed) | h2 with the twice-set `byteMask` for the sine | 17 / 13 (shape 10) |
| h10, `AGBCC_NOEQUIV=29` | knob, diagnostic | exact (build and `-g`) |
| h11 | h10 with `byteMask` declared after `offset` | 17 / 13 |
| h12 | the twice-set `byteMask` on the cosine side | 25 / 21 |
| lb1, lb2, lb4 | the table as an integer address: `u32 table = RANDOM_TABLE_ADDR` (or `(u32)gRandomTable`), read as `*(const u16 *)(*index * 2 + table)` | 17 / 13, as h10 |
| lb3 | the integer address written in the helper, no table variable | 25 / 22 |
| un1 | h10 with `union RandomTableView table = {gRandomTable};` (PR 944) | 17 / 13, as h10 |
| un2 | un1 with the union opened inside the loop | 17 / 13, as h10 |

The drafts are in `build/scope/drafts/hr/` (worktree scratch, not committed).

Two levers that the Monsters depin work found for the random table were tried
after the note was first written. Reading the table as an integer address
(SceneActor_LeapTowardTarget) still leaves a pseudo set from a constant, and cse
puts the same `REG_EQUAL` on it (lb1 to lb4). Reusing a pointer that held a
non-constant value first (Dog_UpdateTurnSequence, fact 20) has nothing to reuse
here. Every value set before the loop (i, index, mask, sine, offset, motion) lives
into the loop, so none can share the table's pseudo.

The one-member union (PR 944, README fact 21) was tried too (un1, un2). It is a
lever for loop.c, not for update_equiv_regs. Its initializer emits a CLOBBER and
then the set, so in a loop the table has two sets and loop.c leaves the load in
the loop. Here the ROM loads the table before the loop, and once flow has run
the CLOBBER is dead (the set follows it) and is deleted. By local-alloc the
table is again set once from the pool, with `REG_EQUAL`.

## The table across the ROM

The same spilled table, with no equivalence, appears in loops in 08068500,
080BF0E4, 080C8774 and 080CCD7C. It also appears outside loops in 081EEECC
and 0807D2E0. 080C8774 and 080CCD7C have matched since (PR 936, PR 940). In
both, the shared chase step sets `randomSamples = gRandomTable` many blocks
before the turn, and the turn copies it into a `struct
SceneActorChaseRandomTable`. cse sees the copy in a later extended block, so it
gets no `REG_EQUAL` and no equivalence
(`notes/opus-golem-displacement-recovery_2026-11-01.md`). That works only
because a label separates the copy from the pool load. This function has no
such label before its loop. `scope.py frame FUNC` now finds the pattern: a ROM
slot that holds a pool constant, the draft pseudo whose `REG_EQUIV` keeps it out
of memory, and a score with `AGBCC_NOEQUIV` on that pseudo. It reports MATCH
here and for 082335F0.

## Scope gaps

* **Folded movables were invisible.** `hoist` showed the cosine's 255 with
  savings 2 and no hint that the sine's 255 was folded into it. The trace now
  prints a `match` line in combine_movables, and `hoist` marks the moved set
  `+ equal set insn 183 line 57` with the lever above (commit `59d2b1077`;
  `tools/scope/README.md` fact 21).
* **The spilled-constant pattern had no detector.** It has one now: `frame`
  matches a ROM `ldr rN, =CONST; str rN, [sp, #K]` that is read back only from
  `[sp, #K]`. It names the draft pseudos with that constant as `REG_EQUIV` and
  scores `AGBCC_NOEQUIV` on them (2026-11-01). When this note was first
  written, both knobs had to be found by hand: the pseudo number from the `-g`
  dump and the 255's insn from `hoist`.
