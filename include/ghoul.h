#ifndef GUARD_GHOUL_H
#define GUARD_GHOUL_H

#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
#include "display_sequence.h"
#include "room_actor.h"

/* Ghoul's allocation shares the actor prefix through the pursuit group.
 * The species parameters and flag meanings are tentative. */
struct Ghoul {
    struct RoomActor base;
    struct SceneActorChaseGroup group;
    u16 delayedScript;
    u8 scriptDelay, scriptElapsed;
    u16 discFlags;
    u16 unknown62A;
    struct ContactPosition discTargets[2];
    u16 discCooldown, throwCount;
    u8 discKind;
    u8 unknown641[7];
    u16 routeDelay, recoveryDelay;
    u8 routeBounds[0x24];
    struct ContactPosition routePosition;
};

static inline struct ContactPosition *Ghoul_InitialDiscTarget(struct Ghoul *context)
{
    return context->discTargets;
}

struct GhoulPackedPair {
    u32 first : 16;
    u32 second : 16;
};

struct GhoulCollisionVector {
    struct GhoulPackedPair xy, zw;
};

struct GhoulRouteOrigin {
    u8 unknown0[0x1C];
    s16 x;
    u8 unknown1E[2];
    s16 z;
};

void Ghoul_OnAttackCollision(struct ContactCollider *, struct ContactCollider *,
                             struct SceneActorRegistryResetView *);
void Ghoul_OnBodyCollision(void);
void Ghoul_UpdateAimedDiscThrow(struct SceneActorRegistryResetView *);
void Ghoul_UpdateIdleState(struct SceneActorRegistryResetView *);
void Ghoul_UpdateStandingState(struct SceneActorRegistryResetView *);
void Ghoul_TurnTowardRoute(struct SceneActorRegistryResetView *);
void Ghoul_UpdateDyingState(struct SceneActorRegistryResetView *, s32);
void Ghoul_UpdateDeathAnimation(struct SceneActorRegistryResetView *, s32);
void Ghoul_UpdateHiddenRecovery(struct SceneActorRegistryResetView *, s32, u32);

void Ghoul_UpdateDisplacementRecovery(struct SceneActorRegistryResetView *actor, s32 frame);
void Ghoul_UpdateDiscThrow(struct SceneActorRegistryResetView *actor);
void Ghoul_UpdateScriptedAction(struct SceneActorRegistryResetView *obj);
void Ghoul_ResumeDelayedChase(struct SceneActorRegistryResetView *actor);
void Ghoul_SetBehaviorHandlers(struct SceneActorRegistryResetView *actor);
void Ghoul_UpdateProbeApproach(struct SceneActorRegistryResetView *actor);
void SceneActorControl_RestartOrAdvance(struct SceneActorRegistryResetView *actor);
void Ghoul_WaitForRouteResult(struct SceneActorRegistryResetView *actor);
void Ghoul_SetActionHandlers(struct SceneActorRegistryResetView *actor);
void Ghoul_UpdateAimedDiscControl(struct SceneActorRegistryResetView *actor);
void Ghoul_SetSpecialHandlers(struct SceneActorRegistryResetView *actor);
void Ghoul_EnterIdleControl(struct SceneActorRegistryResetView *actor);
void SceneActorControl_RefreshMotionRoute_0808A90C(struct SceneActorRegistryResetView *actor);
void Ghoul_ResumeContactRoute(struct SceneActorRegistryResetView *actor);
void Ghoul_InitCollision(struct SceneActorRegistryResetView *actor);
void Ghoul_ResetRouteState(struct SceneActorRegistryResetView *actor);
void Ghoul_OnLifecycleEvent(void);
void Ghoul_BindProfile(struct SceneActorRegistryResetView *actor);
u32 Bat_LookupAnimationEntry(void *object);
void Ghoul_TickCooldown(struct SceneActorRegistryResetView *actor);
void Ghoul_SelectRandomRoutePoint(struct SceneActorRegistryResetView *actor);
void Ghoul_SelectRetreatRoutePoint(struct SceneActorRegistryResetView *actor);
void Ghoul_BeginAlternateRoute(struct SceneActorRegistryResetView *actor);
void Vorn_ParseField624_0808B320(struct SceneActorRegistryResetView *actor);
void Bat_TickDelayedScript(struct SceneActorRegistryResetView *actor);
u8 Ghoul_ReactToContact(struct SceneActorRegistryResetView *actor);
s32 Ghoul_ResumePendingRoute(struct SceneActorRegistryResetView *actor);
u32 Ghoul_UpdateCooldown(struct SceneActorRegistryResetView *actor);
void Ghoul_UpdateSpecialActionControl(struct SceneActorRegistryResetView *actor);
void Ghoul_UpdateAnimationElevenControl(struct SceneActorRegistryResetView *actor);
void Ghoul_UpdateAttackControl(struct SceneActorRegistryResetView *actor);
void Ghoul_SetBehaviorTable(struct SceneActorRegistryResetView *actor);
s32 Ghoul_Update(struct SceneActorRegistryResetView *actor);
u32 Ghoul_OnMessage(struct SceneActorRegistryResetView *actor);
u32 Ghoul_Destroy(struct SceneActorRegistryResetView *actor);
s32 Ghoul_Init(void *object);
void Ghoul_Create(void);

u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
void SceneActorReaction_PlayAnimationBUntilComplete(void);

#endif
