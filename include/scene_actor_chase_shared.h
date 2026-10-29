#ifndef GUARD_SCENE_ACTOR_CHASE_SHARED_H
#define GUARD_SCENE_ACTOR_CHASE_SHARED_H

#include "scene_actor_chase_types.h"
#include "engine.h"
#include "engine_globals.h"
#include "ram.h"
#include "scene_actor_animation.h"

extern u8 *gUnknown_02000580[];
u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 Mod(s32, s32);
s32 Div(s32, s32);
void sub_0807E334(struct SceneActorRegistryResetView *);
s32 Nav_IsPositionInMesh(void *, struct ContactPosition *, struct ContactPosition *);
s32 Terrain_ApplyBlocked(void *, struct ContactPosition *, struct ContactPosition *);

/* Leaf operations shared by the pursuit and contact state machines. */
static inline void SceneActorChase_SetActorFlags(void *object, u32 bits)
{
    struct SceneActorRegistryResetView *view = object;
    view->flags2CC |= bits;
}

#if SCENE_ACTOR_CHASE_TYPED_GROUP
static inline struct SceneActorChaseGroup *
SceneActorChase_GroupOf(struct SceneActorRegistryResetView *actor)
#else
static inline struct SceneActorChaseGroup *SceneActorChase_GroupOf(void *object)
#endif
{
#if !SCENE_ACTOR_CHASE_TYPED_GROUP
    struct SceneActorRegistryResetView *actor = object;
#endif
    return (struct SceneActorChaseGroup *)&actor->group;
}

static inline u8 SceneActorChase_Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}

static inline u8 SceneActorChase_TestField(u16 *field, u32 mask)
{
    if (*field & mask)
        return 1;
    return 0;
}

static inline u16 SceneActorChase_ReadMotionFlags(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    return actor->flags2D4;
}

static inline u16 SceneActorChase_MotionFlags(void *object, u32 mask)
{
    struct SceneActorRegistryResetView *actor = object;
    u16 value = SceneActorChase_ReadMotionFlags(actor);
    return value & mask;
}

static inline u32 SceneActorChase_Bits32(u32 *field, u32 mask)
{
    return *field & mask;
}

static inline void SceneActorChase_InstallControl(void *object, void *callback, u32 kind)
{
    struct SceneActorRegistryResetView *actor = object;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
}

static inline void SceneActorChase_ResetMotionControl(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    actor->field2C8 = 0;
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = 0;
}

static inline void SceneActorChase_SetControl(void *object, void *callback, u32 kind)
{
    struct SceneActorRegistryResetView *actor = object;
    SceneActorChase_InstallControl(actor, callback, kind);
    SceneActorChase_ResetMotionControl(actor);
}

static inline void SceneActorChase_ResetStateEntry(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline struct EngineMap *SceneActorChase_CurrentMap(void)
{
    return gTerrainMap;
}

static inline struct SceneActorChaseTerrainCell *SceneActorChase_MapCell(struct EngineMap *map,
                                                                         s32 cell)
{
    return &((struct SceneActorChaseTerrainGrid *)map->cells)->cells[cell];
}

static inline struct SceneActorChaseTerrainCell *SceneActorChase_CellProperties(s32 cell)
{
    struct SceneActorChaseTerrainOverride *override =
        (void *)TerrainModifiers_FindCellOverride(cell, 1);
    if (override)
        return &override->properties;
    return SceneActorChase_MapCell(SceneActorChase_CurrentMap(), cell);
}

static inline u32 SceneActorChase_MapRowStart(const struct EngineMap *map, s32 row)
{
    return map->rows[row];
}

static inline void SceneActorChase_ClearBits(u16 *field, u32 bits)
{
    *field &= ~bits;
}

static inline u16 *SceneActorChase_MotionFlagsOf(struct SceneActorRegistryResetView *actor)
{
    return &actor->flags2D4;
}

static inline struct ContactPosition *
SceneActorChase_MotionOf(struct SceneActorRegistryResetView *actor)
{
    return &actor->motion;
}

static inline void *SceneActorChase_CurrentControl(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    return actor->target;
}

static inline u8 SceneActorChase_ControlKind(struct SceneActorRegistryResetView *actor)
{
    return actor->field2B5;
}

static inline u8 SceneActorChase_CurrentState(struct SceneActorRegistryResetView *actor)
{
    return actor->state2A8;
}

static inline u8 SceneActorChase_CurrentPhase(struct SceneActorRegistryResetView *actor)
{
    return actor->phase2A9;
}

static inline u8 SceneActorChase_CurrentBehavior(struct SceneActorRegistryResetView *actor)
{
    return actor->behavior2AA;
}

static inline void SceneActorChase_SaveControl(void *p, void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseSavedControl *packet = p;
    packet->control = SceneActorChase_CurrentControl(actor);
    packet->kind = SceneActorChase_ControlKind(actor);
    packet->state = SceneActorChase_CurrentState(actor);
    packet->phase = SceneActorChase_CurrentPhase(actor);
    packet->behavior = SceneActorChase_CurrentBehavior(actor);
}

#define SceneActorChase_NextSample(table_, seed_, wrap_, bits_)                                    \
    ({                                                                                             \
        const struct SceneActorChaseRandomTable *table_view_ = (table_);                           \
        const u16 *samples_ = table_view_->samples;                                                \
        u32 *cursor_ = (seed_);                                                                    \
        u32 wrap_mask_ = (wrap_);                                                                  \
        u32 result_mask_ = (bits_);                                                                \
        u32 index_ = (*cursor_ + 1) & wrap_mask_;                                                  \
        u32 sample_;                                                                               \
        *cursor_ = index_;                                                                         \
        sample_ = samples_[index_];                                                                \
        sample_ &= result_mask_;                                                                   \
        sample_;                                                                                   \
    })

static inline u32 SceneActorChase_RandomDirectionExcept(u32 current, const u16 *randomSamples)
{
    s32 tries;
    struct SceneActorChaseRandomTable table = {randomSamples};
    for (tries = 0; tries < 4; tries++) {
        u32 direction = SceneActorChase_NextSample(&table, &gRandomIndex, 1023, 3) << 6;
        if (direction != current)
            return direction;
    }
    return current;
}

#if SCENE_ACTOR_CHASE_WIDE_MAP_WIDTH
static inline s32 SceneActorChase_MapWidth(struct EngineMap *map)
#else
static inline s16 SceneActorChase_MapWidth(struct EngineMap *map)
#endif
{
    return ((struct SceneActorChaseTerrainGrid *)map->cells)->width;
}

static inline s16 SceneActorChase_TerrainLevelHeight(u32 level)
{
    return level << 8;
}

#define SceneActorChase_CellCenterCoordinate(coordinate_)                                          \
    ({                                                                                             \
        s32 grid_coordinate_ = (coordinate_);                                                      \
        (s16)(((u32)grid_coordinate_ << 8) + 0x80);                                                \
    })

static inline void SceneActorChase_CellCenter(s32 cell, struct ContactPosition *position)
{
    s32 x = Mod(cell, SceneActorChase_MapWidth(SceneActorChase_CurrentMap()));
    s32 z = Div(cell, SceneActorChase_MapWidth(SceneActorChase_CurrentMap()));
    position->x = SceneActorChase_CellCenterCoordinate(x);
    {
        u32 attributes = SceneActorChase_CellProperties(cell)->attributes;
        u32 height = 15;
        height &= attributes;
        position->y = SceneActorChase_TerrainLevelHeight(height);
    }
    position->z = SceneActorChase_CellCenterCoordinate(z);
}

static inline s32 SceneActorChase_PlayerSlot(struct SceneActorChaseTarget *target)
{
    if (target->playerIndex <= 3)
        return target->playerIndex;
    return -1;
}

static inline u16 SceneActorChase_QueryMotionFlags(struct SceneActorRegistryResetView *actor,
                                                   u16 mask)
{
    return actor->flags2D4 & mask;
}

static inline void SceneActorChase_ClearMotionFlags(struct SceneActorRegistryResetView *actor,
                                                    u32 bits)
{
    u32 mask = ~bits;
    actor->flags2D4 &= mask;
}

static inline u8 SceneActorChase_TrackingFlags(struct SceneActorChaseTracking *tracking, u32 mask)
{
    if (tracking->flags & mask)
        return 1;
    return 0;
}

static inline void SceneActorChase_ClearWordFlags(u32 *flags, u32 bits)
{
    u32 mask = ~bits;
    *flags &= mask;
}

static inline void SceneActorChase_HalfwordFlagsSet(u16 *field, u32 bits)
{
    u32 value = *field;
    bits |= value;
    *field = bits;
}

static inline void SceneActorChase_GroupSetFlags(struct SceneActorChaseGroup *owner, u32 bits)
{
    SceneActorChase_HalfwordFlagsSet((u16 *)&owner->flags, bits);
}

static inline void SceneActorChase_SetTypedControl(struct SceneActorRegistryResetView *actor,
                                                   void *control, u32 kind)
{
    SceneActorChase_InstallControl(actor, control, kind);
    SceneActorChase_ResetMotionControl(actor);
}

/* Contact animation and target selection shared by the ground-monster reactions. */
static inline u8 SceneActorChase_BeginContactAnimation(struct SceneActorRegistryResetView *actor)
{
    if (!actor->field5CA && (actor->field2E6 & 8)) {
        actor->field2B8 = 0x2D;
        actor->field5CA = 0x41;
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)actor, 3);
        return 1;
    }
    return 0;
}

static inline void SceneActorChase_TrackPlayer(struct SceneActorRegistryResetView *actor)
{
    ((struct SceneActorChaseTracking *)&actor->fieldDC)->target =
        &((struct SceneActorChasePlayer *)gUnknown_02000580[0])->target;
}

#endif
