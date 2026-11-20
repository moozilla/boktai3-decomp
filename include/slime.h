#ifndef GUARD_SLIME_H
#define GUARD_SLIME_H

#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_terrain.h"
#include "scene_actor_availability.h"
#include "display_sequence.h"
#include "scene_actor_motion.h"
#include "room_actor.h"

/* Species allocation; the roles of the private flags remain tentative. */
struct Slime {
    struct RoomActor base;
    u16 enrageCount, launchThreshold, previousGround, halfHealth;
    u8 unknown3DC[4];
    struct SceneActorMotionSubject wanderTarget;
    u8 unknown3F4[0x10];
    u16 splitScript, splitParameter, waitTimer, flags;
};

struct SlimeHalfFlags {
    s16 bits : 16;
};

struct SlimePackedPair {
    u32 first : 16, second : 16;
};
struct SlimeCollisionVector {
    struct SlimePackedPair xy, zw;
};

static inline u8 Slime_TakePrepared(struct ChaseActor *actor)
{
    if (actor->base.field2B2) {
        actor->base.field2B2 = 0;
        return 1;
    }
    return 0;
}
void SceneActorControl_OnIdle(u8 *);
void Slime_ApplyBodyHit(struct ContactCollider *target, struct ContactCollider *info,
                        struct ChaseActor *actor);
void Slime_ResolveBodyContact(struct ContactCollider *contact, void *other,
                              struct ChaseActor *actor);
void Slime_ResetAnimation(struct ChaseActor *actor);
void Slime_StartWalkChoice(struct ChaseActor *actor);
void Slime_RollMoveFlag(struct ChaseActor *actor);
void Slime_StartRandomWait(struct ChaseActor *actor);
void Slime_UpdateReturnToIdle(struct ChaseActor *actor);
void Slime_UpdateRandomWait(struct ChaseActor *actor);
void Slime_PlayWaitAnimation(struct ChaseActor *actor);
void Slime_TickWaitTimer(struct ChaseActor *actor);
void Slime_UpdateWalkTransition(struct ChaseActor *p);
void Slime_OnUnusedControlEvent(void);
void Slime_ConvergeReactionMotion(struct ChaseActor *actor);
void Slime_UpdateShake(struct ChaseActor *actor, s32 elapsed);
void Slime_UpdateLaunch(struct ChaseActor *actor);
void Slime_UpdateStill(struct ChaseActor *actor);
void Slime_RunDefeatScript(struct ChaseActor *actor, s32 frame);
void Slime_UpdateActionScript(struct ChaseActor *obj);
void Slime_SetSecondaryStateTable(struct ChaseActor *actor);
void Slime_ApproachSavedTarget(struct ChaseActor *actor);
void Slime_RestoreSavedTarget(struct ChaseActor *actor);
void Slime_RunUpdateThenWait(struct ChaseActor *actor);
void Slime_SetPrimaryStateTable(struct ChaseActor *actor);
void Slime_InitColliders(struct ChaseActor *actor);
void Slime_ResetIdlePlayback(struct ChaseActor *actor);
void Slime_OnUnusedEvent(void);
void Slime_InitProfile(struct ChaseActor *actor);
s32 Slime_GetAnimationMovePulse(struct ChaseActor *actor);
void Slime_SelectWanderTarget(struct ChaseActor *actor, u32 direction);
u32 Slime_TryEnrage(struct ChaseActor *actor);
void Slime_FollowPlayer(struct ChaseActor *actor);
void Slime_OnInterrupt(struct ChaseActor *actor);
void Slime_ReadSplitScript(struct ChaseActor *actor);
void Slime_RunPendingActionScript(struct ChaseActor *actor);
u32 Slime_GetRandomVariantFlags(struct ChaseActor *actor);
void Slime_EmitDeathDebris(struct ChaseActor *actor);
void Slime_EmitTargetedReactionBurst(struct ChaseActor *input);
void Slime_EmitScatteredReactionBurst(struct ChaseActor *input);
void Slime_PlayPendingSound(struct ChaseActor *actor);
void SceneActorMotion_ArmPrimaryCollider(struct SceneActorMotionView *actor);
u32 Slime_ResolveDamageReaction(struct ChaseActor *actor);
u32 Slime_ProcessPendingStateChange(struct ChaseActor *actor);
s32 SceneActorMotion_RefreshGroundHeight(struct Slime *slime);
u32 Slime_Update(struct ChaseActor *actor);
u32 Slime_OnMessage(struct ChaseActor *actor);
u32 Slime_Destroy(struct ChaseActor *actor);
s32 Slime_Init(void *object);
void Slime_Create(void);

#endif
