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
void SceneActorTerrain_RequestRetreatRoute(struct SceneActorRegistryResetView *);
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
    return &actor->group;
}

static inline void SceneActorChase_SetFlagComponent(struct SceneActorChaseFlags16 *field, u16 bits)
{
    field->value = bits | field->value;
}

static inline void SceneActorChase_ClearFlagComponent(struct SceneActorChaseFlags16 *field,
                                                      u32 bits)
{
    u32 mask = ~bits;
    field->value = mask & field->value;
}

static inline u16 SceneActorChase_FlagComponentBits(struct SceneActorChaseFlags16 *field, u16 mask)
{
    return field->value & mask;
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
    return &actor->body.motion;
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

#if SCENE_ACTOR_CHASE_DIRECT_SAVED_CONTROL
static inline void SceneActorChase_SaveControl(struct SceneActorChaseSavedControl *packet,
                                               void *object)
#else
static inline void SceneActorChase_SaveControl(void *p, void *object)
#endif
{
    struct SceneActorRegistryResetView *actor = object;
#if SCENE_ACTOR_CHASE_DIRECT_SAVED_CONTROL
    packet->control = actor->target;
    packet->kind = actor->field2B5;
    packet->state = actor->state2A8;
    packet->phase = actor->phase2A9;
    packet->behavior = actor->behavior2AA;
#else
    struct SceneActorChaseSavedControl *packet = p;
    packet->control = SceneActorChase_CurrentControl(actor);
    packet->kind = SceneActorChase_ControlKind(actor);
    packet->state = SceneActorChase_CurrentState(actor);
    packet->phase = SceneActorChase_CurrentPhase(actor);
    packet->behavior = SceneActorChase_CurrentBehavior(actor);
#endif
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

#if SCENE_ACTOR_CHASE_SCALAR_COORDINATE_OUTPUT
static inline s32 SceneActorChase_CellCenterCoordinate(s32 coordinate)
{
    return ((u32)coordinate << 8) + 0x80;
}

static inline void SceneActorChase_StoreCoordinate(s16 *field, s32 coordinate)
{
    *field = coordinate;
}
#else
#define SceneActorChase_CellCenterCoordinate(coordinate_)                                          \
    ({                                                                                             \
        s32 grid_coordinate_ = (coordinate_);                                                      \
        (s16)(((u32)grid_coordinate_ << 8) + 0x80);                                                \
    })
#endif

static inline void SceneActorChase_CellCenter(s32 cell, struct ContactPosition *position)
{
    s32 x = Mod(cell, SceneActorChase_MapWidth(SceneActorChase_CurrentMap()));
    s32 z = Div(cell, SceneActorChase_MapWidth(SceneActorChase_CurrentMap()));
#if SCENE_ACTOR_CHASE_SCALAR_COORDINATE_OUTPUT
    SceneActorChase_StoreCoordinate(&position->x, SceneActorChase_CellCenterCoordinate(x));
#else
    position->x = SceneActorChase_CellCenterCoordinate(x);
#endif
    {
        u32 attributes = SceneActorChase_CellProperties(cell)->attributes;
        u32 height = 15;
        height &= attributes;
        position->y = SceneActorChase_TerrainLevelHeight(height);
    }
#if SCENE_ACTOR_CHASE_SCALAR_COORDINATE_OUTPUT
    SceneActorChase_StoreCoordinate(&position->z, SceneActorChase_CellCenterCoordinate(z));
#else
    position->z = SceneActorChase_CellCenterCoordinate(z);
#endif
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

/* Query the separately owned halfword component; used by recovery and status
 * operations that also pass this flag storage through component interfaces. */
static inline u16
SceneActorChase_QueryMotionFlagComponent(struct SceneActorRegistryResetView *actor, u16 mask)
{
    return SceneActorChase_FlagComponentBits((struct SceneActorChaseFlags16 *)&actor->flags2D4,
                                             mask);
}

static inline void SceneActorChase_ClearMotionFlags(struct SceneActorRegistryResetView *actor,
                                                    u32 bits)
{
    u32 mask = ~bits;
    actor->flags2D4 &= mask;
}

static inline void
SceneActorChase_ClearMotionFlagComponent(struct SceneActorRegistryResetView *actor, u32 bits)
{
    SceneActorChase_ClearFlagComponent((struct SceneActorChaseFlags16 *)&actor->flags2D4, bits);
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
    SceneActorChase_HalfwordFlagsSet((u16 *)&owner->common.gate.flags, bits);
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
    if (!actor->group.hitReactionLatch && (actor->field2E6 & 8)) {
        actor->field2B8 = 0x2D;
        actor->group.hitReactionLatch = 0x41;
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)actor, 3);
        return 1;
    }
    return 0;
}

static inline void SceneActorChase_TrackPlayer(struct SceneActorRegistryResetView *actor)
{
    actor->tracking.target = &((struct SceneActorChasePlayer *)gUnknown_02000580[0])->target;
}

static inline void SceneActorChase_ClearActorFlags(void *object, u32 bits)
{
    struct SceneActorRegistryResetView *view = object;
    view->flags2CC &= ~bits;
}

static inline void SceneActorChase_SetReactionFlags(void *object, u32 bits)
{
    struct SceneActorRegistryResetView *actor = object;
    SceneActorChase_SetFlagComponent((struct SceneActorChaseFlags16 *)&actor->flags2D6, bits);
}

/* Randomly select another stored route when the current route is chosen again. */
#if SCENE_ACTOR_CHASE_ROUTE_COUNT_ACCESSOR
static inline u8 SceneActorChase_RouteCount(struct SceneActorChaseRouteSelection *route)
{
    return route->points->count;
}
#define SCENE_ACTOR_CHASE_ROUTE_COUNT(route_) SceneActorChase_RouteCount(route_)
#else
#define SCENE_ACTOR_CHASE_ROUTE_COUNT(route_) ((route_)->points->count)
#endif

static inline void SceneActorChase_ChooseRoute(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseRouteSelection *route = actor->body.currentRoute;
    u8 choice;
    {
        u32 index = (gRandomIndex + 1) & 1023;
        s32 value;
        gRandomIndex = index;
        index *= 2;
        value = *(const u16 *)(RANDOM_TABLE_ADDR + index);
        value >>= 3;
        choice = Mod(value, SCENE_ACTOR_CHASE_ROUTE_COUNT(route));
    }
    if (choice == route->index)
        choice = Mod(choice + 1, SCENE_ACTOR_CHASE_ROUTE_COUNT(route));
    route->index = choice;
    route->phase = 0;
    {
        u8 *base = gTerrainMap->routes;
        base += route->points->offset;
        route->current = base + route->index * 8;
    }
}
#undef SCENE_ACTOR_CHASE_ROUTE_COUNT

#endif
