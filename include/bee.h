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
/* Tentative private state of the complete 0x490-byte Bee allocation. */
struct Bee {
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
struct BeeDrawable {
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
                              struct SceneActorRegistryResetView *actor);
void Bee_UpdateTurnAnimation(struct SceneActorAnimationView *actor);
void Bee_ResetIdleAnimation(struct SceneActorRegistryResetView *actor);
void Bee_ResetReactionAnimation(struct SceneActorRegistryResetView *actor);
void Bee_UpdateRecoveryAnimation(struct SceneActorRegistryResetView *actor);
void Bee_UpdateFacingAnimation(struct SceneActorAnimationView *actor);
void Bee_UpdateMovementStop(struct SceneActorRegistryResetView *actor);
void Bee_UpdateDefeat(struct SceneActorRegistryResetView *actor, s32 frame);
void Bee_ReturnToPlayerWhenStopped(struct SceneActorRegistryResetView *actor);
void Bee_InitActionPreset(struct SceneActorRegistryResetView *actor);
void Bee_StartMoveToBody(struct SceneActorRegistryResetView *actor, u16 *args);
void Bee_ResetRoute(struct SceneActorRegistryResetView *actor);
void Bee_UpdateFlightHeight(struct SceneActorRegistryResetView *actor);
void Bee_InitColliders(u8 *p);
u32 Bee_GetMovementAnimationCode(struct SceneActorRegistryResetView *actor);
void Bee_SetMovementEndpoints(struct SceneActorRegistryResetView *actor,
                              struct ContactPosition *target);
u8 Bee_AdvanceMovementCurve(struct SceneActorRegistryResetView *actor);
void Bee_InitProfile(struct SceneActorRegistryResetView *actor);
void Bee_DisableMovement(struct SceneActorRegistryResetView *actor);
void Bee_BeginMoveToPosition(struct SceneActorRegistryResetView *actor,
                             struct ContactPosition *target);
void Bee_GetDirectionalFrameAndFlip(u16 a, s32 *b, s32 *c);
void Bee_TickBehaviorCooldown(struct SceneActorRegistryResetView *actor);
void Bee_BuildMovementCurve(struct SceneActorRegistryResetView *actor,
                            struct ContactPosition *target);
void *Bee_SubVector(void *unused, struct BeeDisplacement *a, struct BeeDisplacement *b);
void SceneActorCollision_FireSecondaryAttack(struct SceneActorRegistryActor *actor);
u32 Bee_OnDetectionCheck(struct SceneActorRegistryResetView *actor);
void Slime_SetStateFlag(struct SceneActorRegistryResetView *actor);
u32 Bee_GetMovementVariant(struct SceneActorRegistryResetView *actor);
void Bee_TickActionCooldown(struct Bee *context);
u32 Bee_UpdateGroundHeight(struct SceneActorRegistryResetView *actor);
u32 Bee_UpdateTimersAndContacts(u8 *p);
void Bee_FollowWanderRoute(struct SceneActorRegistryResetView *actor);
void SceneActorControl_TickPendingTarget(struct SceneActorRegistryResetView *actor);
void Bee_UpdateReactionPhase(struct SceneActorRegistryResetView *actor);
void Bee_ApproachSavedPlayerPosition(struct SceneActorRegistryResetView *actor);
void Bee_UpdateWanderPause(struct SceneActorRegistryResetView *actor);
void Bee_FollowAnchorRoute(struct SceneActorRegistryResetView *actor);
void Bee_InitBehaviorHandlers(struct SceneActorRegistryResetView *actor);
void Bee_InitReactionHandlers(struct SceneActorRegistryResetView *actor);
u32 Bee_OnUpdate(struct SceneActorRegistryResetView *actor);
u32 Bee_OnPlayerInteraction(struct SceneActorRegistryResetView *actor);
u32 Bee_Destroy(struct SceneActorRegistryResetView *actor);
s32 Bee_Init(void *object);
void Bee_Create(void);
#endif
