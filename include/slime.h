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

struct SlimeDrawable {
    u32 flags;
    u8 unknown4[4];
    u8 scaleX;
    s8 scaleY;
    u8 unknownA[6];
    u16 frame;
    u8 unknown12[0xA];
    struct ContactPosition position;
    u8 unknown24[8];
    u8 animation[0x1C];
    struct DisplaySequenceState sequence;
    const void *resource;
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

static inline u8 Slime_TakePrepared(struct SceneActorRegistryResetView *actor)
{
    if (actor->base.field2B2) {
        actor->base.field2B2 = 0;
        return 1;
    }
    return 0;
}
void SceneActorControl_OnIdle(u8 *);
void Slime_ApplyBodyHit(struct ContactCollider *target, struct ContactCollider *info,
                        struct SceneActorRegistryResetView *actor);
void Slime_ResolveBodyContact(struct ContactCollider *contact, void *other,
                              struct SceneActorRegistryResetView *actor);
void Slime_ResetAnimation(struct SceneActorRegistryResetView *actor);
void Slime_StartWalkChoice(struct SceneActorRegistryResetView *actor);
void Slime_RollMoveFlag(struct SceneActorRegistryResetView *actor);
void Slime_StartRandomWait(struct SceneActorRegistryResetView *actor);
void Slime_UpdateReturnToIdle(struct SceneActorRegistryResetView *actor);
void Slime_UpdateRandomWait(struct SceneActorRegistryResetView *actor);
void Slime_PlayWaitAnimation(struct SceneActorRegistryResetView *actor);
void Slime_TickWaitTimer(struct SceneActorRegistryResetView *actor);
void Slime_UpdateWalkTransition(struct SceneActorRegistryResetView *p);
void Slime_OnUnusedControlEvent(void);
void Slime_ConvergeReactionMotion(struct SceneActorRegistryResetView *actor);
void Slime_UpdateShake(struct SceneActorRegistryResetView *actor, s32 elapsed);
void Slime_UpdateLaunch(struct SceneActorRegistryResetView *actor);
void Slime_UpdateStill(struct SceneActorRegistryResetView *actor);
void Bat_RunDefeatScript(struct SceneActorRegistryResetView *actor, s32 frame);
void Bat_UpdateActionScript(struct SceneActorRegistryResetView *obj);
void Slime_SetSecondaryStateTable(struct SceneActorRegistryResetView *actor);
void Slime_ApproachSavedTarget(struct SceneActorRegistryResetView *actor);
void Slime_RestoreSavedTarget(struct SceneActorRegistryResetView *actor);
void Slime_RunUpdateThenWait(struct SceneActorRegistryResetView *actor);
void Slime_SetPrimaryStateTable(struct SceneActorRegistryResetView *actor);
void Slime_InitColliders(struct SceneActorRegistryResetView *actor);
void Slime_ResetIdlePlayback(struct SceneActorRegistryResetView *actor);
void Slime_OnUnusedEvent(void);
void Slime_InitProfile(struct SceneActorRegistryResetView *actor);
s32 Slime_GetAnimationMovePulse(struct SceneActorRegistryResetView *actor);
void Slime_SelectWanderTarget(struct SceneActorRegistryResetView *actor, u32 direction);
u32 Slime_TryEnrage(struct SceneActorRegistryResetView *actor);
void Slime_FollowPlayer(struct SceneActorRegistryResetView *actor);
void Slime_OnInterrupt(struct SceneActorRegistryResetView *actor);
void Bat_ReadScriptValuePair(struct SceneActorRegistryResetView *actor);
void Slime_RunPendingActionScript(struct SceneActorRegistryResetView *actor);
u32 Slime_GetRandomVariantFlags(struct SceneActorRegistryResetView *actor);
void Slime_EmitDeathDebris(struct SceneActorRegistryResetView *actor);
void Slime_EmitTargetedReactionBurst(struct SceneActorRegistryResetView *input);
void Slime_EmitScatteredReactionBurst(struct SceneActorRegistryResetView *input);
void Slime_PlayPendingSound(struct SceneActorRegistryResetView *actor);
void SceneActorMotion_ArmPrimaryCollider(struct SceneActorMotionView *actor);
u32 Slime_ResolveDamageReaction(struct SceneActorRegistryResetView *actor);
u32 Slime_ProcessPendingStateChange(struct SceneActorRegistryResetView *actor);
s32 SceneActorMotion_RefreshGroundHeight(struct Slime *slime);
u32 Slime_Update(struct SceneActorRegistryResetView *actor);
u32 Slime_OnMessage(struct SceneActorRegistryResetView *actor);
u32 Slime_Destroy(struct SceneActorRegistryResetView *actor);
s32 Slime_Init(void *object);
void Slime_Create(void);

#endif
