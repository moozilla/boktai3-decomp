# depin-manual-3 worklog (2026-10-21)

Worker branch cloud/depin-manual-3. List: wip/depin_list3.tsv (50 functions).
Method: `depin.py --write` over all 50 files, then stripped copies in wip/depin3/ run through `tools/permute.py` (100 s, -j 2), plus hand variants checked with `tools/check.py`. Permuter output is ported by hand and re-checked (score 0 ignores branch offsets, and several score-0 results rely on dummy statements such as `actor++; actor--;` or `s->q = s->q;`, which I did not commit).

## Clean (no pins, no asm)

* Actor_InitLaunchVelocity (080DDD28), SceneActor_ConfigureSecondaryProfileEntry (080DE92C): depin.py alone.
* Nidhoggr_ChangeCompletedPartState (08190C98): `asm("" ::: "r1")` was only stopping 0x71c2 from being derived from 0x7210; two early returns (`if (!*ready) return 0; ... if (*current == state) return 0;`) instead of nested ifs.
* ScreenTransition_FadeBlendOut (081D71CC): the zero store went through an asm operand; now `do { ++p->state; } while (0);` followed by a plain `gUnk_030051D4 = 0;` (the loop wrapper makes the constant materialise after the halfword store).
* ResultMenu_CreateGraphics (0801BCA8): CpuSet through `static inline void CpuSet_2(u32 control, void *dest, const void *src)` (control first); plain locals for src/dst and for the two byte pointers (r8/sb pins gone).
* Vanargand_WaitForActionRelease (081ACAC4): `(*(&p))->pending = 0;` and an extra pointer temp `new_var = &p->delayF20; field = new_var;` replace the r1 pin on the delay value.
* SceneActorBehavior_RequestDescriptorTurn (0807864C): reuse `targetQuadrant` for the shifted value (`targetQuadrant = shifted >> 24`), no separate `direction` variable; three pins/asm gone.
* Axe_TickAuxiliarySpriteTimers (080FBEDC): NEW. The `asm volatile("" : "+r"(one))` only stopped loop.c from hoisting the constant 1 out of the loop. Setting the constant twice (`u32 one = 1;` before the loop, `one = 1;` again inside) makes it non-movable (the pseudo has two sets), so `movs r1,#1` stays in the loop and the outer set is dead.
* Wireless_TryStartParentNiSend (08035F5C): `int check = 1;` (literal, not a copy of `one`), `do { check &= flags; } while (0);`, `1 << *slot`; the volatile pointer cast in the do-while condition also became unnecessary.

* Player_SetAnimation (0813AE18): permuter. Mix `&actor->animation` expressions with the `anim` local (anim only kept for the `flags |= 4` branch) and re-read `actor->animationData` for the final Anim_Step; replaces the r5 pin on data.
* ChildGroup_WaitForInactiveMembers (080F34C8): permuter. Plain `u16 one = 1`, first store through `*q` wrapped in `do { *q = one; } while (0);`.
* SceneItemValue_CalculateEquipment (0805F7E0): permuter. Dropped the `base` local and reused `result` for the definition value and the Div result (no r3 pin / asm). The pre-existing `volatile u8` read of `equipment->kind` stays (needed for the double ldrb).
* DarkLoan_CreateHiddenDisplay (081BD854): permuter. `int new_var = 0xB; Task_Create(new_var, ...)` plus the rate-field block in `do { } while (0)`; replaces the r5 pin on panel.
* StagedEffect_BlendAndShake (0804DAC8): permuter. Second random read as `new_var = &index; *(&sample) = random[*new_var];` (address-taken temps are opaque to CSE); replaces the r0/r1 pins.

## Fewer hints

* Axe_CopyAuxiliaryVector (080FC894): 2 -> 1 (depin.py).
* StatusPanel_DrawDecimalValue (08169A9C): 3 -> 2 (depin.py).

## Not solved (blockers)

Near misses (instruction diff counts are for the fully unpinned code):
* Axe_CopyAuxiliaryVector (6 diffs): the q value must land in r2 and the 0xa54 constant in r0. Permuter only found `s->q = s->q;` (dummy), not committed.
* Mummy_WaitReplaceResponse: permuter score 0 only with `actor++; actor--;` (dummy), not committed. `group = actor + 0x3D4` is folded into the load offset unless pinned.
* SpriteEffectController_UpdatePool: score 40 with a dummy `if (slot) return 0; else return 0;` that keeps slot live to the end.
* Wireless_TryStartParentNiSend needed an empty-ish `do { } while (0)` before the wrapped form; solved with the wrapped statement.
* SingletonSlot7_OnTargetContact / BeginScaledCooldown (2 / 3 diffs): the middle (`level <= 5`) branch allocates its 0x5F2 offset constant to r4 / r2 instead of r1; every source form of the three branches tried (about 100 combinations) gives the same.
* SceneInput_TickChoice (2 diffs): original copies `confirm` (r2) into r0 and ands with keys; unpinned code copies keys and ands with r2 (commutative operand order). 300 s of permuter found nothing.
* TemporaryAnimationPool_CreateEntry (2): `lifetime` must go through r1 (`mov r1, sb`), not r0.
* ActorAttachment_NotifySourceState (2): bits in r1 vs r0 (best permuter score 10, `s32 k2 = 1; one = k2;` helps but is not enough).
* Anim_SetSequence (2): `base = state->commands;` reuse fixes the first, `adds r1,r1,r0` vs `adds r0,r1,r0` for the command address remains.
* sub_08126000 / ParticleEffectPool_Update (1 diff each): `(*(&off))` on the first 0x69C use removes 9 of 10 diffs; the second use must be `ldr r0,=0x69C` again but CSE reuses the register (`adds r0,r2,#0`).
* ScenePool_SelectRepeatOrExit: the two identical tails are merged (cross-jumped) without the asm; wrapper in one branch and plain call in the other gives 2 diffs (argument order).
* StagedEffect_ShakeAndReveal / PlayShakeSequence: the random-read part is fixed with the BlendAndShake technique; the remaining pin is `state` (needs r1 while nextFrame keeps r0).
* SceneObject_UpdateRaising: barriers keep two RMWs separate; no plain form.
* SceneEffect_InitParticleRing: `z = 0` set in the loop must not be hoisted but must stay in r0 after the loop; a dead pre-loop set did not help here.
* Untouched beyond the first permuter pass (scores > 200): the remaining 5+ hint functions (SceneActorRegistry_SetMovementModesFromScript, SceneInventory_BuildConsumableCatalog, SceneActorControl_ConsumePendingEntry, ActorAttachment_ReleasePartsResource, SceneInventory_ClassifyConsumableMove / EquipmentMove, SpriteEffectController_QueueColliders, SceneCommands_FadePalette, ResultGame_UpdatePlayerMarker, StageIconDisplay_TickShake, ScenePool_SelectDirectionOrRepeat, SceneActorControl_TickJitter, SceneActorPlacement_ConsumeCallbackFlag, sub_081C4708, RecordList_InitializeSprites, Nidhoggr_UpdateDistanceLimitedEffect (constant 0x180 derived from 0x182 via `subs r3,#2` unless r3 is clobbered), StatusPanel_DrawDecimalValue (mask must be a copy of the const in r1)).

## New techniques

* Non-movable constant: `u32 one = 1;` before a loop and `one = 1;` again inside makes loop.c leave the in-loop constant (two sets) instead of hoisting it (Axe_TickAuxiliarySpriteTimers). Replaces `asm volatile("" : "+r"(c))`.
* Reusing a local for a second purpose (`result` for base and Div result; `targetQuadrant` for the shifted value; `base` for state->commands) changes register choice like a pin does.
* Re-deriving an address (`&actor->animation`) in some uses while a local copy exists for others (Player_SetAnimation).
* `int new_var = 0xB;` passed as a call argument instead of a literal (DarkLoan_CreateHiddenDisplay); `new_var = &index; *(&sample) = random[*new_var];` for the RNG idiom (StagedEffect_*).
* `do { stmt; } while (0);` around one store or the read-modify-write (ChildGroup, Wireless_TryStartParentNiSend, ScreenTransition_FadeBlendOut).
* Static inline CpuSet wrapper with control first (`CpuSet_2(control, dest, src)`).
* Permuter wins that are only dummies (`actor++; actor--;`, `s->q = s->q;`, `if (x) {A} else {A}`) were not committed.
