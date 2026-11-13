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
    void *object;
    void *drawable;
    u8 drawableKind;
    u8 unknown9[0x37];
    u8 movementProbe[0x10];
    struct SceneActorBody body;
    u8 category;
    u8 unknown99;
    u16 field9A;
    s16 health;
    u8 field9E, field9F, fieldA0, fieldA1;
    u16 fieldA2, fieldA4;
    s16 attachmentLimit; /* +0xA6: signed attachment restart threshold. */
    s16 fieldA8;
    u16 fieldAA;
    u32 fieldAC;
    s16 defaultHealth;
    u16 fieldB2;
    struct SceneActorAttackParameters attacks;
    struct SceneActorChaseTracking tracking;
    u8 unknown100[4];
    u32 word104;
    u32 word108, word10C, word110;
    u32 flags114;
    struct SceneActorDefaultControl saved;
    struct ContactCollider colliderA;
    struct ContactCollider colliderB;
    u16 alternatePalette; /* +0x1DC: selected by status bit 0x4000. */
    u16 paletteBank;
    u16 basePaletteBank;
    u16 paletteAlternateBank;
    u16 alternateOffset; /* +0x1E4: alternate palette offset. */
    u16 paletteOffset;
    u16 field1E8;
    u16 field1EA;
    u32 field1EC, field1F0;
    u8 unknown1F4[0x28];
    void *target;
    void *field220;
    void *entryTarget224;
    void *entryTarget228;
    void *savedTarget;
    void *reactionTarget230;
    void *deathControl; /* +0x234: controller installed by defeat dispatch. */
    void *field238, *field23C;
    void *field240;
    u8 unknown244[4];
    void *callback248;
    void *messageCallback;
    union SceneActorCommandTable commandTable;
    u8 unknown254[4];
    void *updateCallback, *destroyCallback;
    union SceneActorAnimationQuerySlot animationQuery;
    void *damageCallback, *contactCallback;
    void *update26C;
    union SceneActorContactQuerySlot contactQuery;
    void *contextCallback, *defaultReaction;
    void **callbacks27C, **callbacks280, **callbacks284;
    void **alternateBehaviorCallbacks; /* +0x288: indexed by behavior2AA. */
    void **behaviorCallbacks;          /* +0x28C: indexed by behavior2AA. */
    void **callbacks290, **callbacks294;
    void **phaseCallbacks; /* +0x298: indexed by phase2A9. */
    void **callbacks29C, **callbacks2A0;
    u8 unknown2A4[4];
    u8 state2A8, phase2A9, behavior2AA, unknown2AB;
    u32 counter2AC;
    u8 field2B0, field2B1, field2B2, field2B3;
    u8 enabled, field2B5, field2B6, animationIndex;
    u16 field2B8;
    u8 unknown2BA[2];
    u16 defeatTimer; /* 0x2BC: set to 60 on entry to Spider_UpdateDefeat. */
    u16 groundHeight2BE;
    u16 flags2C0, field2C2;
    u32 profileMode;
    u16 field2C8;
    u16 field2CA;
    u32 flags2CC;
    u32 flags2D0;
    u16 flags2D4;
    u16 flags2D6;
    u32 field2D8, field2DC, field2E0;
    u16 reactionSoundVariant;
    u16 field2E6;
    u16 field2E8;
    u16 field2EA;
    struct ContactPosition secondaryMotion;
    u16 terrainCell;
    u16 previousTerrainCell;
    u16 savedTargetCell;
    u16 regionId;
    struct ContactPosition previousPosition;
    u32 field304;
    u32 profileId;
    u32 field30C;
    struct ContactPosition reactionStart,
        reactionEnd; /* +0x310/+0x318: reaction position snapshots. */
    u8 shadow[0xAC]; /* +0x320: first byte gates the shadow enable/disable API. */
    u8 field3CC, unknown3CD[3];
    void *context;
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
                                struct SceneActorRegistryResetView *input);

void SceneActorBehavior_OnContactForward(u16 *source, void *destination,
                                         struct SceneActorRegistryResetView *actor);

void Ghost_UpdateEnergyDrainAnimation(struct SceneActorAnimationView *actor);

void Ghost_UpdateDirectionalPose(struct SceneActorAnimationView *actor);

void Ghost_UpdateDirectionalPoseB(struct SceneActorAnimationView *actor);

void Ghost_TurnTowardHeading(struct SceneActorRegistryResetView *actor);

void Ghost_UpdateWaitingAnimation(struct SceneActorAnimationView *actor, s32 frame);

void Ghost_UpdateRecoilAnimation(struct SceneActorRegistryResetView *actor, s32 tick);

void Ghost_UpdateHitStagger(struct SceneActorRegistryResetView *actor, s32 tick);

void Ghost_UpdateDefeat(struct SceneActorRegistryResetView *actor);

void Ghost_FollowRoute(struct SceneActorRegistryResetView *actor);

void Ghost_DecelerateMovement(struct SceneActorRegistryResetView *a);

void Ghost_ChooseRouteDestination(struct SceneActorRegistryResetView *actor);

void Ghost_RecoilTowardEndpoint(struct SceneActorRegistryResetView *actor);

void Ghost_AdvanceApproachRoute(struct SceneActorRegistryResetView *actor);

void Ghost_InitMotionPreset(struct SceneActorRegistryResetView *p);

void SceneActor_BeginLinkedIdleReaction(struct SceneActorRegistryResetView *s);

void Ghost_RequestVisibility(struct SceneActorRegistryResetView *input, const u16 *inputRequest);

void SceneActor_EasePlayerHeight(struct SceneActorRegistryResetView *actor);

void Ghost_UpdateCurveSway(struct SceneActorRegistryResetView *p);

u8 Ghost_SelectRandomRouteEntry(struct SceneActorRouteCursor *p);

void SceneActor_InitLinkedReactionColliders(struct SceneActorRegistryResetView *actor);

s32 Ghost_GetBehaviorCode(void);

void Ghost_SetMovementEndpoints(struct SceneActorRegistryResetView *p, struct ContactPosition *q);

u8 Ghost_AdvanceMovementCurve(struct SceneActorRegistryResetView *actor);

void Ghost_InitActorProfile(struct SceneActorRegistryResetView *p);

void Ghost_ResetMovementState(struct SceneActorRegistryResetView *actor);

void SceneActor_EndLinkedReaction(struct SceneActorRegistryResetView *actor);

void Ghost_GetDirectionalFrame(u16 heading, s32 *variant, s32 *flags);

void Ghost_StartMovementToPosition(struct SceneActorRegistryResetView *s, s16 *v);

void Ghost_ClearActionFlags(struct SceneActorRegistryResetView *s);

void Ghost_UpdateRandomDestination(struct SceneActorRegistryResetView *actor);

void Ghost_ReactToPlayerMovement(struct SceneActorRegistryResetView *actor);

u32 Ghost_GetControlStatus(void);

void Ghost_OnControlCleanup(void);

void Ghost_SetAfterimagePaletteWord(struct SceneActorRegistryResetView *p, u32 value);

void SceneActor_InitLinkedSprites(struct SceneActorRegistryResetView *actor);

void Ghost_EmitAfterimage(struct SceneActorRegistryResetView *actor);

void ActorContact_TickAuxiliaryTimers(struct SceneActorRegistryResetView *actor);

void Ghost_RemoveAuxiliarySprites(struct Ghost *p);

void Ghost_BuildMovementCurve(struct SceneActorRegistryResetView *actor,
                              struct ContactPosition *target);

void Ghost_TickVisibilityTransition(struct SceneActorRegistryResetView *input);

void ActorVisibility_TickRevealDelay(struct SceneActorRegistryResetView *a);

void Ghost_EndPlayerSlowdown(struct SceneActorRegistryResetView *s);

void ActorTick_DecrementCounterAA(struct SceneActorRegistryResetView *actor);

u32 Ghost_UpdateVisibility(struct SceneActorRegistryResetView *a);

u32 ActorContact_UpdateColliderPositions(struct SceneActorBehaviorView *a);

void Ghost_FollowDrainedPlayer(struct SceneActorRegistryResetView *actor);

void Ghost_FollowPlayerLoop(struct SceneActorRegistryResetView *actor);

void Ghost_PursuePlayerLoop(struct SceneActorRegistryResetView *actor);

void Ghost_InitActionTable(struct SceneActorRegistryResetView *p);

void Ghost_ApproachPlayer(struct SceneActorRegistryResetView *actor);

void Ghost_ChooseMovementDestination(struct SceneActorRegistryResetView *actor);

void Ghost_InitReactionTable(struct SceneActorRegistryResetView *p);

void Ghost_UpdateFacingTrigger(struct SceneActorRegistryResetView *actor);

void Ghost_UpdateTriggerControl(struct SceneActorRegistryResetView *actor);

void Ghost_InitEffectTable(struct SceneActorRegistryResetView *p);

u32 Ghost_OnBehaviorUpdate(struct SceneActorRegistryResetView *p);

u32 Ghost_OnActorUpdate(struct SceneActorRegistryResetView *p);

u32 Ghost_Destroy(struct SceneActorRegistryResetView *s);

s32 Ghost_Init(void *object);

void Ghost_Create(void);

#endif
