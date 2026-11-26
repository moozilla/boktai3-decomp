#ifndef GUARD_SKELETON_H
#define GUARD_SKELETON_H
#include "scene/motion/scene_actor_chase_reaction.h"
#include "scene/actor/scene_actor_control_phase.h"
#include "scene/motion/scene_actor_terrain.h"
#include "npc/cutscene_actor/actor_animation_controller.h"
#include "monsters/scene_actor_init_primitives.h"
#include "monsters/scene_actor_init_group.h"
#include "scene/actor/scene_actor_init_dispatch.h"
#include "monsters/scene_actor_init_graphics.h"
#include "monsters/scene_actor_init_body.h"
#include "engine/video/display_sequence_inline.h"
#include "monsters/chase_actor.h"

/* Full 0x74C Skeleton allocation; unresolved script roles remain provisional. */
struct Skeleton {
    struct ChaseActor chase;
    struct ContactCollider bodyCollider, attackCollider;
    u16 callScriptId;
    u8 callLimit, callCount;
    u8 collisionDelay, unknown6D1;
    u16 revivalDelay; /* keyword d: hidden-state frames; values above 19999 disable revival. */
    u16 downedExperience, defeatExperience; /* keyword E: temporary and final defeat rewards. */
    u16 timer6D8;
    u8 unknown6DA[2];
    u32 actionFlags;
    u32 distanceThresholds[3];
    u8 unknown6EC[4];
    s16 pursuitTimer, timer6F2, timer6F4, timer6F6;
    u16 parameters6F8[4];
    u8 unknown700[2];
    u8 parameter702;
    u8 attempts; /* distance reactions tried since the last reset */
    u16 actionCooldown; /* Frames until the next timed turn or action. */
    u16 parameter706;
    u32 nearDistance, farDistance; /* target distance bands for the reaction choice */
    u8 unknown710[8];
    s32 healthThreshold;
    struct ChaseRouteProbe routeProbe;
    struct ContactPosition destination;
    u16 visibilityFrame; /* Frames elapsed in the visibility flicker transition. */
    u8 unknown74A[2];
};

typedef char SkeletonSize[(sizeof(struct Skeleton) == 0x74C) ? 1 : -1];

void SceneActorScript_TriggerCallOnCount(struct ChaseActor *actor);
u8 SceneActorReaction_IsRangeReady(struct ChaseActor *actor);
u8 sub_0812F3CC(struct ChaseActor *);
void Skeleton_UpdateTargetTransition(struct ChaseActor *a);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
u32 NavMesh_ConsumeProbeEvent(void *);
u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, u32, u32, s32);
u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, u32, u32, s32);
void SceneActorApproach_UpdateRange(struct ChaseActor *actor);
extern const u32 gUnk_086060B4[];
s32 SceneActorReaction_TickHiddenResponse(struct ChaseActor *actor);
void SceneActorControl_DispatchControl(void *);
struct SkeletonActionFlagsActor;
s32 Skeleton_UpdateActionFlags(struct SkeletonActionFlagsActor *p);
extern u32 gUnk_02000560;
void SceneActorPlacement_ConsumeShakeFlags(struct ChaseActor *);
void SceneActorTarget_OnBindingChanged(struct ChaseActor *);
void SceneActorBehavior_ActivatePendingControl(struct ChaseActor *);
s32 SceneActorBehavior_DispatchPendingFlags(struct ChaseActor *);
s32 SceneActorBehavior_UpdateTimersAndContacts(struct ChaseActor *);
void SceneActorMotion_Update(struct ChaseActor *);
s32 SceneActor_UpdateStatus(struct ChaseActor *);
void SceneActorEffects_ReleaseResources(u8 *);
void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);
u32 SceneActorReaction_OnEnemyDefault(void);
void SceneActorReaction_ResolvePlacementDamage(void);
void SceneActorScript_OnCommand(void *actor, const u16 *command);
void SceneActorBehavior_InstallCallbackTables(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
void SceneActorBehavior_InitializeShadow(void *);
void SceneActor_UpdateProfileForCategory(struct ChaseActor *actor);
void SceneActorReaction_ParseOptions(struct ChaseActor *actor);
void SceneActorScript_ReadPParameters(struct ChaseActor *actor);
void SceneActorScript_ReadCallScriptConfig(struct ChaseActor *actor);
void SceneActorControl_InitBehaviorColliders(struct ChaseActor *actor);
void SceneActorMovement_ResolveRoutePosition(void *);
void Skeleton_InitVariantMotion(void *pIn);
void Skeleton_SetMovementControlPreset(struct ChaseActor *actor);
void Skeleton_SetAttackControlTable(void *);
void Skeleton_SetReactionControlTable(void *);
s32 Skeleton_Update(struct Skeleton *actor);
u16 SceneActorReaction_SelectTileParameter(struct ChaseActor *actor);
void Skeleton_TurnTowardRouteFacing(struct ChaseActor *a);
void Skeleton_UpdateInactiveState(u8 *p);
void Skeleton_UpdateIdleState(u8 *p);
void Skeleton_ConvergeDisplacedPosition(struct ChaseActor *actor);
void Skeleton_UpdateDefeat(struct ChaseActor *);
u8 SceneActorTarget_UpdateTrackedContact(void *);
void Skeleton_UpdateReactionTransition(u8 *s, s32 arg);
s32 SceneActorTarget_ResolvePendingObjectSkeleton(struct ChaseActor *actor);
void SceneActorPlacement_ResetStateThree(void);
void Skeleton_UpdateDownedState(struct ChaseActor *, s32);
void SceneActorReaction_ReturnToRouteEntry(struct ChaseActor *actor, s32 frame);
void SceneActorReaction_BeginTargetRecovery(void);
void Skeleton_ResetRouteReaction(struct ChaseActor *actor);
void SceneActorReaction_UpdateGate(struct ChaseActor *p);
void Skeleton_RestorePursuitRoute(struct ChaseActor *actor);
u8 Skeleton_ReactToContact(struct Skeleton *actor);
void SceneActorReaction_SinkThenDefeat(struct ChaseActor *actor, s32 frame);
void Skeleton_BeginKnockedFlight(struct ChaseActor *input);
void SceneActorReaction_TickHiddenTimeout(struct ChaseActor *p, s32 elapsed);
s32 Skeleton_UpdateBehaviorCallbacks(struct ChaseActor *p);
u32 Skeleton_Destroy(struct Skeleton *actor);
extern const u8 gUnk_086060FC[];
s32 Skeleton_Init(void *object);
extern const s16 gUnk_086149C4[];
void Skeleton_UpdateProbeApproach(struct Skeleton *actor);
void Skeleton_UpdateEntryPhase(struct Skeleton *actor);
void Skeleton_UpdateEntryRecoveryPhase(struct Skeleton *actor);
void Skeleton_Create(void);
u32 Actor_StepTowardTargetAxis(struct ChaseActor *obj, struct ActorSprite *target, u32 nearX,
                               u32 nearZ, s32 speed);
s32 SceneActorTerrain_CompareNeighborHeight(s32 cell, s32 height, s32 direction);
void Skeleton_StepPosition(struct ContactPosition *position, s32 n);
u8 SceneActorRegistry_LinkActor(struct ChaseActor *);
void SceneActorPlacement_HideActor(struct ChaseActor *, s32);
void SceneActorPlacement_ShowActor(struct ChaseActor *, s32);
static inline void Skeleton_SetState(struct ChaseActor *actor, u32 state, u32 phase, u32 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}
static inline void Skeleton_ReturnToBehavior(struct ChaseActor *actor, u32 behavior)
{
    actor->base.state2A8 = 0;
    actor->base.phase2A9 = 0;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}
#endif
