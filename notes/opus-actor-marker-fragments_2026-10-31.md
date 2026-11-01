# ActorMarker_LaunchFragments (08181514): holdout

Tags: #matching #actor-marker #compiler #reload #local-alloc #combine #scope-gap

**Not matched; holdout.** `ActorMarker_LaunchFragments` is 288 bytes. The
unit's guarded ordinary-C draft (`src/fn/sub_08181514.c`, unchanged) is the
best draft: 284 bytes, with 11 ROM / 9 draft instructions differing in scope
(shape 8). `tools/check.py --nonmatching` finds 20 differing instructions. No
variant tried this session beats it. The forked store
`build/matchsearch/opus_08181514` keeps the draft (node `4ec42525`) and four
probes. The earlier stores in `build/handoff/matchsearch/`
(`resume_08181514`, `resume_08181514_index`, `resume2_08181514_rng`) hold the
earlier rounds' drafts. Scratch variants are in `build/scope/drafts/am/` and
`build/scope/drafts/am2/` (worktree scratch, not committed).

## What the ROM shows

Two compiler knobs, now part of scope's compiler (commit `e53c8ca1f`), make
the unit draft match the ROM exactly:

```
scope.py src/fn/sub_08181514.c ActorMarker_LaunchFragments AGBCC_NOREG=65,76,87 AGBCC_COMBKEEP=75
MATCH
```

So the ROM is the draft's RTL with two differences:

1. **The three origin loads have no hard register.** The loads are `*posX`,
   `*posY` and `*posZ`: pseudos 65, 76 and 87, each `(set (reg:HI) (mem:HI
   (reg pos)))` with a REG_EQUIV note for that memory. Reload substitutes the
   memory at each add and loads it into a reload register right before the add:
   `mov r3, r8; ldrh r3, [r3]; adds r0, r1, r3`. The offset is operand 1 and
   the origin operand 2, the order of the RTL.
2. **The Y offset load (75) stays out of combine.** In the draft, combine folds
   the single-use Y offset into its add as operand 2 (`(plus (subreg (reg:HI
   76)) (subreg (mem offset+2)))`). That puts the origin first, the reverse of
   the ROM's `ldrh r0, [r4, #2]; ...; adds r0, r0, r7`.

The rest of the ROM follows from those two. `AGBCC_RTRACE=1` shows it in
`build/scope/ActorMarker_LaunchFragments/stderr.txt`:

* Each add with a substituted origin needs two LO reload registers, one for the
  address (r8, sb and sl are high registers) and one for the value. At the x
  and y adds only r3 is free. Reload's cost order makes `i` in r7 the cheapest
  live register: 9 weighted refs × 5 = 45, against 48 or more for the others.
  So `i` is kicked out at every add (`RT kick pseudo 25 from r7`, 10 times).
  It lives at sp+4, and r7 becomes the Y origin's reload register and the
  0x410 temporary.
* At the z add, the z sum (88) is kicked out of r0 and gets sp+8. Its
  output-reload store `str r0, [sp, #8]` survives. `emit_reload_insns` resets
  `new_spill_reg_store[r0]` for each reload using r0, and a later-numbered
  input reload of the same insn shares r0. `delete_output_reload` is then never
  tried (`RT insn 205 inherit r0 store=0`).
* The frame is 12 bytes: fast at sp+0 (no register), `i` at sp+4, the z sum
  at sp+8.

Knob subsets, as ROM / draft instructions differing:

| Knobs | Differing |
|---|---|
| `AGBCC_NOREG=76,87 AGBCC_COMBKEEP=75` | 2 / 2, only the x origin's register (r0, tied to its sum, against the ROM's r3) |
| `AGBCC_NOREG=65,76 AGBCC_COMBKEEP=75` | 6 / 4 |
| `AGBCC_NOREG=76 AGBCC_COMBKEEP=75` | 8 / 6 |
| `AGBCC_NOREG=65,76,87` | 15 / 15 |
| `AGBCC_COMBKEEP=75` | 52 / 50 |

The earlier rounds reached the same diagnosis with a private compiler that is
no longer available (`AGBCC_NOREG_ORIGINS=1`, `AGBCC_NOCOMBINE_HISI=1`,
`AGBCC_COMBINE_KEEP_REG=75`; `build/handoff/codex-evidence/scene-actors-3/scope/campaign/resume_08181514/REPORT.md`).

## Why ordinary C doesn't get there

Each route by which agbcc could leave the origins without a register was
checked against its source:

* **combine can't fold an origin into its add.** Substituting the load gives
  `(plus (reg x) (subreg (mem)))`. The commutative canonicalization in
  combine.c:3324 puts the operand that is not a register or memory first:
  `(subreg (mem))`. addsi3's operand 1 (`s_register_operand`) rejects that.
  Only the Y add folds, and in the wrong direction (point 2 above).
* **local-alloc always finds a register.** The origins are single-block,
  single-death pseudos (`REG_BASIC_BLOCK >= 0`, `REG_N_DEATHS == 1`), which is
  also what their REG_EQUIV memory note requires (local-alloc.c:835).
  local-alloc runs before global, so inside the loop only other local pseudos
  compete. Each origin gets r0, tied to its sum.
* **Global's override doesn't apply.** `find_reg` in global.c takes a hard
  register back from local pseudos only while no global allocno holds it, and
  only when their total refs over total live length is below the allocno's.
  The only allocno left without a register is fast (3 refs over 94 insns).
  r0, where the origins and their sums live, goes to `value` first.
* **Reload kicks `i`, not an origin.** An origin's load needs one LO register
  for the address; the add needs none. At the x and y loads r3 is free. At the
  z load (insn 189) no LO register is free, and reload's cost order puts `i` in
  r7 (45) ahead of the z origin's own r0 (48), so `i` is kicked. If `i` cost
  more than 48 (10 or more weighted refs), reload would kick the z origin
  instead, which is the ROM's z code. x and y would still differ.
* **The direct replacement in `update_equiv_regs` doesn't apply.** It needs
  `REG_N_REFS == 2` and a pseudo that is not local (local-alloc.c:868). The
  origins are local, and in the loop they have 4 weighted refs.

So the original's RTL must differ from this draft family before allocation:
in the pseudos the origins are read into, or in which values are local. No
spelling tried here produces that.

## Ruled out (this session)

The shapes are all variations of the unit draft. The numbers are scope's ROM
/ draft differing instructions (base 11 / 9):

* Types, temporaries and operand order of the three position adds (32
  variants, `am2/v_*`). x/y/z as u16, u32, s32 or s16; no sum temporary, or a
  u16/u32/s32 one; `offset + origin` or `origin + offset`. u16 without a sum,
  or with a u16 sum, in either order: no change. Every other variant is worse
  (12 / 10 to 55 / 53).
* Post-increment offset reads (`*offset++`, `am2/p1` to `p4`): 34 / 34 to
  54 / 56. Loop strength reduction rewrites the offsets.
* Direct reads of `gUnk_0824E818[...]` instead of an `offset` pointer
  (`am2/c1`): 51 / 70, 328 bytes. cse merges the repeated const reads, but
  combine still doesn't fold the origin (`(plus (subreg (reg:HI 61)) (subreg
  (reg:HI 62)))` stays), and loop adds givs.
* Loop counter spellings that could change `i`'s weighted refs (`am2/q1` to
  `q5`): u32 `i`, a `for` loop (36 / 35), `i < 9`, `++i <= 8`, `p->fragments
  + i`. None changes the refs; u32 is worse (12 / 10).
* Earlier variants in `am/`: origin locals (u16 or u32) loaded before the
  adds (a3, a4: 52 / 50), declaration and pointer order (a1: 28 / 26;
  a2, a5, a6: no change), the table and random pointers as locals or globals
  (b1 to b3: no change), no pointer locals (n1: 47 / 48).
* The earlier rounds' reports list their attempts (descriptor and RNG owner
  shapes, index forms, mask order). The pinned 97.9% frontier
  (`.../drafts/sub_08181514-resume-pinned-frontier.c`) uses register pins and
  is not allowed.

## Scope gaps

* **No-allocation and combine-keep what-ifs were missing.** Scope's compiler
  now has `AGBCC_NOREG`, `AGBCC_COMBKEEP` and `AGBCC_RTRACE` (commit
  `e53c8ca1f`, `tools/scope/README.md`). Without them this diagnosis needed a
  private compiler copy.
* **`why` and `spill` can't recognize a REG_EQUIV-substituted operand.** A ROM
  operand loaded by `mov rN, rHI; ldrh rN, [rN]` right before its only use,
  with no register of its own, is a pseudo without a hard register whose
  memory reload substituted. `why` reports it as a register choice. It could
  check the pseudo's REG_EQUIV note and suggest `AGBCC_NOREG=N`.
* **Output-reload stores that survive aren't explained.** A ROM-only `str rN,
  [sp, #K]` that is never read again comes from a kicked-out pseudo whose
  output reload `delete_output_reload` never tried. `spill` could flag it and
  name the shared reload register (the `RT ... inherit rN store=0` line).
