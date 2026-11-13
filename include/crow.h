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

/* Complete 0x43C species allocation; private field roles are tentative. */

struct Crow {
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

void Crow_UpdateDirectionalPose(struct SceneActorAnimationView *actor);

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
