#ifndef GUARD_SCENE_ACTOR_INIT_ACTOR_H
#define GUARD_SCENE_ACTOR_INIT_ACTOR_H

#include "engine/shared/game_state_flags.h"
#include "engine/shared/scene_actor_state.h"
#include "engine/shared/scene_actor_init_primitives.h"
#include "system/game_state/game_state.h"

struct ChaseActor;
void SceneActorControl_OnIdle(u8 *actor);
s32 Region_FindContaining(void *position);
void SceneActorRegistry_RegisterCategoryReceiver(struct ChaseActor *actor);

static inline u32 MonsterInit_GetTerrainCell(struct ChaseActor *actor)
{
    return actor->base.terrainCell;
}

static inline struct ContactPosition *MonsterInit_BodyPosition(void *object)
{
    struct SceneActorBody *body = object;
    struct ContactPosition *position = &body->position;
    return position;
}

/* Callback slots have distinct indirect-call contracts. Write through the typed
 * slot so the compiler retains that contract even when the owner exposes a union. */
static inline void MonsterInit_WriteCallback(void **slot, void *callback)
{
    *slot = callback;
}

static inline void MonsterInit_WriteAnimationQuery(SceneActorAnimationQuery *slot,
                                                   SceneActorAnimationQuery query)
{
    *slot = query;
}

static inline void MonsterInit_WriteCommandTable(SceneActorCommandHandler const **slot,
                                                 SceneActorCommandHandler const *table)
{
    *slot = table;
}

static inline void MonsterInit_WriteDataPointer(const void **slot, const void *data)
{
    *slot = data;
}

static inline void MonsterInit_WriteHalfword(u16 *slot, u32 value)
{
    *slot = value;
}

/* Older actor views keep these dispatch slots opaque. The stored callback's
 * signature belongs to the operation that installs it. */
static inline void MonsterInit_WriteResultCallback(void *storage, SceneActorResultCallback callback)
{
    SceneActorResultCallback *slot = storage;
    *slot = callback;
}

static inline void MonsterInit_WriteActorAction(void *storage, SceneActorAction callback)
{
    SceneActorAction *slot = storage;
    *slot = callback;
}

static inline void MonsterInit_WriteDefaultReaction(void *storage,
                                                    SceneActorDefaultReaction callback)
{
    SceneActorDefaultReaction *slot = storage;
    *slot = callback;
}

static inline void MonsterInit_WriteSequenceQuery(SceneActorSequenceQuery *slot,
                                                  SceneActorSequenceQuery query)
{
    *slot = query;
}

static inline void MonsterInit_WriteContactPredicate(SceneActorContactPredicate *slot,
                                                     SceneActorContactPredicate predicate)
{
    *slot = predicate;
}

static inline void MonsterInit_WriteValueQuery(void *storage, SceneActorValueQuery query)
{
    SceneActorValueQuery *slot = storage;
    *slot = query;
}

/* Actor metadata and arithmetic queries. Stored halfword snapshots keep explicit
 * halfword return domains; masks, palette arithmetic and cell IDs use words. */
static inline u8 MonsterInit_GetDrawableKind(struct ChaseActor *actor)
{
    return actor->base.drawableKind;
}
static inline u8 *MonsterInit_Drawable(struct ChaseActor *actor)
{
    return actor->base.drawable;
}
static inline u8 MonsterInit_GetCategory(void *object)
{
    struct ChaseActor *actor = object;
    return actor->base.category;
}
static inline void MonsterInit_SetCategory(void *object, u8 category)
{
    struct ChaseActor *actor = object;
    actor->base.category = category;
}
static inline s32 MonsterInit_GetHealth(struct ChaseActor *actor)
{
    return actor->base.health;
}
static inline u16 MonsterInit_GetInitialHealth(struct ChaseActor *actor)
{
    return actor->base.defaultHealth;
}
static inline u16 MonsterInit_GetGroundHeight(struct ChaseActor *actor)
{
    return actor->base.groundHeight;
}
static inline u32 MonsterInit_GetPaletteBase(struct ChaseActor *actor)
{
    return actor->base.basePaletteBank;
}
static inline u32 MonsterInit_GetAlternatePalette(struct ChaseActor *actor)
{
    return actor->base.paletteAlternateBank;
}
static inline u32 MonsterInit_GetAlternatePaletteOffset(struct ChaseActor *actor)
{
    return actor->base.alternateOffset;
}
static inline u32 MonsterInit_GetPaletteOffset(struct ChaseActor *actor)
{
    return actor->base.paletteOffset;
}
static inline void MonsterInit_SetAlternatePaletteOffset(struct ChaseActor *actor, u32 palette)
{
    actor->base.alternateOffset = palette;
}
static inline u32 MonsterInit_GetMotionFlags(struct ChaseActor *actor)
{
    return actor->base.placementFlags;
}
static inline u16 MonsterInit_GetHeightOffset(struct ChaseActor *actor)
{
    return actor->base.heightOffset;
}
static inline u32 MonsterInit_GetBehaviorFlags(struct ChaseActor *actor)
{
    return actor->base.behaviorFlags;
}
static inline u32 MonsterInit_GetStatusFlags(struct ChaseActor *actor)
{
    return actor->base.statusFlags;
}
static inline u8 MonsterInit_GetState(struct ChaseActor *actor)
{
    return actor->base.state2A8;
}
static inline u8 MonsterInit_GetPhase(struct ChaseActor *actor)
{
    return actor->base.phase2A9;
}
static inline u8 MonsterInit_GetBehavior(struct ChaseActor *actor)
{
    return actor->base.behavior2AA;
}
static inline void *MonsterInit_GetCurrentControl(struct ChaseActor *actor)
{
    return actor->base.target;
}
static inline void *MonsterInit_GetStandingControl(struct ChaseActor *actor)
{
    return actor->base.onStanding;
}
static inline u8 MonsterInit_GetControlKind(struct ChaseActor *actor)
{
    return actor->base.controlKind;
}
static inline u8 MonsterInit_GetDirection(struct ChaseActor *actor)
{
    return actor->base.body.direction;
}
static inline struct SceneActorDefaultControl *MonsterInit_SavedControlOf(struct ChaseActor *actor)
{
    return &actor->base.saved;
}
static inline void MonsterInit_SaveControl(struct SceneActorDefaultControl *saved,
                                           struct ChaseActor *actor)
{
    saved->motionFlags = MonsterInit_GetMotionFlags(actor);
    saved->heightOffset = MonsterInit_GetHeightOffset(actor);
    saved->behaviorFlags = MonsterInit_GetBehaviorFlags(actor);
    saved->statusFlags = MonsterInit_GetStatusFlags(actor);
    saved->direction = MonsterInit_GetDirection(actor);
    saved->state = MonsterInit_GetState(actor);
    saved->phase = MonsterInit_GetPhase(actor);
    saved->behavior = MonsterInit_GetBehavior(actor);
    saved->control = MonsterInit_GetCurrentControl(actor);
    saved->kind = MonsterInit_GetControlKind(actor);
}
static inline void MonsterInit_SetBehaviorFlags(struct ChaseActor *actor, u32 mask)
{
    actor->base.behaviorFlags |= mask;
}
static inline void MonsterInit_SetStatusFlags(struct ChaseActor *actor, u32 mask)
{
    actor->base.statusFlags |= mask;
}
static inline void MonsterInit_SetMotionFlags(struct ChaseActor *actor, u16 mask)
{
    actor->base.placementFlags |= mask;
}
static inline u8 MonsterInit_TestMotionFlags(struct ChaseActor *actor, u32 mask)
{
    if (MonsterInit_GetMotionFlags(actor) & mask)
        return 1;
    return 0;
}
static inline void MonsterInit_MaskControlFlags(struct ChaseActor *actor, u32 keepMask)
{
    actor->base.pendingControlFlags &= keepMask;
}
static inline void MonsterInit_SetMotionControl(struct ChaseActor *actor, void *callback, u32 kind)
{
    actor->base.pendingControl = (u32)callback;
    actor->base.pendingControlData = 0;
    actor->base.pendingControlRelease = 0;
    actor->base.pendingControlKind = kind;
    actor->base.pendingControlFlags = 1;
}
static inline void MonsterInit_ClearMotion(void *object)
{
    struct ChaseActor *actor = object;
    u32 zero = 0;
    MonsterInit_SetMotionControl(actor, 0, 0);
    actor->base.pendingControlFlags = zero;
}
static inline void MonsterInit_SetControl(struct ChaseActor *actor, void *callback, u32 behavior)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = behavior;
    actor->base.target = callback;
    actor->base.field2C8 = 0;
    MonsterInit_MaskControlFlags(actor, ~1);
    actor->base.pendingControl = 0;
}
static inline void MonsterInit_SetState(struct ChaseActor *actor, u32 state, u32 phase,
                                        u32 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}
static inline s32 MonsterInit_ClampHealth(u16 raw)
{
    s32 health = (s16)raw;
    if (health <= 0)
        health = 0;
    return health;
}
static inline void MonsterInit_InstallIdle(struct ChaseActor *actor, void *idle)
{
    actor->base.target = idle;
}
static inline void MonsterInit_SetInitialHealth(struct ChaseActor *actor, u16 health)
{
    actor->base.defaultHealth = health;
}
static inline void MonsterInit_InitializeHealth(struct ChaseActor *actor, u16 raw, s32 firstLimit,
                                                s32 secondLimit)
{
    actor->base.motionFlags = 0;
    MonsterInit_InstallIdle(actor, SceneActorControl_OnIdle);
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.health = MonsterInit_ClampHealth(raw);
    actor->base.attachmentTimer = 0;
    if (secondLimit <= 0)
        secondLimit = firstLimit;
    actor->base.attachmentLimit = firstLimit;
    actor->base.attachmentRestart = secondLimit;
    actor->base.regionId = 0;
}
static inline void MonsterInit_SetFirstAttack(struct ChaseActor *actor, s32 index, s32 value)
{
    actor->base.attacks.first[index] = value;
}
static inline struct ChaseActor *MonsterInit_ReadAttacks(struct ChaseActor *actor)
{
    s32 i;
    if (Script_SeekToKeyword('a')) {
        i = 0;
        while (1) {
            MonsterInit_SetFirstAttack(actor, i, MonsterInit_ReadHalfword(24));
            actor->base.attacks.second[i] = MonsterInit_ReadHalfword(24);
            actor->base.attacks.flags[i] = MonsterInit_ReadWord(0);
            actor->base.attacks.parameter[i] = MonsterInit_ReadHalfword(0);
            ++i;
            if (i > 3)
                break;
        }
        return actor;
    }
    return actor;
}
static inline u8 MonsterInit_IsInCurrentRegion(struct ChaseActor *actor)
{
    if (gGameState->region == actor->base.regionId)
        return 1;
    return 0;
}
#define MonsterInit_TestWorldFlags GameState_TestWorldFlags
static inline void MonsterInit_InitializeRegion(struct ChaseActor *actor)
{
    if (MonsterInit_TestMotionFlags(actor, 2))
        actor->base.regionId = gGameState->region;
    else
        actor->base.regionId = Region_FindContaining(&actor->base.body.position);
}
static inline void MonsterInit_ClearReceiver(struct ChaseActor *actor)
{
    actor->base.counter304 = 0;
}
static inline void MonsterInit_InitializeReceiver(struct ChaseActor *actor)
{
    MonsterInit_ClearReceiver(actor);
    SceneActorRegistry_RegisterCategoryReceiver((void *)actor);
}

static inline s32 MonsterInit_Half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -(-value >> 1);
}

static inline void MonsterInit_MaskColliderFlags(struct ContactCollider *collider, u32 keepMask)
{
    collider->flags = keepMask & collider->flags;
}

static inline void MonsterInit_ReadInitialMode(struct ChaseActor *arg0)
{
    s32 mode;
    mode = Script_GetKeywordValue('u', 0);
    arg0->group.effect = 0x21;
    if (mode != 0) {
        MonsterInit_SetBehaviorFlags(arg0, 0x40000);
        if (mode == 2)
            MonsterInit_SetBehaviorFlags(arg0, 0x80000);
    }
}

static inline void MonsterInit_SetContactCallback(struct ChaseActor *actor, void *callback)
{
    actor->base.onContact = callback;
}

static inline void MonsterInit_SetReactionGate(struct ChaseActor *actor, void *callback)
{
    actor->base.reactionGate = callback;
}

static inline void MonsterInit_SetRouteReset(struct ChaseActor *actor, void *callback)
{
    actor->base.routeReset = callback;
}

#endif
