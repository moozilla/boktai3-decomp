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

/* Species allocation; the roles of the private flags remain tentative. */
struct Slime {
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
    if (actor->field2B2) {
        actor->field2B2 = 0;
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
