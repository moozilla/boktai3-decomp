# depin-manual worklog (2026-10-21)

Worker branch cloud/depin-manual. List: wip/depin_list.tsv (60 functions).
Step 1 (`tools/depin.py --write` over all 60) auto-dropped hints in 9 files (committed as "fewer hints (auto)").

## Fully clean (no pins, no asm)

* ScriptUtility_SetFlag800 (08056D24): three nested empty `do {} while (0)` around `flags = &gUnk_030053F4;` raise the pointer's loop-depth weight so it allocates first (r1); two levels were not enough, three were.
* WirelessStatusMenu_Destroy (08037110): the pin on the stored zero (r0) was compensating for a missing `return 0;`. The function returns int; ending with `gUnk_020000F8 = NULL; return 0;` puts the zero in r0 naturally.
* SceneActorRegistry_AdvanceCursor (0806FDE0): `+r` barrier on the constant `mask = 2` was only there to keep the `movs` before the byte load. Declaring the constant as `u8 mask = 2` (instead of `u32`) does it with plain C.
* AreaEffect_SetBounds (0804E180): pins on x/z only fixed operand order of `xRadius + x`. Storing `entry->xMax = xRadius + x;` directly (no `x = ...; entry->xMax = x;` re-assignment) gives the same registers/operand order.
* MotionPool_UpdateUntilDeadline (080459F8): pin on `result` (r0) replaced by `if (frame >= end) return 1; return 0;` (two returns, each branch loads its own constant; condition inverted to match bcs direction).

* PanelText_DrawDecimal (0810D60C) and PanelText_DrawDigits (0811D270): pin on `cell` (r6) was ranking `cell` below `value`. `do { value = value - digit * divisor; } while (0);` inside the loop raises `value`'s priority so it gets r5 and `cell` r6.
* StatModifier_AddForGlobalMode (0811E77C): `do { return input; } while (0);` on both early returns lifts `input` above `modifier` (input r3, modifier r4); same in StatModifier_ScaleForGlobalMode (0811E728). StatModifier_AddForOwnerMode (0811E7C8): `do { result = value; } while (0);` on both fallbacks.
* ContactBurst_OnContact (08047F68): `do { effect->contactTimer = 18; } while (0);` lifts `effect` above `other` (r4 vs r5).
* SceneActorTerrain_FindOpenNeighborDirection (080B713C): one `do { mask = 3; } while (0);` around the loop-invariant constant raises it above the actor pointer (mask r7, actor r8), replacing the `+r` barrier.
* SceneActorContacts_SetDistanceGate (08072E50): the `input*` copies of the parameters were the problem (they moved metric/threshold to r6/r7); use the parameters directly, then one `do { gate->enabled = result; } while (0);` makes gate outrank actor.
* ContactActor_SetDirectionalSequence (08042B84) / ContactSwarm_SetDirectionalSequence (08044174): pins on base/actor were only compensating for copies of the parameters; using `pool`/`slot` directly yields r6/r5 with no hints at all.
* DirectionalParticle_Update (08047DBC): the per-axis `movement`/`current` temporaries (pinned r0/r4) replaced by `position->x = (u16)position->x + (u16)motion->velocity.x;`. Operand order of the `+` decides which load is emitted first and thus the register.
* WirelessResultPanel_Create (0803B6B8): early return `p = GetInstance(); if (p) return p;` instead of `result`/`p` copy pair + use-barrier.

## Not solved (what blocked)

(see end of file)
