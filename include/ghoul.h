#ifndef GUARD_GHOUL_H
#define GUARD_GHOUL_H

#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
#include "display_sequence.h"
#include "chase_actor.h"

/* Ghoul's allocation shares the actor prefix through the pursuit group.
 * The species parameters and flag meanings are tentative. */
struct Ghoul {
    struct ChaseActor chase;
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

void Ghoul_OnAttackCollision(struct ContactCollider *, struct ContactCollider *,
                             struct ChaseActor *);
void Ghoul_OnBodyCollision(struct ContactCollider *, struct ContactCollider *, struct ChaseActor *);
void Ghoul_UpdateAimedDiscThrow(struct ChaseActor *);
void Ghoul_UpdateIdleState(struct ChaseActor *);
void Ghoul_UpdateStandingState(struct ChaseActor *);
void Ghoul_TurnTowardRoute(struct ChaseActor *);
void Ghoul_UpdateDyingState(struct ChaseActor *, s32);
void Ghoul_UpdateDeathAnimation(struct ChaseActor *, s32);
void Ghoul_UpdateHiddenRecovery(struct ChaseActor *, s32, u32);

void Ghoul_UpdateDisplacementRecovery(struct ChaseActor *actor, s32 frame);
void Ghoul_UpdateDiscThrow(struct ChaseActor *actor);
void Ghoul_UpdateScriptedAction(struct ChaseActor *obj);
void Ghoul_ResumeDelayedChase(struct ChaseActor *actor);
void Ghoul_SetBehaviorHandlers(struct ChaseActor *actor);
void Ghoul_UpdateProbeApproach(struct ChaseActor *actor);
void SceneActorControl_RestartOrAdvance(struct ChaseActor *actor);
void Ghoul_WaitForRouteResult(struct ChaseActor *actor);
void Ghoul_SetActionHandlers(struct ChaseActor *actor);
void Ghoul_UpdateAimedDiscControl(struct ChaseActor *actor);
void Ghoul_SetSpecialHandlers(struct ChaseActor *actor);
void Ghoul_EnterIdleControl(struct ChaseActor *actor);
void Ghoul_RefreshMotionRoute(struct ChaseActor *actor);
void Ghoul_ResumeContactRoute(struct ChaseActor *actor);
void Ghoul_InitCollision(struct ChaseActor *actor);
void Ghoul_ResetRouteState(struct ChaseActor *actor);
void Ghoul_OnLifecycleEvent(void);
void Ghoul_BindProfile(struct ChaseActor *actor);
u32 Bat_LookupAnimationEntry(void *object);
void Ghoul_TickCooldown(struct ChaseActor *actor);
void Ghoul_SelectRandomRoutePoint(struct ChaseActor *actor);
void Ghoul_SelectRetreatRoutePoint(struct ChaseActor *actor);
void Ghoul_BeginAlternateRoute(struct ChaseActor *actor);
void Ghoul_ReadDelayedScript(struct ChaseActor *actor);
void Bat_TickDelayedScript(struct ChaseActor *actor);
u8 Ghoul_ReactToContact(struct ChaseActor *actor);
s32 Ghoul_ResumePendingRoute(struct ChaseActor *actor);
u32 Ghoul_UpdateCooldown(struct ChaseActor *actor);
void Ghoul_UpdateSpecialActionControl(struct ChaseActor *actor);
void Ghoul_UpdateAnimationElevenControl(struct ChaseActor *actor);
void Ghoul_UpdateAttackControl(struct ChaseActor *actor);
void Ghoul_SetBehaviorTable(struct ChaseActor *actor);
s32 Ghoul_Update(struct ChaseActor *actor);
u32 Ghoul_OnMessage(struct ChaseActor *actor);
u32 Ghoul_Destroy(struct ChaseActor *actor);
s32 Ghoul_Init(void *object);
void Ghoul_Create(void);

u8 sub_0812F3CC(struct ChaseActor *);
void SceneActorReaction_PlayAnimationBUntilComplete(void);

#endif
