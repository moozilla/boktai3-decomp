#ifndef GUARD_GHOST_H

#define GUARD_GHOST_H

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

/* Existing component macros shared by position addition and retreat construction.
 * Plain assignments change register allocation in the approach-route routine. */
#define GHOST_WRITE_COMPONENT(out_, field_, value_)                                                \
    do {                                                                                           \
        (out_)->field_ = (value_);                                                                 \
    } while (0)
#define GHOST_SET_VERTICAL(out_, height_)                                                          \
    do {                                                                                           \
        GHOST_WRITE_COMPONENT(out_, y, height_);                                                   \
    } while (0)

struct GhostVisualResource {
    u8 unknown0[0xC];
    u32 paletteWord;
    u8 unknown10[0xC];
};

struct GhostVisualSequence {
    struct GhostVisualResource animation;
    struct ActorAnim anim;
    void *resource;
    u16 life, unknown5E;
};

struct GhostVisual {
    struct ActorSprite sprite;
    u8 unknown24[8];
    struct GhostVisualSequence sequence;
};

struct GhostRouteCursor {
    u8 unknown0[2], pointIndex, phase;
    struct SceneActorRouteDefinition *definition;
    struct SceneActorRoutePoint *point;
    u8 currentPoint, branches[2], activeBranch;
    u8 routeId, parameterB, pointId, unknown13;
};

/* Complete species allocation. Private field interpretations are tentative. */

struct Ghost {
    struct RoomActor base;
    struct GhostVisual visuals[4];
    u16 visualIndex, visualInterval;
    u32 flags, approachDistance;
    u8 drainInterval, drainDuration;
    u16 drainCounter;
    u16 baseHeight, routeSpeed;
    struct ContactPosition points[9];
    u8 index, unknown5B1;
    u16 visibilityTimer, curveTime;
    u8 flightPhase;
    s8 curveDirection;
    s16 curve;
    u8 reactionDelay, revealTicks, particleStyle, drainElapsed, drainStart, unknown5BF;
    u16 effectScript;
    u8 unknown5C2[2];
    u32 triggerRange;
    struct SceneActorMotionSubject curveSubject;
    u8 unknown5DC[0x10];
    struct ContactPosition start, end, motion, previousPlayer;
    struct GhostRouteCursor route;
    u8 unknown620[2];
    u16 delayDuration, delay;
    u8 unknown626[6];
};

struct GhostSignedFlags16 {
    s32 bits : 16;
};
struct GhostPackedPosition {
    u32 xy, z;
};

union GhostColliderVector {
    struct ContactPackedVector packed;
    struct ContactPosition position;
};

void SceneActorControl_OnIdle(u8 *);

static inline s32 Ghost_TerrainCell(s32 x, s32 z)
{
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

void Ghost_NoOp(void);

void Ghost_BeginContactReaction(void *unused, struct ContactCollider *contact,
                                struct ChaseActor *input);

void SceneActorBehavior_OnContactForward(u16 *source, void *destination, struct ChaseActor *actor);

void Ghost_UpdateEnergyDrainAnimation(struct RoomActor *actor);

void Ghost_UpdateDirectionalPose(struct RoomActor *actor);

void Ghost_UpdateDirectionalPoseB(struct RoomActor *actor);

void Ghost_TurnTowardHeading(struct ChaseActor *actor);

void Ghost_UpdateWaitingAnimation(struct RoomActor *actor, s32 frame);

void Ghost_UpdateRecoilAnimation(struct ChaseActor *actor, s32 tick);

void Ghost_UpdateHitStagger(struct ChaseActor *actor, s32 tick);

void Ghost_UpdateDefeat(struct ChaseActor *actor);

void Ghost_FollowRoute(struct ChaseActor *actor);

void Ghost_DecelerateMovement(struct ChaseActor *a);

void Ghost_ChooseRouteDestination(struct ChaseActor *actor);

void Ghost_RecoilTowardEndpoint(struct ChaseActor *actor);

void Ghost_AdvanceApproachRoute(struct ChaseActor *actor);

void Ghost_InitMotionPreset(struct ChaseActor *p);

void SceneActor_BeginLinkedIdleReaction(struct ChaseActor *s);

void Ghost_RequestVisibility(struct ChaseActor *input, const u16 *inputRequest);

void SceneActor_EasePlayerHeight(struct ChaseActor *actor);

void Ghost_UpdateCurveSway(struct ChaseActor *p);

u8 Ghost_SelectRandomRouteEntry(struct SceneActorRouteCursor *p);

void SceneActor_InitLinkedReactionColliders(struct ChaseActor *actor);

s32 Ghost_GetBehaviorCode(void);

void Ghost_SetMovementEndpoints(struct ChaseActor *p, struct ContactPosition *q);

u8 Ghost_AdvanceMovementCurve(struct ChaseActor *actor);

void Ghost_InitActorProfile(struct ChaseActor *p);

void Ghost_ResetMovementState(struct ChaseActor *actor);

void SceneActor_EndLinkedReaction(struct ChaseActor *actor);

void Ghost_GetDirectionalFrame(u16 heading, s32 *variant, s32 *flags);

void Ghost_StartMovementToPosition(struct ChaseActor *s, s16 *v);

void Ghost_ClearActionFlags(struct ChaseActor *s);

void Ghost_UpdateRandomDestination(struct ChaseActor *actor);

void Ghost_ReactToPlayerMovement(struct ChaseActor *actor);

u32 Ghost_GetControlStatus(void);

void Ghost_OnControlCleanup(void);

void Ghost_SetAfterimagePaletteWord(struct ChaseActor *p, u32 value);

void SceneActor_InitLinkedSprites(struct ChaseActor *actor);

void Ghost_EmitAfterimage(struct ChaseActor *actor);

void ActorContact_TickAuxiliaryTimers(struct ChaseActor *actor);

void Ghost_RemoveAuxiliarySprites(struct Ghost *p);

void Ghost_BuildMovementCurve(struct ChaseActor *actor, struct ContactPosition *target);

void Ghost_TickVisibilityTransition(struct ChaseActor *input);

void ActorVisibility_TickRevealDelay(struct ChaseActor *a);

void Ghost_EndPlayerSlowdown(struct ChaseActor *s);

void ActorTick_DecrementCounterAA(struct ChaseActor *actor);

u32 Ghost_UpdateVisibility(struct ChaseActor *a);

u32 ActorContact_UpdateColliderPositions(struct SceneActorBehaviorView *a);

void Ghost_FollowDrainedPlayer(struct ChaseActor *actor);

void Ghost_FollowPlayerLoop(struct ChaseActor *actor);

void Ghost_PursuePlayerLoop(struct ChaseActor *actor);

void Ghost_InitActionTable(struct ChaseActor *p);

void Ghost_ApproachPlayer(struct ChaseActor *actor);

void Ghost_ChooseMovementDestination(struct ChaseActor *actor);

void Ghost_InitReactionTable(struct ChaseActor *p);

void Ghost_UpdateFacingTrigger(struct ChaseActor *actor);

void Ghost_UpdateTriggerControl(struct ChaseActor *actor);

void Ghost_InitEffectTable(struct ChaseActor *p);

u32 Ghost_OnBehaviorUpdate(struct ChaseActor *p);

u32 Ghost_OnActorUpdate(struct ChaseActor *p);

u32 Ghost_Destroy(struct ChaseActor *s);

s32 Ghost_Init(void *object);

void Ghost_Create(void);

#endif
