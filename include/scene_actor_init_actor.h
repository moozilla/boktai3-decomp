#ifndef GUARD_SCENE_ACTOR_INIT_ACTOR_H
#define GUARD_SCENE_ACTOR_INIT_ACTOR_H

#include "scene_actor_state.h"
#include "scene_actor_init_primitives.h"
#include "game_state.h"

struct SceneActorRegistryActor;
void SceneActorControl_OnIdle(u8 *actor);
s32 Region_FindContaining(void *position);
void SceneActorRegistry_RegisterCategoryReceiver(struct SceneActorRegistryActor *actor);

static inline u32 MonsterInit_GetTerrainCell(struct SceneActorRegistryResetView *actor)
{
    return actor->terrainCell;
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
static inline u8 MonsterInit_GetDrawableKind(struct SceneActorRegistryResetView *actor)
{
    return actor->drawableKind;
}
static inline u8 *MonsterInit_Drawable(struct SceneActorRegistryResetView *actor)
{
    return actor->drawable;
}
static inline u8 MonsterInit_GetCategory(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    return actor->category;
}
static inline void MonsterInit_SetCategory(void *object, u8 category)
{
    struct SceneActorRegistryResetView *actor = object;
    actor->category = category;
}
static inline s32 MonsterInit_GetHealth(struct SceneActorRegistryResetView *actor)
{
    return actor->health;
}
static inline u16 MonsterInit_GetInitialHealth(struct SceneActorRegistryResetView *actor)
{
    return actor->defaultHealth;
}
static inline u16 MonsterInit_GetGroundHeight(struct SceneActorRegistryResetView *actor)
{
    return actor->groundHeight2BE;
}
static inline u32 MonsterInit_GetPaletteBase(struct SceneActorRegistryResetView *actor)
{
    return actor->basePaletteBank;
}
static inline u32 MonsterInit_GetAlternatePalette(struct SceneActorRegistryResetView *actor)
{
    return actor->paletteAlternateBank;
}
static inline u32 MonsterInit_GetCurrentPalette(struct SceneActorRegistryResetView *actor)
{
    return actor->alternateOffset;
}
static inline u32 MonsterInit_GetPaletteOffset(struct SceneActorRegistryResetView *actor)
{
    return actor->paletteOffset;
}
static inline void MonsterInit_SetCurrentPalette(struct SceneActorRegistryResetView *actor,
                                                 u32 palette)
{
    actor->alternateOffset = palette;
}
static inline u32 MonsterInit_GetMotionFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2C0;
}
static inline u16 MonsterInit_GetHeightOffset(struct SceneActorRegistryResetView *actor)
{
    return actor->field2EA;
}
static inline u32 MonsterInit_GetBehaviorFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2CC;
}
static inline u32 MonsterInit_GetStatusFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2D0;
}
static inline u8 MonsterInit_GetState(struct SceneActorRegistryResetView *actor)
{
    return actor->state2A8;
}
static inline u8 MonsterInit_GetPhase(struct SceneActorRegistryResetView *actor)
{
    return actor->phase2A9;
}
static inline u8 MonsterInit_GetBehavior(struct SceneActorRegistryResetView *actor)
{
    return actor->behavior2AA;
}
static inline void *MonsterInit_GetCurrentControl(struct SceneActorRegistryResetView *actor)
{
    return actor->target;
}
static inline void *MonsterInit_GetStandingControl(struct SceneActorRegistryResetView *actor)
{
    return actor->savedTarget;
}
static inline u8 MonsterInit_GetControlKind(struct SceneActorRegistryResetView *actor)
{
    return actor->field2B5;
}
static inline u8 MonsterInit_GetDirection(struct SceneActorRegistryResetView *actor)
{
    return actor->body.direction;
}
static inline struct SceneActorDefaultControl *
MonsterInit_SavedControlOf(struct SceneActorRegistryResetView *actor)
{
    return &actor->saved;
}
static inline void MonsterInit_SaveControl(struct SceneActorDefaultControl *saved,
                                           struct SceneActorRegistryResetView *actor)
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
static inline void MonsterInit_SetBehaviorFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2CC |= mask;
}
static inline void MonsterInit_SetStatusFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2D0 |= mask;
}
static inline void MonsterInit_SetMotionFlags(struct SceneActorRegistryResetView *actor, u16 mask)
{
    actor->flags2C0 |= mask;
}
static inline u8 MonsterInit_TestMotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    if (MonsterInit_GetMotionFlags(actor) & mask)
        return 1;
    return 0;
}
static inline void MonsterInit_ClearControlFlags(struct SceneActorRegistryResetView *actor,
                                                 u32 mask)
{
    actor->flags114 &= mask;
}
static inline void MonsterInit_SetMotionControl(struct SceneActorRegistryResetView *actor,
                                                void *callback, u32 kind)
{
    actor->word104 = (u32)callback;
    actor->word108 = 0;
    actor->word10C = 0;
    actor->word110 = kind;
    actor->flags114 = 1;
}
static inline void MonsterInit_ClearMotion(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    u32 zero = 0;
    MonsterInit_SetMotionControl(actor, 0, 0);
    actor->flags114 = zero;
}
static inline void MonsterInit_SetControl(struct SceneActorRegistryResetView *actor, void *callback,
                                          u32 behavior)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = behavior;
    actor->target = callback;
    actor->field2C8 = 0;
    MonsterInit_ClearControlFlags(actor, ~1);
    actor->word104 = 0;
}
static inline void MonsterInit_SetState(struct SceneActorRegistryResetView *actor, u32 state,
                                        u32 phase, u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline s32 MonsterInit_ClampHealth(u16 raw)
{
    s32 health = (s16)raw;
    if (health <= 0)
        health = 0;
    return health;
}
static inline void MonsterInit_InstallIdle(struct SceneActorRegistryResetView *actor, void *idle)
{
    actor->target = idle;
}
static inline void MonsterInit_SetInitialHealth(struct SceneActorRegistryResetView *actor,
                                                u16 health)
{
    actor->defaultHealth = health;
}
static inline void MonsterInit_InitializeHealth(struct SceneActorRegistryResetView *actor, u16 raw,
                                                s32 firstLimit, s32 secondLimit)
{
    actor->flags2D4 = 0;
    MonsterInit_InstallIdle(actor, SceneActorControl_OnIdle);
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->health = MonsterInit_ClampHealth(raw);
    actor->fieldAA = 0;
    if (secondLimit <= 0)
        secondLimit = firstLimit;
    actor->attachmentLimit = firstLimit;
    actor->fieldA8 = secondLimit;
    actor->regionId = 0;
}
static inline void MonsterInit_SetFirstAttack(struct SceneActorRegistryResetView *actor, s32 index,
                                              s32 value)
{
    actor->attacks.first[index] = value;
}
static inline struct SceneActorRegistryResetView *
MonsterInit_ReadAttacks(struct SceneActorRegistryResetView *actor)
{
    s32 i;
    if (Script_SeekToKeyword('a')) {
        i = 0;
        while (1) {
            MonsterInit_SetFirstAttack(actor, i, MonsterInit_ReadHalfword(24));
            actor->attacks.second[i] = MonsterInit_ReadHalfword(24);
            actor->attacks.flags[i] = MonsterInit_ReadWord(0);
            actor->attacks.parameter[i] = MonsterInit_ReadHalfword(0);
            ++i;
            if (i > 3)
                break;
        }
        return actor;
    }
    return actor;
}
static inline u8 MonsterInit_IsInCurrentRegion(struct SceneActorRegistryResetView *actor)
{
    if (gGameState->region == actor->regionId)
        return 1;
    return 0;
}
static inline u8 MonsterInit_TestWorldFlags(u32 mask)
{
    if (gGameState->flags868 & mask)
        return 1;
    return 0;
}
static inline void MonsterInit_InitializeRegion(struct SceneActorRegistryResetView *actor)
{
    if (MonsterInit_TestMotionFlags(actor, 2))
        actor->regionId = gGameState->region;
    else
        actor->regionId = Region_FindContaining(&actor->body.position);
}
static inline void MonsterInit_ClearReceiver(struct SceneActorRegistryResetView *actor)
{
    actor->field304 = 0;
}
static inline void MonsterInit_InitializeReceiver(struct SceneActorRegistryResetView *actor)
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

static inline void MonsterInit_ClearColliderFlags(struct ContactCollider *collider, u32 mask)
{
    collider->flags = mask & collider->flags;
}

#endif
