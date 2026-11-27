# Plain flag and local-constant probes

Tags: #compiler #phase3 #tooling

Each row represents one linter binding or constant argument. A local probe uses the `fold_literals.py --every-read` edit for that binding. The CSV keeps the linter weight, including multiple uses charged to the same local. The sample uses a fixed seed and takes up to 40 sites per directory before the rest of the sweep.

PLAIN is a byte-identical scratch rewrite. BOUND-SINGLE is a mismatch with one use found in the containing source function. BOUND-SHARED is a mismatch with other uses listed in the CSV. SKIP records an unsupported form, inactive code or check failure. A mismatch identifies a dependency in the current reconstruction. Confirm mask order and alternative matching C before inferring an original accessor or macro.

The ROM-side scan follows recognized register provenance and omits cases it cannot attribute. Its branch-consumed tests are reported alongside both raw AND and boolean source forms. The compiler probes also cover non-flag locals.

## Sample

| Scope | PLAIN | BOUND-SINGLE | BOUND-SHARED | SKIP | Pending |
| --- | --- | --- | --- | --- | --- |
| All sites | 4 | 164 | 136 | 204 | 0 |

4 of 304 compiler-tested sites matched (1.3%).

## Overall

| Scope | PLAIN | BOUND-SINGLE | BOUND-SHARED | SKIP | Pending |
| --- | --- | --- | --- | --- | --- |
| All sites | 4 | 164 | 136 | 204 | 4863 |

4 of 304 compiler-tested sites matched (1.3%).

## Directory

| Directory | PLAIN | BOUND-SINGLE | BOUND-SHARED | SKIP | Pending |
| --- | --- | --- | --- | --- | --- |
| include | 0 | 0 | 0 | 40 | 103 |
| src/bosses | 0 | 14 | 15 | 11 | 869 |
| src/effects | 0 | 11 | 12 | 17 | 298 |
| src/engine | 0 | 11 | 16 | 7 | 0 |
| src/lib | 0 | 0 | 0 | 1 | 0 |
| src/link | 0 | 14 | 10 | 16 | 181 |
| src/minigames | 0 | 13 | 18 | 9 | 36 |
| src/monsters | 0 | 16 | 8 | 16 | 1769 |
| src/npc | 0 | 21 | 11 | 8 | 153 |
| src/objects | 0 | 7 | 9 | 24 | 226 |
| src/player | 3 | 20 | 7 | 10 | 178 |
| src/scene | 1 | 15 | 6 | 18 | 742 |
| src/script | 0 | 1 | 0 | 4 | 0 |
| src/system | 0 | 12 | 7 | 9 | 0 |
| src/ui | 0 | 9 | 17 | 14 | 308 |

## Helper

| Helper | PLAIN | BOUND-SINGLE | BOUND-SHARED | SKIP | Pending |
| --- | --- | --- | --- | --- | --- |
| ARMOR_QUEUE_PART_ACTION | 0 | 0 | 0 | 0 | 2 |
| Actor16_UpdateMovementSlowdown | 0 | 0 | 0 | 0 | 1 |
| ActorControllerMotion_ClearFlags | 0 | 0 | 0 | 0 | 1 |
| ActorController_ClearColliderFlags | 0 | 0 | 0 | 0 | 8 |
| ActorController_ClearMotionFlags | 0 | 0 | 0 | 0 | 8 |
| ActorController_SetColliderFlags | 0 | 0 | 0 | 0 | 4 |
| ActorFlag | 0 | 1 | 0 | 0 | 0 |
| ActorHalfBits | 0 | 0 | 0 | 0 | 1 |
| ActorStats_SetDamageFlags | 0 | 0 | 1 | 0 | 2 |
| ActorStats_TestDamageFlags | 0 | 0 | 0 | 0 | 1 |
| ActorStatus_BehaviorFlags | 0 | 0 | 0 | 1 | 16 |
| ActorStatus_BodyFlags | 0 | 0 | 0 | 0 | 7 |
| ActorStatus_ClearMotionFlags | 0 | 1 | 0 | 0 | 14 |
| ActorStatus_ClearPlacementFlags | 0 | 0 | 0 | 0 | 1 |
| ActorStatus_PalettePlayerFlags | 0 | 0 | 0 | 0 | 1 |
| ActorStatus_SetDrawableFlags | 0 | 0 | 0 | 1 | 20 |
| ActorStatus_SetPendingBits | 0 | 0 | 0 | 0 | 6 |
| ActorStatus_SetPlacementFlags | 0 | 0 | 1 | 0 | 0 |
| ActorStatus_SetWord | 0 | 1 | 0 | 1 | 5 |
| ActorStatus_StatusFlags | 0 | 0 | 0 | 2 | 2 |
| AnyMotion | 0 | 0 | 0 | 0 | 1 |
| Arg1 | 0 | 0 | 0 | 0 | 1 |
| Armor_ContactFlags | 0 | 0 | 0 | 0 | 1 |
| AttackFlags | 0 | 0 | 0 | 0 | 1 |
| Axe_ReadBindingFlags | 0 | 0 | 0 | 0 | 1 |
| Axe_SetEffectFlags | 0 | 0 | 0 | 0 | 2 |
| Bat_ReadDefeatMotionFlags | 0 | 0 | 0 | 0 | 1 |
| Bat_TestProbeFlags | 0 | 0 | 0 | 0 | 1 |
| Bee_ClearRouteFlags | 0 | 0 | 0 | 0 | 1 |
| Bee_HasRouteFlag | 0 | 0 | 0 | 0 | 3 |
| Bee_SetRouteFlags | 0 | 0 | 0 | 0 | 2 |
| Bg_DisableDisplay | 0 | 0 | 2 | 0 | 10 |
| Bg_EnableDisplay | 0 | 5 | 12 | 0 | 11 |
| CameraTracking_ClearRequests | 0 | 0 | 1 | 0 | 11 |
| CameraTracking_SetRequests | 0 | 0 | 0 | 0 | 3 |
| CentipedeSpread_Test | 0 | 0 | 0 | 0 | 8 |
| Chase_Bits32 | 0 | 0 | 0 | 0 | 3 |
| Chase_Bits32_1 | 0 | 0 | 0 | 0 | 1 |
| Chase_ClearBits | 0 | 0 | 0 | 0 | 17 |
| Chase_MotionFlags | 0 | 0 | 0 | 1 | 3 |
| Chase_RandomBits | 0 | 0 | 0 | 0 | 1 |
| Chase_Test | 0 | 0 | 0 | 0 | 5 |
| ClearActivity | 0 | 0 | 0 | 0 | 1 |
| ClearActorFlag | 0 | 0 | 0 | 0 | 1 |
| ClearAnimationFlags | 0 | 0 | 0 | 0 | 1 |
| ClearColliderFlag | 0 | 0 | 0 | 0 | 1 |
| ClearContext | 0 | 0 | 0 | 0 | 1 |
| ClearContext_SelectRecoveryRoute | 0 | 0 | 0 | 0 | 1 |
| ClearDamageFlag | 0 | 0 | 0 | 0 | 2 |
| ClearDrawFlags | 0 | 0 | 0 | 0 | 1 |
| ClearFlag | 0 | 0 | 0 | 0 | 5 |
| ClearFlags | 0 | 2 | 1 | 0 | 11 |
| ClearHalf | 0 | 0 | 0 | 0 | 5 |
| ClearHalfFlags | 0 | 0 | 0 | 0 | 2 |
| ClearHidden | 0 | 0 | 0 | 0 | 1 |
| ClearHit | 0 | 0 | 0 | 0 | 2 |
| ClearHitFlags | 0 | 0 | 0 | 0 | 1 |
| ClearMaskedValue | 0 | 0 | 0 | 1 | 0 |
| ClearMotionFlags | 0 | 0 | 0 | 0 | 1 |
| ClearReaction | 0 | 0 | 0 | 0 | 1 |
| ClearRotationFlag | 0 | 0 | 0 | 0 | 1 |
| ClearTargetFlags | 0 | 0 | 0 | 0 | 1 |
| ClearWord | 0 | 0 | 0 | 0 | 1 |
| ClearWordValue | 0 | 0 | 0 | 0 | 1 |
| Clone_MaskColliderFlags | 0 | 0 | 1 | 0 | 1 |
| CoffinBike_ActorFlags | 0 | 0 | 0 | 1 | 1 |
| CoffinBike_ClearDamageFlag | 0 | 0 | 0 | 0 | 1 |
| CoffinBike_ReadMode | 0 | 0 | 0 | 1 | 0 |
| CommandFlip | 0 | 0 | 0 | 0 | 1 |
| Command_SetMotionFlag | 0 | 0 | 0 | 0 | 1 |
| ContactActor_ReadMotionFlags | 0 | 0 | 0 | 0 | 1 |
| ContactCollider_ClearFlags | 0 | 0 | 0 | 0 | 25 |
| ContactCollider_ClearReactionFlags | 0 | 0 | 0 | 1 | 9 |
| ContactCollider_SetFlags | 0 | 0 | 1 | 1 | 14 |
| ContactCollider_SetReactionFlags | 0 | 1 | 0 | 0 | 22 |
| ContactCollider_TestContactFlags | 0 | 1 | 0 | 0 | 2 |
| ContactCollider_TestFlags | 0 | 2 | 0 | 0 | 4 |
| ContactCollider_TestReactionFlags | 1 | 0 | 0 | 0 | 26 |
| ContextFlag | 0 | 0 | 0 | 0 | 1 |
| ContextFlags | 0 | 0 | 0 | 0 | 3 |
| ContextHasFlag | 0 | 0 | 0 | 0 | 1 |
| Crow_ClearFlags | 0 | 0 | 0 | 0 | 1 |
| Crow_SetFlags | 0 | 0 | 0 | 0 | 1 |
| Crow_TestAnimationFlags | 0 | 0 | 0 | 0 | 1 |
| Dog_ClearAttackFlags | 0 | 0 | 0 | 0 | 1 |
| Dog_HasAwarenessFlag | 0 | 0 | 0 | 0 | 4 |
| Dog_ScaleMagnitude | 0 | 0 | 0 | 0 | 2 |
| Dog_SetContextFlags | 0 | 0 | 0 | 0 | 1 |
| Dog_TestApproachFlag | 0 | 0 | 0 | 0 | 1 |
| EnableFlags | 0 | 0 | 0 | 0 | 3 |
| Engine_ClearPauseFlags | 0 | 0 | 2 | 0 | 4 |
| Engine_SetPauseFlags | 0 | 2 | 0 | 0 | 9 |
| Entry_TestFlags | 0 | 0 | 0 | 0 | 5 |
| FeatherBlockedSlots | 0 | 0 | 0 | 0 | 1 |
| Flag | 0 | 0 | 0 | 0 | 8 |
| FlagsSave | 0 | 0 | 0 | 0 | 2 |
| Flags_MaskedWord | 0 | 1 | 0 | 1 | 23 |
| GameState_SetWorldFlags | 0 | 0 | 0 | 0 | 2 |
| GameState_TestWorldFlags | 0 | 4 | 0 | 0 | 21 |
| GetFlags | 0 | 0 | 0 | 0 | 1 |
| GetReactionFlags | 0 | 0 | 0 | 0 | 2 |
| Ghost_ClearFlags | 0 | 0 | 0 | 0 | 3 |
| Ghost_ClearMotionFlags | 0 | 0 | 0 | 0 | 1 |
| Ghost_FirstLoopRandom | 0 | 0 | 0 | 0 | 2 |
| Ghost_SetFlags | 0 | 0 | 1 | 0 | 7 |
| Ghost_TestFlags | 0 | 0 | 0 | 0 | 9 |
| Ghost_TestRouteFlags | 0 | 0 | 0 | 0 | 3 |
| Ghoul_ClearDiscFlags | 0 | 0 | 0 | 0 | 3 |
| Ghoul_SetDiscFlags | 0 | 0 | 0 | 0 | 2 |
| Ghoul_TestDiscFlags | 0 | 0 | 0 | 0 | 1 |
| Ghoul_TestPendingRoute | 0 | 0 | 0 | 0 | 1 |
| GolemReaction_SetBodyFlags | 0 | 0 | 0 | 0 | 1 |
| GolemSlide_ClearColliderFlags | 0 | 0 | 0 | 0 | 1 |
| Golem_MarkContactPlacement | 0 | 0 | 0 | 0 | 1 |
| Golem_ScaleMagnitude | 0 | 0 | 0 | 0 | 2 |
| Golem_SetMotionFlags | 0 | 1 | 0 | 0 | 6 |
| Golem_SetRouteFlags | 0 | 0 | 0 | 0 | 1 |
| GridOption | 0 | 0 | 0 | 1 | 3 |
| HalfwordFlags_And | 0 | 1 | 0 | 0 | 7 |
| HalfwordFlags_Clear | 0 | 0 | 0 | 0 | 5 |
| HalfwordFlags_Set | 0 | 0 | 0 | 0 | 9 |
| HalfwordFlags_Test | 0 | 0 | 0 | 0 | 2 |
| HasBit | 0 | 0 | 0 | 1 | 1 |
| HasFlag | 0 | 0 | 0 | 0 | 5 |
| HasFlag_BeginNearbyPlayerContact | 0 | 0 | 0 | 0 | 1 |
| HasFlag_BeginPlayerPursuit | 0 | 0 | 0 | 0 | 1 |
| HasFlag_SelectPlayerApproach | 0 | 0 | 0 | 0 | 1 |
| HasMotionFlag | 0 | 0 | 0 | 0 | 2 |
| HasPlayerEvent | 0 | 0 | 0 | 0 | 1 |
| HasStatusFlag | 0 | 0 | 0 | 0 | 1 |
| HasTargetFlag | 0 | 0 | 0 | 0 | 1 |
| HitSpark_NextIndex | 0 | 0 | 0 | 1 | 6 |
| Hresvelgr_SetFlags | 0 | 3 | 0 | 0 | 27 |
| Hresvelgr_TestFlags | 0 | 0 | 0 | 0 | 9 |
| IvyInit_ClearMotionFlags | 0 | 0 | 0 | 0 | 1 |
| Ivy_ClearHalf | 0 | 0 | 0 | 0 | 1 |
| Ivy_ClearStart | 0 | 0 | 0 | 0 | 1 |
| Ivy_ClearWord | 0 | 0 | 0 | 0 | 1 |
| Ivy_SetFlags | 0 | 0 | 0 | 0 | 1 |
| LinkAura_SetFlags | 0 | 0 | 1 | 0 | 3 |
| LinkReady_HasMode | 0 | 0 | 0 | 0 | 2 |
| LinkSelection_IsChoice | 0 | 0 | 0 | 0 | 1 |
| LinkSelection_IsReady | 0 | 0 | 0 | 1 | 2 |
| LinkSelection_IsSelected | 0 | 0 | 0 | 0 | 1 |
| LinkSelection_IsStart | 0 | 0 | 0 | 1 | 0 |
| LootDeposit_TestPlayerFlag | 0 | 0 | 0 | 0 | 1 |
| Lun_ClearReactionFlags | 0 | 0 | 0 | 0 | 1 |
| Lun_SetContactFlag | 0 | 0 | 0 | 0 | 1 |
| MarkHitFlag | 0 | 0 | 0 | 0 | 1 |
| Mask | 0 | 0 | 0 | 0 | 4 |
| Mask16 | 0 | 0 | 0 | 0 | 2 |
| MaskContactFlags | 0 | 0 | 0 | 0 | 1 |
| MaskKeys | 0 | 0 | 0 | 0 | 1 |
| MaskedMessageFlags | 0 | 0 | 0 | 0 | 1 |
| MemberHasBit | 0 | 0 | 0 | 0 | 1 |
| MimicChase_ClearBits | 0 | 0 | 0 | 0 | 2 |
| ModifierMenu_MarkDirty | 0 | 0 | 0 | 0 | 3 |
| MonsterInit_MaskColliderFlags | 0 | 0 | 0 | 0 | 22 |
| MonsterInit_MaskControlFlags | 0 | 0 | 0 | 1 | 0 |
| MonsterInit_SetBehaviorFlags | 0 | 0 | 0 | 1 | 32 |
| MonsterInit_SetMotionFlags | 0 | 0 | 0 | 0 | 21 |
| MonsterInit_SetStatusFlags | 0 | 1 | 0 | 2 | 101 |
| MonsterInit_TestMotionFlags | 0 | 0 | 0 | 0 | 44 |
| MonsterInit_TestTargetingFlags | 0 | 0 | 0 | 0 | 1 |
| MotionFlag | 0 | 0 | 0 | 0 | 1 |
| MotionFlagSet | 0 | 0 | 0 | 0 | 2 |
| MotionFlags | 0 | 0 | 0 | 0 | 8 |
| MotionState_ClearFlags | 0 | 1 | 0 | 0 | 4 |
| MotionState_SetFlags | 0 | 0 | 0 | 0 | 1 |
| MotionState_TestFlags | 0 | 0 | 1 | 0 | 1 |
| MovingPlatform_DisableFlags | 0 | 0 | 3 | 0 | 8 |
| MovingPlatform_EnableFlags | 0 | 0 | 2 | 0 | 17 |
| Mummy_BeginContactTracking | 0 | 0 | 0 | 0 | 1 |
| Mummy_ClearCheckpointFlags | 0 | 0 | 0 | 0 | 1 |
| Mummy_LoadContactLatch | 0 | 0 | 0 | 0 | 1 |
| Mummy_TestContext | 0 | 0 | 0 | 0 | 2 |
| Mummy_TestContextOwned | 0 | 0 | 0 | 0 | 1 |
| NamedObjectFlags_Add | 0 | 0 | 0 | 0 | 1 |
| NamedObjectFlags_Has | 0 | 0 | 0 | 5 | 16 |
| NamedObjectFlags_Remove | 0 | 0 | 0 | 0 | 1 |
| NidhoggrDuel_AddFlags | 0 | 0 | 2 | 0 | 25 |
| NidhoggrDuel_ClearFlags | 0 | 0 | 0 | 0 | 1 |
| NidhoggrDuel_TestFlags | 0 | 0 | 2 | 0 | 5 |
| Nidhoggr_AddTargetFlags | 0 | 0 | 0 | 0 | 1 |
| Nidhoggr_MaskPartFlags | 0 | 0 | 0 | 0 | 4 |
| Nidhoggr_SetPartExtraFlags | 0 | 0 | 0 | 0 | 6 |
| Octopus_EnableDefeatMotion | 0 | 0 | 0 | 0 | 1 |
| Or16 | 0 | 0 | 0 | 0 | 5 |
| OrV | 0 | 0 | 0 | 1 | 0 |
| OwnerWidgetsEnabled | 0 | 0 | 0 | 0 | 1 |
| PackKeys | 0 | 0 | 0 | 2 | 2 |
| PackPaletteGreen | 0 | 0 | 0 | 1 | 2 |
| PhaseObject_SetFlags | 0 | 0 | 0 | 0 | 1 |
| PhaseObject_TakeEntry | 0 | 0 | 0 | 0 | 7 |
| PlacementDamage_SetHitFlags | 0 | 0 | 0 | 0 | 1 |
| PlacementDamage_TestField | 0 | 0 | 0 | 0 | 1 |
| PlayStats_TakeFlag | 0 | 0 | 0 | 0 | 10 |
| PlayerCanMove | 0 | 0 | 0 | 0 | 2 |
| PlayerFlags | 0 | 0 | 0 | 0 | 1 |
| PlayerHasFlag | 0 | 0 | 0 | 0 | 1 |
| PlayerMask | 0 | 0 | 0 | 0 | 1 |
| PlayerModifier_TestTargetFlags | 2 | 1 | 0 | 0 | 7 |
| PlayerMotion_ClearAnimationFlags | 0 | 0 | 0 | 0 | 1 |
| PlayerScript_TestFlags | 0 | 0 | 0 | 0 | 1 |
| PlayerSlot_ReadStatus | 0 | 0 | 0 | 0 | 31 |
| Player_AddStatusFlags | 0 | 5 | 0 | 0 | 27 |
| Player_ReadStatusBits | 0 | 6 | 4 | 0 | 55 |
| Player_SetStatusBits | 0 | 4 | 1 | 0 | 16 |
| PooledActor_SetFlags | 0 | 0 | 0 | 0 | 4 |
| PooledActor_TestFlags | 0 | 0 | 0 | 0 | 3 |
| RandomBits | 0 | 0 | 0 | 0 | 2 |
| RandomFirst | 0 | 0 | 0 | 0 | 2 |
| Ratatosk_SetFlags | 0 | 1 | 0 | 0 | 38 |
| Ratatosk_TestFlags | 0 | 0 | 0 | 0 | 2 |
| Reaction_ClearHalf | 0 | 0 | 0 | 0 | 1 |
| Reaction_ClearMotion | 0 | 0 | 0 | 0 | 2 |
| Reaction_ClearWord | 1 | 0 | 0 | 0 | 2 |
| Reaction_SetWord | 0 | 0 | 0 | 0 | 1 |
| Recoil_TestDamageFlags | 0 | 0 | 0 | 1 | 0 |
| RecoveryRoute_AdvanceRandomCursor | 0 | 0 | 0 | 0 | 1 |
| Recovery_ClearTrackingFlags | 0 | 0 | 0 | 0 | 2 |
| RegionActor_HasBehavior | 0 | 0 | 0 | 0 | 11 |
| RegistryActor_SetMotionFlags | 0 | 0 | 0 | 0 | 2 |
| RegistryActor_SetUpdateFlags | 0 | 0 | 0 | 0 | 4 |
| RegistryActor_TestDamageFlags | 0 | 0 | 0 | 1 | 0 |
| RegistryActor_TestMotionFlags | 0 | 0 | 0 | 0 | 1 |
| RotationPhase_Mask | 0 | 0 | 0 | 0 | 2 |
| Sabata_SetFlags | 0 | 1 | 0 | 0 | 15 |
| SaveControlFlags | 0 | 0 | 0 | 0 | 1 |
| SaveFlags | 0 | 0 | 0 | 0 | 1 |
| ScalarApproach_ApplyDelta | 0 | 0 | 0 | 0 | 3 |
| Scale | 0 | 0 | 0 | 0 | 2 |
| ScaleTowardZero | 0 | 0 | 0 | 0 | 1 |
| ScaleTowardZeroResult | 0 | 0 | 0 | 0 | 1 |
| SceneActorAttachment_MotionBits | 0 | 0 | 0 | 0 | 2 |
| SceneActorAttachment_StatusBits | 0 | 0 | 0 | 0 | 4 |
| SceneActorBehavior_MotionBits | 0 | 0 | 0 | 0 | 6 |
| SceneActorBehavior_OverrideBits | 0 | 1 | 0 | 0 | 6 |
| SceneActorBehavior_SetWordFlags | 0 | 0 | 0 | 0 | 3 |
| SceneActorBody_SetFlags | 0 | 1 | 0 | 0 | 0 |
| SceneActorChase_AndMotionFlags | 0 | 0 | 0 | 0 | 1 |
| SceneActorChase_AndWordFlags | 0 | 1 | 0 | 0 | 33 |
| SceneActorChase_Bits32 | 0 | 1 | 0 | 0 | 38 |
| SceneActorChase_ClearActorFlags | 0 | 0 | 0 | 1 | 10 |
| SceneActorChase_ClearBehaviorFlags | 0 | 0 | 0 | 0 | 1 |
| SceneActorChase_ClearBits | 0 | 0 | 2 | 0 | 37 |
| SceneActorChase_ClearFlagComponent | 0 | 0 | 0 | 1 | 38 |
| SceneActorChase_ClearMotionFlags | 0 | 0 | 0 | 1 | 15 |
| SceneActorChase_ClearStatusFlags | 0 | 0 | 0 | 0 | 5 |
| SceneActorChase_ClearWordFlags | 0 | 0 | 0 | 4 | 99 |
| SceneActorChase_FlagComponentBits | 0 | 0 | 0 | 0 | 5 |
| SceneActorChase_GroupClearFlags | 0 | 0 | 0 | 0 | 3 |
| SceneActorChase_HasPlacementFlags | 0 | 0 | 0 | 0 | 2 |
| SceneActorChase_NextSample | 0 | 0 | 0 | 0 | 2 |
| SceneActorChase_OrReactionFlags | 0 | 0 | 0 | 0 | 4 |
| SceneActorChase_QueryMotionFlags | 0 | 1 | 1 | 0 | 49 |
| SceneActorChase_SetActorFlags | 0 | 0 | 0 | 2 | 89 |
| SceneActorChase_SetBehaviorFlags | 0 | 0 | 0 | 0 | 5 |
| SceneActorChase_SetFlagComponent | 0 | 0 | 0 | 0 | 14 |
| SceneActorChase_SetMotionFlags | 0 | 0 | 0 | 0 | 7 |
| SceneActorChase_SetStatusFlags | 0 | 0 | 0 | 0 | 4 |
| SceneActorChase_StatusFlags | 0 | 1 | 0 | 0 | 14 |
| SceneActorChase_Test | 0 | 0 | 0 | 4 | 42 |
| SceneActorChase_TestField | 0 | 3 | 1 | 0 | 27 |
| SceneActorContact_MotionFlags | 0 | 0 | 0 | 0 | 32 |
| SceneActorContact_QueryMotionField | 0 | 0 | 0 | 0 | 4 |
| SceneActorContact_ReadActorFlags | 0 | 0 | 0 | 0 | 4 |
| SceneActorContact_SetTrackingFlags | 0 | 0 | 0 | 0 | 3 |
| SceneActorContact_TestTracking | 0 | 0 | 0 | 0 | 1 |
| SceneActorContact_TestTrackingOwned | 0 | 0 | 0 | 0 | 2 |
| SceneActorControl_TestFlags | 0 | 0 | 0 | 0 | 19 |
| SceneActorDamage_OwnershipBits | 0 | 0 | 0 | 0 | 1 |
| SceneActorDamage_TestFlags | 0 | 0 | 0 | 0 | 3 |
| SceneActorMotion_TestStatus | 0 | 0 | 0 | 0 | 1 |
| SceneActorPlacement_ClearColliderFlags | 0 | 0 | 0 | 0 | 1 |
| SceneActorPlacement_ClearFlags | 0 | 2 | 0 | 0 | 13 |
| SceneActorPlacement_ReadFlags16 | 0 | 0 | 0 | 1 | 4 |
| SceneActorPlacement_ReadFlags32 | 0 | 0 | 0 | 0 | 6 |
| SceneActorPlacement_SetFlags | 0 | 0 | 0 | 0 | 4 |
| SceneActorPlacement_TestFlags | 0 | 0 | 0 | 0 | 10 |
| SceneActorPlacement_TestMask | 0 | 0 | 0 | 0 | 2 |
| SceneActorPlacement_TestValue | 0 | 0 | 0 | 0 | 1 |
| SceneActorReaction_EnableMotionFlags | 0 | 0 | 0 | 0 | 15 |
| SceneActorReaction_ReadFlags | 0 | 0 | 0 | 0 | 1 |
| SceneActorRecovery_ClearGate | 0 | 0 | 0 | 0 | 1 |
| SceneActorRoute_SetContextFlags | 0 | 0 | 0 | 0 | 1 |
| SceneActorState_ClearFlags | 0 | 0 | 0 | 0 | 1 |
| SceneActorState_SetFlags | 0 | 1 | 2 | 0 | 47 |
| SceneActorState_TestFlags | 0 | 0 | 0 | 0 | 16 |
| SceneActorState_TestPlayerFlags | 0 | 0 | 0 | 0 | 1 |
| SceneActorStimulus_CaptureMotionFlags | 0 | 0 | 0 | 0 | 1 |
| SceneActorTurn_ActorFlags | 0 | 1 | 0 | 1 | 30 |
| SceneActorTurn_Flags | 0 | 0 | 1 | 0 | 35 |
| SceneActor_MaskedHalfword | 0 | 0 | 0 | 0 | 12 |
| SceneActor_SetFlags | 0 | 3 | 0 | 0 | 9 |
| SceneActor_TestFlags | 0 | 1 | 0 | 0 | 3 |
| SceneActor_TestPendingFlag | 0 | 0 | 0 | 0 | 3 |
| SceneActors_SetDamageEffectFlags | 0 | 0 | 0 | 0 | 2 |
| SceneBody_TestFlags | 0 | 0 | 0 | 0 | 1 |
| SceneCollider_ReadContactFlags | 0 | 0 | 0 | 0 | 4 |
| SceneCollider_ReadReactionFlags | 0 | 2 | 0 | 0 | 3 |
| SceneEffectSchedule_TestOccupied | 0 | 0 | 0 | 0 | 1 |
| SceneHud_ClearMarkerSlots | 0 | 0 | 0 | 0 | 1 |
| SceneObject_ReadFlags | 0 | 0 | 2 | 0 | 2 |
| ScenePoolActor_SetStateFlags | 0 | 3 | 1 | 0 | 6 |
| ScenePool_HasControlRestriction | 0 | 0 | 0 | 0 | 1 |
| ScenePool_PlayerHasFlag | 0 | 0 | 0 | 1 | 6 |
| ScenePool_TestPlayerFlags | 0 | 0 | 0 | 0 | 1 |
| SceneStatusIndicator_IsAllocated | 0 | 0 | 0 | 0 | 1 |
| SegmentFlagSet | 0 | 0 | 0 | 0 | 1 |
| Set32 | 0 | 0 | 0 | 0 | 1 |
| SetAnimationFlags | 0 | 0 | 0 | 0 | 1 |
| SetAttachmentFlags | 0 | 0 | 0 | 0 | 1 |
| SetContextFlags | 0 | 0 | 0 | 0 | 1 |
| SetDigitVisibility | 0 | 0 | 0 | 0 | 3 |
| SetDisplayFlag | 0 | 0 | 0 | 0 | 2 |
| SetFlag | 0 | 0 | 0 | 0 | 4 |
| SetFlag_UpdatePlayerDestinationPhase | 0 | 0 | 0 | 0 | 1 |
| SetFlags | 0 | 1 | 0 | 1 | 14 |
| SetHit | 0 | 0 | 0 | 0 | 1 |
| SetHitFlags | 0 | 0 | 0 | 0 | 4 |
| SetMotion_SelectReactionControl | 0 | 0 | 0 | 0 | 1 |
| SetRegionFlag | 0 | 0 | 0 | 0 | 1 |
| Shift | 0 | 0 | 0 | 1 | 6 |
| ShiftRandom | 0 | 0 | 0 | 1 | 2 |
| ShiftTowardZero | 0 | 0 | 0 | 1 | 6 |
| SideStep_SetFlags | 0 | 0 | 0 | 0 | 1 |
| SignedShift | 0 | 0 | 0 | 0 | 8 |
| SkeletonInit_SetFlags | 0 | 0 | 0 | 0 | 2 |
| SkeletonInit_TestSkeletonFlags | 0 | 0 | 0 | 0 | 2 |
| Skeleton_SetContextFlags | 0 | 0 | 0 | 0 | 1 |
| Slime_ClearMotionBits | 0 | 0 | 0 | 0 | 1 |
| Slime_ClearPendingFlags | 0 | 0 | 0 | 0 | 2 |
| Slime_ClearReactionBits | 0 | 0 | 0 | 0 | 2 |
| Slime_ContactFlags | 0 | 0 | 0 | 0 | 1 |
| Slime_EnableReactionMotion | 0 | 0 | 0 | 0 | 1 |
| Slime_HasContextFlag | 0 | 0 | 0 | 0 | 2 |
| Slime_MaskedDamage | 0 | 0 | 0 | 0 | 1 |
| Slime_SetFlags | 0 | 0 | 0 | 0 | 2 |
| Slime_TestFlags | 0 | 0 | 0 | 0 | 1 |
| Slime_TestPendingFlag | 0 | 0 | 0 | 0 | 2 |
| Slime_TestReactionFlag | 0 | 0 | 0 | 0 | 1 |
| SparkExplosion_IsOccupied | 0 | 0 | 0 | 0 | 1 |
| SpatialEntry_TestFlags | 0 | 0 | 0 | 0 | 2 |
| Spider_ClearContextFlags | 0 | 0 | 0 | 0 | 1 |
| Spider_ClearMotionFlags | 0 | 0 | 0 | 0 | 1 |
| Spider_SetFlags | 0 | 0 | 0 | 0 | 2 |
| StagedEffect_AddOuterSpriteFlags | 0 | 0 | 0 | 0 | 2 |
| StateFlag | 0 | 0 | 0 | 0 | 3 |
| Sword_ContactFlags | 0 | 0 | 0 | 0 | 1 |
| TargetMotion_SetFlag | 0 | 0 | 0 | 0 | 1 |
| TentacleSegment_HasStatus | 0 | 0 | 0 | 0 | 4 |
| Tentacle_MaskTrailFlags | 0 | 0 | 0 | 0 | 1 |
| Tentacle_MaskWaveFlags | 0 | 0 | 0 | 0 | 1 |
| Tentacle_Shift | 0 | 0 | 0 | 0 | 2 |
| Tentacle_ShiftTowardZero | 0 | 0 | 0 | 0 | 2 |
| Tentacle_TestSegmentFlags | 0 | 0 | 0 | 0 | 2 |
| Tentacle_TestTrailFlags | 0 | 0 | 0 | 0 | 1 |
| Tentacle_TestWaveFlags | 0 | 0 | 1 | 0 | 0 |
| Test | 0 | 1 | 1 | 1 | 19 |
| TestActor | 0 | 0 | 0 | 0 | 1 |
| TestActorFlag | 0 | 0 | 0 | 0 | 6 |
| TestActorFlags | 0 | 1 | 0 | 0 | 0 |
| TestBits | 0 | 0 | 0 | 0 | 2 |
| TestColliderFlags | 0 | 0 | 0 | 0 | 1 |
| TestContext | 0 | 0 | 0 | 0 | 3 |
| TestContext_BeginTargetPursuit | 0 | 0 | 0 | 1 | 2 |
| TestFalling | 0 | 0 | 0 | 0 | 1 |
| TestField | 0 | 0 | 0 | 0 | 2 |
| TestFlag | 0 | 0 | 1 | 0 | 2 |
| TestFlags | 0 | 2 | 0 | 0 | 11 |
| TestGlobal | 0 | 0 | 0 | 0 | 2 |
| TestManagerFlag | 0 | 0 | 0 | 0 | 3 |
| TestMask | 0 | 1 | 0 | 0 | 8 |
| TestModeFlags | 0 | 0 | 0 | 0 | 1 |
| TestMotionFlag | 0 | 0 | 0 | 0 | 1 |
| TestPlayerFlag | 0 | 0 | 0 | 0 | 1 |
| TestStateFlags | 0 | 0 | 0 | 0 | 1 |
| TestV | 0 | 0 | 0 | 0 | 2 |
| Test_HasActiveResponse | 0 | 0 | 0 | 0 | 1 |
| Test_SelectHitRecovery | 0 | 1 | 0 | 0 | 0 |
| TruncateShift | 0 | 0 | 0 | 0 | 7 |
| Tst | 0 | 0 | 0 | 1 | 0 |
| Tst32 | 0 | 0 | 0 | 0 | 2 |
| VanargandHead_IncreaseDuration | 0 | 0 | 0 | 0 | 4 |
| VanargandPairAttack_SetFlags | 0 | 0 | 0 | 0 | 12 |
| VanargandPairAttack_TestFlags | 0 | 0 | 0 | 0 | 1 |
| Vanargand_GetModeShifted | 0 | 0 | 0 | 0 | 1 |
| Vanargand_MaskedFlags | 0 | 0 | 0 | 1 | 1 |
| Vanargand_SetFlags | 0 | 1 | 1 | 0 | 28 |
| Vanargand_TruncateShift | 0 | 0 | 0 | 1 | 12 |
| Vg_SetFlags | 0 | 0 | 0 | 0 | 9 |
| Vg_ShiftTowardZero | 0 | 0 | 0 | 0 | 4 |
| Vorn_ActorFlags | 0 | 0 | 0 | 0 | 1 |
| Vorn_AdjustContextFlags | 0 | 0 | 0 | 0 | 1 |
| Vorn_AdjustMotionFlags | 0 | 0 | 0 | 0 | 1 |
| Vorn_ClearContextFlags | 0 | 0 | 0 | 0 | 4 |
| Vorn_ClearTrackingFlags | 0 | 0 | 0 | 0 | 3 |
| Vorn_GetRandomBits | 0 | 0 | 0 | 0 | 2 |
| Vorn_GetRestorableMotion | 0 | 0 | 0 | 0 | 1 |
| Vorn_HasPursuitFlag | 0 | 0 | 0 | 0 | 1 |
| Vorn_Mask16 | 0 | 0 | 0 | 0 | 2 |
| Vorn_MaskLow | 0 | 0 | 0 | 0 | 1 |
| Vorn_SetResponseFlags | 0 | 0 | 0 | 0 | 2 |
| Vorn_SetTrackingFlags | 0 | 0 | 0 | 0 | 3 |
| Vorn_TestBody | 0 | 0 | 0 | 0 | 3 |
| Vorn_TestContext | 0 | 0 | 0 | 0 | 1 |
| Vorn_TestNear | 0 | 0 | 0 | 0 | 3 |
| Vorn_TestResponseFlags | 0 | 0 | 0 | 0 | 2 |
| Weather_ClearDisplayFlag | 0 | 1 | 0 | 0 | 0 |
| WordFlag | 0 | 0 | 0 | 0 | 2 |
| WordFlags_And | 0 | 0 | 0 | 0 | 2 |
| WordFlags_Has | 0 | 0 | 0 | 0 | 11 |
| WordFlags_Set | 0 | 1 | 0 | 0 | 16 |
| WordFlags_Test | 0 | 1 | 4 | 0 | 18 |
| addCategory | 0 | 0 | 1 | 0 | 5 |
| addFlags | 0 | 0 | 0 | 0 | 5 |
| andm | 0 | 0 | 0 | 0 | 2 |
| any | 0 | 0 | 0 | 0 | 1 |
| clearBits16 | 0 | 0 | 0 | 0 | 3 |
| clearColliderFlags | 0 | 0 | 0 | 0 | 2 |
| clearDrawFlags | 0 | 1 | 0 | 0 | 0 |
| clearFlags | 0 | 0 | 0 | 0 | 2 |
| clearFlags16 | 0 | 0 | 0 | 0 | 1 |
| flag | 0 | 0 | 0 | 0 | 2 |
| flags | 0 | 0 | 1 | 0 | 14 |
| globalFlags | 0 | 0 | 0 | 0 | 1 |
| hasControllerFlag | 0 | 0 | 0 | 0 | 1 |
| hasFlag | 0 | 0 | 0 | 0 | 1 |
| hitFlags | 0 | 0 | 0 | 0 | 1 |
| maskIE | 0 | 0 | 0 | 1 | 0 |
| masked | 0 | 0 | 0 | 0 | 1 |
| orm | 0 | 0 | 0 | 0 | 1 |
| orr | 0 | 0 | 0 | 0 | 4 |
| playerFlags | 0 | 0 | 0 | 0 | 1 |
| scale | 0 | 0 | 0 | 0 | 5 |
| setFlag | 0 | 0 | 0 | 0 | 1 |
| setFlags | 0 | 0 | 0 | 0 | 2 |
| setf | 0 | 0 | 0 | 0 | 1 |
| shift | 0 | 0 | 0 | 1 | 9 |
| test | 0 | 0 | 0 | 0 | 15 |
| testFlag | 0 | 0 | 0 | 2 | 0 |
| testFlags | 0 | 0 | 0 | 0 | 1 |
| testFlagsP | 0 | 0 | 0 | 0 | 3 |
| testReaction | 0 | 0 | 0 | 0 | 2 |
| testRequest | 0 | 0 | 0 | 0 | 1 |
| truncateShift | 0 | 0 | 0 | 2 | 6 |
| tst | 0 | 0 | 0 | 0 | 1 |

## Typed field and operation

The ROM share is mask-first among classified mask-first and load-first operations. Interleaved observations are shown separately. A dash means the ROM scan has no classified observations for that row.

| Type / field / operation | PLAIN | BOUND-SINGLE | BOUND-SHARED | SKIP | Pending | Helper and local spellings | ROM mask / load / interleaved |
| --- | --- | --- | --- | --- | --- | --- | --- |
| (global) / gDispCntShadow / and | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| (global) / gDispCntShadow / and-store | 0 | 2 | 0 | 0 | 0 | `Weather_ClearDisplayFlag`, `mask` | — |
| (global) / gDispCntShadow / clear | 0 | 0 | 2 | 0 | 13 | `Bg_DisableDisplay`, `mask`, `visible` | — |
| (global) / gDispCntShadow / set | 0 | 5 | 12 | 0 | 15 | `Bg_EnableDisplay`, `Or16`, `bgEnable` | — |
| (global) / gEngineFlags / and-store | 0 | 0 | 0 | 0 | 2 | `SceneActorChase_AndWordFlags` | 2 / 5 / 1 (28.6%) |
| (global) / gEnginePauseFlags / and | 0 | 0 | 0 | 0 | 1 | `Mask` | 0 / 32 / 0 (0.0%) |
| (global) / gEnginePauseFlags / and-store | 0 | 1 | 0 | 0 | 2 | `SceneActorChase_AndWordFlags`, `andm`, `mask` | 10 / 1 / 0 (90.9%) |
| (global) / gEnginePauseFlags / clear | 0 | 0 | 2 | 0 | 4 | `Engine_ClearPauseFlags` | — |
| (global) / gEnginePauseFlags / set | 0 | 3 | 1 | 0 | 11 | `Engine_SetPauseFlags`, `WordFlags_Set`, `four`, `mask` | 18 / 0 / 0 (100.0%) |
| (global) / gUnk_02000560 / and | 0 | 2 | 1 | 0 | 30 | `Flags_MaskedWord`, `SceneActorBehavior_OverrideBits`, `TestFlags`, `TestGlobal`, `TestModeFlags`, `Tst32`, `any`, `blockedMask`, `button`, `four`, `globalFlags`, `m`, `mask`, `one`, `pause`, `test` | — |
| (unresolved type) / flags / and | 0 | 0 | 0 | 0 | 2 | `mask`, `one` | — |
| (unresolved type) / flags / clear | 0 | 0 | 0 | 0 | 1 | `bit` | — |
| (unresolved type) / flags868 / and | 0 | 0 | 0 | 0 | 8 | `bit`, `mask` | — |
| (unresolved type) / flags868 / nonzero | 0 | 4 | 0 | 0 | 22 | `GameState_TestWorldFlags`, `OwnerWidgetsEnabled` | — |
| (unresolved type) / flags868 / set | 0 | 0 | 0 | 0 | 4 | `GameState_SetWorldFlags`, `bit`, `mask` | — |
| (unresolved type) / status / and | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| (unresolved type) / statusFlags / and | 0 | 0 | 0 | 0 | 1 | `SceneActorChase_Bits32` | — |
| struct A / fl / set | 0 | 0 | 0 | 0 | 2 | `m` | — |
| struct ActorAnimController / flags / set | 0 | 0 | 1 | 0 | 23 | `one` | — |
| struct ActorAttachmentFollowView / flags / and | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct ActorControllerMotion / flags / clear | 0 | 0 | 1 | 0 | 1 | `one` | — |
| struct ActorControllerMotion / flags / set | 0 | 0 | 0 | 0 | 7 | `one` | — |
| struct ActorEffectStatus / flags / and | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct ActorReaction / flags / set | 0 | 0 | 0 | 0 | 1 | `flags` | — |
| struct ActorSoundPhase / flags / set | 0 | 0 | 0 | 0 | 3 | `flag` | — |
| struct ActorStats / damageFlags / and-store | 0 | 0 | 0 | 0 | 1 | `ClearDamageFlag` | — |
| struct ActorStats / damageFlags / set | 0 | 0 | 0 | 0 | 1 | `ActorStats_SetDamageFlags` | — |
| struct AnimSprite / flags / and-store | 0 | 0 | 0 | 0 | 1 | `clear` | — |
| struct AreaEffectManager / player.statusFlags / and | 0 | 0 | 0 | 0 | 1 | `tst` | — |
| struct ArmorActor / part0.reactionFlags / set | 0 | 0 | 0 | 0 | 2 | `ContactCollider_SetReactionFlags` | — |
| struct ArmorActor / part1.reactionFlags / set | 0 | 0 | 0 | 0 | 2 | `ContactCollider_SetReactionFlags` | — |
| struct ArmorChaseActor / collision.flags / clear | 0 | 0 | 0 | 0 | 2 | `ContactCollider_ClearFlags` | — |
| struct ArmorChaseActor / collision.flags / set | 0 | 0 | 0 | 0 | 2 | `ContactCollider_SetFlags` | — |
| struct ArmorCollisionControl / flags / and-store | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct ArmorCollisionControl / flags / set | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct ArmorContact / contactFlags / and | 0 | 0 | 0 | 0 | 1 | `Armor_ContactFlags` | — |
| struct ArmorDecisionActor / collision.flags / clear | 0 | 0 | 0 | 0 | 1 | `ContactCollider_ClearFlags` | — |
| struct ArmorInitPart / b.b1 / set | 0 | 0 | 0 | 0 | 1 | `active` | — |
| struct ArmorPart / flags119 / set | 0 | 0 | 0 | 0 | 2 | `active`, `one` | — |
| struct Axe / flagsE4C / and | 0 | 0 | 0 | 0 | 2 | `Flags_MaskedWord`, `RotationPhase_Mask` | — |
| struct Axe / flagsE4C / clear | 0 | 0 | 0 | 0 | 2 | `clearFlags`, `k1` | — |
| struct Axe / flagsE4C / nonzero | 0 | 0 | 0 | 0 | 3 | `WordFlags_Has` | — |
| struct Axe / flagsE4C / set | 0 | 0 | 0 | 0 | 4 | `Axe_SetEffectFlags`, `WordFlags_Set` | — |
| struct AxeFlags16 / bits / and-store | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct AxePaletteContext / flags / and | 0 | 0 | 0 | 0 | 1 | `Flags_MaskedWord` | — |
| struct Bee / flags / set | 0 | 0 | 0 | 0 | 2 | `Bee_SetRouteFlags` | — |
| struct BouncingContact / flags / and | 0 | 0 | 0 | 0 | 1 | `WordFlags_Test` | — |
| struct CameraTracking / flags / and | 0 | 0 | 0 | 0 | 1 | `Vanargand_MaskedFlags` | — |
| struct CameraTracking / requests / and | 0 | 0 | 0 | 0 | 1 | `testRequest` | — |
| struct ChaseActor / base.behaviorFlags / and | 0 | 2 | 0 | 1 | 41 | `Flags_MaskedWord`, `SceneActorBehavior_OverrideBits`, `SceneActorTurn_ActorFlags`, `WordFlag`, `invalidMask`, `mask`, `mask3`, `one`, `test` | — |
| struct ChaseActor / base.behaviorFlags / and-store | 0 | 0 | 0 | 0 | 5 | `MaskContactFlags`, `SceneActorChase_AndWordFlags` | — |
| struct ChaseActor / base.behaviorFlags / set | 0 | 0 | 0 | 0 | 41 | `ActorStatus_SetWord`, `MonsterInit_SetBehaviorFlags`, `SceneActorBehavior_SetWordFlags`, `SceneActorChase_SetBehaviorFlags`, `bit`, `mask` | — |
| struct ChaseActor / base.colliderA.flags / and-store | 0 | 0 | 0 | 0 | 17 | `MonsterInit_MaskColliderFlags` | — |
| struct ChaseActor / base.colliderA.flags / clear | 0 | 0 | 0 | 0 | 2 | `ContactCollider_ClearFlags` | — |
| struct ChaseActor / base.colliderA.flags / set | 0 | 0 | 0 | 0 | 2 | `ContactCollider_SetFlags` | — |
| struct ChaseActor / base.colliderB.flags / and-store | 0 | 0 | 0 | 0 | 1 | `MonsterInit_MaskColliderFlags` | — |
| struct ChaseActor / base.colliderB.flags / clear | 0 | 0 | 0 | 0 | 2 | `ContactCollider_ClearFlags` | — |
| struct ChaseActor / base.colliderB.flags / set | 0 | 0 | 1 | 0 | 1 | `ContactCollider_SetFlags` | — |
| struct ChaseActor / base.context.captureFlags / and | 0 | 0 | 0 | 0 | 1 | `ContextFlag` | — |
| struct ChaseActor / base.context.flags / nonzero | 0 | 0 | 0 | 0 | 1 | `Ghost_TestFlags` | — |
| struct ChaseActor / base.context.flags / set | 0 | 0 | 0 | 0 | 1 | `Dog_SetContextFlags` | — |
| struct ChaseActor / base.context.flags3D4 / clear | 0 | 0 | 0 | 0 | 1 | `MotionState_ClearFlags` | — |
| struct ChaseActor / base.context.flags62C / nonzero | 0 | 0 | 0 | 0 | 1 | `HasFlag` | — |
| struct ChaseActor / base.motionFlags / and | 0 | 1 | 1 | 0 | 98 | `Chase_MotionFlags`, `ContactActor_ReadMotionFlags`, `Mask16`, `SceneActorBehavior_MotionBits`, `SceneActorChase_QueryMotionFlags`, `SceneActorContact_MotionFlags`, `SceneActorTurn_Flags`, `SceneActor_MaskedHalfword`, `chasing`, `liftFlag`, `m`, `mask`, `partFlag` | — |
| struct ChaseActor / base.motionFlags / and-store | 0 | 0 | 0 | 0 | 13 | `ClearMotionFlags`, `HalfwordFlags_And`, `SceneActorChase_AndMotionFlags`, `SceneActorPlacement_ClearFlags`, `Slime_ClearMotionBits`, `n` | — |
| struct ChaseActor / base.motionFlags / clear | 0 | 1 | 0 | 0 | 32 | `ActorStatus_ClearMotionFlags`, `Chase_ClearBits`, `MimicChase_ClearBits`, `SceneActorChase_ClearBits`, `k1`, `motionMask` | — |
| struct ChaseActor / base.motionFlags / nonzero | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct ChaseActor / base.motionFlags / set | 0 | 1 | 0 | 0 | 16 | `Golem_SetMotionFlags`, `Or16`, `SceneActorChase_SetMotionFlags`, `mask` | — |
| struct ChaseActor / base.pendingControlFlags / and-store | 0 | 0 | 0 | 0 | 1 | `SceneActorChase_AndWordFlags` | — |
| struct ChaseActor / base.placementFlags / and | 0 | 0 | 0 | 0 | 5 | `ClearRotationFlag`, `SceneActor_MaskedHalfword`, `mask` | — |
| struct ChaseActor / base.placementFlags / and-store | 0 | 0 | 0 | 0 | 1 | `IvyInit_ClearMotionFlags` | — |
| struct ChaseActor / base.placementFlags / clear | 0 | 0 | 0 | 0 | 1 | `clearFlags16` | — |
| struct ChaseActor / base.placementFlags / nonzero | 0 | 0 | 0 | 0 | 10 | `ActorStatus_BodyFlags`, `PlacementDamage_TestField`, `SceneActorChase_HasPlacementFlags`, `SceneActorChase_TestField` | — |
| struct ChaseActor / base.placementFlags / set | 0 | 0 | 0 | 0 | 29 | `HalfwordFlags_Set`, `MonsterInit_SetMotionFlags`, `Or16`, `SceneActorPlacement_SetFlags` | — |
| struct ChaseActor / base.reactionFlags / and | 0 | 0 | 0 | 0 | 2 | `ClearReaction`, `mask` | — |
| struct ChaseActor / base.reactionFlags / and-store | 0 | 0 | 0 | 0 | 1 | `Slime_ClearPendingFlags` | — |
| struct ChaseActor / base.reactionFlags / clear | 0 | 0 | 0 | 0 | 7 | `SceneActorChase_ClearBits` | — |
| struct ChaseActor / base.reactionFlags / set | 0 | 0 | 0 | 0 | 15 | `ActorStatus_SetPendingBits`, `HalfwordFlags_Set`, `SceneActorChase_OrReactionFlags` | — |
| struct ChaseActor / base.statusFlags / and | 0 | 1 | 0 | 0 | 29 | `Flags_MaskedWord`, `SceneActorChase_StatusFlags`, `SceneActorPlacement_ReadFlags32`, `WordFlag`, `k2`, `mask` | — |
| struct ChaseActor / base.statusFlags / and-store | 0 | 0 | 0 | 0 | 4 | `ClearWord`, `SceneActorChase_AndWordFlags` | — |
| struct ChaseActor / base.statusFlags / set | 0 | 1 | 0 | 0 | 107 | `ActorStatus_SetWord`, `MonsterInit_SetStatusFlags`, `SceneActorChase_SetStatusFlags`, `SetFlags`, `WordFlags_Set`, `c`, `mask`, `status` | — |
| struct ChaseActor / base.tracking.flags / set | 0 | 0 | 0 | 0 | 2 | `Vorn_SetTrackingFlags` | — |
| struct ChaseActor / group.common.gate.flags / set | 0 | 0 | 0 | 0 | 1 | `SetFlags` | — |
| struct ChaseFlags40 / bits / and | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct ChaseFlags40 / bits / and-store | 0 | 0 | 0 | 0 | 2 | `mask`, `motionMask` | — |
| struct ChaseFlags40 / bits / set | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct CloudSource / flags / and | 0 | 0 | 0 | 0 | 1 | `bit` | — |
| struct CoffinBike / draw.flags / and-store | 0 | 1 | 0 | 0 | 0 | `clearDrawFlags` | — |
| struct CoffinBike / flags / set | 0 | 0 | 4 | 0 | 6 | `bit` | — |
| struct CoffinBike / motion.damageFlags / nonzero | 0 | 1 | 0 | 0 | 0 | `TestActorFlags` | — |
| struct CoffinBikeDraw / flags / and | 0 | 0 | 1 | 0 | 0 | `bit` | — |
| struct CoffinBikeMotion / damageFlags / and-store | 0 | 0 | 0 | 0 | 1 | `CoffinBike_ClearDamageFlag` | — |
| struct ContactActor / flags / and | 0 | 0 | 0 | 0 | 1 | `testFlags` | — |
| struct ContactCollider / contactFlags / and | 0 | 1 | 0 | 0 | 2 | `ContactCollider_TestContactFlags` | 2 / 1 / 0 (66.7%) |
| struct ContactCollider / flags / and | 0 | 3 | 0 | 0 | 15 | `ContactCollider_TestFlags`, `SceneActorTurn_Flags`, `SceneActor_MaskedHalfword`, `Slime_ContactFlags`, `TestColliderFlags`, `angleFlag`, `directionMask`, `mask` | 9 / 2 / 0 (81.8%) |
| struct ContactCollider / flags / and-store | 0 | 1 | 2 | 0 | 14 | `ClearFlags`, `Clone_MaskColliderFlags`, `HalfwordFlags_And`, `Nidhoggr_MaskPartFlags`, `clearMask`, `mask` | — |
| struct ContactCollider / flags / clear | 0 | 0 | 0 | 0 | 5 | `ContactCollider_ClearFlags` | — |
| struct ContactCollider / flags / set | 0 | 0 | 0 | 0 | 7 | `ContactCollider_SetFlags`, `bodyFlags`, `mask` | — |
| struct ContactCollider / reactionFlags / and | 1 | 4 | 1 | 0 | 42 | `AttackFlags`, `ContactCollider_TestReactionFlags`, `Flags_MaskedWord`, `Mask`, `Player_ReadStatusBits`, `m`, `mask`, `one`, `reactionMask`, `testReaction` | 4 / 3 / 0 (57.1%) |
| struct ContactCollider / reactionFlags / and-store | 0 | 0 | 0 | 0 | 2 | `mask` | — |
| struct ContactCollider / reactionFlags / set | 0 | 0 | 0 | 0 | 3 | `mask` | — |
| struct ContactCollider / statRewardFlags / and | 0 | 0 | 0 | 0 | 6 | `Mask`, `mask`, `mask2`, `one` | 3 / 5 / 0 (37.5%) |
| struct ControllerSlot / flags / set | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct Count / camera.requests / clear | 0 | 0 | 0 | 0 | 2 | `CameraTracking_ClearRequests` | — |
| struct Count / camera.requests / set | 0 | 0 | 0 | 0 | 1 | `CameraTracking_SetRequests` | — |
| struct Count / overlayCollider.flags / clear | 0 | 0 | 0 | 0 | 2 | `ContactCollider_ClearFlags` | — |
| struct Count / overlayCollider.flags / set | 0 | 0 | 0 | 0 | 1 | `ContactCollider_SetFlags` | — |
| struct CrateHit / flags / and | 0 | 0 | 0 | 0 | 1 | `hitFlags` | — |
| struct Crow / flags / clear | 0 | 0 | 0 | 0 | 1 | `Crow_ClearFlags` | — |
| struct Crow / flags / nonzero | 0 | 0 | 0 | 0 | 2 | `Crow_TestAnimationFlags`, `SceneActorPlacement_TestValue` | — |
| struct Crow / flags / set | 0 | 0 | 0 | 0 | 1 | `Crow_SetFlags` | — |
| struct DashActor / flags / set | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct Disc / falling / nonzero | 0 | 0 | 0 | 0 | 1 | `TestFalling` | — |
| struct DisplaySequenceSprite / flags / and-store | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct DisplaySequenceSprite / flags / set | 0 | 0 | 0 | 0 | 1 | `one` | — |
| struct Dog / flags / and | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct Dog / flags / and-store | 0 | 0 | 0 | 0 | 3 | `SceneActorChase_AndWordFlags` | — |
| struct E / fl / set | 0 | 0 | 0 | 0 | 1 | `m` | — |
| struct EffectCollider / flags / and-store | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct EffectGroupEntry / flags / set | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct EffectManager / actor.statusFlags / and | 0 | 0 | 0 | 0 | 2 | `Player_ReadStatusBits` | — |
| struct EffectVariantSource / flags / and | 0 | 0 | 0 | 0 | 1 | `WordFlags_Test` | — |
| struct EnemyParticle / flags / set | 0 | 0 | 0 | 0 | 1 | `m` | — |
| struct Entry / flags / set | 0 | 0 | 0 | 0 | 1 | `value` | — |
| struct FlagHalfword / bits / and-store | 0 | 0 | 0 | 0 | 2 | `ClearHit` | — |
| struct Flags6 / flags / and | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct FrameDisplay / flags / and | 0 | 0 | 1 | 0 | 0 | `WordFlags_Test` | 1 / 0 / 0 (100.0%) |
| struct FrameDisplay / flags / set | 0 | 0 | 0 | 0 | 1 | `WordFlags_Set` | 1 / 0 / 0 (100.0%) |
| struct FrameDisplay / sprite.flags / set | 0 | 0 | 0 | 0 | 2 | `WordFlags_Set` | 2 / 0 / 0 (100.0%) |
| struct GameState / flags868 / and | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct Ghost / flags / nonzero | 0 | 0 | 0 | 0 | 7 | `Ghost_TestFlags` | — |
| struct Ghost / flags / set | 0 | 0 | 1 | 0 | 7 | `Ghost_SetFlags` | — |
| struct Ghoul / discFlags / clear | 0 | 0 | 0 | 0 | 4 | `Ghoul_ClearDiscFlags`, `SceneActorChase_ClearBits` | — |
| struct Ghoul / discFlags / nonzero | 0 | 0 | 0 | 0 | 1 | `Ghoul_TestDiscFlags` | — |
| struct Ghoul / discFlags / set | 0 | 0 | 0 | 0 | 2 | `Ghoul_SetDiscFlags` | — |
| struct Golem / chase.base.behaviorFlags / set | 0 | 0 | 0 | 0 | 1 | `MonsterInit_SetBehaviorFlags` | — |
| struct Golem / chase.base.colliderA.flags / and-store | 0 | 0 | 0 | 0 | 2 | `MonsterInit_MaskColliderFlags` | — |
| struct Golem / chase.base.motionFlags / and | 0 | 0 | 0 | 0 | 2 | `SceneActorChase_QueryMotionFlags`, `SceneActorContact_MotionFlags` | — |
| struct Golem / chase.base.statusFlags / and | 0 | 0 | 0 | 0 | 1 | `SceneActorChase_StatusFlags` | — |
| struct Golem / chase.base.statusFlags / set | 0 | 0 | 0 | 0 | 5 | `MonsterInit_SetStatusFlags` | — |
| struct Golem / flags628 / set | 0 | 0 | 0 | 0 | 1 | `GolemReaction_SetBodyFlags` | — |
| struct GridAttackActor / camera.requests / clear | 0 | 0 | 0 | 0 | 2 | `CameraTracking_ClearRequests` | — |
| struct GridAttackActor / camera.requests / set | 0 | 0 | 0 | 0 | 1 | `CameraTracking_SetRequests` | — |
| struct GridAttackActor / damageCollider.flags / clear | 0 | 0 | 0 | 0 | 2 | `ContactCollider_ClearFlags` | — |
| struct GridAttackActor / damageCollider.flags / set | 0 | 0 | 0 | 0 | 1 | `ContactCollider_SetFlags` | — |
| struct GridAttackActor / flags / and | 0 | 0 | 2 | 0 | 5 | `NidhoggrDuel_TestFlags` | — |
| struct GridAttackActor / flags / clear | 0 | 0 | 0 | 0 | 1 | `NidhoggrDuel_ClearFlags` | — |
| struct GridAttackActor / flags / set | 0 | 0 | 2 | 0 | 25 | `NidhoggrDuel_AddFlags` | — |
| struct GrowthHazardAnimationEvent / flags38 / and | 0 | 0 | 0 | 0 | 1 | `WordFlags_Test` | — |
| struct GrowthHazardAnimationEvent / flagsA / and | 0 | 0 | 0 | 0 | 1 | `HalfwordFlags_Test` | — |
| struct HitMotionFlags / bits / and-store | 0 | 1 | 0 | 0 | 0 | `bits` | — |
| struct HitMotionFlags / bits / set | 0 | 0 | 0 | 0 | 1 | `MarkHitFlag` | — |
| struct HomingCollider / flags / and | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct Hresvelgr / flags / and | 0 | 0 | 0 | 0 | 1 | `TestFlags` | — |
| struct Hresvelgr / flags / set | 0 | 0 | 0 | 0 | 6 | `EnableFlags`, `SetFlags`, `mask` | — |
| struct Hresvelgr / player.statusFlags / and | 0 | 0 | 0 | 0 | 1 | `PlayerHasFlag` | — |
| struct HresvelgrDamage / controllerFlags / and | 0 | 0 | 0 | 0 | 1 | `hasControllerFlag` | — |
| struct HresvelgrDamage / flags / set | 0 | 0 | 0 | 0 | 5 | `addFlags` | — |
| struct HresvelgrMotion / motion.flags / set | 0 | 0 | 0 | 0 | 1 | `setFlags` | — |
| struct HresvelgrTravelActor / behaviorFlags / and | 0 | 0 | 0 | 0 | 1 | `one` | — |
| struct HresvelgrTravelActor / flags / and | 0 | 0 | 0 | 0 | 9 | `Hresvelgr_TestFlags` | — |
| struct HresvelgrTravelActor / flags / set | 0 | 3 | 0 | 0 | 30 | `Hresvelgr_SetFlags`, `mask` | — |
| struct HresvelgrTravelActor / player.statusFlags / and | 0 | 0 | 0 | 0 | 1 | `PlayerFlags` | — |
| struct Item / flags / and | 0 | 0 | 1 | 0 | 0 | `one` | — |
| struct Ivy / captureFlags / set | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct IvyBurstPrefix / control.flags / and | 0 | 0 | 0 | 0 | 2 | `m`, `mask` | — |
| struct IvyBurstPrefix / control.flags / set | 0 | 0 | 0 | 0 | 1 | `Ivy_SetFlags` | — |
| struct LinkAura / f20 / set | 0 | 0 | 1 | 0 | 3 | `LinkAura_SetFlags` | — |
| struct LinkedEffectCollider / flags / clear | 0 | 0 | 1 | 0 | 0 | `ClearFlags` | — |
| struct Lun / flags3D4 / and | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct MenuCursor / stateFlags / set | 0 | 0 | 0 | 0 | 1 | `m` | — |
| struct MenuCursorBounds / flags / and | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct MenuCursorEntry / bounds.flags / and | 0 | 1 | 0 | 0 | 6 | `Entry_TestFlags`, `mask`, `one` | — |
| struct Mimic / reactionFlags / and | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct Mimic / reactionFlags / clear | 0 | 0 | 0 | 0 | 1 | `SceneActorChase_ClearBits` | — |
| struct ModifierMenu / dirtyFlags / set | 0 | 0 | 0 | 0 | 3 | `ModifierMenu_MarkDirty` | — |
| struct Motion0818Object / flags119 / set | 0 | 0 | 0 | 0 | 2 | `one` | — |
| struct MotionPoolSlot / flags / and | 0 | 0 | 0 | 0 | 1 | `flag` | — |
| struct MotionPoolSlot / flags / set | 0 | 0 | 0 | 0 | 1 | `flag` | — |
| struct MotionTarget / flags / and-store | 0 | 0 | 0 | 0 | 1 | `andm` | — |
| struct MotionTarget / flags / set | 0 | 0 | 0 | 0 | 2 | `Nidhoggr_AddTargetFlags`, `orm` | — |
| struct Mummy / chase.base.behaviorFlags / set | 0 | 0 | 0 | 0 | 1 | `MonsterInit_SetBehaviorFlags` | — |
| struct Mummy / chase.base.colliderA.flags / and-store | 0 | 0 | 0 | 0 | 2 | `MonsterInit_MaskColliderFlags` | — |
| struct Mummy / chase.base.statusFlags / set | 0 | 0 | 0 | 0 | 2 | `MonsterInit_SetStatusFlags` | — |
| struct Mummy / flags6F2 / clear | 0 | 0 | 0 | 0 | 1 | `contextBits` | — |
| struct Mummy / flags6F2 / nonzero | 0 | 0 | 0 | 0 | 2 | `Mummy_TestContext` | — |
| struct NamedObject / flags / and | 0 | 1 | 0 | 0 | 1 | `mask` | — |
| struct NamedObject / flags / clear | 0 | 0 | 3 | 0 | 8 | `MovingPlatform_DisableFlags` | — |
| struct NamedObject / flags / set | 0 | 0 | 2 | 0 | 18 | `MovingPlatform_EnableFlags`, `NamedObjectFlags_Add` | — |
| struct Npc / flags / set | 0 | 1 | 0 | 0 | 2 | `mask` | — |
| struct Npc / flagsC / and | 0 | 0 | 0 | 0 | 2 | `m`, `two` | — |
| struct Obj / flags / and | 0 | 0 | 1 | 0 | 0 | `mask` | — |
| struct Obj / flags / set | 0 | 0 | 1 | 0 | 0 | `one` | — |
| struct OrbitEffect / flags / and | 0 | 0 | 1 | 0 | 0 | `WordFlags_Test` | — |
| struct OrbitEffect / flags / and-store | 0 | 0 | 0 | 0 | 2 | `SceneActorChase_AndWordFlags` | — |
| struct OrbitEffect / flags / set | 0 | 0 | 0 | 0 | 1 | `flag` | — |
| struct OtenkoActor / displayFlags / and | 0 | 0 | 0 | 0 | 1 | `m8` | — |
| struct OtenkoActor / displayFlags / set | 0 | 1 | 0 | 0 | 1 | `m`, `mask` | — |
| struct P08142C1C / f20 / and | 0 | 0 | 0 | 0 | 1 | `Player_ReadStatusBits` | — |
| struct P08142F94 / f20 / and | 0 | 0 | 0 | 0 | 1 | `Player_ReadStatusBits` | — |
| struct P08159BB8 / f20 / set | 0 | 0 | 0 | 0 | 1 | `Player_SetStatusBits` | — |
| struct P0823A5EC / flags / clear | 0 | 0 | 0 | 0 | 2 | `ClearFlags` | — |
| struct P0823A5EC / flags / set | 0 | 1 | 0 | 0 | 0 | `SetFlags` | — |
| struct P0823CFDC / f20 / and | 0 | 0 | 0 | 0 | 1 | `Player_ReadStatusBits` | — |
| struct P0823D370 / f20 / and | 0 | 0 | 0 | 0 | 1 | `Player_ReadStatusBits` | — |
| struct PairedSceneHalf / actionFlags / and | 0 | 0 | 0 | 0 | 6 | `bit`, `mask`, `masked` | — |
| struct PairedSceneHalf / actionFlags / set | 0 | 1 | 1 | 0 | 39 | `Vanargand_SetFlags`, `mask`, `setFlags`, `twelve` | — |
| struct PairedSceneHalf / flags / and | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct PairedSceneHalf / flags / set | 0 | 0 | 0 | 0 | 1 | `SetFlags` | — |
| struct Particle / flags / and-store | 0 | 0 | 0 | 0 | 1 | `ClearFlags` | — |
| struct PhaseObject / groupMode.flags / set | 0 | 0 | 0 | 0 | 1 | `PhaseObject_SetFlags` | — |
| struct PlayStatsScreen / flags / set | 0 | 0 | 0 | 0 | 2 | `bit` | — |
| struct Player / actionFlags / and | 0 | 1 | 1 | 0 | 18 | `Player_ReadStatusBits`, `WordFlags_Test` | 4 / 2 / 0 (66.7%) |
| struct Player / statBonusFlags / and | 0 | 1 | 1 | 0 | 7 | `Player_ReadStatusBits` | 3 / 0 / 0 (100.0%) |
| struct Player / statusFlags / and | 0 | 2 | 1 | 0 | 34 | `Flag`, `PlayerScript_TestFlags`, `Player_ReadStatusBits`, `SceneActorChase_Bits32`, `SceneActorState_TestPlayerFlags`, `TestField`, `TestFlags`, `TestPlayerFlag`, `blocked`, `excluded`, `flag`, `m`, `mask`, `playerMask` | 3 / 0 / 0 (100.0%) |
| struct Player / statusFlags / set | 0 | 7 | 0 | 0 | 38 | `Player_AddStatusFlags`, `Player_SetStatusBits`, `flag` | 7 / 0 / 0 (100.0%) |
| struct PlayerCloneSlot / flags / set | 0 | 0 | 0 | 0 | 4 | `WordFlags_Set` | — |
| struct PlayerEventCall / arguments.header / and | 0 | 0 | 0 | 0 | 1 | `countMask` | — |
| struct PlayerModifierTarget / flags / and | 2 | 1 | 0 | 0 | 7 | `PlayerModifier_TestTargetFlags` | — |
| struct PlayerPushState / flags / set | 0 | 0 | 1 | 0 | 2 | `Player_SetStatusBits` | — |
| struct PlayerStatModifierContext / attackCollider.reactionFlags / set | 0 | 1 | 0 | 0 | 0 | `ContactCollider_SetReactionFlags` | — |
| struct PooledActor / flags / and | 0 | 0 | 0 | 0 | 4 | `PooledActor_TestFlags`, `mask` | — |
| struct PooledActor / flags / set | 0 | 0 | 0 | 0 | 4 | `PooledActor_SetFlags` | — |
| struct PresentationCollider / flags / clear | 0 | 0 | 0 | 0 | 1 | `ClearColliderFlag` | — |
| struct R / flags / set | 0 | 0 | 0 | 0 | 1 | `c` | — |
| struct Ratatosk / camera.requests / clear | 0 | 0 | 0 | 0 | 4 | `CameraTracking_ClearRequests` | — |
| struct Ratatosk / camera.requests / set | 0 | 0 | 0 | 0 | 1 | `CameraTracking_SetRequests` | — |
| struct Ratatosk / flags / and | 0 | 0 | 1 | 0 | 16 | `Ratatosk_TestFlags`, `flags` | — |
| struct Ratatosk / flags / clear | 0 | 0 | 0 | 0 | 1 | `clearFlags` | — |
| struct Ratatosk / flags / set | 0 | 1 | 0 | 0 | 38 | `Ratatosk_SetFlags` | — |
| struct RatatoskParticle / motionFlags / and-store | 0 | 0 | 0 | 0 | 1 | `ClearFlags` | — |
| struct RatatoskParticle / motionFlags / set | 0 | 0 | 0 | 0 | 1 | `one` | — |
| struct RegionActor / behaviorFlags / and | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct RegionActor / behaviorFlags / nonzero | 0 | 0 | 0 | 0 | 11 | `RegionActor_HasBehavior` | — |
| struct RegistryActor / control.motion.flags / and-store | 0 | 0 | 0 | 0 | 1 | `ClearDrawFlags` | — |
| struct RegistryActor / control.motion.flags / set | 0 | 0 | 0 | 0 | 2 | `RegistryActor_SetMotionFlags` | — |
| struct RegistryActor / control.updateFlags / set | 0 | 0 | 0 | 0 | 4 | `RegistryActor_SetUpdateFlags` | — |
| struct RegistryActor / stats.damageFlags / set | 0 | 0 | 0 | 0 | 1 | `ActorStats_SetDamageFlags` | — |
| struct RegistryChoiceMenu / cursor.stateFlags / and | 0 | 2 | 0 | 0 | 0 | `TestFlags` | — |
| struct RepeatedEffectCollider / contactFlags / and | 0 | 0 | 0 | 0 | 1 | `Flags_MaskedWord` | — |
| struct RepeatedEffectCollider / responseFlags / and | 0 | 0 | 0 | 0 | 1 | `Flags_MaskedWord` | — |
| struct RoamingObjectListener / flags / and-store | 0 | 1 | 0 | 0 | 1 | `mask` | — |
| struct RoomActor / behaviorFlags / and | 0 | 0 | 0 | 0 | 2 | `Flags_MaskedWord`, `bit` | 1 / 0 / 0 (100.0%) |
| struct RoomActor / behaviorFlags / set | 0 | 0 | 0 | 0 | 1 | `mask` | 1 / 0 / 0 (100.0%) |
| struct RoomActor / motionFlags / and | 0 | 1 | 0 | 0 | 3 | `SceneActorPlacement_ReadFlags16`, `m`, `mask` | 11 / 1 / 0 (91.7%) |
| struct RoomActor / motionFlags / set | 0 | 0 | 0 | 0 | 3 | `SetFlags` | 1 / 0 / 0 (100.0%) |
| struct RoomActor / reactionFlags / and-store | 0 | 1 | 0 | 0 | 2 | `SceneActorPlacement_ClearFlags` | — |
| struct RoomActor / statusFlags / and | 0 | 0 | 0 | 0 | 1 | `SceneActorPlacement_ReadFlags32` | 5 / 0 / 0 (100.0%) |
| struct RoomActor / statusFlags / and-store | 0 | 0 | 0 | 0 | 4 | `SceneActorChase_AndWordFlags` | 1 / 0 / 0 (100.0%) |
| struct RoomActor / statusFlags / set | 0 | 0 | 0 | 0 | 1 | `m` | — |
| struct RouteSpriteActor / sprite.flags / set | 0 | 0 | 0 | 0 | 1 | `one` | — |
| struct S / f9c / set | 0 | 0 | 0 | 0 | 1 | `setf` | — |
| struct S / fl / and-store | 0 | 0 | 0 | 0 | 2 | `m` | — |
| struct S / h0e / set | 0 | 1 | 0 | 0 | 0 | `mask` | — |
| struct SabataActorState / bodyCollider.flags / clear | 0 | 0 | 0 | 0 | 3 | `ContactCollider_ClearFlags` | — |
| struct SabataActorState / camera.requests / clear | 0 | 0 | 1 | 0 | 3 | `CameraTracking_ClearRequests` | — |
| struct SabataActorState / flags / and | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct SabataActorState / flags / set | 0 | 0 | 0 | 0 | 5 | `mask`, `one` | — |
| struct SabataAttackActor / flags / and | 0 | 0 | 1 | 0 | 0 | `mask` | — |
| struct SabataAttackActor / flags / set | 0 | 1 | 0 | 0 | 15 | `Sabata_SetFlags` | — |
| struct SceneActor / flags / and | 0 | 2 | 0 | 0 | 4 | `ActorFlag`, `SceneActor_TestFlags`, `blockedMask` | — |
| struct SceneActor / flags / set | 0 | 3 | 0 | 0 | 9 | `SceneActor_SetFlags` | — |
| struct SceneActorAnimationLink / flags / and | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct SceneActorBehaviorContactView / flags / and | 0 | 0 | 0 | 0 | 3 | `SceneActor_MaskedHalfword`, `TestFlag` | — |
| struct SceneActorBody / flags / nonzero | 0 | 0 | 0 | 0 | 1 | `SceneBody_TestFlags` | — |
| struct SceneActorBody / flags / set | 0 | 1 | 0 | 0 | 1 | `SceneActorBody_SetFlags`, `flag` | — |
| struct SceneActorChaseGroup / common.gate.flags / clear | 0 | 0 | 0 | 0 | 11 | `SceneActorChase_GroupClearFlags`, `bits` | — |
| struct SceneActorChaseGroup / common.gate.flags.value / set | 0 | 0 | 0 | 0 | 2 | `SceneActorChase_SetFlagComponent` | — |
| struct SceneActorChaseGroup / common.placement.flags / set | 0 | 0 | 1 | 0 | 0 | `ActorStatus_SetPlacementFlags` | — |
| struct SceneActorChasePlacement / flags / set | 0 | 0 | 0 | 0 | 1 | `one` | — |
| struct SceneActorChaseTracking / flags / and | 0 | 0 | 0 | 0 | 5 | `bm`, `m2`, `mask`, `trackingMask` | — |
| struct SceneActorChaseTracking / flags / and-store | 0 | 0 | 0 | 0 | 6 | `ClearTargetFlags`, `Reaction_ClearMotion`, `mask` | — |
| struct SceneActorChaseTracking / flags / set | 0 | 0 | 0 | 0 | 2 | `m`, `mask` | — |
| struct SceneActorContactTracking / tracking.flags / nonzero | 0 | 0 | 0 | 0 | 1 | `SceneActorContact_TestTracking` | — |
| struct SceneActorContactTracking / tracking.flags / set | 0 | 0 | 0 | 0 | 2 | `SceneActorContact_SetTrackingFlags` | — |
| struct SceneActorControlMotionState / flags3D4 / and-store | 0 | 0 | 0 | 0 | 1 | `SceneActorChase_AndWordFlags` | — |
| struct SceneActorControlMotionState / flags3D4 / clear | 0 | 1 | 0 | 0 | 2 | `MotionState_ClearFlags` | — |
| struct SceneActorControlMotionState / flags3D4 / nonzero | 0 | 0 | 1 | 0 | 1 | `MotionState_TestFlags` | — |
| struct SceneActorControlMotionState / flags3D4 / set | 0 | 0 | 0 | 0 | 1 | `MotionState_SetFlags` | — |
| struct SceneActorReactionMotion / flags / and | 0 | 0 | 0 | 0 | 2 | `mask` | — |
| struct SceneActorRegistryControlGroup / placement.flags / set | 0 | 0 | 0 | 0 | 1 | `Golem_MarkContactPlacement` | — |
| struct SceneActorRegistryManager / flags / and | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct SceneActorRegistryManager / flags / and-store | 0 | 0 | 0 | 0 | 2 | `WordFlags_And`, `mask` | — |
| struct SceneActorRegistryManager / flags / nonzero | 0 | 0 | 0 | 0 | 6 | `HasFlag`, `WordFlags_Has` | — |
| struct SceneActorRegistrySequenceDrawable / flags / and-store | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct SceneActorRouteContext / flags628 / set | 0 | 0 | 0 | 0 | 1 | `SceneActorRoute_SetContextFlags` | — |
| struct SceneActorScriptPropertiesView / flags6DC / and | 0 | 0 | 0 | 0 | 3 | `StateFlag` | — |
| struct SceneActorScriptPropertiesView / flags6DC / nonzero | 0 | 0 | 0 | 0 | 1 | `ContextHasFlag` | — |
| struct SceneActorScriptPropertiesView / flags6DC / set | 0 | 0 | 0 | 0 | 1 | `SetContextFlags` | — |
| struct SceneCollider / contactFlags / and | 0 | 0 | 0 | 0 | 4 | `SceneCollider_ReadContactFlags` | — |
| struct SceneCollider / contactFlags2 / and | 0 | 2 | 0 | 0 | 3 | `SceneCollider_ReadReactionFlags` | — |
| struct SceneColliderEffect / motionFlags / nonzero | 0 | 0 | 1 | 0 | 1 | `TestFlag` | — |
| struct SceneCombatActor / colliderGroup.motion.flags / clear | 0 | 0 | 0 | 0 | 8 | `ActorController_ClearMotionFlags` | — |
| struct SceneHudState / actionOwner.statBonusFlags / and | 0 | 0 | 1 | 0 | 2 | `WordFlags_Test` | — |
| struct SceneHudState / actionOwner.statusFlags / and | 0 | 0 | 0 | 0 | 2 | `WordFlags_Test` | — |
| struct SceneNpc / displayFlags / set | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct SceneObject / flags / and | 0 | 0 | 2 | 0 | 2 | `SceneObject_ReadFlags` | — |
| struct ScenePoolActor / flags5ee / nonzero | 0 | 0 | 0 | 0 | 1 | `ScenePool_HasControlRestriction` | — |
| struct ScenePoolActor / proximityCollider.flags / and-store | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct ScenePoolActor / stateFlags / set | 0 | 3 | 1 | 0 | 6 | `ScenePoolActor_SetStateFlags` | — |
| struct Segment / flags / nonzero | 0 | 0 | 0 | 0 | 4 | `test` | — |
| struct ShopWidgetFlags / flags / set | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct Skeleton / actionFlags / and | 0 | 0 | 0 | 0 | 3 | `mask` | — |
| struct Skeleton / actionFlags / and-store | 0 | 0 | 0 | 0 | 4 | `SceneActorChase_AndWordFlags` | — |
| struct Skeleton / actionFlags / nonzero | 0 | 0 | 0 | 0 | 5 | `SkeletonInit_TestSkeletonFlags`, `TestBits`, `WordFlags_Has` | — |
| struct Skeleton / actionFlags / set | 0 | 0 | 0 | 0 | 5 | `SkeletonInit_SetFlags`, `Skeleton_SetContextFlags`, `WordFlags_Set`, `mask` | — |
| struct Skeleton / attackCollider.flags / set | 0 | 0 | 0 | 0 | 1 | `ContactCollider_SetFlags` | — |
| struct Skeleton / bodyCollider.flags / set | 0 | 0 | 0 | 0 | 1 | `ContactCollider_SetFlags` | — |
| struct Skeleton / chase.base.motionFlags / and | 0 | 0 | 0 | 0 | 5 | `SceneActorChase_QueryMotionFlags`, `m` | — |
| struct Slime / flags / and-store | 0 | 0 | 0 | 0 | 1 | `Slime_ClearPendingFlags` | — |
| struct Slime / flags / nonzero | 0 | 0 | 0 | 0 | 4 | `Slime_HasContextFlag`, `Slime_TestFlags`, `Slime_TestReactionFlag` | — |
| struct Slime / flags / set | 0 | 0 | 0 | 0 | 2 | `Slime_SetFlags` | — |
| struct SpatialEntry / flags / and | 0 | 0 | 0 | 0 | 2 | `SpatialEntry_TestFlags` | — |
| struct SpatialEntry / flags / set | 0 | 0 | 0 | 0 | 2 | `one` | — |
| struct Spider / flags62C / and | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct Spider / flags62C / and-store | 0 | 0 | 0 | 0 | 1 | `ClearContext` | — |
| struct Spider / flags62C / nonzero | 0 | 0 | 0 | 0 | 2 | `HasFlag_BeginPlayerPursuit`, `Test` | — |
| struct Spider / flags62C / set | 0 | 0 | 0 | 0 | 2 | `Spider_SetFlags` | — |
| struct StateActor / actionFlags / and | 0 | 0 | 0 | 0 | 8 | `SceneActorState_TestFlags`, `mask` | — |
| struct StateActor / actionFlags / clear | 0 | 0 | 0 | 0 | 1 | `SceneActorState_ClearFlags` | — |
| struct StateActor / actionFlags / set | 0 | 1 | 2 | 0 | 40 | `SceneActorState_SetFlags`, `mask` | — |
| struct StateActor / displayFlags / set | 0 | 0 | 0 | 0 | 3 | `SetDisplayFlag`, `mask` | — |
| struct StateActorRise / flags / set | 0 | 0 | 0 | 0 | 1 | `m` | — |
| struct SwordContact / contactFlags / and | 0 | 0 | 0 | 0 | 1 | `Sword_ContactFlags` | — |
| struct T / fl / and | 0 | 0 | 0 | 0 | 1 | `m` | — |
| struct TargetMotionExit / flags119 / and | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct TargetMotionExit / flags119 / set | 0 | 0 | 0 | 0 | 1 | `TargetMotion_SetFlag` | — |
| struct Task4E01Projectile / flags / and-store | 0 | 0 | 0 | 0 | 1 | `ClearFlags` | — |
| struct Task4E01Projectile / flags / set | 0 | 0 | 0 | 0 | 11 | `bit`, `mask`, `one`, `two` | — |
| struct TentacleActor / flags2D0 / and | 0 | 0 | 0 | 0 | 1 | `HasStatusFlag` | — |
| struct TentacleActor / motionFlags / and | 0 | 0 | 0 | 0 | 1 | `HasMotionFlag` | — |
| struct TentacleAnimationSegment / animation.status / nonzero | 0 | 0 | 0 | 0 | 2 | `CentipedeSpread_Test` | — |
| struct TentacleAnimationSegment / animation.status / set | 0 | 0 | 0 | 0 | 1 | `setFlag` | — |
| struct TentacleBody / attackFlags / and-store | 0 | 0 | 0 | 0 | 1 | `Tentacle_MaskTrailFlags` | — |
| struct TentacleBody / attackFlags / nonzero | 0 | 0 | 0 | 0 | 1 | `Tentacle_TestTrailFlags` | — |
| struct TentacleBody / attackFlags / set | 0 | 0 | 0 | 0 | 1 | `m` | — |
| struct TurningProjectile / flags / set | 0 | 0 | 0 | 0 | 1 | `one` | — |
| struct Vanargand / flags / set | 0 | 1 | 0 | 0 | 11 | `SetFlags`, `flag`, `flags`, `mask` | — |
| struct VanargandBoss / flags / set | 0 | 0 | 0 | 0 | 9 | `Vg_SetFlags` | — |
| struct VanargandPairAttack / flags / and | 0 | 0 | 0 | 0 | 2 | `VanargandPairAttack_TestFlags`, `two` | — |
| struct VanargandPairAttack / flags / set | 0 | 0 | 1 | 0 | 13 | `VanargandPairAttack_SetFlags`, `m`, `mask` | — |
| struct VanargandPairAttack / player.statusFlags / and | 0 | 0 | 0 | 0 | 1 | `TestFlags` | — |
| struct VanargandPresentation / flags / and | 0 | 0 | 0 | 0 | 6 | `TestActorFlag` | — |
| struct VanargandPresentation / flags / clear | 0 | 0 | 0 | 0 | 1 | `ClearActorFlag` | — |
| struct VanargandPresentation / flags / set | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct VanargandTargets / flags / and | 0 | 0 | 0 | 0 | 2 | `TestFlags` | — |
| struct Visual / flags / and | 0 | 0 | 0 | 0 | 3 | `mask` | — |
| struct Vorn / chase.base.behaviorFlags / and | 0 | 0 | 0 | 0 | 2 | `Vorn_ActorFlags`, `Vorn_MaskLow` | — |
| struct Vorn / chase.base.context.flags6D4 / nonzero | 0 | 0 | 0 | 1 | 1 | `TestContext_BeginTargetPursuit` | — |
| struct Vorn / chase.base.motionFlags / and | 0 | 0 | 0 | 0 | 6 | `SceneActorContact_MotionFlags`, `TestMotionFlag`, `Vorn_Mask16`, `mask` | — |
| struct Vorn / chase.base.motionFlags / clear | 0 | 0 | 0 | 0 | 5 | `Chase_ClearBits` | — |
| struct Vorn / chase.base.reactionFlags / clear | 0 | 0 | 0 | 0 | 1 | `Chase_ClearBits` | — |
| struct Vorn / chase.base.tracking.flags / set | 0 | 0 | 0 | 0 | 1 | `Vorn_SetTrackingFlags` | — |
| struct Vorn / flags6D4 / and | 0 | 0 | 0 | 0 | 2 | `mask` | — |
| struct Vorn / flags6D4 / nonzero | 0 | 1 | 0 | 0 | 5 | `ContextFlags`, `TestActor`, `TestContext_BeginTargetPursuit`, `Test_HasActiveResponse`, `Test_SelectHitRecovery` | — |
| struct VornResponseFlags / bits / set | 0 | 0 | 0 | 0 | 2 | `Vorn_SetResponseFlags` | — |
| struct WirelessGameExchange / exchangeFlags / and | 0 | 0 | 1 | 0 | 4 | `WordFlags_Test` | — |
| struct WirelessGameExchange / exchangeFlags / and-store | 0 | 0 | 0 | 0 | 1 | `mask` | — |
| struct WorldMapMenu / selectionCursor.stateFlags / and | 0 | 0 | 1 | 0 | 0 | `Test` | — |
| union DisplaySequenceControl / raw / and | 0 | 0 | 1 | 0 | 4 | `active`, `bit`, `mask` | — |
