# Ratatosk_SpawnAnimatedParticle (081FC26C): exact match

Tags: #matching #ratatosk #compiler #combine #jump2 #flow #dead-code

**Exact match in ordinary C.** `Ratatosk_SpawnAnimatedParticle`, 940 bytes, now
compiles byte-identically (`tools/check.py src/fn/sub_081FC26C.c`: MATCH, 0
differing bytes). It has no register pins, barriers, inline assembly or compiler
overrides. Commit `fc3350f85` on `codex/opus-ratatosk-defeat` (merged to main
as PR 907); matchsearch node
`782c1d777313` in `build/matchsearch/opus_081FC26C` (forked store, label
`exact-guard`). The starting point was the guarded 99.6% draft from earlier
rounds (1 ROM / 1 draft instruction: a dead `adds r0, r3, #0` was missing). About 60
variants were compiled, kept in `build/scope/drafts/pt3/` (worktree scratch,
not committed).

## The deciding source property

The ROM keeps a dead copy of the kind right after the kind store:

```
cmp  r3, #3
bne  1f
movs r3, #1
1: strh r3, [r5, #0x14]
adds r0, r3, #0        @ dead: r0 is overwritten two instructions later
ldrh r2, [r5, #0x14]
movs r0, #0xf
```

It comes from a narrowed copy of the kind, tested by a guard whose second half
can never be true:

```c
kind = RandomSample() & 3;
if (kind == 3)
    kind = 1;
e->config.appearanceFlags = kind;
{
    u16 type = kind;
    // Reject a kind outside the 4-bit field (kind is 0, 1 or 2 here).
    if (type >= 3 && (type & ~15))
        return -1;
}
if ((e->config.appearanceFlags & 15) <= 1) {
```

The guard is deleted completely, so its constants and its body can't be
recovered from the ROM. All of these match: first test `type >= 3`, `type > 2`,
`type != 2`, `type != 1`, `type != 0`, or `kind >= 3` (j1; the first test only
has to stay a real conditional jump). Second test: any bit outside the kind's
two low bits (`& 4`, `& ~3`, `& ~15`, `& 0xF0`, `& 0x200`, `& 0x8000`, or `& 0x80`
on a `u8 type`). Body: `return -1;` or `e->sprite.flags |= 4;`. Nested ifs match
too. The chosen spelling reads as a range check.

What the shape needs: the copy's last use is a test that combine folds, and a
conditional jump that combine keeps lies between the copy and that test.

## Mechanism, pass by pass

Dumps: `scope.py` on the unit, then `build/scope/Ratatosk_SpawnAnimatedParticle/u.i.*`
(insn numbers below are from the landed source).

1. **cse keeps the copy.** `u16 type = kind` is a zero-extension (shift pair)
   that cse can't see through, so `type` stays its own pseudo. A plain `u32`
   copy is folded into `kind` by cse and disappears (variants c1, c3, f15).
2. **The guard ends the cse path.** Both `&&` jumps go to the end label, which
   then has two uses, so cse's path stops there. The next `ldrh` of the
   flags stays a real load, and the zero constant isn't carried into the
   `<= 1` branches. Without the guard, the single-store form
   `if (kind == 3) kind = 1; store` lets cse skip around the then-block
   (`AROUND`) and continue: 57 instructions differ (y1 to y5, m1 to m3). That
   is why the earlier draft needed the store duplicated in both arms of an
   if/else.
3. **combine folds the second test and leaves a USE.** The kind's nonzero bits
   are 3 (its sets are `& 3` and `1`). combine turns the zero-extension into an
   SImode copy (insn 286). Merging the second test into its jump, it folds
   `type & ~15` to 0, so that jump becomes unconditional (298) and the body is
   unreachable. The folded test was the copy's last use. `distribute_notes`
   walks back from it to place the death note, stops at the first test's jump
   (292; the walk only crosses plain insns and calls) and inserts
   `(use (reg type))` there with the note (1418). The first compare
   `type >= 3` (`cmp r0, #2; bls`) still reads the copy. combine can't
   substitute the copy into it, because the copy is used again later.
4. **Allocation.** The copy is a short global pseudo; global-alloc takes it
   first and gives it r0; the kind conflicts with r0 (the `& 3` constant uses
   it) and gets r3.
5. **flow2 deletes only the body's store.** After `reload_cse_regs`, toplev runs
   `find_basic_blocks` and `life_analysis` again (`toplev.c`, "Re-create the
   death notes"); the `.greg` dump is written after it. After reload,
   find_basic_blocks keeps unreachable blocks (`if (! reload_completed)
   delete_unreachable_blocks ();`), but it makes edges only for reachable
   ones. Nothing is live at the end of the unreachable body, so the final
   propagate pass deletes its `r0 = -1` (301) as a dead store. Its jump,
   barrier and label stay. The copy is live: the compare and the USE read it.
   The USE is its last use, so the death note goes there, not on the compare.
6. **jump2 deletes the rest but not the copy.** `delete_barrier_successors`
   removes everything between a barrier and the next label: the body's jump
   and barrier. The `b` that combine made now jumps to the following insn and
   is deleted. After reload `prev_active_insn` skips USE insns, so the `bls`
   jumps to the following insn too. `delete_jump` deletes it, and
   `delete_computation` deletes its cc0 setter `cmp r0, #2`.
   delete_computation goes back only to the setters of registers that die in
   the insns it deletes. The compare has no death note for r0, so
   `adds r0, r3, #0` stays. The USE emits nothing.

## Pass-model correction

My earlier analysis of this function ended the pass order at `reload_cse →
jump2`. The real order has a second flow pass in between:
**reload → reload_cse_regs → find_basic_blocks + life_analysis → jump2**.
After reload that pass keeps unreachable blocks but gives them no successors,
and it deletes every dead store, in reachable and unreachable blocks alike. A
dead instruction in the ROM was therefore live in that pass, and its last use
went away later without taking the setter along. jump2 removes uses that way:
code after a barrier, and the cc0 setter of a redundant conditional jump when
that setter carries no death note for the register. A `(use (reg))` insn keeps
a register live through the pass and emits nothing. A use removed earlier (by
combine, reload or reload_cse) leaves the instruction dead in that pass, and
the pass deletes it.

Single-test guards fail for this reason, in two ways (c5, d1, d2; `u16 y =
kind; if (y & 0x8000) ...`). combine makes the only jump unconditional, so no
kept jump separates the copy from the folded test:

* When the body reads the copy (c5 `return y;`, d1 `return y + kind;`), the
  copy's last use is in the unreachable body, so combine leaves no USE. The
  copy reaches greg (in r0) and flow2 deletes it as dead.
* When the body doesn't read it (d2 `return kind;`), `distribute_notes` walks
  back from the folded test to the copy itself and deletes it in combine.

## Ruled out (this session and the prior rounds)

* Earlier rounds (`build/handoff/matchsearch/sub_081FC26C/resume/STATE.md`):
  kind as u8/s16/s32 (same), s8 (worse), setters returning u16/u32, kind
  captures, full initializers, packed headers, separate appearance pointer,
  OR/switch dispatch, helpers with output kind, small struct/union setters,
  caller-local union, u16 RandomSample, AppearanceKind accessor.
* HImode copies so reload_cse would delete a store of the copy: a local
  union of a u16 is 4 bytes here (`arm_structure_size_boundary` is 32), so it is
  SImode and adds an RMW mask (m1, m2); loading it from the field makes cse
  delete the conditional store early, with no copy (m3).
* A u16 copy stored back conditionally (y4): combine makes an SImode copy, so
  reload_cse loses the HImode memory fact and the store survives.
* A redundant conditional store of the kind (y3): reload_cse deletes the
  store and jump2 its jump, but that leaves no copy, and the store's
  single-use label lets cse continue as in step 2 (57 instructions differ).
* First test on the copy, second on the kind (j2, `type >= 3 && (kind & ~15)`):
  the compare is then the copy's only use, so combine substitutes the copy
  into it and deletes it.
* Guards that don't fit the mechanism: an equality first test (`type == 2 &&
  ...`) lets cse fold the second test on the equal path (f7b); sign tests
  (`(s16)type < 0`) give different code and break the cross-jumped stores (f4,
  f9, f10); a body that changes `appearanceFlags` changes the first then-branch's
  allocation (f13, f14); inline helpers returning the condition (g2, g5) and
  inline `(u16)` casts without a local (g1) don't match.
* Simplifications on the matching source: dropping `s32 zero` matches and was
  taken; removing the static branches' `draw`/`frame` locals still fails (s2 to
  s4, 4 to 27 instructions).
* A 25-minute decomp-permuter run on the kind lines (`build/permute/sub_081FC26C/`)
  found only a live pointer copy (`new_var = e`) at the right position, with the
  wrong registers.

## Scope gaps

* scope's pass model and `born` column have no entry for the post-reload flow
  pass, and the dumps don't show which pass deleted an insn. A "deleted in"
  column (lreg → greg → jump2) would have shown the d1 copy reaching greg in r0
  and dying in flow2 at once.
* For a ROM-only `adds rX, rY, #0` that nothing reads, scope could suggest the
  mechanisms that survive flow2: a `(use)` that combine left after a kept jump
  (the guard pattern above), or reload inheritance. A `(use (reg))` insn in the
  `.greg` dump next to a ROM-only copy is the sign of the first.
