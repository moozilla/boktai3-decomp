#ifndef GUARD_GHOUL_H
#define GUARD_GHOUL_H

#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
#include "display_sequence.h"

/* Ghoul's allocation shares the actor prefix through the pursuit group.
 * The species parameters and flag meanings are tentative. */
struct Ghoul {
    void *object;
    void *drawable;
    u8 drawableKind;
    u8 unknown9[0x47];
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
    struct SceneActorChaseGroup group;
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

struct GhoulRouteOrigin {
    u8 unknown0[0x1C];
    s16 x;
    u8 unknown1E[2];
    s16 z;
};

void Ghoul_OnAttackCollision(struct ContactCollider *, struct ContactCollider *,
                             struct SceneActorRegistryResetView *);
void Ghoul_OnBodyCollision(void);
void Ghoul_UpdateAimedDiscThrow(struct SceneActorRegistryResetView *);
void Ghoul_UpdateIdleState(struct SceneActorRegistryResetView *);
void Ghoul_UpdateStandingState(struct SceneActorRegistryResetView *);
void Ghoul_TurnTowardRoute(struct SceneActorRegistryResetView *);
void Ghoul_UpdateDyingState(struct SceneActorRegistryResetView *, s32);
void Ghoul_UpdateDeathAnimation(struct SceneActorRegistryResetView *, s32);
void Ghoul_UpdateHiddenRecovery(struct SceneActorRegistryResetView *, s32, u32);

void Ghoul_UpdateDisplacementRecovery(struct SceneActorRegistryResetView *actor, s32 frame);
void Ghoul_UpdateDiscThrow(struct SceneActorRegistryResetView *actor);
void Ghoul_UpdateScriptedAction(struct SceneActorRegistryResetView *obj);
void Ghoul_ResumeDelayedChase(struct SceneActorRegistryResetView *actor);
void Ghoul_SetBehaviorHandlers(struct SceneActorRegistryResetView *actor);
void Ghoul_UpdateProbeApproach(struct SceneActorRegistryResetView *actor);
void SceneActorControl_RestartOrAdvance(struct SceneActorRegistryResetView *actor);
void Ghoul_WaitForRouteResult(struct SceneActorRegistryResetView *actor);
void Ghoul_SetActionHandlers(struct SceneActorRegistryResetView *actor);
void Ghoul_UpdateAimedDiscControl(struct SceneActorRegistryResetView *actor);
void Ghoul_SetSpecialHandlers(struct SceneActorRegistryResetView *actor);
void Ghoul_EnterIdleControl(struct SceneActorRegistryResetView *actor);
void SceneActorControl_RefreshMotionRoute_0808A90C(struct SceneActorRegistryResetView *actor);
void Ghoul_ResumeContactRoute(struct SceneActorRegistryResetView *actor);
void Ghoul_InitCollision(struct SceneActorRegistryResetView *actor);
void Ghoul_ResetRouteState(struct SceneActorRegistryResetView *actor);
void Ghoul_OnLifecycleEvent(void);
void Ghoul_BindProfile(struct SceneActorRegistryResetView *actor);
u32 Bat_LookupAnimationEntry(void *object);
void Ghoul_TickCooldown(struct SceneActorRegistryResetView *actor);
void Ghoul_SelectRandomRoutePoint(struct SceneActorRegistryResetView *actor);
void Ghoul_SelectRetreatRoutePoint(struct SceneActorRegistryResetView *actor);
void Ghoul_BeginAlternateRoute(struct SceneActorRegistryResetView *actor);
void Vorn_ParseField624_0808B320(struct SceneActorRegistryResetView *actor);
void Bat_TickDelayedScript(struct SceneActorRegistryResetView *actor);
u8 Ghoul_ReactToContact(struct SceneActorRegistryResetView *actor);
s32 Ghoul_ResumePendingRoute(struct SceneActorRegistryResetView *actor);
u32 Ghoul_UpdateCooldown(struct SceneActorRegistryResetView *actor);
void Ghoul_UpdateSpecialActionControl(struct SceneActorRegistryResetView *actor);
void Ghoul_UpdateAnimationElevenControl(struct SceneActorRegistryResetView *actor);
void Ghoul_UpdateAttackControl(struct SceneActorRegistryResetView *actor);
void Ghoul_SetBehaviorTable(struct SceneActorRegistryResetView *actor);
s32 Ghoul_Update(struct SceneActorRegistryResetView *actor);
u32 Ghoul_OnMessage(struct SceneActorRegistryResetView *actor);
u32 Ghoul_Destroy(struct SceneActorRegistryResetView *actor);
s32 Ghoul_Init(void *object);
void Ghoul_Create(void);

u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
void SceneActorReaction_PlayAnimationBUntilComplete(void);

#endif
