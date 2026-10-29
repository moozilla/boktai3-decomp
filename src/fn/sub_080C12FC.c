#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
#include "engine.h"
#include "ram.h"
extern u8 *gUnknown_02000580[];

#include "engine_globals.h"

u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 Mod(s32, s32);
s32 Div(s32, s32);
void sub_0807E334(struct SceneActorRegistryResetView *);
s32 Nav_IsPositionInMesh(void *, struct ContactPosition *, struct ContactPosition *);
s32 Terrain_ApplyBlocked(void *, struct ContactPosition *, struct ContactPosition *);

struct SkeletonReactionTarget {
    u8 unknown0[8];
    u8 playerIndex;
    u8 unknown9[3];
    struct ContactPosition position;
};

struct SkeletonReactionPlayerCell {
    u16 unknown0[3];
    u16 index;
};
struct SkeletonReactionPlayer {
    u8 unknown0[0x20];
    u32 flags;
    struct SkeletonReactionTarget target;
    u8 unknown38[8];
    struct SkeletonReactionPlayerCell *cell;
};

struct SkeletonReactionBody {
    u8 unknown0[0x6F0];
    s16 pursuitTimer;
};

struct SkeletonReactionRoutePoint {
    s16 x, z;
};
struct SkeletonReactionRouteLink {
    u8 unknown0[8];
    struct SkeletonReactionRoutePoint *point;
    u8 unknownC[8];
};
struct SkeletonReactionSavedControl {
    void *control;
    u8 state, phase, behavior, kind;
};
struct SkeletonReactionTurnState {
    u8 direction, savedDirection;
    s8 turnsRemaining;
    u8 unknown3;
};
struct SkeletonReactionGroup {
    u8 unknown0[0x40];
    s16 flags;
    u16 field42, timer44, unknown46;
    void *homeControl;
    struct SceneActorRegistryControl control;
    u8 unknown68[0xC];
    struct SkeletonReactionRouteLink homeRoute;
    u8 unknown88[4];
    struct SkeletonReactionSavedControl saved;
    u8 unknown94[0x162];
    u8 hitReactionLatch;
    u8 unknown1F7[5];
    void *route;
    struct ContactPosition *destination;
    struct SkeletonReactionTurnState turn;
    void *contactControl;
    u8 unknown20C[0x10];
    void *turnControl, *routePhaseCallback, *fallback;
};

struct SkeletonReactionTracking {
    struct SkeletonReactionTarget *target;
    struct ContactPosition destination;
    u8 unknownC, heading, unknownE, flags;
};

static inline void SkeletonReaction_SetActorFlags(void *object, u32 bits)
{
    struct SceneActorRegistryResetView *view = object;
    view->flags2CC |= bits;
}

static inline struct SkeletonReactionGroup *SkeletonReaction_GroupOf(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    return (struct SkeletonReactionGroup *)&actor->group;
}

static inline u8 SkeletonReaction_Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}

static inline u8 SkeletonReaction_TestField(u16 *field, u32 mask)
{
    if (*field & mask)
        return 1;
    return 0;
}

static inline u16 SkeletonReaction_ReadMotionFlags(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    return actor->flags2D4;
}
static inline u16 SkeletonReaction_MotionFlags(void *object, u32 mask)
{
    struct SceneActorRegistryResetView *actor = object;
    u16 value = SkeletonReaction_ReadMotionFlags(actor);
    return value & mask;
}

static inline u32 SkeletonReaction_Bits32(u32 *field, u32 mask)
{
    return *field & mask;
}

static inline void SkeletonReaction_InstallControl(void *object, void *callback, u32 kind)
{
    struct SceneActorRegistryResetView *actor = object;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
}

static inline void SkeletonReaction_ResetMotionControl(void *object)
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

static inline void SkeletonReaction_SetControl(void *object, void *callback, u32 kind)
{
    struct SceneActorRegistryResetView *actor = object;
    SkeletonReaction_InstallControl(actor, callback, kind);
    SkeletonReaction_ResetMotionControl(actor);
}

static inline void SkeletonReaction_SelectState(void *object, u8 state, u8 phase, u8 behavior)
{
    struct SceneActorRegistryResetView *actor = object;
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
}

static inline void SkeletonReaction_ResetStateEntry(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline void SkeletonReaction_SetState(void *object, u32 state, u32 phase, u32 behavior)
{
    struct SceneActorRegistryResetView *actor = object;
    SkeletonReaction_SelectState(actor, state, phase, behavior);
    SkeletonReaction_ResetStateEntry(actor);
}

static inline void SkeletonReaction_SetPhase(void *object, u32 phase, u32 behavior)
{
    struct SceneActorRegistryResetView *actor = object;
    SkeletonReaction_SelectState(actor, 0, phase, behavior);
    SkeletonReaction_ResetStateEntry(actor);
}

static inline void SkeletonReaction_SetBehavior(void *object, u32 behavior)
{
    struct SceneActorRegistryResetView *actor = object;
    SkeletonReaction_SetPhase(actor, 0, behavior);
}

static inline struct EngineMap *SkeletonReaction_CurrentMap(void)
{
    return gTerrainMap;
}
struct SkeletonReactionTerrainCell {
    u8 attributes;
    u8 tile;
    u16 flags;
};
struct SkeletonReactionTerrainGrid {
    u32 field0;
    s16 width, height;
    s16 originX, originZ;
    struct SkeletonReactionTerrainCell cells[1];
};
struct SkeletonReactionTerrainOverride {
    u16 flags, index;
    struct SkeletonReactionTerrainCell properties;
};
static inline struct SkeletonReactionTerrainCell *SkeletonReaction_MapCell(struct EngineMap *map,
                                                                           s32 cell)
{
    return &((struct SkeletonReactionTerrainGrid *)map->cells)->cells[cell];
}
static inline struct SkeletonReactionTerrainCell *SkeletonReaction_CellProperties(s32 cell)
{
    struct SkeletonReactionTerrainOverride *override =
        (void *)TerrainModifiers_FindCellOverride(cell, 1);
    if (override)
        return &override->properties;
    return SkeletonReaction_MapCell(SkeletonReaction_CurrentMap(), cell);
}

static inline u32 SkeletonReaction_MapRowStart(const struct EngineMap *map, s32 row)
{
    return map->rows[row];
}

static inline s32 SkeletonReaction_CellAt(struct ContactPosition *position)
{
    s32 x = position->x >> 8;
    s32 z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return SkeletonReaction_MapRowStart(SkeletonReaction_CurrentMap(), z) + x;
}

static inline u32 SkeletonReaction_HeightAt(s32 cell, struct ContactPosition *position)
{
    struct SkeletonReactionTerrainCell *data = SkeletonReaction_CellProperties(cell);
    u32 kind, height;
    kind = data->attributes >> 4;
    height = (data->attributes & 15) << 8;
    switch (kind) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}

static inline u32 SkeletonReaction_Height(struct ContactPosition *position)
{
    return SkeletonReaction_HeightAt(SkeletonReaction_CellAt(position), position);
}

static inline void SkeletonReaction_SnapToRoute(struct ContactPosition *position,
                                                struct SkeletonReactionRouteLink *route)
{
    position->x = route->point->x;
    position->z = route->point->z;
    position->y = SkeletonReaction_Height(position);
}

static inline void SkeletonReaction_ClearBits(u16 *field, u32 bits)
{
    *field &= ~bits;
}

static inline u16 *SkeletonReaction_MotionFlagsOf(struct SceneActorRegistryResetView *actor)
{
    return &actor->flags2D4;
}

static inline struct ContactPosition *
SkeletonReaction_MotionOf(struct SceneActorRegistryResetView *actor)
{
    return &actor->motion;
}

static inline u8 SkeletonReaction_TryRoute(struct SceneActorRegistryResetView *actor, void *route,
                                           struct ContactPosition *position,
                                           struct ContactPosition *target)
{
    s32 result;
    if (!Nav_IsPositionInMesh(route, position, target)) {
        SkeletonReaction_ClearBits(SkeletonReaction_MotionFlagsOf(actor), 0x200);
        return 0;
    } else {
        result = Terrain_ApplyBlocked(route, position, target);
        if (result) {
            if (result != 2)
                Memory_Clear(SkeletonReaction_MotionOf(actor), 8);
            return 1;
        } else {
            SkeletonReaction_ClearBits(SkeletonReaction_MotionFlagsOf(actor), 0x200);
            return 0;
        }
    }
}

static inline void *SkeletonReaction_CurrentControl(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    return actor->target;
}

static inline u8 SkeletonReaction_ControlKind(struct SceneActorRegistryResetView *actor)
{
    return actor->field2B5;
}

static inline u8 SkeletonReaction_CurrentState(struct SceneActorRegistryResetView *actor)
{
    return actor->state2A8;
}

static inline u8 SkeletonReaction_CurrentPhase(struct SceneActorRegistryResetView *actor)
{
    return actor->phase2A9;
}

static inline u8 SkeletonReaction_CurrentBehavior(struct SceneActorRegistryResetView *actor)
{
    return actor->behavior2AA;
}

static inline void SkeletonReaction_SaveControl(void *p, void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    struct SkeletonReactionSavedControl *packet = p;
    packet->control = SkeletonReaction_CurrentControl(actor);
    packet->kind = SkeletonReaction_ControlKind(actor);
    packet->state = SkeletonReaction_CurrentState(actor);
    packet->phase = SkeletonReaction_CurrentPhase(actor);
    packet->behavior = SkeletonReaction_CurrentBehavior(actor);
}

struct SkeletonRandomTableView {
    const u16 *samples;
};
#define SkeletonReaction_NextSample(table_, seed_, wrap_, bits_)                                   \
    ({                                                                                             \
        const struct SkeletonRandomTableView *table_view_ = (table_);                              \
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
static inline u32 SkeletonReaction_RandomDirectionExcept(u32 current, const u16 *randomSamples)
{
    s32 tries;
    struct SkeletonRandomTableView table = {randomSamples};
    for (tries = 0; tries < 4; tries++) {
        u32 direction = SkeletonReaction_NextSample(&table, &gRandomIndex, 1023, 3) << 6;
        if (direction != current)
            return direction;
    }
    return current;
}

static inline void SkeletonReaction_BeginTurn(void *obj, u32 saved, const u16 *randomSamples)
{
    struct SceneActorRegistryResetView *actor = obj;
    struct SkeletonReactionGroup *group = SkeletonReaction_GroupOf(obj);
    if (!SkeletonReaction_MotionFlags(actor, 0x400) &&
        !SkeletonReaction_TestField((u16 *)&group->flags, 4)) {
        group->turn.direction =
            SkeletonReaction_RandomDirectionExcept(actor->direction, randomSamples);
        SkeletonReaction_SaveControl(&group->saved, actor);
        group->turn.savedDirection = saved;
        SkeletonReaction_SetControl(actor, group->turnControl, 0x1B);
        SkeletonReaction_SetState(actor, 3, 1, 0);
        group->turn.turnsRemaining = 6;
    }
}

static inline s16 SkeletonReaction_MapWidth(struct EngineMap *map)
{
    return ((struct SkeletonReactionTerrainGrid *)map->cells)->width;
}
static inline s16 SkeletonReaction_TerrainLevelHeight(u32 level)
{
    return level << 8;
}
#define SkeletonReaction_CellCenterCoordinate(coordinate_)                                         \
    ({                                                                                             \
        s32 grid_coordinate_ = (coordinate_);                                                      \
        (s16)(((u32)grid_coordinate_ << 8) + 0x80);                                                \
    })

static inline void SkeletonReaction_CellCenter(s32 cell, struct ContactPosition *position)
{
    s32 x = Mod(cell, SkeletonReaction_MapWidth(SkeletonReaction_CurrentMap()));
    s32 z = Div(cell, SkeletonReaction_MapWidth(SkeletonReaction_CurrentMap()));
    position->x = SkeletonReaction_CellCenterCoordinate(x);
    {
        u32 attributes = SkeletonReaction_CellProperties(cell)->attributes;
        u32 height = 15;
        height &= attributes;
        position->y = SkeletonReaction_TerrainLevelHeight(height);
    }
    position->z = SkeletonReaction_CellCenterCoordinate(z);
}

static inline s32 SkeletonReaction_PlayerSlot(struct SkeletonReactionTarget *target)
{
    if (target->playerIndex <= 3)
        return target->playerIndex;
    return -1;
}
/* A successful cell-center reroute returns one. Other paths leave the
 * caller to try the separate partner-position route. */
static inline u8 SkeletonReaction_Arrive(void *obj)
{
    struct ContactPosition target;
    struct SceneActorRegistryResetView *actor = obj;
    struct SkeletonReactionGroup *group = SkeletonReaction_GroupOf(obj);
    struct SkeletonReactionTracking *tracking = (struct SkeletonReactionTracking *)&actor->fieldDC;
    s32 cell, player;
    const u16 *randomSamples = gRandomTable;
    void **callback;
    SkeletonReaction_SnapToRoute(group->destination, &group->homeRoute);
    if (SkeletonReaction_Test(gEngineFlags, 0x4800)) {
        struct SkeletonReactionTarget *t = tracking->target;
        if (!t)
            goto invalid_target;
        player = SkeletonReaction_PlayerSlot(t);
        if (player < 0) {
        invalid_target:
            callback = &group->fallback;
            goto invoke;
        }
        cell = ((struct SkeletonReactionPlayer *)gUnknown_02000580[player])->cell->index;
    } else {
        struct SkeletonReactionTarget *t = tracking->target;
        cell = SkeletonReaction_CellAt(&t->position);
        player = 0;
    }
    if (cell != actor->terrainCell &&
        !SkeletonReaction_Bits32(
            &((struct SkeletonReactionPlayer *)gUnknown_02000580[player])->flags, 0x20000))
        goto reroute;
    if (SkeletonReaction_MotionFlags(actor, 0x400))
        return 0;
    {
        u32 bits = 4;
        group->flags &= ~bits;
    }
    if (SkeletonReaction_MotionFlags(actor, 0x80)) {
        SkeletonReaction_SetControl(actor, group->homeControl, 0x1A);
        SkeletonReaction_SetBehavior(actor, 12);
    } else {
        SkeletonReaction_SetControl(actor, actor->entryTarget228, 1);
        SkeletonReaction_SetBehavior(actor, 5);
        SkeletonReaction_SnapToRoute(group->destination, &group->homeRoute);
        (group->destination)->y = SkeletonReaction_HeightAt(
            SkeletonReaction_CellAt(group->destination), group->destination);
        (group->destination)->pad = 0;
    }
    {
        u8 saved = actor->direction;
        SkeletonReaction_BeginTurn(obj, saved, randomSamples);
    }
    sub_0807E334(obj);
    actor->field2C2 = 0x20;
    return 0;
reroute: {
    SkeletonReaction_CellCenter(cell, &target);
    if (SkeletonReaction_TryRoute(obj, group->route, &actor->position, &target))
        goto success;
}
    callback = &group->fallback;
invoke:
    Callback_InvokeActor(actor, *callback);
    return 0;
success:
    return 1;
}

static inline u16 SkeletonReaction_QueryMotionFlags(struct SceneActorRegistryResetView *actor,
                                                    u16 mask)
{
    return actor->flags2D4 & mask;
}
static inline void SkeletonReaction_ClearMotionFlags(struct SceneActorRegistryResetView *actor,
                                                     u32 bits)
{
    u32 mask = ~bits;
    actor->flags2D4 &= mask;
}
static inline u8 SkeletonReaction_TrackingFlags(struct SkeletonReactionTracking *tracking, u32 mask)
{
    if (tracking->flags & mask)
        return 1;
    return 0;
}

static inline void SkeletonReaction_ClearWordFlags(u32 *flags, u32 bits)
{
    u32 mask = ~bits;
    *flags &= mask;
}

static inline void SkeletonReaction_HalfwordFlagsSet(u16 *field, u32 bits)
{
    u32 value = *field;
    bits |= value;
    *field = bits;
}
static inline void SkeletonReaction_GroupSetFlags(struct SkeletonReactionGroup *owner, u32 bits)
{
    SkeletonReaction_HalfwordFlagsSet((u16 *)&owner->flags, bits);
}

void SceneActorScript_TriggerCallOnCount(struct SceneActorRegistryActor *);

static inline u8 SkeletonReaction_StartRoar(struct SceneActorRegistryResetView *actor)
{
    if (!actor->field5CA && (actor->field2E6 & 8)) {
        actor->field2B8 = 0x2D;
        actor->field5CA = 0x41;
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)actor, 3);
        return 1;
    }
    return 0;
}

static inline void SkeletonReaction_TrackPlayer(struct SceneActorRegistryResetView *actor)
{
    ((struct SkeletonReactionTracking *)&actor->fieldDC)->target =
        &((struct SkeletonReactionPlayer *)gUnknown_02000580[0])->target;
}

/* Resolve contact flags, animation, and pursuit routing. The normal chase
 * continuation returns zero even after installing its control. */
u8 Skeleton_ReactToContact(struct SceneActorRegistryResetView *actor)
{
    struct SkeletonReactionBody *body = actor->context;
    struct SkeletonReactionGroup *group = (struct SkeletonReactionGroup *)&actor->group;

    if (!(actor->field2D8 & 0x20000)) {
        if (actor->reactionSoundVariant == 0)
            Sound_PlaySong(0x211);
        else
            Sound_PlaySong(0x13E);
    }
    SceneActorScript_TriggerCallOnCount((struct SceneActorRegistryActor *)actor);
    if (!SkeletonReaction_QueryMotionFlags(actor, 0x2000))
        goto otherContact;
    {
        actor->direction = actor->field2B6 + 0x80;
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)actor, 0);
        SkeletonReaction_ClearMotionFlags(actor, 0x2020);
        SkeletonReaction_SetActorFlags(actor, 0x10);
        group->destination->pad = 0;
        SkeletonReaction_TrackPlayer(actor);
        if (!SkeletonReaction_Arrive(actor)) {
            if (!SkeletonReaction_TryRoute(
                    actor, group->route, &actor->position,
                    &((struct SkeletonReactionTracking *)&actor->fieldDC)->target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return 0;
            }
        }
        SkeletonReaction_SetControl(actor, actor->entryTarget228, 1);
        SkeletonReaction_SetBehavior(actor, 7);
        actor->field2C2 = Div(0x40, 3);
        return 1;
    }
otherContact:
    if (SkeletonReaction_QueryMotionFlags(actor, 0x8002)) {
        SkeletonReaction_ClearMotionFlags(actor, 0x20);
        return 0;
    }
    if (actor->field2DC & 0xC0000) {
        SkeletonReaction_SetControl(actor, actor->reactionTarget230, 0x12);
        SkeletonReaction_SetState(actor, 3, 0, 0);
        SkeletonReaction_ClearWordFlags(&actor->flags2D0, 0x1000000);
        return 1;
    }
    SkeletonReaction_ClearMotionFlags(actor, 0x20);
    if (SkeletonReaction_QueryMotionFlags(actor, 0x100))
        return 0;
    if (actor->field2B8)
        return 0;
    if (SkeletonReaction_TrackingFlags((struct SkeletonReactionTracking *)&actor->fieldDC, 0x10))
        return 0;
    if (SkeletonReaction_QueryMotionFlags(actor, 0x80)) {
        if (SkeletonReaction_StartRoar(actor))
            Sound_PlaySong(0x3A7);
        if (!((struct SkeletonReactionTracking *)&actor->fieldDC)->target)
            return 0;
        SkeletonReaction_SetActorFlags(actor, 0x80);
        SkeletonReaction_GroupSetFlags(group, 4);
        SkeletonReaction_TrackPlayer(actor);
        if (!SkeletonReaction_Arrive(actor)) {
            body->pursuitTimer = 0;
            if (!SkeletonReaction_TryRoute(
                    actor, group->route, &actor->position,
                    &((struct SkeletonReactionTracking *)&actor->fieldDC)->target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return 0;
            }
        }
        SkeletonReaction_SetControl(actor, group->homeControl, 0x1A);
        SkeletonReaction_SetBehavior(actor, 12);
        actor->field2C2 = 0x20;
        return 1;
    }
    if (SkeletonReaction_StartRoar(actor))
        Sound_PlaySong(0x3A7);
    if (!((struct SkeletonReactionTracking *)&actor->fieldDC)->target)
        return 0;
    SkeletonReaction_TrackPlayer(actor);
    SkeletonReaction_SetActorFlags(actor, 0x10);
    group->destination->pad = 0;
    if (!SkeletonReaction_Arrive(actor)) {
        body->pursuitTimer = 0;
        if (!SkeletonReaction_TryRoute(
                actor, group->route, &actor->position,
                &((struct SkeletonReactionTracking *)&actor->fieldDC)->target->position)) {
            Callback_InvokeActor(actor, group->fallback);
            return 0;
        }
    }
    SkeletonReaction_SetControl(actor, actor->entryTarget228, 1);
    SkeletonReaction_SetBehavior(actor, 7);
    actor->field2C2 = 0x20;
    return 0;
}
