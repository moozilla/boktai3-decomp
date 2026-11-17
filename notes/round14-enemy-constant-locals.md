# Round 14: bounded enemy constant-local pass

Tags: #round14 #concessions #matching

## Result and scope

This pass removes **33 actual constant temporaries across eight source units**.
The ten-unit assignment was Axe, Skeleton, Dog, Octopus, Lun, Ghost, Mummy,
Vorn, Ivy, and Bee. The starting integrated worker state was `f4518be3c`.
Skeleton and Lun retain their existing source after the direct substitutions
failed to match.

The fixed source transformation was to remove literal zero/one locals and use
the corresponding value at each store, argument, or expression. A local with a
separate literal assignment was handled the same way. Each proposed removal
was compiled with the pinned stock compiler and compared across its entire
unit; only exact results were retained. Whole-function groups were also checked
where several zero/one locals occurred together. That added no further matches.

Manual review found one embedded literal assignment in `Ghost_UpdateDefeat`:
the local held the result of storing 1 to `field2B2`. Keeping that field store
and using 1 at the later consumers also matches. This is included in the 33.

Every landed diff was reviewed. Independent `tools/check.py --target-size`
comparisons for the eight changed units cover **522 functions and 214,684 bytes**,
all exact. The concession checks for those units report zero errors, no new
warning sites, and unchanged counts for all other warning kinds. These are
source removals, with zero shared-helper deduplications. No renames, ignores,
header edits, build overrides, or replacement steering were added.

## Source-only warning accounting

The following before/after counts use the scanner from the starting worker
state so that source changes are measured consistently. That scanner also
mistook `goto zero`, `goto one`, and `return zero` statements for declarations.
The false-positive column is separate diagnostic work; those statements were
left intact. Root's compiler-diagnostic correction is also separate from this
source accounting.

| Unit | Constant-local before | After source edits | Removed locals | Control-flow false positives | Stock bytes | Functions |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| `axe.c` | 30 | 24 | 6 | 5 | 29,148 | 97 |
| `skeleton_main.c` | 28 | 28 | 0 | 12 | 31,956 | 71 |
| `dog.c` | 25 | 23 | 2 | 8 | 37,088 | 73 |
| `octopus.c` | 22 | 15 | 7 | 0 | 14,980 | 46 |
| `lun.c` | 22 | 22 | 0 | 8 | 16,116 | 50 |
| `ghost.c` | 20 | 14 | 6 | 0 | 22,680 | 63 |
| `mummy_entry.c` | 18 | 17 | 1 | 4 | 49,808 | 61 |
| `vorn_main.c` | 16 | 14 | 2 | 0 | 25,404 | 76 |
| `ivy.c` | 14 | 8 | 6 | 0 | 21,464 | 61 |
| `bee.c` | 14 | 11 | 3 | 4 | 14,112 | 45 |
| **Total** | **209** | **176** | **33** | **41** | **262,756** | **643** |

The last two total columns include the unchanged Skeleton and Lun baselines.
Subtracting the 41 control-flow false positives from both sides would give
168 to 135 actual declaration sites. That additional reduction is diagnostic,
not source cleanup.

## Remaining literal substitutions that did not match

There were 133 actual literal cases checked, including the manually reviewed
embedded assignment. **100 remain open** because the direct replacement changes
the stock bytes. Their allocation effects are a diagnosis, not an admission
reason. The lists below identify their function or private inline helper and
local names. Repeated names indicate distinct scopes in the same function.

### `axe.c`: 13 open literal cases

- `SceneActorControl_UpdatePausePhase`: `zero`.
- `Axe_BeginMoveToPosition`: `one`, `zero`.
- `Axe_ClearForcedMovement`: `zero`.
- `Axe_InitAttackCooldowns`: `zero`.
- `Axe_InitEffectPalettes`: `zero`.
- `SpriteGroup_InitAnimatedSprites`: `zero`.
- `Axe_TickAuxiliarySpriteTimers`: `one`.
- `Restart`: `one`, `zero`.
- `Restart_UpdateControl2APhase`: `one`.
- `Axe_FollowDeceleratingRoute`: `one`.
- `Axe_FollowAcceleratingRoute`: `one`.

### `skeleton_main.c`: 13 open literal cases

- `Hide`: `one`.
- `Skeleton_UpdateVisibilityFlicker`: `one`.
- `SceneActorControl_BeginTimedBehavior56`: `zero`, `one`.
- `Skeleton_TryEnterIdleRange`: `zero`, `one`.
- `SceneActorControl_TryBeginNearbyBehavior16`: `zero`, `one`.
- `SceneActorReaction_UpdateGate`: `one`.
- `SceneActorScript_TriggerCallOnCount`: `zero`.
- `SceneActorReaction_TickHiddenResponse`: `one`.
- `SetPhase`: `zero`.
- `SceneActorReaction_SinkThenDefeat`: `zero`.

### `dog.c`: 11 open literal cases

- `Dog_UpdateTargetGate`: `one`.
- `QueueAction`: `zero`.
- `SceneActorControl_TickActionCooldown`: `one`.
- `Dog_SelectRetreatOrChase`: `zero`.
- `SceneActorAnimation_EmitDirectionalTrail`: `zero`.
- `SceneActorMotion_TestTargetTransition`: `zero`.
- `SetPhase`: `zero`.
- `Dog_UpdateScriptedAction`: `zero`.
- `SceneActor_UpdateAlternateEntryPhase`: `one`, `zero`, `zero`.

### `octopus.c`: 13 open literal cases

- `SceneActor_BeginReactionAnimation`: `zero`, `zero`.
- `SceneActorReaction_PlayEntry17`: `zero`, `one`.
- `Octopus_ReleaseCapturedTarget`: `one`.
- `SceneActorReaction_AwaitTargetControl`: `zero`.
- `SceneActorReaction_RestoreAfterControl`: `one`.
- `Ivy_ResetPhaseActive`: `one`, `zero`.
- `ResetState_ReturnToIdle`: `one`, `zero`.
- `SetState_BeginReturnToRoutePoint`: `zero`.
- `Octopus_BeginIdleAnimation`: `one`.

### `lun.c`: 11 open literal cases

- `SetState`: `one`.
- `SceneActor_BeginLinkedIdleReaction_0809C834`: `zero`, `one`.
- `Lun_ResetMovementState`: `one`.
- `Lun_BeginMoveToPosition`: `one`, `zero`.
- `SceneActorControl_ResetTargetFlags`: `zero`.
- `RouteActor_ReflectAtRaisedTerrain`: `zero`.
- `Lun_EnterIdleOrCopyCell`: `one`.
- `Lun_PlayResponseAndResetState`: `zero`, `one`.

### `ghost.c`: 11 open literal cases

- `Ghost_UpdateEnergyDrainAnimation`: `zero`.
- `Ghost_MakeRetreatOffset`: `zero`.
- `SceneActor_BeginLinkedIdleReaction`: `zero`, `one`.
- `Ghost_ResetMovementState`: `one`.
- `Ghost_StartMovementToPosition`: `one`, `zero`.
- `Ghost_ClearActionFlags`: `zero`.
- `Ghost_SetIdleBehavior`: `zero`.
- `Ghost_FollowPlayerLoop`: `one`.
- `Ghost_PursuePlayerLoop`: `one`.

### `mummy_entry.c`: 9 open literal cases

- `Mummy_UpdateRevival`: `one`.
- `Mummy_UpdateScriptedAction`: `zero`.
- `SetControl`: `zero`.
- `Mummy_EnterIdleControl`: `zero`, `one`, `zero`, `one`.
- `SceneActor_EnterMotionPhase`: `one`.
- `Mummy_ResetResponseState`: `zero`.

### `vorn_main.c`: 9 open literal cases

- `SelectPursuit`: `zero`.
- `SceneActorPhase_BeginTimedMotion`: `one`, `zero`.
- `SceneActorPhase_TryBeginSpecialMotion`: `zero`, `one`.
- `Vorn_BeginAction16`: `zero`, `one`.
- `Vorn_RestoreControlAfterItemLaunch`: `zero`, `one`.

### `ivy.c`: 5 open literal cases

- `SceneActorReaction_PlayHiddenCue`: `zero`.
- `Ivy_EnterActive`: `one`.
- `Ivy_UpdateRestartFlags`: `one`.
- `Ivy_BeginHold`: `zero`.
- `SceneActorControl_HideAndDisableCollision`: `zero`.

### `bee.c`: 5 open literal cases

- `Bee_BeginMoveToPosition`: `one`, `zero`.
- `Bee_InstallControl`: `zero`.
- `SceneActorControl_TickPendingTarget`: `one`.
- `Bee_UpdateWanderPause`: `zero`.

## Other retained declaration sites

The other **35 declarations** require a different source/type question from a
literal substitution. They comprise seven aggregate fields, thirteen
address-taken fill words, and fifteen values computed from conditions or
callbacks. The address-taken words are passed to fill/initialization helpers
and cannot be replaced by a value argument without changing those interfaces.
The computed values are frequently reused in a branch where they are known to
be zero; they still require an explicit dataflow review. No declaration in this
set was renamed to make the warning disappear.

### Aggregate fields (7)

- `axe.c`: `Axe_UpdateDefeat.zero`, `Axe_SpawnSparks.zero`, `Axe_UpdateTrailColliders.zero`, `Axe_UpdateBodyColliders.zero`.
- `skeleton_main.c`: `SceneActorReaction_TickForwardBurst.zero`, `Skeleton_TickDecayingLunge.zero`.
- `ivy.c`: `Ivy_UpdateDefeat.zero`.

### Address-taken fill words (13)

- `axe.c`: `SceneActorControl_InitializeHoverState.zero`, `Axe_Init.zero`.
- `dog.c`: `Dog_Init.zero`.
- `octopus.c`: `Octopus_Init.zero`.
- `lun.c`: `Lun_UpdateLaunchedReaction.zero`, `Lun_Init.zero`.
- `ghost.c`: `Ghost_RecoilTowardEndpoint.zero`.
- `mummy_entry.c`: `Mummy_UpdateBurnDamage.zero`, `Mummy_Init.zero`.
- `vorn_main.c`: `Vorn_Init.zero`.
- `ivy.c`: `Ivy_Init.zero`.
- `bee.c`: `Bee_ReturnToPlayerWhenStopped.zero`, `Bee_Init.zero`.

### Computed condition/callback results (15)

- `skeleton_main.c`: `Skeleton_UpdateDownedState.zero`.
- `dog.c`: `Dog_UpdateDefeatDelay.zero`, `Dog_UpdateDelayedPursuit.zero`, `Dog_BeginDelayedRouteRecovery.zero`.
- `octopus.c`: `SceneActorReaction_WaitForAdjacentTarget.zero`.
- `lun.c`: `Lun_EnterIdleOrCopyCell.zero`.
- `ghost.c`: `Ghost_BeginContactReaction.zero`, `Ghost_UpdateFacingTrigger.zero`.
- `mummy_entry.c`: `Mummy_UpdateDying.zero`, `Mummy_UpdateDelayedPursuit.zero`.
- `vorn_main.c`: `Vorn_QueueDelayedResponse.zero`, `SceneActorControl_WaitForRouteResult.zero`, `SceneActor_TrySoloAction.zero`, `Vorn_UpdateContactResponse.zero`.
- `ivy.c`: `SceneActorControl_WaitForTarget.zero`.

## Continuation

The compiler-warning holdouts in `round14-volatile-stage2.md` and
`round14-volatile-known-zero.md` remain open. This bounded pass does not change
those decisions. The remaining literal cases above need evidence for their
original field types, shared helpers, and control flow; the successful direct
substitutions do not justify retaining the unsuccessful ones indefinitely.
