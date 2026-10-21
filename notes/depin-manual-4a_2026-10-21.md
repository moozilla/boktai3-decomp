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

## Rules learned (also see "New techniques")

* optabs.c expand_binop: for `x = A op B` with a register destination equal to the second operand B,
  the operands are swapped so the destination comes first (`adds x, x, A`). With a memory destination
  or a different register the source order is kept. So the order of `adds/ands/orrs rd, rn, rm`
  operands and which register the result lands in can be chosen by writing the result in place on the
  operand that should keep the register (`two |= v`).
* A sum that is advanced in place (`p += c`) is no longer equal to the earlier `p`, so CSE will not
  reuse it; a pinned `asm("" : "+r")` was only blocking that reuse.

## Not solved

(filled in at the end)
