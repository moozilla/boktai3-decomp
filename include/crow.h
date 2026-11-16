#ifndef GUARD_CROW_H

#define GUARD_CROW_H

#include "scene_actor_reset.h"

#include "scene_actor_animation.h"

#include "scene_actor_control_phase.h"

#include "scene_actor_chase_reaction.h"

#include "scene_actor_control_motion.h"

#include "scene_actor_terrain.h"

#include "scene_actor_availability.h"

#include "scene_actor_motion.h"

#include "scene_actor_route.h"

#include "actor_animation_controller.h"

#include "display_sequence.h"

#include "engine.h"

#include "scene_actor_behavior.h"
#include "room_actor.h"

/* Complete 0x43C species allocation; private field roles are tentative. */

struct Crow {
    struct RoomActor base;
    u32 flags;
    u8 sequencePhase, distance;
    u16 approachTime;
    u8 speed, turnDelay;
    u16 groundHeight;
    u32 orbitRange;
    u8 approachTurns, unknown3E5[3];
    u16 homeCell, heightOffset;
    u8 bobPhase, wanderTicks, field3EE, field3EF;
    struct SceneActorMotionSubject curveSubject;
    u8 unknown404[0x18];
    struct ContactPosition vectors[3];
    struct ContactPosition storedPosition;
};

struct CrowPackedPosition {
    u32 xy, z;
};

struct CrowColliderHalfwords {
    u32 lo : 16;
    u32 hi : 16;
};

struct CrowColliderVector {
    struct CrowColliderHalfwords a, b;
};

struct CrowDrawable {
    struct ActorSprite sprite;
    u8 unknown24[8];
    u8 animation[0x1C];
    struct DisplaySequenceState sequence;
    void *resource;
};

struct CrowSignedFlags {
    s32 bits : 16;
};

void SceneActorControl_OnIdle(u8 *);

void Crow_OnAttackContact(u8 *unused, struct ContactCollider *obj,
                          struct SceneActorRegistryResetView *ctx);

void Crow_OnBodyContact(struct SceneActorBehaviorContactView *source,
                        struct SceneActorBehaviorContactView *destination,
                        struct SceneActorRegistryResetView *actor);

void Crow_OnIdleAnimation(struct SceneActorRegistryResetView *p);

void Crow_UpdatePeriodicAnimation(struct SceneActorRegistryActor *actor, s32 tick);

void Crow_UpdateAnimationSequence(struct SceneActorRegistryActor *actor);

void Crow_PlayEntry2(struct SceneActorRegistryResetView *actor);

void Crow_UpdateDirectionalPose(struct RoomActor *actor);

void Crow_OnSpecialAnimation(struct SceneActorRegistryResetView *p);

void Crow_RecoverFromDisplacement(struct SceneActorRegistryResetView *actor, s32 elapsed);

void Crow_UpdateDefeat(struct SceneActorRegistryResetView *actor, s32 frame);

void Crow_HopWithinHomeCell(struct SceneActorRegistryResetView *actor);

void Crow_SteerAroundPlayer(struct SceneActorRegistryResetView *actor);

void Crow_UpdateWanderTimer(struct SceneActorRegistryResetView *actor);

void Crow_UpdateReturnAnimation(struct SceneActorRegistryResetView *s);

void Crow_ApproachPlayer(struct SceneActorRegistryResetView *actor);

void Crow_InitActionTable(struct SceneActorRegistryResetView *p);

void Crow_BeginScriptedAction(struct SceneActorRegistryResetView *s);

void Crow_ResetGroundState(struct SceneActorRegistryResetView *actor);

void Crow_UpdateGroundHeight(struct SceneActorRegistryResetView *actor);

void Crow_InitColliders(u8 *p);

u32 Crow_GetTimedAction(struct SceneActorRegistryResetView *p);

void Crow_InitProfile(struct SceneActorRegistryResetView *p);

void Crow_DisableAttackCollision(struct SceneActorRegistryResetView *s);

void Crow_GetDirectionalFrameAndFlip(u16 a, s32 *b, s32 *c);

void Crow_StopAtRaisedTerrain(struct SceneActorRegistryResetView *actor);

void Crow_PlayAnimationCue(struct SceneActorRegistryResetView *p);

s32 Crow_TryActivatePlayerProximityResponse(struct SceneActorRegistryResetView *s);

void Crow_BeginReaction(struct SceneActorRegistryResetView *s);

void Crow_ReadSpawnScript(void *input);

u32 Crow_UpdatePreAction(struct SceneActorRegistryResetView *actor);

u32 Crow_UpdatePostAction(struct SceneActorRegistryResetView *actor);

void Crow_UpdateKnockback(struct SceneActorRegistryResetView *actor);

void Crow_ApproachPlayerThenRecoil(struct SceneActorRegistryResetView *actor);

void Crow_ReturnToGround(struct SceneActorRegistryResetView *s);

void Crow_SetBehaviorTable(struct SceneActorRegistryResetView *p);

u32 Crow_Update(struct SceneActorRegistryResetView *p);

u32 Crow_InitPlayerTarget(struct SceneActorRegistryResetView *p);

u32 Crow_Destroy(struct SceneActorRegistryResetView *s);

s32 Crow_Init(void *object);

void Crow_Create(void);

#endif
