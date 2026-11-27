# Plain flag and local-constant probes

Tags: #compiler #phase3 #tooling

Each row represents one linter binding or constant argument. A local probe uses the `fold_literals.py --every-read` edit for that binding. The CSV keeps the linter weight, including multiple uses charged to the same local. The sample uses a fixed seed and takes up to 40 sites per directory before the rest of the sweep.

PLAIN is a byte-identical scratch rewrite. BOUND-SINGLE is a mismatch with one use found in the containing source function. BOUND-SHARED is a mismatch with other uses listed in the CSV. SKIP records an unsupported form, inactive code or check failure. A mismatch identifies a dependency in the current reconstruction. The other-use scan looks for reads of the same binding and equal literals or named constants in the containing function. It cannot see every value produced by inlining or optimization. Confirm mask order and alternative matching C before inferring an original accessor or macro.

The ROM-side scan follows recognized register provenance and omits cases it cannot attribute. Its branch-consumed tests are reported alongside both raw AND and boolean source forms. AND and BIC stores share a ROM denominator for clear and keep-mask forms. The compiler probes also cover non-flag locals. A local used on several fields appears once in each applicable field row.

The ROM census found 1,160 classifiable operations. Among BOUND-SINGLE sites with one ROM observation on the same function, field, operation and value, 208 are mask-first, 16 load-first, and 0 interleaved. The CSV includes their ROM addresses and retains multiple observations when the correspondence is ambiguous.

To reproduce the sweep, run `build/venv/bin/python tools/plainprobe.py`. Use `--resume` to continue from the CSV. Run `build/venv/bin/python tools/plainprobe_rom.py` to refresh the typed ROM census, then run plainprobe with `--resume` to import it.

## Sample

| Scope | PLAIN | BOUND-SINGLE | BOUND-SHARED | SKIP | Pending |
| --- | --- | --- | --- | --- | --- |
| All sites | 33 | 228 | 205 | 42 | 0 |

33 of 466 compiler-tested sites matched (7.1%).

## Overall

| Scope | PLAIN | BOUND-SINGLE | BOUND-SHARED | SKIP | Pending |
| --- | --- | --- | --- | --- | --- |
| All sites | 312 | 2665 | 2160 | 234 | 0 |

312 of 5,137 compiler-tested sites matched (6.1%).

## Directory sample

| Directory | PLAIN | BOUND-SINGLE | BOUND-SHARED | SKIP | Pending |
| --- | --- | --- | --- | --- | --- |
| include | 7 | 22 | 10 | 1 | 0 |
| src/bosses | 4 | 15 | 20 | 1 | 0 |
| src/effects | 2 | 12 | 22 | 4 | 0 |
| src/engine | 0 | 11 | 16 | 7 | 0 |
| src/lib | 0 | 0 | 0 | 1 | 0 |
| src/link | 4 | 16 | 13 | 7 | 0 |
| src/minigames | 1 | 15 | 23 | 1 | 0 |
| src/monsters | 1 | 24 | 13 | 2 | 0 |
| src/npc | 0 | 25 | 14 | 1 | 0 |
| src/objects | 3 | 15 | 20 | 2 | 0 |
| src/player | 4 | 20 | 9 | 7 | 0 |
| src/scene | 2 | 26 | 11 | 1 | 0 |
| src/script | 0 | 3 | 2 | 0 | 0 |
| src/system | 4 | 12 | 7 | 5 | 0 |
| src/ui | 1 | 12 | 25 | 2 | 0 |

## Warning kind

| Warning kind | PLAIN | BOUND-SINGLE | BOUND-SHARED | SKIP | Pending |
| --- | --- | --- | --- | --- | --- |
| const-local | 24 | 60 | 376 | 168 | 0 |
| literal-local | 30 | 1121 | 810 | 60 | 0 |
| literal-param | 258 | 1484 | 974 | 6 | 0 |

## Directory

| Directory | PLAIN | BOUND-SINGLE | BOUND-SHARED | SKIP | Pending |
| --- | --- | --- | --- | --- | --- |
| include | 28 | 55 | 55 | 5 | 0 |
| src/bosses | 62 | 409 | 412 | 26 | 0 |
| src/effects | 46 | 107 | 164 | 21 | 0 |
| src/engine | 0 | 11 | 16 | 7 | 0 |
| src/lib | 0 | 0 | 0 | 1 | 0 |
| src/link | 12 | 88 | 72 | 49 | 0 |
| src/minigames | 2 | 31 | 41 | 2 | 0 |
| src/monsters | 79 | 1068 | 642 | 20 | 0 |
| src/npc | 8 | 103 | 78 | 4 | 0 |
| src/objects | 13 | 116 | 127 | 10 | 0 |
| src/player | 16 | 108 | 59 | 35 | 0 |
| src/scene | 27 | 447 | 287 | 21 | 0 |
| src/script | 0 | 3 | 2 | 0 | 0 |
| src/system | 4 | 12 | 7 | 5 | 0 |
| src/ui | 15 | 107 | 198 | 28 | 0 |

## Helper

| Helper | PLAIN | BOUND-SINGLE | BOUND-SHARED | SKIP | Pending |
| --- | --- | --- | --- | --- | --- |
| ARMOR_QUEUE_PART_ACTION | 0 | 0 | 0 | 2 | 0 |
| Actor16_UpdateMovementSlowdown | 0 | 1 | 0 | 0 | 0 |
| ActorControllerMotion_ClearFlags | 0 | 0 | 1 | 0 | 0 |
| ActorController_ClearColliderFlags | 0 | 0 | 8 | 0 | 0 |
| ActorController_ClearMotionFlags | 0 | 0 | 8 | 0 | 0 |
| ActorController_SetColliderFlags | 4 | 0 | 0 | 0 | 0 |
| ActorFlag | 0 | 1 | 0 | 0 | 0 |
| ActorHalfBits | 0 | 1 | 0 | 0 | 0 |
| ActorStats_SetDamageFlags | 0 | 1 | 2 | 0 | 0 |
| ActorStats_TestDamageFlags | 0 | 0 | 1 | 0 | 0 |
| ActorStatus_BehaviorFlags | 2 | 7 | 8 | 0 | 0 |
| ActorStatus_BodyFlags | 0 | 5 | 2 | 0 | 0 |
| ActorStatus_ClearMotionFlags | 0 | 7 | 8 | 0 | 0 |
| ActorStatus_ClearPlacementFlags | 0 | 0 | 1 | 0 | 0 |
| ActorStatus_PalettePlayerFlags | 0 | 1 | 0 | 0 | 0 |
| ActorStatus_SetDrawableFlags | 1 | 0 | 20 | 0 | 0 |
| ActorStatus_SetPendingBits | 4 | 2 | 0 | 0 | 0 |
| ActorStatus_SetPlacementFlags | 0 | 0 | 1 | 0 | 0 |
| ActorStatus_SetWord | 4 | 1 | 2 | 0 | 0 |
| ActorStatus_StatusFlags | 0 | 4 | 0 | 0 | 0 |
| AnyMotion | 0 | 0 | 1 | 0 | 0 |
| Arg1 | 0 | 0 | 1 | 0 | 0 |
| Armor_ContactFlags | 0 | 1 | 0 | 0 | 0 |
| AttackFlags | 0 | 1 | 0 | 0 | 0 |
| Axe_ReadBindingFlags | 0 | 0 | 1 | 0 | 0 |
| Axe_SetEffectFlags | 0 | 1 | 1 | 0 | 0 |
| Bat_ReadDefeatMotionFlags | 0 | 0 | 1 | 0 | 0 |
| Bat_TestProbeFlags | 0 | 0 | 1 | 0 | 0 |
| Bee_ClearRouteFlags | 0 | 0 | 1 | 0 | 0 |
| Bee_HasRouteFlag | 0 | 2 | 1 | 0 | 0 |
| Bee_SetRouteFlags | 0 | 2 | 0 | 0 | 0 |
| Bg_DisableDisplay | 0 | 5 | 7 | 0 | 0 |
| Bg_EnableDisplay | 1 | 9 | 18 | 0 | 0 |
| CameraTracking_ClearRequests | 4 | 0 | 8 | 0 | 0 |
| CameraTracking_SetRequests | 0 | 0 | 3 | 0 | 0 |
| CentipedeSpread_Test | 0 | 6 | 2 | 0 | 0 |
| Chase_Bits32 | 1 | 2 | 0 | 0 | 0 |
| Chase_Bits32_1 | 0 | 1 | 0 | 0 | 0 |
| Chase_ClearBits | 0 | 11 | 6 | 0 | 0 |
| Chase_MotionFlags | 4 | 0 | 0 | 0 | 0 |
| Chase_RandomBits | 1 | 0 | 0 | 0 | 0 |
| Chase_Test | 2 | 3 | 0 | 0 | 0 |
| ClearActivity | 0 | 1 | 0 | 0 | 0 |
| ClearActorFlag | 0 | 0 | 1 | 0 | 0 |
| ClearAnimationFlags | 1 | 0 | 0 | 0 | 0 |
| ClearColliderFlag | 0 | 0 | 1 | 0 | 0 |
| ClearContext | 0 | 1 | 0 | 0 | 0 |
| ClearContext_SelectRecoveryRoute | 0 | 1 | 0 | 0 | 0 |
| ClearDamageFlag | 1 | 1 | 0 | 0 | 0 |
| ClearDrawFlags | 0 | 1 | 0 | 0 | 0 |
| ClearFlag | 2 | 3 | 0 | 0 | 0 |
| ClearFlags | 3 | 9 | 2 | 0 | 0 |
| ClearHalf | 0 | 5 | 0 | 0 | 0 |
| ClearHalfFlags | 0 | 2 | 0 | 0 | 0 |
| ClearHidden | 0 | 1 | 0 | 0 | 0 |
| ClearHit | 0 | 2 | 0 | 0 | 0 |
| ClearHitFlags | 0 | 1 | 0 | 0 | 0 |
| ClearMaskedValue | 1 | 0 | 0 | 0 | 0 |
| ClearMotionFlags | 0 | 1 | 0 | 0 | 0 |
| ClearReaction | 0 | 1 | 0 | 0 | 0 |
| ClearRotationFlag | 0 | 1 | 0 | 0 | 0 |
| ClearTargetFlags | 0 | 1 | 0 | 0 | 0 |
| ClearWord | 0 | 1 | 0 | 0 | 0 |
| ClearWordValue | 0 | 0 | 1 | 0 | 0 |
| Clone_MaskColliderFlags | 0 | 0 | 2 | 0 | 0 |
| CoffinBike_ActorFlags | 2 | 0 | 0 | 0 | 0 |
| CoffinBike_ClearDamageFlag | 0 | 1 | 0 | 0 | 0 |
| CoffinBike_ReadMode | 0 | 0 | 1 | 0 | 0 |
| CommandFlip | 1 | 0 | 0 | 0 | 0 |
| Command_SetMotionFlag | 0 | 0 | 1 | 0 | 0 |
| ContactActor_ReadMotionFlags | 0 | 1 | 0 | 0 | 0 |
| ContactCollider_ClearFlags | 0 | 1 | 24 | 0 | 0 |
| ContactCollider_ClearReactionFlags | 5 | 0 | 5 | 0 | 0 |
| ContactCollider_SetFlags | 12 | 1 | 3 | 0 | 0 |
| ContactCollider_SetReactionFlags | 14 | 1 | 8 | 0 | 0 |
| ContactCollider_TestContactFlags | 0 | 3 | 0 | 0 | 0 |
| ContactCollider_TestFlags | 0 | 6 | 0 | 0 | 0 |
| ContactCollider_TestReactionFlags | 9 | 9 | 9 | 0 | 0 |
| ContextFlag | 0 | 1 | 0 | 0 | 0 |
| ContextFlags | 0 | 2 | 1 | 0 | 0 |
| ContextHasFlag | 0 | 1 | 0 | 0 | 0 |
| Crow_ClearFlags | 0 | 0 | 1 | 0 | 0 |
| Crow_SetFlags | 0 | 0 | 1 | 0 | 0 |
| Crow_TestAnimationFlags | 0 | 1 | 0 | 0 | 0 |
| Dog_ClearAttackFlags | 1 | 0 | 0 | 0 | 0 |
| Dog_HasAwarenessFlag | 0 | 0 | 4 | 0 | 0 |
| Dog_ScaleMagnitude | 0 | 0 | 2 | 0 | 0 |
| Dog_SetContextFlags | 0 | 0 | 1 | 0 | 0 |
| Dog_TestApproachFlag | 0 | 0 | 1 | 0 | 0 |
| EnableFlags | 0 | 3 | 0 | 0 | 0 |
| Engine_ClearPauseFlags | 0 | 2 | 4 | 0 | 0 |
| Engine_SetPauseFlags | 0 | 7 | 4 | 0 | 0 |
| Entry_TestFlags | 0 | 0 | 5 | 0 | 0 |
| FeatherBlockedSlots | 0 | 1 | 0 | 0 | 0 |
| Flag | 1 | 3 | 4 | 0 | 0 |
| FlagsSave | 0 | 1 | 1 | 0 | 0 |
| Flags_MaskedWord | 1 | 13 | 11 | 0 | 0 |
| GameState_SetWorldFlags | 0 | 2 | 0 | 0 | 0 |
| GameState_TestWorldFlags | 0 | 19 | 6 | 0 | 0 |
| GetFlags | 0 | 0 | 1 | 0 | 0 |
| GetReactionFlags | 0 | 2 | 0 | 0 | 0 |
| Ghost_ClearFlags | 0 | 0 | 3 | 0 | 0 |
| Ghost_ClearMotionFlags | 1 | 0 | 0 | 0 | 0 |
| Ghost_FirstLoopRandom | 2 | 0 | 0 | 0 | 0 |
| Ghost_SetFlags | 0 | 3 | 5 | 0 | 0 |
| Ghost_TestFlags | 0 | 5 | 4 | 0 | 0 |
| Ghost_TestRouteFlags | 1 | 0 | 2 | 0 | 0 |
| Ghoul_ClearDiscFlags | 0 | 0 | 3 | 0 | 0 |
| Ghoul_SetDiscFlags | 0 | 2 | 0 | 0 | 0 |
| Ghoul_TestDiscFlags | 0 | 0 | 1 | 0 | 0 |
| Ghoul_TestPendingRoute | 0 | 0 | 1 | 0 | 0 |
| GolemReaction_SetBodyFlags | 0 | 1 | 0 | 0 | 0 |
| GolemSlide_ClearColliderFlags | 0 | 0 | 1 | 0 | 0 |
| Golem_MarkContactPlacement | 0 | 0 | 1 | 0 | 0 |
| Golem_ScaleMagnitude | 0 | 0 | 2 | 0 | 0 |
| Golem_SetMotionFlags | 0 | 7 | 0 | 0 | 0 |
| Golem_SetRouteFlags | 0 | 1 | 0 | 0 | 0 |
| GridOption | 4 | 0 | 0 | 0 | 0 |
| HalfwordFlags_And | 0 | 6 | 2 | 0 | 0 |
| HalfwordFlags_Clear | 0 | 3 | 2 | 0 | 0 |
| HalfwordFlags_Set | 0 | 7 | 2 | 0 | 0 |
| HalfwordFlags_Test | 0 | 2 | 0 | 0 | 0 |
| HasBit | 0 | 1 | 1 | 0 | 0 |
| HasFlag | 0 | 1 | 4 | 0 | 0 |
| HasFlag_BeginNearbyPlayerContact | 0 | 0 | 1 | 0 | 0 |
| HasFlag_BeginPlayerPursuit | 0 | 0 | 1 | 0 | 0 |
| HasFlag_SelectPlayerApproach | 0 | 0 | 1 | 0 | 0 |
| HasMotionFlag | 0 | 1 | 1 | 0 | 0 |
| HasPlayerEvent | 0 | 0 | 1 | 0 | 0 |
| HasStatusFlag | 0 | 1 | 0 | 0 | 0 |
| HasTargetFlag | 0 | 0 | 1 | 0 | 0 |
| HitSpark_NextIndex | 7 | 0 | 0 | 0 | 0 |
| Hresvelgr_SetFlags | 0 | 28 | 2 | 0 | 0 |
| Hresvelgr_TestFlags | 2 | 2 | 5 | 0 | 0 |
| IvyInit_ClearMotionFlags | 0 | 1 | 0 | 0 | 0 |
| Ivy_ClearHalf | 1 | 0 | 0 | 0 | 0 |
| Ivy_ClearStart | 0 | 0 | 1 | 0 | 0 |
| Ivy_ClearWord | 1 | 0 | 0 | 0 | 0 |
| Ivy_SetFlags | 0 | 0 | 1 | 0 | 0 |
| LinkAura_SetFlags | 0 | 1 | 3 | 0 | 0 |
| LinkReady_HasMode | 2 | 0 | 0 | 0 | 0 |
| LinkSelection_IsChoice | 0 | 1 | 0 | 0 | 0 |
| LinkSelection_IsReady | 0 | 3 | 0 | 0 | 0 |
| LinkSelection_IsSelected | 1 | 0 | 0 | 0 | 0 |
| LinkSelection_IsStart | 1 | 0 | 0 | 0 | 0 |
| LootDeposit_TestPlayerFlag | 0 | 1 | 0 | 0 | 0 |
| Lun_ClearReactionFlags | 1 | 0 | 0 | 0 | 0 |
| Lun_SetContactFlag | 0 | 1 | 0 | 0 | 0 |
| MarkHitFlag | 0 | 1 | 0 | 0 | 0 |
| Mask | 0 | 1 | 3 | 0 | 0 |
| Mask16 | 0 | 2 | 0 | 0 | 0 |
| MaskContactFlags | 0 | 1 | 0 | 0 | 0 |
| MaskKeys | 0 | 1 | 0 | 0 | 0 |
| MaskedMessageFlags | 0 | 0 | 1 | 0 | 0 |
| MemberHasBit | 0 | 0 | 1 | 0 | 0 |
| MimicChase_ClearBits | 0 | 0 | 2 | 0 | 0 |
| ModifierMenu_MarkDirty | 0 | 3 | 0 | 0 | 0 |
| MonsterInit_MaskColliderFlags | 0 | 22 | 0 | 0 | 0 |
| MonsterInit_MaskControlFlags | 0 | 1 | 0 | 0 | 0 |
| MonsterInit_SetBehaviorFlags | 1 | 31 | 1 | 0 | 0 |
| MonsterInit_SetMotionFlags | 0 | 11 | 10 | 0 | 0 |
| MonsterInit_SetStatusFlags | 0 | 94 | 10 | 0 | 0 |
| MonsterInit_TestMotionFlags | 0 | 29 | 15 | 0 | 0 |
| MonsterInit_TestTargetingFlags | 0 | 1 | 0 | 0 | 0 |
| MotionFlag | 0 | 0 | 1 | 0 | 0 |
| MotionFlagSet | 0 | 0 | 2 | 0 | 0 |
| MotionFlags | 4 | 2 | 2 | 0 | 0 |
| MotionState_ClearFlags | 0 | 3 | 2 | 0 | 0 |
| MotionState_SetFlags | 0 | 0 | 1 | 0 | 0 |
| MotionState_TestFlags | 0 | 1 | 1 | 0 | 0 |
| MovingPlatform_DisableFlags | 0 | 1 | 10 | 0 | 0 |
| MovingPlatform_EnableFlags | 0 | 11 | 8 | 0 | 0 |
| Mummy_BeginContactTracking | 0 | 0 | 1 | 0 | 0 |
| Mummy_ClearCheckpointFlags | 0 | 1 | 0 | 0 | 0 |
| Mummy_LoadContactLatch | 0 | 1 | 0 | 0 | 0 |
| Mummy_TestContext | 0 | 0 | 2 | 0 | 0 |
| Mummy_TestContextOwned | 0 | 0 | 1 | 0 | 0 |
| NamedObjectFlags_Add | 0 | 1 | 0 | 0 | 0 |
| NamedObjectFlags_Has | 0 | 10 | 11 | 0 | 0 |
| NamedObjectFlags_Remove | 0 | 0 | 1 | 0 | 0 |
| NidhoggrDuel_AddFlags | 1 | 17 | 9 | 0 | 0 |
| NidhoggrDuel_ClearFlags | 0 | 1 | 0 | 0 | 0 |
| NidhoggrDuel_TestFlags | 0 | 2 | 5 | 0 | 0 |
| Nidhoggr_AddTargetFlags | 0 | 0 | 1 | 0 | 0 |
| Nidhoggr_MaskPartFlags | 0 | 0 | 4 | 0 | 0 |
| Nidhoggr_SetPartExtraFlags | 1 | 1 | 4 | 0 | 0 |
| Octopus_EnableDefeatMotion | 0 | 0 | 1 | 0 | 0 |
| Or16 | 0 | 5 | 0 | 0 | 0 |
| OrV | 0 | 1 | 0 | 0 | 0 |
| OwnerWidgetsEnabled | 0 | 1 | 0 | 0 | 0 |
| PackKeys | 4 | 0 | 0 | 0 | 0 |
| PackPaletteGreen | 2 | 0 | 1 | 0 | 0 |
| PhaseObject_SetFlags | 0 | 1 | 0 | 0 | 0 |
| PhaseObject_TakeEntry | 0 | 0 | 7 | 0 | 0 |
| PlacementDamage_SetHitFlags | 0 | 1 | 0 | 0 | 0 |
| PlacementDamage_TestField | 0 | 1 | 0 | 0 | 0 |
| PlayStats_TakeFlag | 1 | 0 | 9 | 0 | 0 |
| PlayerCanMove | 0 | 2 | 0 | 0 | 0 |
| PlayerFlags | 0 | 1 | 0 | 0 | 0 |
| PlayerHasFlag | 0 | 1 | 0 | 0 | 0 |
| PlayerMask | 0 | 1 | 0 | 0 | 0 |
| PlayerModifier_TestTargetFlags | 4 | 6 | 0 | 0 | 0 |
| PlayerMotion_ClearAnimationFlags | 1 | 0 | 0 | 0 | 0 |
| PlayerScript_TestFlags | 0 | 1 | 0 | 0 | 0 |
| PlayerSlot_ReadStatus | 4 | 14 | 13 | 0 | 0 |
| Player_AddStatusFlags | 0 | 25 | 7 | 0 | 0 |
| Player_ReadStatusBits | 3 | 46 | 16 | 0 | 0 |
| Player_SetStatusBits | 0 | 18 | 3 | 0 | 0 |
| PooledActor_SetFlags | 0 | 4 | 0 | 0 | 0 |
| PooledActor_TestFlags | 0 | 3 | 0 | 0 | 0 |
| RandomBits | 0 | 0 | 2 | 0 | 0 |
| RandomFirst | 2 | 0 | 0 | 0 | 0 |
| Ratatosk_SetFlags | 3 | 24 | 12 | 0 | 0 |
| Ratatosk_TestFlags | 0 | 1 | 1 | 0 | 0 |
| Reaction_ClearHalf | 0 | 1 | 0 | 0 | 0 |
| Reaction_ClearMotion | 0 | 0 | 2 | 0 | 0 |
| Reaction_ClearWord | 1 | 1 | 1 | 0 | 0 |
| Reaction_SetWord | 0 | 1 | 0 | 0 | 0 |
| Recoil_TestDamageFlags | 0 | 0 | 1 | 0 | 0 |
| RecoveryRoute_AdvanceRandomCursor | 1 | 0 | 0 | 0 | 0 |
| Recovery_ClearTrackingFlags | 0 | 0 | 2 | 0 | 0 |
| RegionActor_HasBehavior | 0 | 4 | 7 | 0 | 0 |
| RegistryActor_SetMotionFlags | 0 | 1 | 1 | 0 | 0 |
| RegistryActor_SetUpdateFlags | 0 | 0 | 4 | 0 | 0 |
| RegistryActor_TestDamageFlags | 0 | 0 | 1 | 0 | 0 |
| RegistryActor_TestMotionFlags | 0 | 0 | 1 | 0 | 0 |
| RotationPhase_Mask | 0 | 2 | 0 | 0 | 0 |
| Sabata_SetFlags | 0 | 10 | 6 | 0 | 0 |
| SaveControlFlags | 0 | 1 | 0 | 0 | 0 |
| SaveFlags | 0 | 0 | 1 | 0 | 0 |
| ScalarApproach_ApplyDelta | 3 | 0 | 0 | 0 | 0 |
| Scale | 2 | 0 | 0 | 0 | 0 |
| ScaleTowardZero | 1 | 0 | 0 | 0 | 0 |
| ScaleTowardZeroResult | 1 | 0 | 0 | 0 | 0 |
| SceneActorAttachment_MotionBits | 0 | 0 | 2 | 0 | 0 |
| SceneActorAttachment_StatusBits | 0 | 4 | 0 | 0 | 0 |
| SceneActorBehavior_MotionBits | 0 | 5 | 1 | 0 | 0 |
| SceneActorBehavior_OverrideBits | 0 | 7 | 0 | 0 | 0 |
| SceneActorBehavior_SetWordFlags | 0 | 1 | 2 | 0 | 0 |
| SceneActorBody_SetFlags | 0 | 1 | 0 | 0 | 0 |
| SceneActorChase_AndMotionFlags | 0 | 1 | 0 | 0 | 0 |
| SceneActorChase_AndWordFlags | 1 | 27 | 6 | 0 | 0 |
| SceneActorChase_Bits32 | 0 | 36 | 3 | 0 | 0 |
| SceneActorChase_ClearActorFlags | 0 | 4 | 7 | 0 | 0 |
| SceneActorChase_ClearBehaviorFlags | 0 | 0 | 1 | 0 | 0 |
| SceneActorChase_ClearBits | 0 | 17 | 22 | 0 | 0 |
| SceneActorChase_ClearFlagComponent | 0 | 33 | 6 | 0 | 0 |
| SceneActorChase_ClearMotionFlags | 0 | 4 | 12 | 0 | 0 |
| SceneActorChase_ClearStatusFlags | 0 | 0 | 5 | 0 | 0 |
| SceneActorChase_ClearWordFlags | 6 | 41 | 56 | 0 | 0 |
| SceneActorChase_FlagComponentBits | 0 | 1 | 4 | 0 | 0 |
| SceneActorChase_GroupClearFlags | 0 | 3 | 0 | 0 | 0 |
| SceneActorChase_HasPlacementFlags | 0 | 2 | 0 | 0 | 0 |
| SceneActorChase_NextSample | 0 | 0 | 0 | 2 | 0 |
| SceneActorChase_OrReactionFlags | 0 | 1 | 3 | 0 | 0 |
| SceneActorChase_QueryMotionFlags | 0 | 37 | 14 | 0 | 0 |
| SceneActorChase_SetActorFlags | 0 | 44 | 47 | 0 | 0 |
| SceneActorChase_SetBehaviorFlags | 0 | 2 | 3 | 0 | 0 |
| SceneActorChase_SetFlagComponent | 1 | 12 | 1 | 0 | 0 |
| SceneActorChase_SetMotionFlags | 0 | 5 | 2 | 0 | 0 |
| SceneActorChase_SetStatusFlags | 0 | 2 | 2 | 0 | 0 |
| SceneActorChase_StatusFlags | 0 | 10 | 5 | 0 | 0 |
| SceneActorChase_Test | 0 | 40 | 6 | 0 | 0 |
| SceneActorChase_TestField | 0 | 25 | 6 | 0 | 0 |
| SceneActorContact_MotionFlags | 0 | 25 | 7 | 0 | 0 |
| SceneActorContact_QueryMotionField | 0 | 2 | 2 | 0 | 0 |
| SceneActorContact_ReadActorFlags | 0 | 2 | 2 | 0 | 0 |
| SceneActorContact_SetTrackingFlags | 0 | 1 | 2 | 0 | 0 |
| SceneActorContact_TestTracking | 0 | 1 | 0 | 0 | 0 |
| SceneActorContact_TestTrackingOwned | 0 | 0 | 2 | 0 | 0 |
| SceneActorControl_TestFlags | 0 | 11 | 8 | 0 | 0 |
| SceneActorDamage_OwnershipBits | 0 | 1 | 0 | 0 | 0 |
| SceneActorDamage_TestFlags | 0 | 3 | 0 | 0 | 0 |
| SceneActorMotion_TestStatus | 0 | 1 | 0 | 0 | 0 |
| SceneActorPlacement_ClearColliderFlags | 1 | 0 | 0 | 0 | 0 |
| SceneActorPlacement_ClearFlags | 0 | 12 | 2 | 1 | 0 |
| SceneActorPlacement_ReadFlags16 | 0 | 5 | 0 | 0 | 0 |
| SceneActorPlacement_ReadFlags32 | 0 | 6 | 0 | 0 | 0 |
| SceneActorPlacement_SetFlags | 0 | 4 | 0 | 0 | 0 |
| SceneActorPlacement_TestFlags | 0 | 5 | 5 | 0 | 0 |
| SceneActorPlacement_TestMask | 0 | 0 | 2 | 0 | 0 |
| SceneActorPlacement_TestValue | 0 | 1 | 0 | 0 | 0 |
| SceneActorReaction_EnableMotionFlags | 0 | 12 | 3 | 0 | 0 |
| SceneActorReaction_ReadFlags | 0 | 1 | 0 | 0 | 0 |
| SceneActorRecovery_ClearGate | 0 | 1 | 0 | 0 | 0 |
| SceneActorRoute_SetContextFlags | 0 | 1 | 0 | 0 | 0 |
| SceneActorState_ClearFlags | 0 | 1 | 0 | 0 | 0 |
| SceneActorState_SetFlags | 3 | 28 | 19 | 0 | 0 |
| SceneActorState_TestFlags | 0 | 10 | 6 | 0 | 0 |
| SceneActorState_TestPlayerFlags | 0 | 1 | 0 | 0 | 0 |
| SceneActorStimulus_CaptureMotionFlags | 0 | 1 | 0 | 0 | 0 |
| SceneActorTurn_ActorFlags | 2 | 18 | 12 | 0 | 0 |
| SceneActorTurn_Flags | 0 | 20 | 16 | 0 | 0 |
| SceneActor_MaskedHalfword | 0 | 10 | 2 | 0 | 0 |
| SceneActor_SetFlags | 0 | 11 | 1 | 0 | 0 |
| SceneActor_TestFlags | 0 | 4 | 0 | 0 | 0 |
| SceneActor_TestPendingFlag | 0 | 2 | 1 | 0 | 0 |
| SceneActors_SetDamageEffectFlags | 0 | 0 | 2 | 0 | 0 |
| SceneBody_TestFlags | 0 | 0 | 1 | 0 | 0 |
| SceneCollider_ReadContactFlags | 0 | 3 | 1 | 0 | 0 |
| SceneCollider_ReadReactionFlags | 0 | 3 | 2 | 0 | 0 |
| SceneEffectSchedule_TestOccupied | 1 | 0 | 0 | 0 | 0 |
| SceneHud_ClearMarkerSlots | 1 | 0 | 0 | 0 | 0 |
| SceneObject_ReadFlags | 0 | 0 | 4 | 0 | 0 |
| ScenePoolActor_SetStateFlags | 0 | 8 | 2 | 0 | 0 |
| ScenePool_HasControlRestriction | 0 | 0 | 1 | 0 | 0 |
| ScenePool_PlayerHasFlag | 0 | 0 | 7 | 0 | 0 |
| ScenePool_TestPlayerFlags | 0 | 0 | 1 | 0 | 0 |
| SceneStatusIndicator_IsAllocated | 1 | 0 | 0 | 0 | 0 |
| SegmentFlagSet | 0 | 1 | 0 | 0 | 0 |
| Set32 | 0 | 0 | 0 | 1 | 0 |
| SetAnimationFlags | 0 | 1 | 0 | 0 | 0 |
| SetAttachmentFlags | 0 | 0 | 1 | 0 | 0 |
| SetContextFlags | 0 | 1 | 0 | 0 | 0 |
| SetDigitVisibility | 0 | 0 | 3 | 0 | 0 |
| SetDisplayFlag | 0 | 2 | 0 | 0 | 0 |
| SetFlag | 0 | 3 | 1 | 0 | 0 |
| SetFlag_UpdatePlayerDestinationPhase | 0 | 1 | 0 | 0 | 0 |
| SetFlags | 0 | 12 | 4 | 0 | 0 |
| SetHit | 0 | 0 | 1 | 0 | 0 |
| SetHitFlags | 0 | 0 | 4 | 0 | 0 |
| SetMotion_SelectReactionControl | 0 | 0 | 1 | 0 | 0 |
| SetRegionFlag | 1 | 0 | 0 | 0 | 0 |
| Shift | 7 | 0 | 0 | 0 | 0 |
| ShiftRandom | 0 | 0 | 3 | 0 | 0 |
| ShiftTowardZero | 7 | 0 | 0 | 0 | 0 |
| SideStep_SetFlags | 0 | 1 | 0 | 0 | 0 |
| SignedShift | 8 | 0 | 0 | 0 | 0 |
| SkeletonInit_SetFlags | 0 | 0 | 2 | 0 | 0 |
| SkeletonInit_TestSkeletonFlags | 0 | 0 | 2 | 0 | 0 |
| Skeleton_SetContextFlags | 0 | 0 | 1 | 0 | 0 |
| Slime_ClearMotionBits | 0 | 1 | 0 | 0 | 0 |
| Slime_ClearPendingFlags | 0 | 2 | 0 | 0 | 0 |
| Slime_ClearReactionBits | 0 | 0 | 2 | 0 | 0 |
| Slime_ContactFlags | 0 | 1 | 0 | 0 | 0 |
| Slime_EnableReactionMotion | 0 | 1 | 0 | 0 | 0 |
| Slime_HasContextFlag | 0 | 2 | 0 | 0 | 0 |
| Slime_MaskedDamage | 0 | 1 | 0 | 0 | 0 |
| Slime_SetFlags | 0 | 1 | 1 | 0 | 0 |
| Slime_TestFlags | 0 | 0 | 1 | 0 | 0 |
| Slime_TestPendingFlag | 0 | 2 | 0 | 0 | 0 |
| Slime_TestReactionFlag | 0 | 1 | 0 | 0 | 0 |
| SparkExplosion_IsOccupied | 1 | 0 | 0 | 0 | 0 |
| SpatialEntry_TestFlags | 0 | 1 | 1 | 0 | 0 |
| Spider_ClearContextFlags | 0 | 1 | 0 | 0 | 0 |
| Spider_ClearMotionFlags | 0 | 1 | 0 | 0 | 0 |
| Spider_SetFlags | 2 | 0 | 0 | 0 | 0 |
| StagedEffect_AddOuterSpriteFlags | 0 | 2 | 0 | 0 | 0 |
| StateFlag | 0 | 2 | 1 | 0 | 0 |
| Sword_ContactFlags | 0 | 1 | 0 | 0 | 0 |
| TargetMotion_SetFlag | 0 | 0 | 1 | 0 | 0 |
| TentacleSegment_HasStatus | 0 | 0 | 4 | 0 | 0 |
| Tentacle_MaskTrailFlags | 0 | 1 | 0 | 0 | 0 |
| Tentacle_MaskWaveFlags | 0 | 1 | 0 | 0 | 0 |
| Tentacle_Shift | 2 | 0 | 0 | 0 | 0 |
| Tentacle_ShiftTowardZero | 2 | 0 | 0 | 0 | 0 |
| Tentacle_TestSegmentFlags | 0 | 2 | 0 | 0 | 0 |
| Tentacle_TestTrailFlags | 0 | 1 | 0 | 0 | 0 |
| Tentacle_TestWaveFlags | 0 | 0 | 1 | 0 | 0 |
| Test | 0 | 16 | 6 | 0 | 0 |
| TestActor | 0 | 0 | 1 | 0 | 0 |
| TestActorFlag | 0 | 3 | 3 | 0 | 0 |
| TestActorFlags | 0 | 1 | 0 | 0 | 0 |
| TestBits | 0 | 2 | 0 | 0 | 0 |
| TestColliderFlags | 0 | 0 | 1 | 0 | 0 |
| TestContext | 0 | 2 | 1 | 0 | 0 |
| TestContext_BeginTargetPursuit | 0 | 2 | 1 | 0 | 0 |
| TestFalling | 0 | 0 | 1 | 0 | 0 |
| TestField | 0 | 2 | 0 | 0 | 0 |
| TestFlag | 0 | 1 | 2 | 0 | 0 |
| TestFlags | 0 | 7 | 6 | 0 | 0 |
| TestGlobal | 0 | 1 | 1 | 0 | 0 |
| TestManagerFlag | 0 | 1 | 2 | 0 | 0 |
| TestMask | 1 | 8 | 0 | 0 | 0 |
| TestModeFlags | 0 | 0 | 1 | 0 | 0 |
| TestMotionFlag | 0 | 1 | 0 | 0 | 0 |
| TestPlayerFlag | 0 | 1 | 0 | 0 | 0 |
| TestStateFlags | 0 | 1 | 0 | 0 | 0 |
| TestV | 0 | 2 | 0 | 0 | 0 |
| Test_HasActiveResponse | 0 | 0 | 1 | 0 | 0 |
| Test_SelectHitRecovery | 0 | 1 | 0 | 0 | 0 |
| TruncateShift | 7 | 0 | 0 | 0 | 0 |
| Tst | 0 | 0 | 1 | 0 | 0 |
| Tst32 | 0 | 1 | 1 | 0 | 0 |
| VanargandHead_IncreaseDuration | 4 | 0 | 0 | 0 | 0 |
| VanargandPairAttack_SetFlags | 0 | 8 | 4 | 0 | 0 |
| VanargandPairAttack_TestFlags | 0 | 1 | 0 | 0 | 0 |
| Vanargand_GetModeShifted | 1 | 0 | 0 | 0 | 0 |
| Vanargand_MaskedFlags | 0 | 2 | 0 | 0 | 0 |
| Vanargand_SetFlags | 0 | 17 | 13 | 0 | 0 |
| Vanargand_TruncateShift | 13 | 0 | 0 | 0 | 0 |
| Vg_SetFlags | 0 | 0 | 9 | 0 | 0 |
| Vg_ShiftTowardZero | 4 | 0 | 0 | 0 | 0 |
| Vorn_ActorFlags | 0 | 1 | 0 | 0 | 0 |
| Vorn_AdjustContextFlags | 0 | 1 | 0 | 0 | 0 |
| Vorn_AdjustMotionFlags | 1 | 0 | 0 | 0 | 0 |
| Vorn_ClearContextFlags | 0 | 2 | 2 | 0 | 0 |
| Vorn_ClearTrackingFlags | 0 | 1 | 2 | 0 | 0 |
| Vorn_GetRandomBits | 2 | 0 | 0 | 0 | 0 |
| Vorn_GetRestorableMotion | 0 | 0 | 1 | 0 | 0 |
| Vorn_HasPursuitFlag | 0 | 0 | 1 | 0 | 0 |
| Vorn_Mask16 | 0 | 2 | 0 | 0 | 0 |
| Vorn_MaskLow | 0 | 1 | 0 | 0 | 0 |
| Vorn_SetResponseFlags | 0 | 0 | 2 | 0 | 0 |
| Vorn_SetTrackingFlags | 0 | 2 | 1 | 0 | 0 |
| Vorn_TestBody | 0 | 1 | 2 | 0 | 0 |
| Vorn_TestContext | 0 | 0 | 1 | 0 | 0 |
| Vorn_TestNear | 0 | 0 | 3 | 0 | 0 |
| Vorn_TestResponseFlags | 0 | 2 | 0 | 0 | 0 |
| Weather_ClearDisplayFlag | 0 | 1 | 0 | 0 | 0 |
| WordFlag | 0 | 2 | 0 | 0 | 0 |
| WordFlags_And | 0 | 2 | 0 | 0 | 0 |
| WordFlags_Has | 0 | 6 | 5 | 0 | 0 |
| WordFlags_Set | 2 | 5 | 10 | 0 | 0 |
| WordFlags_Test | 0 | 12 | 11 | 0 | 0 |
| addCategory | 0 | 0 | 6 | 0 | 0 |
| addFlags | 0 | 3 | 2 | 0 | 0 |
| andm | 0 | 2 | 0 | 0 | 0 |
| any | 0 | 1 | 0 | 0 | 0 |
| clearBits16 | 3 | 0 | 0 | 0 | 0 |
| clearColliderFlags | 0 | 0 | 2 | 0 | 0 |
| clearDrawFlags | 0 | 1 | 0 | 0 | 0 |
| clearFlags | 0 | 1 | 1 | 0 | 0 |
| clearFlags16 | 0 | 1 | 0 | 0 | 0 |
| flag | 0 | 0 | 2 | 0 | 0 |
| flags | 2 | 6 | 7 | 0 | 0 |
| globalFlags | 0 | 0 | 1 | 0 | 0 |
| hasControllerFlag | 0 | 0 | 1 | 0 | 0 |
| hasFlag | 1 | 0 | 0 | 0 | 0 |
| hitFlags | 0 | 1 | 0 | 0 | 0 |
| maskIE | 1 | 0 | 0 | 0 | 0 |
| masked | 0 | 1 | 0 | 0 | 0 |
| orm | 0 | 1 | 0 | 0 | 0 |
| orr | 0 | 4 | 0 | 0 | 0 |
| playerFlags | 0 | 1 | 0 | 0 | 0 |
| scale | 5 | 0 | 0 | 0 | 0 |
| setFlag | 0 | 0 | 1 | 0 | 0 |
| setFlags | 0 | 1 | 1 | 0 | 0 |
| setf | 0 | 1 | 0 | 0 | 0 |
| shift | 10 | 0 | 0 | 0 | 0 |
| test | 0 | 6 | 9 | 0 | 0 |
| testFlag | 0 | 2 | 0 | 0 | 0 |
| testFlags | 0 | 1 | 0 | 0 | 0 |
| testFlagsP | 0 | 2 | 1 | 0 | 0 |
| testReaction | 0 | 2 | 0 | 0 | 0 |
| testRequest | 0 | 0 | 1 | 0 | 0 |
| truncateShift | 8 | 0 | 0 | 0 | 0 |
| tst | 0 | 1 | 0 | 0 | 0 |

## Typed field and operation

The ROM share is mask-first among classified mask-first and load-first operations. Interleaved observations are shown separately. A dash means the ROM scan has no classified observations for that row.

| Type / field / operation | PLAIN | BOUND-SINGLE | BOUND-SHARED | SKIP | Pending | Helper and local spellings | ROM mask / load / interleaved |
| --- | --- | --- | --- | --- | --- | --- | --- |
| (global) / gDispCntShadow / and-store | 0 | 16 | 1 | 0 | 0 | `Weather_ClearDisplayFlag`, `clear`, `clearMask`, `m`, `mask` | 39 / 0 / 0 (100.0%) |
| (global) / gDispCntShadow / clear | 0 | 7 | 11 | 0 | 0 | `Bg_DisableDisplay`, `bits`, `mask`, `visible` | 39 / 0 / 0 (100.0%) |
| (global) / gDispCntShadow / set | 1 | 13 | 20 | 0 | 0 | `Bg_EnableDisplay`, `Or16`, `bgEnable`, `bits` | 31 / 0 / 0 (100.0%) |
| (global) / gEngineFlags / and-store | 0 | 2 | 0 | 0 | 0 | `SceneActorChase_AndWordFlags` | 2 / 5 / 1 (28.6%) |
| (global) / gEngineFlags / nonzero | 1 | 54 | 6 | 0 | 0 | `Chase_Test`, `SceneActorChase_Test`, `SceneActorControl_TestFlags`, `SceneActorDamage_TestFlags`, `Test` | 1 / 269 / 1 (0.4%) |
| (global) / gEnginePauseFlags / and | 0 | 1 | 0 | 0 | 0 | `Mask` | 0 / 32 / 0 (0.0%) |
| (global) / gEnginePauseFlags / and-store | 0 | 7 | 0 | 0 | 0 | `SceneActorChase_AndWordFlags`, `andm`, `m`, `mask` | 10 / 1 / 0 (90.9%) |
| (global) / gEnginePauseFlags / clear | 0 | 2 | 4 | 0 | 0 | `Engine_ClearPauseFlags` | 10 / 1 / 0 (90.9%) |
| (global) / gEnginePauseFlags / set | 0 | 9 | 6 | 0 | 0 | `Engine_SetPauseFlags`, `WordFlags_Set`, `four`, `mask` | 18 / 0 / 0 (100.0%) |
| (global) / gUnk_02000560 / and | 0 | 21 | 7 | 0 | 0 | `Flags_MaskedWord`, `SceneActorBehavior_OverrideBits`, `TestFlags`, `TestGlobal`, `TestModeFlags`, `Tst32`, `any`, `blockedMask`, `four`, `globalFlags`, `m`, `mask`, `test` | — |
| (global) / gUnk_02000560 / nonzero | 0 | 1 | 10 | 0 | 0 | `button`, `cameraMask`, `flagMask`, `mask`, `one`, `pause` | — |
| (unresolved type) / context.actionFlags / and | 0 | 0 | 1 | 0 | 0 | `m` | — |
| (unresolved type) / flags / and | 0 | 0 | 1 | 0 | 0 | `mask` | — |
| (unresolved type) / flags / clear | 0 | 0 | 1 | 0 | 0 | `bit` | — |
| (unresolved type) / flags868 / and-store | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| (unresolved type) / flags868 / nonzero | 0 | 32 | 6 | 0 | 0 | `GameState_TestWorldFlags`, `OwnerWidgetsEnabled`, `bit`, `mask`, `testFlag` | — |
| (unresolved type) / flags868 / set | 0 | 5 | 0 | 0 | 0 | `GameState_SetWorldFlags`, `bit`, `mask` | — |
| (unresolved type) / statusFlags / and | 0 | 1 | 0 | 0 | 0 | `SceneActorChase_Bits32` | — |
| struct A / fl / set | 0 | 1 | 1 | 0 | 0 | `m` | 2 / 0 / 0 (100.0%) |
| struct ActorAnimController / flags / set | 0 | 17 | 9 | 0 | 0 | `one` | — |
| struct ActorAttachmentFollowView / flags / and | 0 | 0 | 1 | 0 | 0 | `mask` | — |
| struct ActorControllerMotion / flags / clear | 0 | 0 | 11 | 0 | 0 | `ActorControllerMotion_ClearFlags`, `ActorController_ClearMotionFlags`, `one` | 4 / 0 / 0 (100.0%) |
| struct ActorControllerMotion / flags / set | 0 | 0 | 7 | 0 | 0 | `one` | — |
| struct ActorEffectStatus / flags / nonzero | 0 | 1 | 0 | 0 | 0 | `mask` | 1 / 4 / 0 (20.0%) |
| struct ActorReaction / flags / set | 0 | 1 | 0 | 0 | 0 | `flags` | — |
| struct ActorSoundPhase / flags / set | 0 | 2 | 1 | 0 | 0 | `flag` | — |
| struct ActorStats / damageFlags / and-store | 0 | 1 | 0 | 0 | 0 | `ClearDamageFlag` | 1 / 0 / 0 (100.0%) |
| struct ActorStats / damageFlags / nonzero | 0 | 0 | 2 | 0 | 0 | `ActorStats_TestDamageFlags`, `RegistryActor_TestDamageFlags` | 2 / 0 / 0 (100.0%) |
| struct ActorStats / damageFlags / set | 0 | 1 | 1 | 0 | 0 | `ActorStats_SetDamageFlags` | — |
| struct AnimSprite / flags / and-store | 0 | 1 | 6 | 0 | 0 | `clear`, `mask` | 4 / 19 / 0 (17.4%) |
| struct AnimSprite / flags / set | 2 | 0 | 2 | 0 | 0 | `mask`, `one` | 3 / 6 / 0 (33.3%) |
| struct AreaEffectManager / player.statusFlags / and | 0 | 1 | 0 | 0 | 0 | `tst` | — |
| struct ArmorCollisionControl / flags / and-store | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct ArmorCollisionControl / flags / set | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct ArmorContact / contactFlags / and | 0 | 1 | 0 | 0 | 0 | `Armor_ContactFlags` | — |
| struct ArmorInitB / b1 / set | 0 | 0 | 1 | 0 | 0 | `active` | — |
| struct ArmorPart / flags119 / set | 0 | 0 | 2 | 0 | 0 | `active`, `one` | — |
| struct Axe / flagsE4C / and | 0 | 2 | 0 | 0 | 0 | `Flags_MaskedWord`, `RotationPhase_Mask` | — |
| struct Axe / flagsE4C / and-store | 0 | 3 | 0 | 0 | 0 | `m`, `m2`, `mask` | — |
| struct Axe / flagsE4C / clear | 0 | 2 | 0 | 0 | 0 | `clearFlags`, `k1` | — |
| struct Axe / flagsE4C / nonzero | 0 | 2 | 1 | 0 | 0 | `WordFlags_Has` | — |
| struct Axe / flagsE4C / set | 0 | 5 | 2 | 0 | 0 | `Axe_SetEffectFlags`, `WordFlags_Set`, `flag` | — |
| struct AxeFlags16 / bits / and-store | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct AxeFlashState / flags / set | 0 | 0 | 2 | 0 | 0 | `bit` | — |
| struct AxePaletteContext / flags / and | 0 | 1 | 0 | 0 | 0 | `Flags_MaskedWord` | — |
| struct Bee / flags / clear | 0 | 1 | 1 | 0 | 0 | `Bee_ClearRouteFlags`, `SceneActorChase_ClearWordFlags` | — |
| struct Bee / flags / nonzero | 0 | 2 | 1 | 0 | 0 | `Bee_HasRouteFlag` | — |
| struct Bee / flags / set | 0 | 2 | 0 | 0 | 0 | `Bee_SetRouteFlags` | — |
| struct BouncingContact / flags / and | 0 | 1 | 0 | 0 | 0 | `WordFlags_Test` | 1 / 0 / 0 (100.0%) |
| struct CameraTracking / flags / and | 0 | 1 | 0 | 0 | 0 | `Vanargand_MaskedFlags` | — |
| struct CameraTracking / requests / and | 0 | 0 | 1 | 0 | 0 | `testRequest` | 1 / 0 / 0 (100.0%) |
| struct CameraTracking / requests / clear | 4 | 0 | 8 | 0 | 0 | `CameraTracking_ClearRequests` | — |
| struct CameraTracking / requests / set | 0 | 0 | 3 | 0 | 0 | `CameraTracking_SetRequests` | — |
| struct ChaseActor / base.drawable / set | 0 | 0 | 1 | 0 | 0 | `mask` | — |
| struct ChaseFlags40 / bits / and-store | 0 | 7 | 0 | 0 | 0 | `Mummy_ClearCheckpointFlags`, `mask`, `motionMask` | — |
| struct ChaseFlags40 / bits / clear | 0 | 1 | 0 | 0 | 0 | `Reaction_ClearHalf` | — |
| struct ChaseFlags40 / bits / set | 0 | 2 | 0 | 0 | 0 | `mask` | — |
| struct CloudSource / flags / nonzero | 0 | 0 | 1 | 0 | 0 | `bit` | — |
| struct CmdFlags / flags / and | 0 | 0 | 1 | 0 | 0 | `modeBit` | — |
| struct CoffinBike / flags / and-store | 0 | 1 | 0 | 0 | 0 | `HalfwordFlags_And` | — |
| struct CoffinBike / flags / set | 0 | 0 | 10 | 0 | 0 | `bit` | — |
| struct CoffinBikeDraw / flags / and-store | 0 | 1 | 0 | 0 | 0 | `clearDrawFlags` | — |
| struct CoffinBikeDraw / flags / nonzero | 0 | 0 | 1 | 0 | 0 | `bit` | 1 / 0 / 0 (100.0%) |
| struct CoffinBikeMotion / damageFlags / and-store | 0 | 1 | 0 | 0 | 0 | `CoffinBike_ClearDamageFlag` | — |
| struct CoffinBikeMotion / damageFlags / nonzero | 0 | 1 | 0 | 0 | 0 | `TestActorFlags` | 1 / 0 / 0 (100.0%) |
| struct Col / entryFlags / and-store | 0 | 0 | 1 | 0 | 0 | `m` | — |
| struct ContactActor / flags / and | 0 | 1 | 0 | 0 | 0 | `testFlags` | — |
| struct ContactCollider / contactFlags / and | 0 | 3 | 0 | 0 | 0 | `ContactCollider_TestContactFlags` | 2 / 1 / 0 (66.7%) |
| struct ContactCollider / flags / and | 0 | 11 | 2 | 0 | 0 | `ContactCollider_TestFlags`, `SceneActorTurn_Flags`, `SceneActor_MaskedHalfword`, `Slime_ContactFlags`, `TestColliderFlags`, `mask` | 9 / 2 / 0 (81.8%) |
| struct ContactCollider / flags / and-store | 0 | 35 | 10 | 0 | 0 | `ClearFlags`, `Clone_MaskColliderFlags`, `HalfwordFlags_And`, `MonsterInit_MaskColliderFlags`, `Nidhoggr_MaskPartFlags`, `clearMask`, `mask` | — |
| struct ContactCollider / flags / clear | 0 | 1 | 37 | 0 | 0 | `ActorController_ClearColliderFlags`, `ContactCollider_ClearFlags`, `GolemSlide_ClearColliderFlags`, `attackFlag`, `mask` | — |
| struct ContactCollider / flags / nonzero | 0 | 8 | 1 | 0 | 0 | `angleFlag`, `directionMask`, `mask` | 9 / 2 / 0 (81.8%) |
| struct ContactCollider / flags / set | 16 | 5 | 10 | 0 | 0 | `ActorController_SetColliderFlags`, `ContactCollider_SetFlags`, `attackFlag`, `bodyFlags`, `flags`, `mask` | 1 / 15 / 0 (6.2%) |
| struct ContactCollider / reactionFlags / and | 11 | 18 | 14 | 0 | 0 | `AttackFlags`, `ContactCollider_TestReactionFlags`, `Flags_MaskedWord`, `Mask`, `Player_ReadStatusBits`, `mask`, `testReaction` | 4 / 3 / 0 (57.1%) |
| struct ContactCollider / reactionFlags / and-store | 0 | 1 | 2 | 0 | 0 | `mask` | — |
| struct ContactCollider / reactionFlags / clear | 5 | 0 | 5 | 0 | 0 | `ContactCollider_ClearReactionFlags` | — |
| struct ContactCollider / reactionFlags / nonzero | 0 | 3 | 2 | 0 | 0 | `m`, `mask`, `one`, `reactionMask` | 4 / 3 / 0 (57.1%) |
| struct ContactCollider / reactionFlags / set | 14 | 6 | 8 | 0 | 0 | `ContactCollider_SetReactionFlags`, `m`, `mask` | — |
| struct ContactCollider / statRewardFlags / and | 0 | 3 | 2 | 0 | 0 | `Mask`, `mask`, `mask2` | 3 / 5 / 0 (37.5%) |
| struct ContactCollider / statRewardFlags / nonzero | 0 | 1 | 1 | 0 | 0 | `mask`, `one` | 3 / 5 / 0 (37.5%) |
| struct ContactCollider / statRewardFlags / set | 0 | 0 | 6 | 0 | 0 | `addCategory` | — |
| struct ContactSwarmSprite / flags / set | 2 | 0 | 1 | 0 | 0 | `WordFlags_Set`, `one` | 3 / 18 / 0 (14.3%) |
| struct ControllerSlot / flags / set | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct CrateHit / flags / and | 0 | 1 | 0 | 0 | 0 | `hitFlags` | 1 / 0 / 0 (100.0%) |
| struct Crow / flags / clear | 0 | 2 | 1 | 0 | 0 | `Crow_ClearFlags`, `SceneActorChase_ClearWordFlags` | — |
| struct Crow / flags / nonzero | 0 | 2 | 0 | 0 | 0 | `Crow_TestAnimationFlags`, `SceneActorPlacement_TestValue` | — |
| struct Crow / flags / set | 0 | 0 | 1 | 0 | 0 | `Crow_SetFlags` | — |
| struct DashActor / flags / set | 0 | 1 | 0 | 0 | 0 | `mask` | 1 / 0 / 0 (100.0%) |
| struct DepartureActor / actorFlags / and-store | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct DepartureActor / secondaryFlags / and-store | 0 | 0 | 1 | 0 | 0 | `mask` | — |
| struct Disc / falling / nonzero | 0 | 0 | 1 | 0 | 0 | `TestFalling` | — |
| struct DisplaySequenceSprite / flags / and-store | 0 | 1 | 0 | 0 | 0 | `mask` | 0 / 4 / 0 (0.0%) |
| struct DisplaySequenceSprite / flags / set | 0 | 0 | 1 | 0 | 0 | `one` | 0 / 7 / 0 (0.0%) |
| struct Dog / flags / and-store | 0 | 5 | 0 | 0 | 0 | `SceneActorChase_AndWordFlags`, `mask` | — |
| struct Dog / flags / clear | 0 | 1 | 0 | 0 | 0 | `SceneActorChase_ClearWordFlags` | — |
| struct Dog / flags / nonzero | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct Dog / flags / set | 0 | 1 | 1 | 0 | 0 | `bit`, `one` | — |
| struct E / fl / set | 0 | 0 | 1 | 0 | 0 | `m` | — |
| struct EffectCollider / flags / and-store | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct EffectGroupEntry / flags / set | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct EffectManager / actor.statusFlags / and | 0 | 2 | 0 | 0 | 0 | `Player_ReadStatusBits` | — |
| struct EffectVariantSource / flags / and | 0 | 1 | 0 | 0 | 0 | `WordFlags_Test` | 1 / 0 / 0 (100.0%) |
| struct EnemyParticle / flags / set | 0 | 1 | 0 | 0 | 0 | `m` | 1 / 0 / 0 (100.0%) |
| struct Entry / flags / set | 0 | 1 | 0 | 0 | 0 | `value` | — |
| struct FlagHalfword / bits / and-store | 0 | 5 | 0 | 0 | 0 | `ClearHalfFlags`, `ClearHit`, `ClearHitFlags` | — |
| struct FlagHalfword / bits / set | 0 | 0 | 5 | 0 | 0 | `SetHit`, `SetHitFlags` | — |
| struct Flags6 / flags / and-store | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct FrameDisplay / flags / and | 0 | 0 | 1 | 0 | 0 | `WordFlags_Test` | 1 / 0 / 0 (100.0%) |
| struct FrameDisplay / flags / set | 0 | 0 | 1 | 0 | 0 | `WordFlags_Set` | 1 / 0 / 0 (100.0%) |
| struct GameState / flags868 / and | 0 | 4 | 0 | 0 | 0 | `m` | — |
| struct GameState / flags868 / nonzero | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct Ghost / flags / and | 0 | 1 | 1 | 0 | 0 | `mask` | — |
| struct Ghost / flags / and-store | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct Ghost / flags / clear | 0 | 1 | 3 | 0 | 0 | `Ghost_ClearFlags`, `SceneActorChase_ClearWordFlags` | — |
| struct Ghost / flags / nonzero | 0 | 6 | 4 | 0 | 0 | `Ghost_TestFlags`, `mask` | — |
| struct Ghost / flags / set | 0 | 3 | 5 | 0 | 0 | `Ghost_SetFlags` | — |
| struct Ghoul / discFlags / clear | 0 | 0 | 4 | 0 | 0 | `Ghoul_ClearDiscFlags`, `SceneActorChase_ClearBits` | — |
| struct Ghoul / discFlags / nonzero | 0 | 0 | 1 | 0 | 0 | `Ghoul_TestDiscFlags` | — |
| struct Ghoul / discFlags / set | 0 | 2 | 0 | 0 | 0 | `Ghoul_SetDiscFlags` | — |
| struct Golem / flags628 / set | 0 | 1 | 0 | 0 | 0 | `GolemReaction_SetBodyFlags` | — |
| struct GridAttackActor / flags / and | 0 | 2 | 5 | 0 | 0 | `NidhoggrDuel_TestFlags` | — |
| struct GridAttackActor / flags / clear | 0 | 1 | 0 | 0 | 0 | `NidhoggrDuel_ClearFlags` | — |
| struct GridAttackActor / flags / set | 1 | 17 | 9 | 0 | 0 | `NidhoggrDuel_AddFlags` | 2 / 0 / 0 (100.0%) |
| struct GrowthHazardAnimationEvent / flags38 / and | 0 | 0 | 1 | 0 | 0 | `WordFlags_Test` | — |
| struct GrowthHazardAnimationEvent / flagsA / and | 0 | 1 | 0 | 0 | 0 | `HalfwordFlags_Test` | — |
| struct HitMotionFlags / bits / and-store | 0 | 1 | 0 | 0 | 0 | `bits` | — |
| struct HitMotionFlags / bits / set | 0 | 1 | 0 | 0 | 0 | `MarkHitFlag` | — |
| struct HomingCollider / flags / and-store | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct HomingSprite / flags / set | 0 | 0 | 1 | 0 | 0 | `flagMask` | — |
| struct Hresvelgr / flags / and | 0 | 1 | 0 | 0 | 0 | `TestFlags` | — |
| struct Hresvelgr / flags / set | 0 | 8 | 0 | 0 | 0 | `EnableFlags`, `SetFlags`, `mask` | — |
| struct Hresvelgr / player.statusFlags / and | 0 | 1 | 0 | 0 | 0 | `PlayerHasFlag` | — |
| struct HresvelgrDamage / controllerFlags / and | 0 | 0 | 1 | 0 | 0 | `hasControllerFlag` | — |
| struct HresvelgrDamage / flags / set | 0 | 3 | 2 | 0 | 0 | `addFlags` | — |
| struct HresvelgrMotionFlags / flags / set | 0 | 1 | 0 | 0 | 0 | `setFlags` | — |
| struct HresvelgrTravelActor / behaviorFlags / and | 0 | 0 | 1 | 0 | 0 | `cameraMask` | 1 / 0 / 0 (100.0%) |
| struct HresvelgrTravelActor / behaviorFlags / nonzero | 0 | 0 | 1 | 0 | 0 | `one` | 1 / 0 / 0 (100.0%) |
| struct HresvelgrTravelActor / flags / and | 2 | 2 | 5 | 0 | 0 | `Hresvelgr_TestFlags` | — |
| struct HresvelgrTravelActor / flags / set | 0 | 31 | 5 | 0 | 0 | `Hresvelgr_SetFlags`, `mask` | 12 / 0 / 0 (100.0%) |
| struct HresvelgrTravelActor / player.statusFlags / and | 0 | 1 | 0 | 0 | 0 | `PlayerFlags` | — |
| struct InteractionPartner / flags / set | 0 | 0 | 2 | 0 | 0 | `one` | — |
| struct Item / flags / and | 0 | 0 | 1 | 0 | 0 | `one` | — |
| struct Ivy / captureFlags / and-store | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct Ivy / captureFlags / set | 0 | 0 | 1 | 0 | 0 | `mask` | — |
| struct LinkAura / f20 / set | 0 | 1 | 3 | 0 | 0 | `LinkAura_SetFlags` | — |
| struct LinkedEffectCollider / flags / clear | 0 | 0 | 1 | 0 | 0 | `ClearFlags` | — |
| struct Lun / flags3D4 / clear | 0 | 1 | 0 | 0 | 0 | `SceneActorChase_ClearWordFlags` | — |
| struct Lun / flags3D4 / nonzero | 0 | 2 | 1 | 0 | 0 | `mask` | — |
| struct MenuCursor / stateFlags / and | 0 | 0 | 1 | 0 | 0 | `one` | — |
| struct MenuCursor / stateFlags / set | 0 | 1 | 0 | 0 | 0 | `m` | 2 / 1 / 0 (66.7%) |
| struct MenuCursorBounds / flags / and | 0 | 0 | 6 | 0 | 0 | `Entry_TestFlags`, `mask` | — |
| struct MenuCursorBounds / flags / nonzero | 0 | 1 | 1 | 0 | 0 | `mask`, `one` | — |
| struct Mimic / reactionFlags / clear | 0 | 1 | 0 | 0 | 0 | `SceneActorChase_ClearBits` | — |
| struct Mimic / reactionFlags / nonzero | 0 | 0 | 1 | 0 | 0 | `mask` | — |
| struct ModifierMenu / dirtyFlags / set | 0 | 3 | 0 | 0 | 0 | `ModifierMenu_MarkDirty` | — |
| struct Motion0818Object / flags119 / set | 0 | 0 | 2 | 0 | 0 | `one` | 2 / 0 / 0 (100.0%) |
| struct MotionPoolSlot / flags / nonzero | 0 | 1 | 0 | 0 | 0 | `flag` | — |
| struct MotionPoolSlot / flags / set | 0 | 1 | 0 | 0 | 0 | `flag` | 1 / 0 / 0 (100.0%) |
| struct MotionTarget / flags / and-store | 0 | 2 | 0 | 0 | 0 | `andm`, `mask` | — |
| struct MotionTarget / flags / set | 1 | 2 | 6 | 0 | 0 | `Nidhoggr_AddTargetFlags`, `Nidhoggr_SetPartExtraFlags`, `mask`, `orm` | — |
| struct MovingPairActor / flags / set | 0 | 1 | 2 | 0 | 0 | `mask` | — |
| struct Mummy / flags6F2 / and-store | 0 | 2 | 0 | 0 | 0 | `m`, `mask` | — |
| struct Mummy / flags6F2 / clear | 0 | 1 | 0 | 0 | 0 | `contextBits` | — |
| struct Mummy / flags6F2 / nonzero | 0 | 0 | 2 | 0 | 0 | `Mummy_TestContext` | — |
| struct NamedObject / flags / and | 0 | 0 | 1 | 0 | 0 | `m` | 6 / 0 / 0 (100.0%) |
| struct NamedObject / flags / and-store | 0 | 2 | 0 | 0 | 0 | `mask` | — |
| struct NamedObject / flags / clear | 0 | 1 | 11 | 0 | 0 | `MovingPlatform_DisableFlags`, `NamedObjectFlags_Remove` | — |
| struct NamedObject / flags / set | 0 | 12 | 8 | 0 | 0 | `MovingPlatform_EnableFlags`, `NamedObjectFlags_Add` | 2 / 0 / 0 (100.0%) |
| struct Npc / displayFlags / set | 0 | 2 | 0 | 0 | 0 | `mask` | — |
| struct Npc / flags / set | 0 | 5 | 0 | 0 | 0 | `mask` | — |
| struct Npc / flagsC / and-store | 0 | 1 | 0 | 0 | 0 | `m` | — |
| struct Npc / flagsC / nonzero | 0 | 0 | 1 | 0 | 0 | `two` | — |
| struct Obj / flags / nonzero | 0 | 0 | 1 | 0 | 0 | `mask` | — |
| struct Obj / flags / set | 0 | 0 | 1 | 0 | 0 | `one` | — |
| struct OrbitEffect / flags / and | 0 | 0 | 1 | 0 | 0 | `WordFlags_Test` | — |
| struct OrbitEffect / flags / and-store | 0 | 2 | 0 | 0 | 0 | `SceneActorChase_AndWordFlags` | — |
| struct OrbitEffect / flags / set | 0 | 1 | 0 | 0 | 0 | `flag` | — |
| struct OrbitSprite / flags / set | 0 | 0 | 1 | 0 | 0 | `one` | — |
| struct OtenkoActor / commandFlags / and-store | 0 | 0 | 2 | 0 | 0 | `mask` | — |
| struct OtenkoActor / displayFlags / and | 0 | 1 | 1 | 0 | 0 | `m2`, `m8` | 1 / 0 / 0 (100.0%) |
| struct OtenkoActor / displayFlags / set | 0 | 2 | 0 | 0 | 0 | `m`, `mask` | 1 / 1 / 0 (50.0%) |
| struct P / w34 / and-store | 0 | 0 | 1 | 0 | 0 | `m` | — |
| struct P / w60 / and-store | 0 | 0 | 1 | 0 | 0 | `m` | — |
| struct P08142C1C / f20 / and | 0 | 1 | 0 | 0 | 0 | `Player_ReadStatusBits` | 1 / 0 / 0 (100.0%) |
| struct P08142F94 / f20 / and | 0 | 1 | 0 | 0 | 0 | `Player_ReadStatusBits` | 1 / 0 / 0 (100.0%) |
| struct P08159BB8 / f20 / set | 0 | 1 | 0 | 0 | 0 | `Player_SetStatusBits` | 1 / 0 / 0 (100.0%) |
| struct P0823A5EC / flags / clear | 0 | 1 | 1 | 0 | 0 | `ClearFlags` | — |
| struct P0823A5EC / flags / set | 0 | 1 | 0 | 0 | 0 | `SetFlags` | — |
| struct P0823CFDC / f20 / and | 0 | 1 | 0 | 0 | 0 | `Player_ReadStatusBits` | 1 / 0 / 0 (100.0%) |
| struct P0823D370 / f20 / and | 0 | 1 | 0 | 0 | 0 | `Player_ReadStatusBits` | 1 / 0 / 0 (100.0%) |
| struct PairedHalfTarget / flags / and-store | 0 | 0 | 1 | 0 | 0 | `mask` | — |
| struct PairedHalfTarget / flags / set | 0 | 0 | 1 | 0 | 0 | `mask` | — |
| struct PairedSceneHalf / actionFlags / and | 0 | 9 | 0 | 0 | 0 | `Vanargand_MaskedFlags`, `bit`, `fmask`, `mask`, `masked` | — |
| struct PairedSceneHalf / actionFlags / and-store | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct PairedSceneHalf / actionFlags / set | 0 | 26 | 18 | 0 | 0 | `Vanargand_SetFlags`, `mask`, `setFlags`, `twelve` | — |
| struct PairedSceneHalf / flags / and | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct PairedSceneHalf / flags / set | 0 | 1 | 0 | 0 | 0 | `SetFlags` | — |
| struct Particle / flags / and-store | 0 | 1 | 0 | 0 | 0 | `ClearFlags` | — |
| struct ParticleGroupParticle / flags / set | 0 | 0 | 2 | 0 | 0 | `hidden` | — |
| struct PhaseObjectMode / flags / set | 0 | 1 | 0 | 0 | 0 | `PhaseObject_SetFlags` | 1 / 0 / 0 (100.0%) |
| struct PlacementFlagHalfword / bits / set | 0 | 1 | 0 | 0 | 0 | `PlacementDamage_SetHitFlags` | — |
| struct PlayStatsScreen / flags / set | 0 | 1 | 1 | 0 | 0 | `bit` | — |
| struct Player / actionFlags / and | 1 | 17 | 3 | 0 | 0 | `ActorStatus_PalettePlayerFlags`, `Player_ReadStatusBits`, `WordFlags_Test` | 4 / 2 / 0 (66.7%) |
| struct Player / actionFlags / nonzero | 0 | 1 | 0 | 0 | 0 | `mask` | 4 / 2 / 0 (66.7%) |
| struct Player / statBonusFlags / and | 0 | 7 | 2 | 0 | 0 | `Player_ReadStatusBits` | 3 / 0 / 0 (100.0%) |
| struct Player / statusFlags / and | 0 | 25 | 9 | 0 | 0 | `Flag`, `PlayerScript_TestFlags`, `Player_ReadStatusBits`, `SceneActorChase_Bits32`, `SceneActorState_TestPlayerFlags`, `TestField`, `TestFlags`, `TestPlayerFlag`, `blocked`, `excluded`, `flag`, `mask`, `playerMask` | 3 / 0 / 0 (100.0%) |
| struct Player / statusFlags / nonzero | 0 | 4 | 0 | 0 | 0 | `m`, `mask` | 3 / 0 / 0 (100.0%) |
| struct Player / statusFlags / set | 0 | 38 | 7 | 0 | 0 | `Player_AddStatusFlags`, `Player_SetStatusBits`, `flag` | 7 / 0 / 0 (100.0%) |
| struct PlayerCloneSlot / flags / clear | 0 | 1 | 0 | 0 | 0 | `ClearFlags` | — |
| struct PlayerCloneSlot / flags / set | 0 | 0 | 4 | 0 | 0 | `WordFlags_Set` | — |
| struct PlayerEventArguments / header / and | 0 | 1 | 0 | 0 | 0 | `countMask` | — |
| struct PlayerModifierTarget / flags / and | 4 | 6 | 0 | 0 | 0 | `PlayerModifier_TestTargetFlags` | 6 / 0 / 0 (100.0%) |
| struct PlayerPushState / flags / set | 0 | 0 | 3 | 0 | 0 | `Player_SetStatusBits` | — |
| struct PooledActor / flags / and | 0 | 3 | 0 | 0 | 0 | `PooledActor_TestFlags` | 1 / 0 / 0 (100.0%) |
| struct PooledActor / flags / nonzero | 0 | 1 | 0 | 0 | 0 | `mask` | 1 / 0 / 0 (100.0%) |
| struct PooledActor / flags / set | 0 | 4 | 0 | 0 | 0 | `PooledActor_SetFlags` | 4 / 0 / 0 (100.0%) |
| struct PresentationCollider / flags / clear | 0 | 0 | 1 | 0 | 0 | `ClearColliderFlag` | — |
| struct ProjectedDisplayEntry / flags / nonzero | 0 | 0 | 1 | 0 | 0 | `mask` | 0 / 1 / 0 (0.0%) |
| struct R / flags / set | 0 | 1 | 0 | 0 | 0 | `c` | — |
| struct Ratatosk / flags / and | 2 | 7 | 8 | 0 | 0 | `Ratatosk_TestFlags`, `flags` | 1 / 0 / 0 (100.0%) |
| struct Ratatosk / flags / clear | 0 | 0 | 1 | 0 | 0 | `clearFlags` | — |
| struct Ratatosk / flags / nonzero | 0 | 1 | 0 | 0 | 0 | `actorMask` | 1 / 0 / 0 (100.0%) |
| struct Ratatosk / flags / set | 3 | 24 | 12 | 0 | 0 | `Ratatosk_SetFlags` | 3 / 0 / 0 (100.0%) |
| struct RatatoskParticle / motionFlags / and-store | 0 | 1 | 0 | 0 | 0 | `ClearFlags` | — |
| struct RatatoskParticle / motionFlags / set | 0 | 0 | 1 | 0 | 0 | `one` | — |
| struct RegionActor / behaviorFlags / and | 0 | 1 | 0 | 0 | 0 | `m` | 2 / 0 / 0 (100.0%) |
| struct RegionActor / behaviorFlags / nonzero | 0 | 5 | 7 | 0 | 0 | `RegionActor_HasBehavior`, `mask` | 2 / 0 / 0 (100.0%) |
| struct RegionActor / behaviorFlags / set | 0 | 1 | 0 | 0 | 0 | `bit` | — |
| struct RegistryActorControl / updateFlags / set | 0 | 0 | 4 | 0 | 0 | `RegistryActor_SetUpdateFlags` | — |
| struct RegistryActorMotion / flags / and-store | 0 | 1 | 0 | 0 | 0 | `ClearDrawFlags` | — |
| struct RegistryActorMotion / flags / nonzero | 0 | 0 | 1 | 0 | 0 | `RegistryActor_TestMotionFlags` | — |
| struct RegistryActorMotion / flags / set | 0 | 1 | 1 | 0 | 0 | `RegistryActor_SetMotionFlags` | — |
| struct RegistryChoiceMenu / cursor.stateFlags / and | 0 | 2 | 0 | 0 | 0 | `TestFlags` | — |
| struct RepeatedEffectCollider / contactFlags / and | 1 | 0 | 0 | 0 | 0 | `Flags_MaskedWord` | 1 / 0 / 0 (100.0%) |
| struct RepeatedEffectCollider / responseFlags / and | 0 | 0 | 1 | 0 | 0 | `Flags_MaskedWord` | 1 / 0 / 0 (100.0%) |
| struct RoamingObjectListener / flags / and-store | 0 | 2 | 0 | 0 | 0 | `mask` | — |
| struct RoomActor / behaviorFlags / and | 4 | 42 | 26 | 0 | 0 | `ActorStatus_BehaviorFlags`, `Flags_MaskedWord`, `SceneActorBehavior_OverrideBits`, `SceneActorChase_Bits32`, `SceneActorTurn_ActorFlags`, `Vorn_ActorFlags`, `Vorn_MaskLow`, `WordFlag`, `actionMask`, `bit`, `invalidMask`, `mask`, `mask3`, `one`, `test` | 16 / 5 / 0 (76.2%) |
| struct RoomActor / behaviorFlags / and-store | 0 | 21 | 3 | 0 | 0 | `MaskContactFlags`, `SceneActorChase_AndWordFlags`, `m`, `mask` | 1 / 0 / 0 (100.0%) |
| struct RoomActor / behaviorFlags / clear | 4 | 19 | 47 | 0 | 0 | `SceneActorChase_ClearActorFlags`, `SceneActorChase_ClearBehaviorFlags`, `SceneActorChase_ClearWordFlags` | 1 / 0 / 0 (100.0%) |
| struct RoomActor / behaviorFlags / nonzero | 0 | 4 | 3 | 0 | 0 | `bit`, `enabled`, `mask` | 16 / 5 / 0 (76.2%) |
| struct RoomActor / behaviorFlags / set | 4 | 99 | 57 | 0 | 0 | `ActorStatus_SetWord`, `Golem_SetRouteFlags`, `MonsterInit_SetBehaviorFlags`, `Reaction_SetWord`, `SceneActorBehavior_SetWordFlags`, `SceneActorChase_SetActorFlags`, `SceneActorChase_SetBehaviorFlags`, `bit`, `mask` | 1 / 0 / 0 (100.0%) |
| struct RoomActor / context.captureFlags / and | 0 | 1 | 0 | 0 | 0 | `ContextFlag` | — |
| struct RoomActor / context.flags / nonzero | 0 | 1 | 0 | 0 | 0 | `Ghost_TestFlags` | — |
| struct RoomActor / context.flags / set | 0 | 0 | 1 | 0 | 0 | `Dog_SetContextFlags` | — |
| struct RoomActor / context.flags3D4 / clear | 0 | 1 | 0 | 0 | 0 | `MotionState_ClearFlags` | — |
| struct RoomActor / context.flags62C / nonzero | 0 | 1 | 0 | 0 | 0 | `HasFlag` | — |
| struct RoomActor / context.flags6D4 / nonzero | 0 | 2 | 0 | 0 | 0 | `TestContext_BeginTargetPursuit` | — |
| struct RoomActor / motionFlags / and | 5 | 106 | 41 | 0 | 0 | `Chase_MotionFlags`, `ContactActor_ReadMotionFlags`, `Mask16`, `SceneActorBehavior_MotionBits`, `SceneActorChase_QueryMotionFlags`, `SceneActorContact_MotionFlags`, `SceneActorPlacement_ReadFlags16`, `SceneActorTurn_Flags`, `SceneActor_MaskedHalfword`, `TestMotionFlag`, `Vorn_Mask16`, `activeMask`, `hm`, `m`, `mask`, `motionMode`, `partFlag`, `testMask` | 108 / 2 / 0 (98.2%) |
| struct RoomActor / motionFlags / and-store | 0 | 35 | 5 | 0 | 0 | `ClearHalf`, `ClearMotionFlags`, `HalfwordFlags_And`, `SceneActorChase_AndMotionFlags`, `SceneActorPlacement_ClearFlags`, `Slime_ClearMotionBits`, `m`, `mask`, `n` | 14 / 0 / 0 (100.0%) |
| struct RoomActor / motionFlags / clear | 0 | 21 | 39 | 0 | 0 | `ActorStatus_ClearMotionFlags`, `Chase_ClearBits`, `MimicChase_ClearBits`, `SceneActorChase_ClearBits`, `SceneActorChase_ClearMotionFlags`, `flag`, `k1`, `mask`, `motionMask` | 14 / 0 / 0 (100.0%) |
| struct RoomActor / motionFlags / nonzero | 0 | 16 | 6 | 0 | 0 | `bit`, `chasing`, `flag`, `liftFlag`, `m`, `mask` | 108 / 2 / 0 (98.2%) |
| struct RoomActor / motionFlags / set | 0 | 19 | 2 | 0 | 0 | `Golem_SetMotionFlags`, `Or16`, `SceneActorChase_SetMotionFlags`, `SetFlags`, `mask` | 6 / 0 / 0 (100.0%) |
| struct RoomActor / pendingControlFlags / and | 0 | 1 | 0 | 0 | 0 | `RotationPhase_Mask` | — |
| struct RoomActor / pendingControlFlags / and-store | 1 | 14 | 1 | 0 | 0 | `MonsterInit_MaskControlFlags`, `SceneActorChase_AndWordFlags`, `mask` | 9 / 0 / 0 (100.0%) |
| struct RoomActor / placementFlags / and | 0 | 3 | 0 | 0 | 0 | `ClearRotationFlag`, `SceneActor_MaskedHalfword`, `mask` | 2 / 0 / 0 (100.0%) |
| struct RoomActor / placementFlags / and-store | 0 | 4 | 0 | 0 | 0 | `IvyInit_ClearMotionFlags`, `mask` | — |
| struct RoomActor / placementFlags / clear | 0 | 4 | 0 | 0 | 0 | `HalfwordFlags_Clear`, `clearFlags16` | — |
| struct RoomActor / placementFlags / nonzero | 0 | 42 | 18 | 0 | 0 | `ActorStatus_BodyFlags`, `MonsterInit_TestMotionFlags`, `PlacementDamage_TestField`, `SceneActorChase_HasPlacementFlags`, `SceneActorChase_TestField`, `mask`, `movementMask` | 2 / 0 / 0 (100.0%) |
| struct RoomActor / placementFlags / set | 0 | 19 | 10 | 0 | 0 | `HalfwordFlags_Set`, `MonsterInit_SetMotionFlags`, `Or16`, `SceneActorPlacement_SetFlags` | — |
| struct RoomActor / reactionFlags / and | 0 | 3 | 0 | 0 | 0 | `ClearReaction`, `clear`, `mask` | 1 / 0 / 0 (100.0%) |
| struct RoomActor / reactionFlags / and-store | 0 | 9 | 0 | 0 | 0 | `SceneActorPlacement_ClearFlags`, `Slime_ClearPendingFlags`, `clear`, `mask` | 3 / 0 / 0 (100.0%) |
| struct RoomActor / reactionFlags / clear | 0 | 4 | 4 | 0 | 0 | `Chase_ClearBits`, `SceneActorChase_ClearBits` | 3 / 0 / 0 (100.0%) |
| struct RoomActor / reactionFlags / nonzero | 0 | 1 | 0 | 0 | 0 | `Slime_TestPendingFlag` | 1 / 0 / 0 (100.0%) |
| struct RoomActor / reactionFlags / set | 4 | 7 | 5 | 0 | 0 | `ActorStatus_SetPendingBits`, `HalfwordFlags_Set`, `SceneActorChase_OrReactionFlags`, `bit` | 2 / 0 / 0 (100.0%) |
| struct RoomActor / statusFlags / and | 1 | 34 | 8 | 0 | 0 | `ActorStatus_StatusFlags`, `Flags_MaskedWord`, `SceneActorChase_Bits32`, `SceneActorChase_StatusFlags`, `SceneActorPlacement_ReadFlags32`, `WordFlag`, `bit`, `mask`, `queryMask`, `suppressed` | 32 / 0 / 0 (100.0%) |
| struct RoomActor / statusFlags / and-store | 1 | 30 | 1 | 0 | 0 | `ClearWord`, `Reaction_ClearWord`, `SceneActorChase_AndWordFlags`, `WordFlags_And`, `clear`, `m`, `mask` | 13 / 3 / 0 (81.2%) |
| struct RoomActor / statusFlags / clear | 2 | 18 | 16 | 0 | 0 | `SceneActorChase_ClearStatusFlags`, `SceneActorChase_ClearWordFlags`, `k2` | 13 / 3 / 0 (81.2%) |
| struct RoomActor / statusFlags / nonzero | 0 | 15 | 3 | 0 | 0 | `k2`, `mask` | 32 / 0 / 0 (100.0%) |
| struct RoomActor / statusFlags / set | 1 | 114 | 14 | 0 | 0 | `ActorStatus_SetWord`, `MonsterInit_SetStatusFlags`, `SceneActorBehavior_SetWordFlags`, `SceneActorChase_SetStatusFlags`, `SetFlags`, `WordFlags_Set`, `c`, `m`, `mask`, `status` | 5 / 0 / 0 (100.0%) |
| struct S / f9c / set | 0 | 1 | 0 | 0 | 0 | `setf` | — |
| struct S / fl / and-store | 0 | 2 | 0 | 0 | 0 | `m` | — |
| struct S / flags / and-store | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct S / h0e / set | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct SabataActorState / flags / and | 0 | 0 | 2 | 0 | 0 | `mask` | 1 / 0 / 0 (100.0%) |
| struct SabataActorState / flags / and-store | 0 | 2 | 0 | 0 | 0 | `mask` | — |
| struct SabataActorState / flags / set | 0 | 17 | 6 | 0 | 0 | `WordFlags_Set`, `flags`, `mask`, `one` | 4 / 0 / 0 (100.0%) |
| struct SabataAttackActor / flags / and | 0 | 0 | 1 | 0 | 0 | `mask` | — |
| struct SabataAttackActor / flags / set | 0 | 10 | 6 | 0 | 0 | `Sabata_SetFlags` | 1 / 0 / 0 (100.0%) |
| struct SceneActor / flags / and | 0 | 6 | 0 | 0 | 0 | `ActorFlag`, `SceneActor_TestFlags`, `blockedMask` | 2 / 0 / 0 (100.0%) |
| struct SceneActor / flags / set | 0 | 12 | 4 | 0 | 0 | `SceneActor_SetFlags`, `mask` | 1 / 0 / 0 (100.0%) |
| struct SceneActorAnimationLink / flags / clear | 0 | 2 | 0 | 0 | 0 | `Vorn_ClearContextFlags` | — |
| struct SceneActorAnimationLink / flags / nonzero | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct SceneActorBehaviorContactView / flags / and | 0 | 3 | 0 | 0 | 0 | `SceneActor_MaskedHalfword`, `TestFlag` | 0 / 1 / 0 (0.0%) |
| struct SceneActorBody / flags / nonzero | 0 | 0 | 1 | 0 | 0 | `SceneBody_TestFlags` | — |
| struct SceneActorBody / flags / set | 0 | 2 | 0 | 0 | 0 | `SceneActorBody_SetFlags`, `flag` | 2 / 0 / 0 (100.0%) |
| struct SceneActorChaseFlags16 / value / and | 0 | 1 | 2 | 0 | 0 | `SceneActorChase_FlagComponentBits` | — |
| struct SceneActorChaseFlags16 / value / and-store | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct SceneActorChaseFlags16 / value / clear | 0 | 28 | 5 | 0 | 0 | `SceneActorChase_ClearFlagComponent` | — |
| struct SceneActorChaseFlags16 / value / set | 0 | 4 | 2 | 0 | 0 | `SceneActorChase_SetFlagComponent`, `bits` | — |
| struct SceneActorChasePlacement / flags / clear | 0 | 0 | 1 | 0 | 0 | `ActorStatus_ClearPlacementFlags` | — |
| struct SceneActorChasePlacement / flags / set | 0 | 0 | 3 | 0 | 0 | `ActorStatus_SetPlacementFlags`, `Golem_MarkContactPlacement`, `one` | — |
| struct SceneActorChaseTracking / flags / and | 0 | 3 | 1 | 0 | 0 | `clearMask`, `m2`, `mask` | 20 / 0 / 0 (100.0%) |
| struct SceneActorChaseTracking / flags / and-store | 0 | 12 | 3 | 0 | 0 | `ClearTargetFlags`, `Reaction_ClearMotion`, `bm`, `m`, `mask`, `trackingMask` | 10 / 0 / 0 (100.0%) |
| struct SceneActorChaseTracking / flags / clear | 0 | 1 | 4 | 0 | 0 | `Recovery_ClearTrackingFlags`, `Vorn_ClearTrackingFlags` | 10 / 0 / 0 (100.0%) |
| struct SceneActorChaseTracking / flags / nonzero | 0 | 2 | 0 | 0 | 0 | `MonsterInit_TestTargetingFlags`, `SceneActorContact_TestTracking` | 20 / 0 / 0 (100.0%) |
| struct SceneActorChaseTracking / flags / set | 0 | 5 | 3 | 0 | 0 | `SceneActorContact_SetTrackingFlags`, `Vorn_SetTrackingFlags`, `m`, `mask` | 8 / 0 / 0 (100.0%) |
| struct SceneActorControlGate / flags / clear | 0 | 13 | 0 | 0 | 0 | `SceneActorChase_GroupClearFlags`, `SceneActorRecovery_ClearGate`, `bits` | 3 / 0 / 0 (100.0%) |
| struct SceneActorControlGate / flags / set | 0 | 2 | 0 | 0 | 0 | `SetFlags`, `four` | 1 / 0 / 0 (100.0%) |
| struct SceneActorControlMotionState / flags3D4 / and | 0 | 0 | 2 | 0 | 0 | `firstMask`, `mask` | — |
| struct SceneActorControlMotionState / flags3D4 / and-store | 0 | 3 | 0 | 0 | 0 | `SceneActorChase_AndWordFlags`, `clearMask`, `mask` | — |
| struct SceneActorControlMotionState / flags3D4 / clear | 0 | 1 | 2 | 0 | 0 | `MotionState_ClearFlags` | — |
| struct SceneActorControlMotionState / flags3D4 / nonzero | 0 | 3 | 1 | 0 | 0 | `MotionState_TestFlags`, `mask` | — |
| struct SceneActorControlMotionState / flags3D4 / set | 0 | 0 | 1 | 0 | 0 | `MotionState_SetFlags` | — |
| struct SceneActorControlState / flags3D4 / and-store | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct SceneActorPlacementTileFlags / flags / set | 0 | 0 | 1 | 0 | 0 | `mask` | — |
| struct SceneActorReactionMotion / flags / and | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct SceneActorReactionMotion / flags / and-store | 0 | 3 | 0 | 0 | 0 | `mask` | — |
| struct SceneActorReactionMotion / flags / nonzero | 0 | 0 | 1 | 0 | 0 | `MotionFlag` | — |
| struct SceneActorReactionMotion / flags / set | 0 | 1 | 1 | 0 | 0 | `SetMotion_SelectReactionControl`, `SideStep_SetFlags` | — |
| struct SceneActorReactionMotionFlags / bits / and-store | 0 | 1 | 0 | 0 | 0 | `ClearHidden` | — |
| struct SceneActorReactionMotionFlags / bits / set | 0 | 12 | 6 | 0 | 0 | `SceneActorReaction_EnableMotionFlags`, `mask` | — |
| struct SceneActorRegistryManager / flags / and | 0 | 1 | 0 | 0 | 0 | `mask` | 1 / 0 / 0 (100.0%) |
| struct SceneActorRegistryManager / flags / and-store | 0 | 3 | 0 | 0 | 0 | `WordFlags_And`, `mask` | 1 / 0 / 0 (100.0%) |
| struct SceneActorRegistryManager / flags / nonzero | 0 | 3 | 7 | 0 | 0 | `HasFlag`, `TestManagerFlag`, `WordFlags_Has`, `activeFlag` | 1 / 0 / 0 (100.0%) |
| struct SceneActorRegistryManager / flags / set | 0 | 1 | 3 | 0 | 0 | `mask` | — |
| struct SceneActorRegistrySequenceDrawable / flags / and-store | 0 | 0 | 1 | 0 | 0 | `mask` | — |
| struct SceneActorRouteContext / flags628 / clear | 0 | 1 | 0 | 0 | 0 | `SceneActorChase_ClearWordFlags` | — |
| struct SceneActorRouteContext / flags628 / set | 0 | 1 | 0 | 0 | 0 | `SceneActorRoute_SetContextFlags` | — |
| struct SceneActorScriptPropertiesView / flags6DC / and | 0 | 2 | 1 | 0 | 0 | `StateFlag` | — |
| struct SceneActorScriptPropertiesView / flags6DC / nonzero | 0 | 1 | 0 | 0 | 0 | `ContextHasFlag` | — |
| struct SceneActorScriptPropertiesView / flags6DC / set | 0 | 1 | 0 | 0 | 0 | `SetContextFlags` | — |
| struct SceneCollider / contactFlags / and | 0 | 3 | 1 | 0 | 0 | `SceneCollider_ReadContactFlags` | 2 / 0 / 0 (100.0%) |
| struct SceneCollider / contactFlags2 / and | 0 | 3 | 2 | 0 | 0 | `SceneCollider_ReadReactionFlags` | 3 / 0 / 0 (100.0%) |
| struct SceneColliderEffect / motionFlags / nonzero | 0 | 0 | 2 | 0 | 0 | `TestFlag` | — |
| struct SceneHudState / actionOwner.statBonusFlags / and | 0 | 0 | 3 | 0 | 0 | `WordFlags_Test` | — |
| struct SceneHudState / actionOwner.statusFlags / and | 0 | 2 | 0 | 0 | 0 | `WordFlags_Test` | — |
| struct SceneNpc / displayFlags / set | 0 | 1 | 2 | 0 | 0 | `mask` | — |
| struct SceneNpc081CEE50 / flagsC / and | 0 | 0 | 1 | 0 | 0 | `one` | — |
| struct SceneNpc081CEE50 / flagsC / clear | 0 | 0 | 1 | 0 | 0 | `one` | — |
| struct SceneNpc081CEE50 / flagsE / set | 0 | 0 | 1 | 0 | 0 | `one` | — |
| struct SceneNpcTransitionActor / flags / set | 0 | 0 | 2 | 0 | 0 | `m` | — |
| struct SceneObject / flags / and | 0 | 0 | 4 | 0 | 0 | `SceneObject_ReadFlags` | 0 / 1 / 0 (0.0%) |
| struct ScenePoolActor / flags5ee / nonzero | 0 | 0 | 1 | 0 | 0 | `ScenePool_HasControlRestriction` | — |
| struct ScenePoolActor / linkedPlayer.flags / and | 0 | 0 | 1 | 0 | 0 | `ScenePool_TestPlayerFlags` | — |
| struct ScenePoolActor / motionFlags / and-store | 0 | 1 | 0 | 0 | 0 | `mask` | 1 / 0 / 0 (100.0%) |
| struct ScenePoolActor / stateFlags / set | 0 | 8 | 2 | 0 | 0 | `ScenePoolActor_SetStateFlags` | — |
| struct SceneProjectile / flags / and-store | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct SceneProjectile / flags / nonzero | 0 | 0 | 1 | 0 | 0 | `mask` | — |
| struct SceneRigSprite / flags / and-store | 1 | 0 | 1 | 0 | 0 | `mask` | — |
| struct Segment / flags / nonzero | 0 | 0 | 4 | 0 | 0 | `test` | — |
| struct ShopWidgetFlags / flags / set | 0 | 0 | 1 | 0 | 0 | `mask` | — |
| struct Skeleton / actionFlags / and | 0 | 4 | 1 | 0 | 0 | `bit`, `flag`, `mask` | 2 / 0 / 0 (100.0%) |
| struct Skeleton / actionFlags / and-store | 0 | 2 | 4 | 0 | 0 | `SceneActorChase_AndWordFlags`, `k`, `mask` | 1 / 0 / 0 (100.0%) |
| struct Skeleton / actionFlags / nonzero | 0 | 7 | 2 | 0 | 0 | `SkeletonInit_TestSkeletonFlags`, `TestBits`, `WordFlags_Has`, `mask` | 2 / 0 / 0 (100.0%) |
| struct Skeleton / actionFlags / set | 0 | 1 | 5 | 0 | 0 | `SkeletonInit_SetFlags`, `Skeleton_SetContextFlags`, `WordFlags_Set`, `bit`, `mask` | — |
| struct Slime / flags / and-store | 0 | 1 | 0 | 0 | 0 | `Slime_ClearPendingFlags` | — |
| struct Slime / flags / nonzero | 0 | 4 | 1 | 0 | 0 | `Slime_HasContextFlag`, `Slime_TestFlags`, `Slime_TestPendingFlag`, `Slime_TestReactionFlag` | — |
| struct Slime / flags / set | 0 | 1 | 1 | 0 | 0 | `Slime_SetFlags` | — |
| struct SpatialEntry / flags / and | 0 | 1 | 1 | 0 | 0 | `SpatialEntry_TestFlags` | — |
| struct SpatialEntry / flags / and-store | 0 | 0 | 1 | 0 | 0 | `mask` | — |
| struct SpatialEntry / flags / set | 0 | 1 | 1 | 0 | 0 | `one` | — |
| struct Spider / flags62C / and | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct Spider / flags62C / and-store | 0 | 2 | 0 | 0 | 0 | `ClearContext`, `mask` | — |
| struct Spider / flags62C / nonzero | 0 | 1 | 2 | 0 | 0 | `HasFlag_BeginPlayerPursuit`, `Test`, `four` | — |
| struct Spider / flags62C / set | 2 | 0 | 0 | 0 | 0 | `Spider_SetFlags` | — |
| struct SpriteGroupSlot / flags / set | 0 | 0 | 1 | 0 | 0 | `one` | — |
| struct SpriteGroupSprite / flags / set | 0 | 0 | 1 | 0 | 0 | `one` | — |
| struct SpriteTrail / spriteFlags / and-store | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct StateActor / actionFlags / and | 0 | 5 | 3 | 0 | 0 | `SceneActorState_TestFlags`, `mask` | 1 / 0 / 0 (100.0%) |
| struct StateActor / actionFlags / clear | 0 | 1 | 0 | 0 | 0 | `SceneActorState_ClearFlags` | 1 / 0 / 0 (100.0%) |
| struct StateActor / actionFlags / set | 3 | 25 | 15 | 0 | 0 | `SceneActorState_SetFlags`, `mask` | 1 / 0 / 0 (100.0%) |
| struct StateActor / displayFlags / set | 0 | 5 | 0 | 0 | 0 | `SetDisplayFlag`, `m`, `mask` | — |
| struct StateActorRise / flags / set | 0 | 1 | 0 | 0 | 0 | `m` | — |
| struct SwordContact / contactFlags / and | 0 | 1 | 0 | 0 | 0 | `Sword_ContactFlags` | — |
| struct SwordPhase / flags / and | 0 | 0 | 1 | 0 | 0 | `mask` | 2 / 0 / 0 (100.0%) |
| struct SwordPhase / flags / clear | 0 | 0 | 1 | 0 | 0 | `Ivy_ClearStart` | 0 / 1 / 0 (0.0%) |
| struct SwordPhase / flags / nonzero | 0 | 1 | 0 | 0 | 0 | `m` | 2 / 0 / 0 (100.0%) |
| struct SwordPhase / flags / set | 0 | 0 | 1 | 0 | 0 | `Ivy_SetFlags` | — |
| struct SwordTaskStatus / flags / and-store | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct SwordTaskStatus / flags / set | 0 | 0 | 1 | 0 | 0 | `mask` | — |
| struct T / fl / nonzero | 0 | 1 | 0 | 0 | 0 | `m` | — |
| struct TargetMotionExit / flags119 / and | 0 | 0 | 1 | 0 | 0 | `mask` | — |
| struct TargetMotionExit / flags119 / set | 0 | 0 | 1 | 0 | 0 | `TargetMotion_SetFlag` | — |
| struct Task4E01Projectile / flags / and-store | 0 | 1 | 0 | 0 | 0 | `ClearFlags` | 1 / 0 / 0 (100.0%) |
| struct Task4E01Projectile / flags / set | 0 | 2 | 13 | 0 | 0 | `bit`, `draw`, `mask`, `one`, `two` | 7 / 0 / 0 (100.0%) |
| struct TentacleActor / flags2D0 / and | 0 | 1 | 0 | 0 | 0 | `HasStatusFlag` | — |
| struct TentacleActor / motionFlags / and | 0 | 1 | 0 | 0 | 0 | `HasMotionFlag` | — |
| struct TentacleBody / attackFlags / and-store | 0 | 2 | 0 | 0 | 0 | `Tentacle_MaskTrailFlags`, `clearMask` | — |
| struct TentacleBody / attackFlags / nonzero | 0 | 3 | 0 | 0 | 0 | `Tentacle_TestTrailFlags`, `mask` | — |
| struct TentacleBody / attackFlags / set | 0 | 0 | 1 | 0 | 0 | `m` | — |
| struct TentacleSegmentAnimation / status / nonzero | 0 | 12 | 7 | 0 | 0 | `CentipedeSpread_Test`, `TentacleSegment_HasStatus`, `Tentacle_TestSegmentFlags`, `TestMask`, `mask` | — |
| struct TentacleSegmentAnimation / status / set | 0 | 0 | 1 | 0 | 0 | `setFlag` | — |
| struct TimedEffect / flags / and-store | 0 | 2 | 0 | 0 | 0 | `mask` | — |
| struct TimedEffect / flags / nonzero | 0 | 0 | 1 | 0 | 0 | `mask` | — |
| struct TurningProjectile / flags / set | 0 | 1 | 0 | 0 | 0 | `one` | — |
| struct Vanargand / flags / set | 0 | 13 | 7 | 0 | 0 | `SetFlags`, `flag`, `flags`, `mask` | — |
| struct VanargandBoss / flags / set | 0 | 0 | 9 | 0 | 0 | `Vg_SetFlags` | 1 / 0 / 0 (100.0%) |
| struct VanargandHeadActor / effectFlags / set | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct VanargandPairAttack / flags / and | 0 | 1 | 0 | 0 | 0 | `VanargandPairAttack_TestFlags` | 2 / 0 / 0 (100.0%) |
| struct VanargandPairAttack / flags / nonzero | 0 | 1 | 2 | 0 | 0 | `mask`, `two` | 2 / 0 / 0 (100.0%) |
| struct VanargandPairAttack / flags / set | 0 | 9 | 5 | 0 | 0 | `VanargandPairAttack_SetFlags`, `m`, `mask` | 11 / 0 / 0 (100.0%) |
| struct VanargandPairAttack / player.statusFlags / and | 0 | 0 | 1 | 0 | 0 | `TestFlags` | — |
| struct VanargandPresentation / flags / and | 0 | 3 | 3 | 0 | 0 | `TestActorFlag` | — |
| struct VanargandPresentation / flags / clear | 0 | 0 | 1 | 0 | 0 | `ClearActorFlag` | — |
| struct VanargandPresentation / flags / set | 0 | 0 | 1 | 0 | 0 | `mask` | — |
| struct VanargandTargets / flags / and | 0 | 2 | 0 | 0 | 0 | `TestFlags` | 1 / 0 / 0 (100.0%) |
| struct Visual / flags / and-store | 0 | 3 | 0 | 0 | 0 | `mask` | — |
| struct Vorn / flags6D4 / and | 0 | 1 | 0 | 0 | 0 | `mask` | 4 / 0 / 0 (100.0%) |
| struct Vorn / flags6D4 / and-store | 0 | 6 | 0 | 0 | 0 | `m`, `mask` | 1 / 0 / 0 (100.0%) |
| struct Vorn / flags6D4 / nonzero | 0 | 5 | 5 | 0 | 0 | `Chase_Test`, `ContextFlags`, `TestActor`, `TestContext_BeginTargetPursuit`, `Test_HasActiveResponse`, `Test_SelectHitRecovery`, `mask`, `t1` | 4 / 0 / 0 (100.0%) |
| struct VornResponseFlags / bits / set | 0 | 0 | 2 | 0 | 0 | `Vorn_SetResponseFlags` | — |
| struct WirelessGameExchange / exchangeFlags / and | 0 | 1 | 4 | 0 | 0 | `WordFlags_Test` | — |
| struct WirelessGameExchange / exchangeFlags / and-store | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct WorldMapMenu / selectionCursor.stateFlags / and | 0 | 0 | 1 | 0 | 0 | `Test` | — |
| union DisplaySequenceControl / raw / and | 6 | 1 | 152 | 0 | 0 | `active`, `bit` | — |
| union DisplaySequenceControl / raw / nonzero | 0 | 0 | 1 | 0 | 0 | `mask` | — |
| union RegistryFlagField / word / clear | 0 | 0 | 2 | 0 | 0 | `SceneActorChase_ClearWordFlags` | — |
| union RegistryFlagField / word / set | 1 | 0 | 0 | 0 | 0 | `bit` | — |

## Linter-use weights

| Verdict | Weighted uses |
| --- | ---: |
| PLAIN | 405 |
| BOUND-SINGLE | 2,665 |
| BOUND-SHARED | 3,052 |
| SKIP | 294 |
| PENDING | 0 |

## Skip reasons

| Reason | Sites |
| --- | ---: |
| helper has no safe inline specialization | 4 |
| helper has other arguments | 1 |
| mask value could not be resolved | 1 |
| no safe fold_literals --every-read proposal | 224 |
| variant compile/link failed | 4 |

## Scratch application control

The control rewrites one argument at 08081C98 and leaves the full 1,260-byte unit on MATCH. The command checks the independent form, then keeps each cumulative edit only if it matches and adds no linter or compiler warning.

```sh
mkdir -p build/plainprobe/apply-control
cp src/scene/actor/reaction/SceneActorReaction_UpdateTimedBurst.c build/plainprobe/apply-control/
build/venv/bin/python tools/plainprobe.py --apply-plain \
  build/plainprobe/apply-control/SceneActorReaction_UpdateTimedBurst.c --out build/plainprobe/apply-control
build/venv/bin/python tools/check.py build/plainprobe/apply-control/SceneActorReaction_UpdateTimedBurst.c --match-only
```


<!-- Worker interpretation below. Preserved on resume. -->

## What the direct probes establish

In the initial sweep, three local folds matched among 1,511 local bindings tested, including
two of the 1,175 tested bindings with weight one. For helper arguments, 90
of 2,292 tested sites matched. These are independent edits. Two edits that
match separately can change allocation when combined, so the apply path
checks the cumulative unit after every edit.

The load-first controls expose a limit of the single-use classification.
Fifteen of the 16 load-first observations are `gEngineFlags & 0x4800`
tests. In `SceneActorBehavior_RecordDefeat` (080789EC), the ROM loads the
word at 08078A3C, completes the mask at 08078A40, then uses the helper's
materialized boolean in a second comparison. The stock scratch rewrite
still loads the word first. It branches directly on the AND result,
changes the actor register, and reduces the unit from 348 to 336 bytes.
That mismatch reflects boolean lowering and surrounding allocation.
The other load-first observation, 081D088C, updates Otenko's display
flags after an earlier test on the same word.

The typed rows distinguish consolidation candidates by operation.
`Player.statusFlags` has several named test and set helpers and all ten
classified ROM operations build the mask first. `ContactCollider.flags`
has a different profile across operations. Nine of 11 classified tests
are mask-first, while 15 of 16 sets are load-first and 12 set probes
already match as plain expressions. These counts support reviewing the
player status family together and applying the independently verified
plain collider sets before evaluating a common accessor for the other
collider operations.

On this worktree, caching the unchanged probe inventory reduced a complete
resume from 26.19 seconds to 0.94 seconds. The unmodified source and planner
allow reuse of the 2.8 MB plan. A warmed resume reproduced the site CSV byte
for byte.

## ROM mask reuse controls

The `uses` column in `flag_sites.tsv` counts classified flag operations
sharing a mask definition and register. The initial sweep's uniquely linked `rom_matches` gave the following counts.

| Verdict | Mask-first, one classified flag use | Mask-first, shared | Load-first, one classified flag use |
| --- | ---: | ---: | ---: |
| PLAIN | 3 | 3 | 1 |
| BOUND-SINGLE | 195 | 0 | 16 |
| BOUND-SHARED | 70 | 4 | 5 |

All six mask-first PLAIN sites have other occurrences of the value in the
source function. In the PLAIN control `SceneActorMotion_Update`, the ROM
loads 1 into r3 at 0807A8E8, uses it in the preceding `enabled` byte store
at 0807A8EA, then uses the same r3 in the behavior flag test at 0807A8F4.
Replacing the test helper with a direct expression matches all 1,736 bytes.
This case still has `uses = 1` because the first consumer is a byte store.
A count of one rules out reuse among identified flag operations. Other
instructions can still share the definition.

The Otenko control at 081D088C reuses the halfword loaded at 081D0876 for
the earlier test. In the plain scratch rewrite, both the load-first order
and the 196-byte unit extent survive. agbcc puts the mask in r3 and copies
it to r0 before the OR, where the ROM builds the mask directly in r0.
That control and the boolean test at 08078A42 are two distinct ways for
a BOUND-SINGLE verdict to arise while the ROM remains load-first.

## Follow-up on skipped sites

Grouped declarations and sibling scopes accounted for 795 of the
1,070 local bindings with no earlier fold proposal. The follow-up folds
every read within the binding's scope, preserves sibling declarations,
and keeps an explicit conversion for out-of-range narrow literals.
The resume recovered 43 PLAIN, 347 BOUND-SINGLE,
and 405 BOUND-SHARED sites. The remaining SKIP count is
773. Previously tested sites kept their verdicts.

For helper bodies outside the direct-expression grammar, the probe compares
two scratch copies of the same helper. The control retains the mask argument
and must reproduce the ROM. The variant substitutes the converted literal at
every mask read. Other statements and arguments stay in place. This checks
the staging of the mask even when the helper includes other behavior.
Of 342 previously skipped helper sites, 146 were PLAIN,
68 BOUND-SINGLE, and 127 BOUND-SHARED.
One specialization still skipped. Total SKIP fell from 773 to 432.

The header pass tests a variant through scratch copies of the affected
include paths. PLAIN requires each translation unit in the include graph
to match. A mismatch in one matching dependent supplies the verdict and
the CSV names that unit. Of the 143 header sites, 28 were PLAIN,
55 BOUND-SINGLE, 54 BOUND-SHARED,
and 6 still skipped. Total SKIP fell from 432 to 295.

The pointer retry preserves the helper's formal pointer type when the
initial direct expression fails to compile. Seven source call sites
through opaque pointers gained verdicts, six BOUND-SINGLE and one
BOUND-SHARED. Total SKIP fell from 295 to 288.

A second local pass excluded the inner scope of a shadowed binding and
folded named expressions whose integer values the source resolver could
determine. It classified 54 more sites, 2 PLAIN,
38 BOUND-SINGLE and 14 BOUND-SHARED.
Total SKIP fell from 288 to 234.

Across the follow-up, 1,334 earlier skips gained verdicts. The final
inventory has 312 matching rewrites or specializations among
5,137 tested sites (6.1%). Even assigning
every remaining SKIP to PLAIN leaves at most 10.2% PLAIN under
these tested forms. This bounds the effect of the earlier skips on the
plain-rewrite rate. Recovering the author's original helper or binding
still calls for field and ROM evidence.

Of the 224 local sites with no safe fold, 74 are named formal parameters
with no local constant initializer. Other remaining sites include addressable
locals, changing bindings, and expressions whose values the source resolver
could not establish. The remaining 10 skips are reported separately by cause
in the generated table.
