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
#include "room_actor.h"

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
    struct RoomActor base;
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
    if (actor->base.field2B1) {
        actor->base.field2B1 = 0;
        return 1;
    }
    return 0;
}

static inline void MimicChase_SetState(struct SceneActorRegistryResetView *actor, u32 s, u32 p,
                                       u32 b)
{
    u32 behavior = b;
    actor->base.state2A8 = s;
    actor->base.phase2A9 = p;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
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
        MimicChase_ClearBits(&actor->base.flags2D4, 0x200);
        return 0;
    }
    result = Terrain_ApplyBlocked(route, position, target);
    if (result) {
        if (result != 2)
            Memory_Clear(&actor->base.body.motion, 8);
        return 1;
    }
    MimicChase_ClearBits(&actor->base.flags2D4, 0x200);
    return 0;
}

s32 Nav_IsPositionInMesh(void *, struct ContactPosition *, struct ContactPosition *);

s32 Terrain_ApplyBlocked(void *, struct ContactPosition *, struct ContactPosition *);

static inline u8 SceneActorReaction_TakePrepared(struct SceneActorRegistryResetView *actor)
{
    if (actor->base.field2B2) {
        actor->base.field2B2 = 0;
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
void *Bestiary_GetEntryForActor(u32, u32);
void Bestiary_UnlockEntry(void *);
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
void SceneActor_EnterFlaggedEntryPhase(struct RoomActor *p);
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
void SceneActor_EnterShadowlessPhase(struct RoomActor *p);
void SceneActor_WaitForCategoryAttachment(struct SceneActorRegistryResetView *p, s32 frame);
void Mimic_UpdateIdleState(void);
void Mimic_BeginActionResponse(struct SceneActorRegistryResetView *actor);
void Mimic_Create(void);
#endif
