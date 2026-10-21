# depin-manual-4a worklog (2026-10-21)

Worker branch cloud/depin-manual-4a. List: wip/depin_list4.tsv (37 functions; first 12 are near misses
from batch 3). `depin.py --write` removed nothing on any of the 37 files. Method after that: stripped
copies in wip/depin4/ (all hints removed), hand variants checked with `tools/check.py`, and
`tools/permute.py` (-j 1, 240 s each, run sequentially in the background; copies must be named
`sub_ADDR.c`, run with TMPDIR set to a writable dir, and `sizeof(struct X)` in the source breaks the
permuter's parser, so use the number there).

## Clean (no pins, no asm)

* Bg_UpdateScrollShadow (082158B0): the first use of the mask is written as a literal
  (`dst[0] = src[0].x & 0xff;`) before `mask = 0xff;`, so the halfword load comes before the constant.
* Anim_SetSequence (08220C8C): two changes. (1) `state->commands = (const u16 *)(base + prod);`
  directly (pointer + u32 with a memory destination) instead of `prod = (u32)base + prod`. (2) The
  command pointer reuses the `base` local: `base = (u8 *)((u32)state->commands + offset); command =
  (const volatile u16 *)base;` (found by the permuter, output of run score 20).
* ActorController_UpdateLayeredEnd (08183D9C): `u32 two = 2; two |= v; *f = two;`. See rule below.
* sub_081C24D0 (081C24D0): `slotAddr += c; *(u16 *)(slotAddr + 0x10) = 0;` (advance the sum in place
  instead of `slotAddr + c + 0x10`), so `p + offset` is not CSE'd and `offset` stays live in r4.
* SceneActorControl_StartPositionCurve (08083E98): `static inline void CpuSet_2(u32 control, void *dest,
  const void *src)` wrapper (arguments in another order) plus a named zero/one temporary.
* TemporaryAnimationPool_Initialize (08047C14): `pool->resource = *resource;` (struct assignment)
  replaced a hand-built pointer + asm.
* ActorGroup_ResetActivePhase (080FD29C): `~c` with `int c = 0x100` and a `u32 *g = &group->flags`
  temporary; both `z3`/`z4` zero temporaries are separate.
* StagedEffect_PlayShakeSequence (0804DB80) and StagedEffect_ShakeAndReveal (0804D980): (1) random read
  through an alias, `u32 *new_var = &index; *(&sample) = random[*new_var];`, with `mask = 31` assigned
  after the read; (2) the tail uses `effect->frame` directly (`if (effect->frame > 89) { u32 state = 7;
  effect->state = state; effect->frame = 0; } else effect->frame++;`) instead of a `nextFrame` local, so
  CSE keeps one load in r0 and the constant in r1.
* StageIconDisplay_TickReveal (08055C04): `do { if (!display->stateFrame) Sound_PlaySong(706); } while (0);`
  around the first test raises `display`'s loop-depth weight so it gets r4 and the `revealed` pointer r5.

## Rules learned (also see "New techniques")

* optabs.c expand_binop: for `x = A op B` with a register destination equal to the second operand B,
  the operands are swapped so the destination comes first (`adds x, x, A`). With a memory destination
  or a different register the source order is kept. So the order of `adds/ands/orrs rd, rn, rm`
  operands and which register the result lands in can be chosen by writing the result in place on the
  operand that should keep the register (`two |= v`).
* A sum that is advanced in place (`p += c`) is no longer equal to the earlier `p`, so CSE will not
  reuse it; a pinned `asm("" : "+r")` was only blocking that reuse.

## Not solved (best state kept in wip/depin4/, not committed)

Near misses and what is left:
* 0805E1D0 SceneInput_TickChoice: 2 bytes. Target has `adds r0, r2, #0; ands r0, r1` (the held constant
  1 copied, then ANDed with the key word). CSE (`fold_rtx`) swaps a known-constant first operand to the
  second position, so every spelling (`confirm & keys`, declaration order, `*(&confirm)`) gives
  `adds r0, r1, #0; ands r0, r2`.
* 08047D24 / 08233140: 2-3 bytes. The reload register for the high-register copy stored just before
  `movs r0,#1; strb` is r0 in ours, r1 in the original (reload rotation); the sibling pair shares it.
* 080645D4: the original tests `bits & 1` with a fresh `movs r0,#1` although r5 already holds 1;
  CSE reuses r5 in every spelling tried (`u8 one`, `*(&one)`, `one = source != 0`).
* 08061CFC: 2 bytes (page-offset constant picks r0, original r2). Permuter version (`new_var = 0x100;
  new_var = keys & new_var;`) is in wip/depin4/sub_08061CFC.p.c.
* 0812D8EC / 08126000: loop-invariant `0x69C + b` is re-materialised by loop.c in the original (fresh
  `ldr rX,=0x69c`), ours reuses the first constant register. Removing the pre-loop `fl`/`tbl` locals and
  using `b->mask` / the table in the loop body got the closest (wip/depin4/sub_0812D8EC.f.c, 16 bytes).
* 081F02EC: 3 bytes; the middle branch's reload constant gets r2, original r1. The `off`/`q` locals can
  be dropped (`duration = (u16)actor->baseDuration;` is the same field).
* 081680F4: `hud`/`i` high registers swapped (r8/r9) and the tail pointer/zero registers; `source` reuse
  from the permuter did not give an exact match.
* 081A681C: 28 bytes, all in the last branch (`index = v * 3` must go to a different register than
  `bank` so the compiler emits `adds r1, r0, #0; adds r1, #9`).
* 082356E4: n/a (u16 recompute of the counter; the asm blocked CSE of `(u16)n`).
* 08036910: the original re-copies the call result (`adds r0, r5, #0`) before `Memory_Clear`; CSE
  deletes it in every spelling.
* 081A0B20: needs a real second copy of the actor pointer (`adds r7, r4, #0`); no asm-free way found.
* Not meaningfully attempted: 0805228C, 080521DC, 08035FF4, 0804694C, 0806C708, 081F05D0, 080829C0,
  08091E58, 08072E98, 0806122C, 08236F48 (the `u32 ta, tb asm("r1");` there is a hidden pin that
  `depin.py` does not strip).

## New techniques (not in docs/REGALLOC.md)

* Remove a local that copies a struct field when the field is read twice: using `effect->frame`
  directly lets CSE keep a single load, and the register comes from the load, not a pinned copy.
* `do { } while (0)` around a use of a pointer parameter is enough to outrank a derived pointer in the
  global allocator (StageIconDisplay_TickReveal); no `asm` and no `register`.
* The alias form `u32 *new_var = &index; *(&sample) = random[*new_var];` also requires assigning the
  mask constant after the table read, otherwise the constant is loaded early.
