#ifndef GUARD_BEE_H
#define GUARD_BEE_H
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_control_motion.h"
#include "scene_actor_terrain.h"
#include "scene_actor_availability.h"
#include "scene_actor_motion.h"
#include "display_sequence.h"
#include "engine.h"
#include "room_actor.h"
/* Tentative private state of the complete 0x490-byte Bee allocation. */
struct Bee {
    struct RoomActor base;
    u32 flags;
    s16 initialHeight;
    u16 previousGround;
    u8 behaviorCooldown, routeReset, unknown3DE, savedFacing;
    struct ContactPosition points[9];
    u8 index, behaviorDelay, routeCooldown, actionCooldown;
    u16 duration;
    u8 unknown42E[2];
    u16 curveTime;
    u8 flightPhase, curveActive;
    s16 curve;
    u8 curveDirection, curveMode;
    u8 unknown438[2];
    s16 remaining;
    u16 bodyId, polarSpeed;
    struct SceneActorMotionSubject *anchor;
    struct SceneActorMotionSubject curveSubject;
    u8 unknown458[0x18];
    struct ContactPosition start, end, motion, initialPosition;
};
struct BeePackedPair {
    u32 lo : 16, hi : 16;
};
struct BeeCollisionVector {
    struct BeePackedPair a, b;
};
struct BeePackedPosition {
    u32 xy, z;
};
struct BeeDisplacement {
    s16 x, y, z;
};
struct BeeSignedFlag {
    s32 bits : 16;
};
void SceneActorControl_OnIdle(u8 *);
void Bee_OnAttackCollision(void);
void Bee_ApplyContactReaction(struct SceneActorBehaviorContactView *source,
                              struct SceneActorBehaviorContactView *destination,
                              struct ChaseActor *actor);
void Bee_UpdateTurnAnimation(struct RoomActor *actor);
void Bee_ResetIdleAnimation(struct ChaseActor *actor);
void Bee_ResetReactionAnimation(struct ChaseActor *actor);
void Bee_UpdateRecoveryAnimation(struct ChaseActor *actor);
void Bee_UpdateFacingAnimation(struct RoomActor *actor);
void Bee_UpdateMovementStop(struct ChaseActor *actor);
void Bee_UpdateDefeat(struct ChaseActor *actor, s32 frame);
void Bee_ReturnToPlayerWhenStopped(struct ChaseActor *actor);
void Bee_InitActionPreset(struct ChaseActor *actor);
void Bee_StartMoveToBody(struct ChaseActor *actor, u16 *args);
void Bee_ResetRoute(struct ChaseActor *actor);
void Bee_UpdateFlightHeight(struct ChaseActor *actor);
void Bee_InitColliders(u8 *p);
u32 Bee_GetMovementAnimationCode(struct ChaseActor *actor);
void Bee_SetMovementEndpoints(struct ChaseActor *actor, struct ContactPosition *target);
u8 Bee_AdvanceMovementCurve(struct ChaseActor *actor);
void Bee_InitProfile(struct ChaseActor *actor);
void Bee_DisableMovement(struct ChaseActor *actor);
void Bee_BeginMoveToPosition(struct ChaseActor *actor, struct ContactPosition *target);
void Bee_GetDirectionalFrameAndFlip(u16 a, s32 *b, s32 *c);
void Bee_TickBehaviorCooldown(struct ChaseActor *actor);
void Bee_BuildMovementCurve(struct ChaseActor *actor, struct ContactPosition *target);
void *Bee_SubVector(void *unused, struct BeeDisplacement *a, struct BeeDisplacement *b);
void SceneActorCollision_FireSecondaryAttack(struct ChaseActor *actor);
u32 Bee_OnDetectionCheck(struct ChaseActor *actor);
void Slime_SetStateFlag(struct ChaseActor *actor);
u32 Bee_GetMovementVariant(struct ChaseActor *actor);
void Bee_TickActionCooldown(struct Bee *context);
u32 Bee_UpdateGroundHeight(struct ChaseActor *actor);
u32 Bee_UpdateTimersAndContacts(u8 *p);
void Bee_FollowWanderRoute(struct ChaseActor *actor);
void SceneActorControl_TickPendingTarget(struct ChaseActor *actor);
void Bee_UpdateReactionPhase(struct ChaseActor *actor);
void Bee_ApproachSavedPlayerPosition(struct ChaseActor *actor);
void Bee_UpdateWanderPause(struct ChaseActor *actor);
void Bee_FollowAnchorRoute(struct ChaseActor *actor);
void Bee_InitBehaviorHandlers(struct ChaseActor *actor);
void Bee_InitReactionHandlers(struct ChaseActor *actor);
u32 Bee_OnUpdate(struct ChaseActor *actor);
u32 Bee_OnPlayerInteraction(struct ChaseActor *actor);
u32 Bee_Destroy(struct ChaseActor *actor);
s32 Bee_Init(void *object);
void Bee_Create(void);
#endif
