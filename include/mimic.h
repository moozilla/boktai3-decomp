#ifndef GUARD_MIMIC_H

#define GUARD_MIMIC_H

#include "scene_actor_chase_reaction.h"

#include "scene_actor_attachment.h"

#include "scene_actor_availability.h"

#include "scene_actor_control_phase.h"

#include "scene_actor_restoration.h"

#include "scene_actor_placement.h"

#include "actor_animation_controller.h"

#include "scene_actor_init_dispatch.h"

#include "scene_actor_init_group.h"

#include "scene_actor_init_graphics.h"

#include "scene_actor_init_body.h"

#include "scene_body_api.h"

#include "display_sequence_inline.h"

#include "scene_actor_control_motion.h"

#include "scene_actor_displacement.h"

#include "player.h"

#include "script_utility.h"

struct MimicReactionFlags {
    s16 bits : 16;
};

struct MimicNavigationProbe {
    u16 flags, unknown2, region, polygon;
    u32 unknown8;
    struct ContactPosition origin, previous, destination;
};

/* Deferred-response packet begins at the pursuit group's saved-control slot. */

struct MimicDeferredResponse {
    struct SceneActorChaseSavedControl snapshot;
    u8 state, phase, behavior, unknownB;
    u32 delay;
    void *control;
};

/* Complete 0x664 allocation, with the common actor prefix kept flat. */

struct Mimic {
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
    u16 defeatTimer;
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
    struct MimicNavigationProbe routeProbe;
    struct ContactPosition destination;
    struct ContactPosition origin;
    u16 originCell, timer;
    u16 reactionFlags;
    u8 leapSpeed, unknown65F;
    s32 healthThreshold;
};

static inline u8 MimicChase_TakeArrived(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1) {
        actor->field2B1 = 0;
        return 1;
    }
    return 0;
}

static inline void MimicChase_SetState(struct SceneActorRegistryResetView *actor, u32 s, u32 p,
                                       u32 b)
{
    u32 behavior = b;
    actor->state2A8 = s;
    actor->phase2A9 = p;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline void MimicChase_ClearBits(u16 *field, u32 bits)
{
    *field &= ~bits;
}

static inline u8 MimicChase_TryRoute(struct SceneActorRegistryResetView *actor, void *route,
                                     struct ContactPosition *position,
                                     struct ContactPosition *target)
{
    s32 result = Nav_IsPositionInMesh(route, position, target);
    if (!result) {
        MimicChase_ClearBits(&actor->flags2D4, 0x200);
        return 0;
    }
    result = Terrain_ApplyBlocked(route, position, target);
    if (result) {
        if (result != 2)
            Memory_Clear(&actor->body.motion, 8);
        return 1;
    }
    MimicChase_ClearBits(&actor->flags2D4, 0x200);
    return 0;
}

s32 Nav_IsPositionInMesh(void *, struct ContactPosition *, struct ContactPosition *);

s32 Terrain_ApplyBlocked(void *, struct ContactPosition *, struct ContactPosition *);

static inline u8 SceneActorReaction_TakePrepared(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B2) {
        actor->field2B2 = 0;
        return 1;
    }
    return 0;
}

void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
extern const u8 gUnk_08606264[];
void Mimic_ResetExpiredTarget(struct SceneActorRegistryResetView *actor);
u32 SceneActor_GetDirectionFlags(s32 direction);
s32 Div(s32, s32);
s32 Mod(s32, s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
u8 Mimic_TryTargetRoute(struct SceneActorRegistryResetView *actor);
u8 Mimic_TryPursuitRoute(struct SceneActorRegistryResetView *actor);
extern const s16 gUnk_086149C4[];
u8 SceneActor_IsCollisionTargetUnavailable(struct SceneActorRegistryResetView *actor);
s32 ActorAttachment_IsUnavailable(void *);
u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);
void SparkParticle_Spawn(void *, u32, void *, s32, s32, s32, s32, s32);
void *BestiaryDisplay_GetEntryForActor(u32, u32);
void GameState_SetEventFlag(void *);
u8 SceneActorTarget_TrySelectNearestParticipant(struct SceneActorRegistryResetView *);
void SceneActor_ResetCollisionReaction(struct SceneActorRegistryResetView *actor);
void SceneActorReaction_HandleColliderContact(void *, s32);
void Mimic_OnAttackContact(u8 *unused, struct ContactCollider *obj,
                           struct SceneActorRegistryResetView *ctx);
void Mimic_OnBodyContact(void *a, s32 b);
void SceneActorCollision_InitAttack(void *, void *, void *, void (*)(void), u32, void *);
void SceneActorCollision_InitBody(void *, void *, void *, void (*)(void), void *);
extern u32 gUnk_02000560[];
void SceneActorBehavior_CallActionScript(u8 *);
void *SceneActorContacts_FindObjectById(u16);
u8 SceneActor_FindNearbyActivePlayer(struct SceneActorRegistryResetView *p);
void Mimic_BeginTargetResponse(struct SceneActorRegistryResetView *obj);
extern const u8 gUnk_086062AC[];
void SceneActorControl_DispatchControl(void *);
s32 Mimic_UpdateReactionState(struct SceneActorRegistryResetView *actor);
void SceneActor_UpdateBehaviorCallbacks(struct SceneActorRegistryResetView *);
void SceneActorEffects_ReleaseResources(struct SceneActorRegistryResetView *);
void Mimic_InitProfile(struct SceneActorRegistryResetView *actor);
void SceneActor_InitializeReactionColliders(void *p);
s32 Mimic_UpdateBehavior(struct SceneActorRegistryResetView *actor);
void SceneActor_EnterFlaggedEntryPhase(struct SceneActorAnimationView *p);
void Mimic_OnAnimationStateChanged(struct SceneActorRegistryResetView *p);
void Mimic_ConvergeAndTurn(struct SceneActorRegistryResetView *actor);
void Mimic_UpdateDefeatBlink(struct SceneActorRegistryResetView *actor, s32 frame);
s32 SceneActor_AcquireCollisionTarget(struct SceneActorRegistryResetView *actor);
u8 Mimic_StartHitReaction(struct SceneActorRegistryResetView *actor);
s32 Mimic_OnBehaviorComplete(void);
void Mimic_ResetPursuitDrawable(struct SceneActorRegistryResetView *actor);
void SceneActor_PrepareStationaryReaction(struct SceneActorRegistryResetView *actor);
void SceneActor_RestoreSavedTargetOnCompletion(struct SceneActorRegistryResetView *p);
void Mimic_InitBehaviorTable(struct SceneActorRegistryResetView *actor);
void Mimic_InitActionTable(struct SceneActorRegistryResetView *actor);
s32 Mimic_OnUpdate(struct SceneActorRegistryResetView *actor);
s32 Mimic_ReleaseResources(struct SceneActorRegistryResetView *actor);
void SceneActorReaction_OnEnemyDefault(void);
u8 SceneActorTarget_UpdateTrackedContact(void *);
void SceneActorTarget_ResolvePendingObject(void);
void SceneActorReaction_ResolvePlacementDamage(void);
void SceneActorPlacement_ResetStateThree(void);
void SceneActorReaction_BeginTargetRecovery(void);
void SceneActorScript_OnCommand(void);
void SceneActorReaction_SetMotionMode(void);
void SceneActorMovement_ResolveRoutePosition(void *);
void SceneActorBehavior_InitializeShadow(void *);
u32 Route_GetCurrentPointFacing(void *);
s32 TargetRegistry_RegisterActor(void *, void *, void *);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorBehavior_InstallCallbackTables(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);
u8 SceneActorRegistry_LinkActor(void *);
s32 Mimic_Init(void *object);
void Mimic_UpdateHitRecovery(struct SceneActorRegistryResetView *actor);
void SceneActor_LeapTowardTarget(struct SceneActorRegistryResetView *actor);
void SceneActor_EnterShadowlessPhase(struct SceneActorAnimationView *p);
void SceneActor_WaitForCategoryAttachment(struct SceneActorRegistryResetView *p, s32 frame);
void Mimic_UpdateIdleState(void);
void Mimic_BeginActionResponse(struct SceneActorRegistryResetView *actor);
void Mimic_Create(void);
#endif
