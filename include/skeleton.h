#ifndef GUARD_SKELETON_H
#define GUARD_SKELETON_H
#include "scene_actor_chase_reaction.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_terrain.h"
#include "actor_animation_controller.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_group.h"
#include "scene_actor_init_dispatch.h"
#include "scene_actor_init_graphics.h"
#include "scene_actor_init_body.h"
#include "display_sequence_inline.h"
#include "room_actor.h"

/* Navigation descriptor initialized by ProximityVolume_TranslateBounds. */
struct SkeletonNavigationProbe {
    u16 flags, unknown2, region, polygon;
    u32 unknown8;
    struct ContactPosition origin, previous, destination;
};
/* Full 0x74C Skeleton allocation; unresolved script roles remain provisional. */
struct Skeleton {
    struct RoomActor base;
    struct SceneActorChaseGroup group;
    struct ContactCollider bodyCollider, attackCollider;
    u16 callScriptId;
    u8 callLimit, callCount;
    u8 collisionDelay, unknown6D1;
    u16 parameter6D2, parameter6D4, parameter6D6;
    u16 timer6D8;
    u8 unknown6DA[2];
    u32 actionFlags;
    u32 distanceThresholds[3];
    u8 unknown6EC[4];
    s16 pursuitTimer, timer6F2, timer6F4, timer6F6;
    u16 parameters6F8[4];
    u8 unknown700[2];
    u8 parameter702, unknown703[3];
    u16 parameter706;
    u8 unknown708[0x10];
    s32 healthThreshold;
    struct SkeletonNavigationProbe routeProbe;
    struct ContactPosition destination;
    u8 unknown748[4];
};

void SceneActorScript_TriggerCallOnCount(struct SceneActorRegistryActor *);
u8 SceneActorReaction_IsRangeReady(struct SceneActorRegistryResetView *);
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
void Skeleton_UpdateTargetTransition(void);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
u32 NavMesh_ConsumeProbeEvent(void *);
u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, u32, u32, s32);
u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, u32, u32, s32);
void SceneActorApproach_UpdateRange(void);
extern const u32 gUnk_086060B4[];
void SceneActorReaction_TickHiddenResponse(void *);
void SceneActorControl_DispatchControl(void *);
void Skeleton_UpdateActionFlags(void *);
extern u32 gUnk_02000560;
void SceneActorPlacement_ConsumeShakeFlags(struct SceneActorRegistryActor *);
void SceneActorTarget_OnBindingChanged(struct SceneActorRegistryActor *);
void SceneActorBehavior_ActivatePendingControl(struct SceneActorRegistryActor *);
s32 SceneActorBehavior_DispatchPendingFlags(struct SceneActorRegistryActor *);
s32 SceneActorBehavior_UpdateTimersAndContacts(struct SceneActorRegistryActor *);
void SceneActorMotion_Update(struct SceneActorRegistryActor *);
s32 SceneActor_UpdateStatus(struct SceneActorRegistryActor *);
void SceneActorEffects_ReleaseResources(u8 *);
void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);
u32 SceneActorReaction_OnEnemyDefault(void);
void SceneActorReaction_ResolvePlacementDamage(void);
void SceneActorScript_OnCommand(void);
void SceneActorBehavior_InstallCallbackTables(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
void SceneActorBehavior_InitializeShadow(void *);
void SceneActor_UpdateProfileForCategory(void *);
void SceneActorReaction_ParseOptions(void *);
void SceneActorScript_ReadPParameters(void *);
void SceneActorScript_ReadCallScriptConfig(void *);
void SceneActorControl_InitBehaviorColliders(void *);
void SceneActorMovement_ResolveRoutePosition(void *);
void Skeleton_InitVariantMotion(void *);
void Skeleton_SetMovementControlPreset(struct SceneActorRegistryResetView *actor);
void Skeleton_SetAttackControlTable(void *);
void Skeleton_SetReactionControlTable(void *);
s32 Skeleton_Update(struct SceneActorRegistryResetView *actor);
u16 SceneActorReaction_SelectTileParameter(void *);
void sub_080BAE04(void *);
void Skeleton_UpdateInactiveState(void *);
void Skeleton_UpdateIdleState(void *);
void Skeleton_ConvergeDisplacedPosition(void *);
void Skeleton_UpdateDefeat(void *);
u8 SceneActorTarget_UpdateTrackedContact(void *);
void TentacleBody_UpdateReactionTransition(void *);
void SceneActorTarget_ResolvePendingObjectSkeleton(void);
void SceneActorPlacement_ResetStateThree(void);
void Skeleton_UpdateDownedState(void);
void SceneActorReaction_ReturnToRouteEntry(void);
void SceneActorReaction_BeginTargetRecovery(void);
void Skeleton_ResetRouteReaction(void);
void SceneActorReaction_UpdateGate(void);
void Skeleton_RestorePursuitRoute(void);
u8 Skeleton_ReactToContact(struct SceneActorRegistryResetView *actor);
void SceneActorReaction_SinkThenDefeat(void);
void Skeleton_BeginKnockedFlight(void);
void SceneActorReaction_TickHiddenTimeout(void);
s32 Skeleton_UpdateBehaviorCallbacks(struct SceneActorRegistryActor *p);
u32 Skeleton_Destroy(struct Skeleton *actor);
extern const u8 gUnk_086060FC[];
s32 Skeleton_Init(void *object);
extern const s16 gUnk_086149C4[];
void Skeleton_UpdateProbeApproach(struct SceneActorRegistryResetView *actor);
void Skeleton_UpdateEntryPhase(struct SceneActorRegistryResetView *actor);
void Skeleton_UpdateEntryRecoveryPhase(struct SceneActorRegistryResetView *actor);
void Skeleton_Create(void);
u32 Actor_StepTowardTargetAxis(struct SceneActorRegistryResetView *obj, struct ActorSprite *target,
                               u32 nearX, u32 nearZ, s32 speed);
s32 SceneActorTerrain_CompareNeighborHeight(s32 cell, s32 height, s32 direction);
void Skeleton_StepPosition(struct ContactPosition *position, s32 n);
u8 SceneActorRegistry_LinkActor(struct SceneActorRegistryActor *);
void SceneActorPlacement_HideActor(struct SceneActorRegistryActor *, s32);
void SceneActorPlacement_ShowActor(struct SceneActorRegistryActor *, s32);
static inline void Skeleton_SetState(struct SceneActorRegistryResetView *actor, u32 state,
                                     u32 phase, u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void Skeleton_ReturnToBehavior(struct SceneActorRegistryResetView *actor,
                                             u32 behavior)
{
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
#endif
