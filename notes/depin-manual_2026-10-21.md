# depin-manual worklog (2026-10-21)

Worker branch cloud/depin-manual. List: wip/depin_list.tsv (60 functions).
Step 1 (`tools/depin.py --write` over all 60) auto-dropped hints in 9 files (committed as "fewer hints (auto)").

## Fully clean (no pins, no asm)

* ScriptUtility_SetFlag800 (08056D24): three nested empty `do {} while (0)` around `flags = &gUnk_030053F4;` raise the pointer's loop-depth weight so it allocates first (r1); two levels were not enough, three were.
* WirelessStatusMenu_Destroy (08037110): the pin on the stored zero (r0) was compensating for a missing `return 0;`. The function returns int; ending with `gUnk_020000F8 = NULL; return 0;` puts the zero in r0 naturally.
* SceneActorRegistry_AdvanceCursor (0806FDE0): `+r` barrier on the constant `mask = 2` was only there to keep the `movs` before the byte load. Declaring the constant as `u8 mask = 2` (instead of `u32`) does it with plain C.
* AreaEffect_SetBounds (0804E180): pins on x/z only fixed operand order of `xRadius + x`. Storing `entry->xMax = xRadius + x;` directly (no `x = ...; entry->xMax = x;` re-assignment) gives the same registers/operand order.
* MotionPool_UpdateUntilDeadline (080459F8): pin on `result` (r0) replaced by `if (frame >= end) return 1; return 0;` (two returns, each branch loads its own constant; condition inverted to match bcs direction).

## Not solved (what blocked)

See bottom section; filled in as work proceeds.
