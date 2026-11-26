#ifndef GUARD_CROW_H

#define GUARD_CROW_H

#include "scene/actor/scene_actor_reset.h"

#include "npc/manager/scene_actor_animation.h"

#include "scene/actor/scene_actor_control_phase.h"

#include "scene/motion/scene_actor_chase_reaction.h"

#include "scene/actor/scene_actor_control_motion.h"

#include "scene/motion/scene_actor_terrain.h"

#include "scene/motion/scene_actor_availability.h"

#include "scene/motion/scene_actor_motion.h"

#include "scene/motion/scene_actor_route.h"

#include "npc/cutscene_actor/actor_animation_controller.h"

#include "engine/video/display_sequence.h"

#include "engine/engine.h"

#include "scene/actor/scene_actor_behavior.h"
#include "link/session/room_actor.h"

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

struct CrowSignedFlags {
    s32 bits : 16;
};

void SceneActorControl_OnIdle(u8 *);

void Crow_OnAttackContact(u8 *unused, struct ContactCollider *obj, struct ChaseActor *ctx);

void Crow_OnBodyContact(struct SceneActorBehaviorContactView *source,
                        struct SceneActorBehaviorContactView *destination,
                        struct ChaseActor *actor);

void Crow_OnIdleAnimation(struct ChaseActor *p);

void Crow_UpdatePeriodicAnimation(struct ChaseActor *actor, s32 tick);

void Crow_UpdateAnimationSequence(struct ChaseActor *actor);

void Crow_PlayEntry2(struct ChaseActor *actor);

void Crow_UpdateDirectionalPose(struct RoomActor *actor);

void Crow_OnSpecialAnimation(struct ChaseActor *p);

void Crow_RecoverFromDisplacement(struct ChaseActor *actor, s32 elapsed);

void Crow_UpdateDefeat(struct ChaseActor *actor, s32 frame);

void Crow_HopWithinHomeCell(struct ChaseActor *actor);

void Crow_SteerAroundPlayer(struct ChaseActor *actor);

void Crow_UpdateWanderTimer(struct ChaseActor *actor);

void Crow_UpdateReturnAnimation(struct ChaseActor *s);

void Crow_ApproachPlayer(struct ChaseActor *actor);

void Crow_InitActionTable(struct ChaseActor *p);

void Crow_BeginScriptedAction(struct ChaseActor *s);

void Crow_ResetGroundState(struct ChaseActor *actor);

void Crow_UpdateGroundHeight(struct ChaseActor *actor);

void Crow_InitColliders(u8 *p);

u32 Crow_GetTimedAction(struct ChaseActor *p);

void Crow_InitProfile(struct ChaseActor *p);

void Crow_DisableAttackCollision(struct ChaseActor *s);

void Crow_GetDirectionalFrameAndFlip(u16 a, s32 *b, s32 *c);

void Crow_StopAtRaisedTerrain(struct ChaseActor *actor);

void Crow_PlayAnimationCue(struct ChaseActor *p);

s32 Crow_TryActivatePlayerProximityResponse(struct ChaseActor *s);

void Crow_BeginReaction(struct ChaseActor *s);

void Crow_ReadSpawnScript(void *input);

u32 Crow_UpdatePreAction(struct ChaseActor *actor);

u32 Crow_UpdatePostAction(struct ChaseActor *actor);

void Crow_UpdateKnockback(struct ChaseActor *actor);

void Crow_ApproachPlayerThenRecoil(struct ChaseActor *actor);

void Crow_ReturnToGround(struct ChaseActor *s);

void Crow_SetBehaviorTable(struct ChaseActor *p);

u32 Crow_Update(struct ChaseActor *p);

u32 Crow_InitPlayerTarget(struct ChaseActor *p);

u32 Crow_Destroy(struct ChaseActor *s);

s32 Crow_Init(void *object);

void Crow_Create(void);

#endif
