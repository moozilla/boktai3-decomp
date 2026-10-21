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

* SceneActorContacts_CanSelectNode (08072BC4): found by decomp-permuter; `u32 flags = node->flags; int mask = 2; mask = flags & mask; if (mask || ...)` (the mask test through a reused int temp) gives the pinned register roles.
* SceneInventory_SelectActionDestination (0806147C): found by decomp-permuter; `short destination = (u16)panel->destination; s8 four; ... (destination & 3) + (four = 4)` (constant assigned to a narrow temp inside the expression; a separate `s8 four = 4;` statement does NOT work).

## Not solved (what blocked)

See the end of the file (appended when the run ends).

## Later fully-clean fixes

* Player_CollectCallbackFlags, PlayerPanel_RemoveEffects, EffectPoolA_AllocateSlot: `(*(&i))++;` replaces `asm("" : "+r"(i))`; makes the induction variable opaque so no biv reversal / early increment (PlayerPanel: `(*(&group))++` after `offset += 0x208`).
* SceneStatusIndicator_Allocate: `u32 bit = one; (*(&bit)) <<= index;` (kept the pre-existing volatile pointer).
* SelectionSlots_ClearValue: `u32 m; do { m = 0xFF; } while (0);` plus `u32 cur = (u8)*slot; *slot = cur | m;`.
* DisplayAnimation_SetPosition (permuter): early `return -1`, no return on success path (r0 left as incoming value).
* ProximityEmitter_TickParticle (permuter): int temp reused for `previous + 1` and later the constant 2.
* SceneActorRegistry_FindAvailableAfterId (permuter): `(u8 *)actor - (-offset)` with `u32 offset = 0x2cc`.
* Player_UpdateResetBlink (permuter): second pointer temp `q = p` used inside the `mode != 0x1E` branch.

## Not solved (blockers)

* sub_08221B9C/BE8/C34/D40: combine moves target load after current load; barrier acted as a second use. D40: r5/r6 swap for values/cursor persists with every `(*(&cursor))` variant.
* sub_08066AE0: loop-invariant stride/dispatch hoisted into extra callee-saved regs.
* sub_08068B8C: needs hoist order [const 0xfc8, used ptr, const 1]; best variants off by 6 bytes.
* sub_0813B18C: the two `direction + 0xe0` get shared (tail merge).
* sub_08199C20: gcc folds branch addresses to `+0x44` then `subs #4`. 08180570: 9 -> 3 hints.
* sub_08056F30: original re-reads global pointer after stores (barrier forced it).
* sub_08080008: pool-folded address vs `group + 0x40`.
* sub_08229440: needs callee-saved r5 for a temp not crossing a call. sub_08074600: CSE deletes `mode = 0`. sub_0821F1C4: stride copy r2 from pin interplay.
* Untried/low odds: 08075FC8, 0821E5C4, 0802D570, 08069788, 08057480, 08058B88, 080322B4, 0804DB60, 08083F3C (permuter best 400 vs 1500, not zero), 0804EBD8, 0802C8CC, 081EC670, 08108E5C, 0801F580, 0805DD98, 08040A74, 0806FCAC, 0810BB0C.
* Permuter queue 2 was stopped after 1.5 functions (time budget).
