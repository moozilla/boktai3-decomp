// MATCHING_CONCESSION: register-pinning
#include "golem_2.h"
#include "script_utility.h"
#include "engine.h"
#include "player.h"
#include "ram.h"

extern const u8 gUnk_08606134[];

void SceneActorReaction_PlayAnimationBUntilComplete(void);
void SceneActorAttachment_ReplaceResponseSlot(u8 *, u32, u32, u32);
void SceneActorBehavior_CallActionScript(u8 *);
void Golem_ResetIdleControl(u8 *);
void Camera_RequestShake(u32);
void Skeleton_TryTriggerAction(void *);
void Skeleton_ClearPendingFlag(void *);
void Golem_UpdateAttackProperties(void *);

inline static u8 TakeFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}

static inline u32 Flags16(struct SceneActorRegistryResetView *a, u32 m)
{
    u32 off = 0x2D4;
    return *(u16 *)((u8 *)a + off) & m;
}

static inline u32 Flags32(struct SceneActorRegistryResetView *a, u32 m)
{
    u32 off = 0x2D0;
    return *(u32 *)((u8 *)a + off) & m;
}

static inline u8 Unavailable(struct SceneActorRegistryResetView *a)
{
    if (a) {
        if (Flags16(a, 0x40) || Flags32(a, 0x20000) || a->health <= 0)
            return 1;
    }
    return 0;
}

void SceneActorEffects_AdvanceSteps(void *, u32);
void SceneActorTerrain_ClearCollisionTimers(void *);
void SceneActorBehavior_CallTerrainScript(void *);
void SceneActorPlacement_PlayCategoryEventSound(void *);
void *BestiaryDisplay_GetEntryForActor(u32, u32);
void GameState_SetEventFlag(void *);
void SparkParticle_Spawn(const void *, s32, const void *, s32, s32, s32, s32, s32);
void ActorShadow_Disable(void *);
void ActorParticles_SpawnWideBurst(void *);
void SceneActorControl_OnIdle(void);
void Golem_OnUnusedState(void)
{
}

static inline u32 Flags32At(struct SceneActorRegistryResetView *a, u32 mask)
{
    return *(u32 *)((u8 *)a + 0x2CC) & mask;
}
static inline u32 RandomFrom(const u16 *table)
{
    u32 index = (gRandomIndex + 1) & 1023;
    u32 v;
    gRandomIndex = index;
    index *= 2;
    v = *(const u16 *)((u32)table + index);
    v &= 3;
    return v + 1;
}

/* Releases the defeated golem, runs its script, and emits the defeat burst. */
void Golem_UpdateDefeat(struct SceneActorRegistryResetView *actor)
{
    s32 values[8];
    struct ContactPackedVector rangeB;
    struct ScriptUtilityArguments arguments;
    u8 *group = (u8 *)&actor->group;
    u16 script;
    if (TakeFlag(&actor->field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 10);
        actor->field2B3 = 0;
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 1, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 3, 127, 0);
        SceneActorEffects_AdvanceSteps(actor, 4);
        {
            struct SceneActorRegistryControl *control = (void *)(group + 0x4C);
            u32 *field = &actor->flags2CC;
            u32 mask = ~0x141C0;
            *field &= mask;
            control->counter = 0;
            *(u16 *)((u8 *)control + 0x14) = 0;
            control->timer = 0;
        }
        Sound_PlaySong(0x13E);
        {
            u16 *flags = (u16 *)((u8 *)actor + 0x13A);
            u32 value = *flags;
            u32 mask = 4;
            asm("" : "+r"(value), "+r"(mask));
            *flags = mask | value;
        }
    }
    SceneActorTerrain_ClearCollisionTimers((void *)actor);
    if (!actor->field2B3)
        return;
    actor->field2B0 = 1;
    SceneActorBehavior_CallTerrainScript(actor);
    script = *(u16 *)((u8 *)actor + 0x1F8);
    if (script) {
        values[0] = actor->position.x;
        values[1] = actor->position.y;
        values[2] = actor->position.z;
        values[3] = *(s32 *)((u8 *)actor + 0x1FC);
        values[4] = *(s32 *)((u8 *)actor + 0x200);
        values[5] = *(s32 *)((u8 *)actor + 0x204);
        values[6] = *(s32 *)((u8 *)actor + 0x2E0);
        values[7] = actor->id;
        arguments.count = 8;
        arguments.values = values;
        Script_Call(script, &arguments);
    }
    if (!Flags16(actor, 0x4000) && (u32)(actor->category - 11) > 2)
        GameState_SetEventFlag(
            BestiaryDisplay_GetEntryForActor(actor->category, actor->paletteOffset));
    if (!Flags32At(actor, 0x400)) {
        u8 category;
        rangeB.x = 80;
        rangeB.y = 60;
        rangeB.z = 60;
        {
            register const u16 *table asm("sl") = gRandomTable;
            s32 duration;
            s32 scale;
            SparkParticle_Spawn(&actor->position, RandomFrom(table), &rangeB, 0x20, 4,
                                duration = 20, scale = 2, 1);
            SparkParticle_Spawn(&actor->position, RandomFrom(table), &rangeB, 0x40, 4, duration,
                                scale, 1);
            SparkParticle_Spawn(&actor->position, RandomFrom(table), &rangeB, 0x5E, 4, duration,
                                scale, 1);
            SparkParticle_Spawn(&actor->position, RandomFrom(table), &rangeB, 0x80, 4, duration,
                                scale, 1);
            SparkParticle_Spawn(&actor->position, RandomFrom(table), &rangeB, 0xC4, 4, duration,
                                scale, 1);
            SparkParticle_Spawn(&actor->position, RandomFrom(table), &rangeB, 0xF0, 4, duration,
                                scale, 1);
        }
        category = actor->category;
        if (category == 4 || category == 1 || category == 5 || category == 7 || category == 6 ||
            category == 0x18 || category == 0x19)
            Sound_PlaySong(0xE5);
    }
    {
        void *callback = (void *)SceneActorControl_OnIdle;
        u32 kind = 16;
        u32 zero;
        u32 one;
        u8 *ready = &actor->field2B2;
        zero = 0;
        one = 1;
        *ready = one;
        actor->field2B0 = zero;
        actor->field2B5 = kind;
        actor->target = callback;
        actor->field2C8 = zero;
        {
            u32 *field = &actor->flags114;
            s32 clear = ~1;
            *field &= clear;
        }
        actor->word104 = zero;
        {
            u32 behavior = 2;
            actor->state2A8 = one;
            actor->phase2A9 = zero;
            actor->behavior2AA = behavior;
            actor->counter2AC = zero;
            actor->field2B1 = one;
        }
    }
    ActorShadow_Disable((u8 *)actor + 0x320);
    if (!*((u8 *)actor + 8))
        **(u32 **)((u8 *)actor + 4) |= 1;
    else {
        struct DefeatOwnerGroup {
            u32 unknown[2];
            u32 flags;
        };
        struct DefeatOwner {
            u8 unknown[0x20];
            struct DefeatOwnerGroup group;
        };
        struct DefeatOwner *owner = *(void **)((u8 *)actor + 4);
        struct DefeatOwnerGroup *ownerGroup = &owner->group;
        ownerGroup->flags |= 1;
    }
    ActorParticles_SpawnWideBurst(actor);
    SceneActorPlacement_PlayCategoryEventSound(actor);
}

extern u8 *gUnknown_02000580[];
void SceneActorRoute_CommitReachableDestination(void *);

#include "engine_globals.h"

u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 Mod(s32, s32);
s32 Div(s32, s32);
void sub_0807E334(struct SceneActorRegistryResetView *);
s32 Nav_IsPositionInMesh(void *, struct ContactPosition *, struct ContactPosition *);
s32 Terrain_ApplyBlocked(void *, struct ContactPosition *, struct ContactPosition *);

struct GolemReactionTarget {
    u8 unknown0[8];
    u8 playerIndex;
    u8 unknown9[3];
    struct ContactPosition position;
};

struct GolemReactionPlayerCell {
    u16 unknown0[3];
    u16 index;
};
struct GolemReactionPlayer {
    u8 unknown0[0x20];
    u32 flags;
    struct GolemReactionTarget target;
    u8 unknown38[8];
    struct GolemReactionPlayerCell *cell;
};

struct GolemReactionBody {
    u8 unknown0[0x624];
    u8 responseCount;
    u8 unknown625[3];
    u32 flags628;
    u8 unknown62C[0x54];
    s16 routeDelay;
    u8 unknown682[0x2A];
    u16 shakeStrength, shakeTimer;
};

struct GolemReactionRoutePoint {
    s16 x, z;
};
struct GolemReactionRouteLink {
    u8 unknown0[8];
    struct GolemReactionRoutePoint *point;
    u8 unknownC[8];
};
struct GolemReactionSavedControl {
    void *control;
    u8 state, phase, behavior, kind;
};
struct GolemReactionTurnState {
    u8 direction, savedDirection;
    s8 turnsRemaining;
    u8 unknown3;
};
struct GolemReactionGroup {
    u8 unknown0[0x40];
    s16 flags;
    u16 field42, timer44, unknown46;
    void *homeControl;
    struct SceneActorRegistryControl control;
    u8 unknown68[0xC];
    struct GolemReactionRouteLink homeRoute;
    u8 unknown88[4];
    struct GolemReactionSavedControl saved;
    u8 unknown94[0x162];
    u8 hitReactionLatch;
    u8 unknown1F7[5];
    void *route;
    struct ContactPosition *destination;
    struct GolemReactionTurnState turn;
    void *contactControl;
    u8 unknown20C[0x10];
    void *turnControl, *routePhaseCallback, *fallback;
};

struct GolemReactionTracking {
    struct GolemReactionTarget *target;
    struct ContactPosition destination;
    u8 unknownC, heading, unknownE, flags;
};

static inline void GolemReaction_SetBodyFlags(struct GolemReactionBody *body, u32 flags)
{
    body->flags628 |= flags;
}
static inline void GolemReaction_SetActorFlags(void *object, u32 bits)
{
    struct SceneActorRegistryResetView *view = object;
    view->flags2CC |= bits;
}

static inline struct GolemReactionGroup *GolemReaction_GroupOf(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    return (struct GolemReactionGroup *)&actor->group;
}

static inline u8 GolemReaction_Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}

static inline u8 GolemReaction_TestField(u16 *field, u32 mask)
{
    if (*field & mask)
        return 1;
    return 0;
}

static inline u16 GolemReaction_ReadMotionFlags(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    return actor->flags2D4;
}
static inline u16 GolemReaction_MotionFlags(void *object, u32 mask)
{
    struct SceneActorRegistryResetView *actor = object;
    u16 value = GolemReaction_ReadMotionFlags(actor);
    return value & mask;
}

static inline u32 GolemReaction_Bits32(u32 *field, u32 mask)
{
    return *field & mask;
}

static inline void GolemReaction_InstallControl(void *object, void *callback, u32 kind)
{
    struct SceneActorRegistryResetView *actor = object;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
}

static inline void GolemReaction_ResetMotionControl(void *object)
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

static inline void GolemReaction_SetControl(void *object, void *callback, u32 kind)
{
    struct SceneActorRegistryResetView *actor = object;
    GolemReaction_InstallControl(actor, callback, kind);
    GolemReaction_ResetMotionControl(actor);
}

static inline void GolemReaction_SelectState(void *object, u32 state, u32 phase, u32 behavior)
{
    struct SceneActorRegistryResetView *actor = object;
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
}

static inline void GolemReaction_ResetStateEntry(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline void GolemReaction_SetState(void *object, u32 state, u32 phase, u32 behavior)
{
    struct SceneActorRegistryResetView *actor = object;
    GolemReaction_SelectState(actor, state, phase, behavior);
    GolemReaction_ResetStateEntry(actor);
}

static inline void GolemReaction_SetPhase(void *object, u32 phase, u32 behavior)
{
    struct SceneActorRegistryResetView *actor = object;
    GolemReaction_SelectState(actor, 0, phase, behavior);
    GolemReaction_ResetStateEntry(actor);
}

static inline void GolemReaction_SetBehavior(void *object, u32 behavior)
{
    struct SceneActorRegistryResetView *actor = object;
    GolemReaction_SetPhase(actor, 0, behavior);
}

static inline struct EngineMap *GolemReaction_CurrentMap(void)
{
    return gTerrainMap;
}
struct GolemReactionTerrainCell {
    u8 attributes;
    u8 tile;
    u16 flags;
};
struct GolemReactionTerrainGrid {
    u32 field0;
    s16 width, height;
    s16 originX, originZ;
    struct GolemReactionTerrainCell cells[1];
};
struct GolemReactionTerrainOverride {
    u16 flags;
    u16 index;
    struct GolemReactionTerrainCell properties;
};
static inline struct GolemReactionTerrainCell *GolemReaction_MapCell(struct EngineMap *map,
                                                                     s32 cell)
{
    return &((struct GolemReactionTerrainGrid *)map->cells)->cells[cell];
}
static inline struct GolemReactionTerrainCell *GolemReaction_CellProperties(s32 cell)
{
    struct GolemReactionTerrainOverride *override =
        (void *)TerrainModifiers_FindCellOverride(cell, 1);
    if (override)
        return &override->properties;
    return GolemReaction_MapCell(GolemReaction_CurrentMap(), cell);
}
static inline u8 GolemReaction_CoordinateFraction(s16 coordinate)
{
    return coordinate;
}
static inline u32 GolemReaction_MapRowStart(const struct EngineMap *map, s32 row)
{
    return map->rows[row];
}
static inline s32 GolemReaction_CellIndex(s32 x, s32 z)
{
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return GolemReaction_MapRowStart(GolemReaction_CurrentMap(), z) + x;
}

static inline s32 GolemReaction_CellAt(struct ContactPosition *position)
{
    return GolemReaction_CellIndex(position->x >> 8, position->z >> 8);
}

static inline u32 GolemReaction_HeightAt(s32 cell, struct ContactPosition *position)
{
    struct GolemReactionTerrainCell *data = GolemReaction_CellProperties(cell);
    u32 kind, height;
    kind = data->attributes >> 4;
    height = (data->attributes & 15) << 8;
    switch (kind) {
        case 1:
            height -= GolemReaction_CoordinateFraction(position->z);
            break;
        case 2:
            height -= GolemReaction_CoordinateFraction(position->x);
            break;
    }
    return height;
}

static inline u32 GolemReaction_Height(struct ContactPosition *position)
{
    return GolemReaction_HeightAt(GolemReaction_CellAt(position), position);
}

static inline void GolemReaction_SnapToRoute(struct ContactPosition *position,
                                             struct GolemReactionRouteLink *route)
{
    position->x = route->point->x;
    position->z = route->point->z;
    position->y = GolemReaction_Height(position);
}

static inline void GolemReaction_ClearBits(u16 *field, u32 bits)
{
    *field &= ~bits;
}

#define GolemReaction_RouteStatus(route_, position_, target_)                                      \
    ({                                                                                             \
        void *route_owner_ = (route_);                                                             \
        struct ContactPosition *route_position_ = (position_);                                     \
        struct ContactPosition *route_target_ = (target_);                                         \
        s32 route_status_ = Nav_IsPositionInMesh(route_owner_, route_position_, route_target_);    \
        if (route_status_)                                                                         \
            route_status_ = Terrain_ApplyBlocked(route_owner_, route_position_, route_target_);    \
        route_status_;                                                                             \
    })

static inline u16 *GolemReaction_MotionFlagsOf(struct SceneActorRegistryResetView *actor)
{
    return &actor->flags2D4;
}

static inline struct ContactPosition *
GolemReaction_MotionOf(struct SceneActorRegistryResetView *actor)
{
    return &actor->motion;
}

static inline u8 GolemReaction_TryRoute(struct SceneActorRegistryResetView *actor, void *route,
                                        struct ContactPosition *position,
                                        struct ContactPosition *target)
{
    s32 result = GolemReaction_RouteStatus(route, position, target);
    if (result) {
        if (result != 2)
            Memory_Clear(GolemReaction_MotionOf(actor), 8);
        return 1;
    }
    GolemReaction_ClearBits(GolemReaction_MotionFlagsOf(actor), 0x200);
    return 0;
}

static inline void *GolemReaction_CurrentControl(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    return actor->target;
}

static inline u8 GolemReaction_ControlKind(struct SceneActorRegistryResetView *actor)
{
    return actor->field2B5;
}

static inline u8 GolemReaction_CurrentState(struct SceneActorRegistryResetView *actor)
{
    return actor->state2A8;
}

static inline u8 GolemReaction_CurrentPhase(struct SceneActorRegistryResetView *actor)
{
    return actor->phase2A9;
}

static inline u8 GolemReaction_CurrentBehavior(struct SceneActorRegistryResetView *actor)
{
    return actor->behavior2AA;
}

static inline void GolemReaction_SaveControl(void *p, void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    struct GolemReactionSavedControl *packet = p;
    packet->control = GolemReaction_CurrentControl(actor);
    packet->kind = GolemReaction_ControlKind(actor);
    packet->state = GolemReaction_CurrentState(actor);
    packet->phase = GolemReaction_CurrentPhase(actor);
    packet->behavior = GolemReaction_CurrentBehavior(actor);
}

struct GolemRandomTableView {
    const u16 *samples;
};
#define GolemReaction_NextSample(table_, seed_, wrap_, bits_)                                      \
    ({                                                                                             \
        const struct GolemRandomTableView *table_view_ = (table_);                                 \
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
static inline u32 GolemReaction_RandomDirectionExcept(u32 current, const u16 *randomSamples)
{
    s32 tries;
    struct GolemRandomTableView table = {randomSamples};
    for (tries = 0; tries < 4; tries++) {
        u32 direction = GolemReaction_NextSample(&table, &gRandomIndex, 1023, 3) << 6;
        if (direction != current)
            return direction;
    }
    return current;
}

static inline void GolemReaction_BeginTurn(void *obj, u32 saved, const u16 *randomSamples)
{
    struct SceneActorRegistryResetView *actor = obj;
    struct GolemReactionGroup *group = GolemReaction_GroupOf(obj);
    if (!GolemReaction_MotionFlags(actor, 0x400) &&
        !GolemReaction_TestField((u16 *)&group->flags, 4)) {
        group->turn.direction =
            GolemReaction_RandomDirectionExcept(actor->direction, randomSamples);
        GolemReaction_SaveControl(&group->saved, actor);
        group->turn.savedDirection = saved;
        GolemReaction_SetControl(actor, group->turnControl, 0x1B);
        GolemReaction_SetState(actor, 3, 1, 0);
        group->turn.turnsRemaining = 6;
    }
}

static inline s16 GolemReaction_MapWidth(struct EngineMap *map)
{
    return ((struct GolemReactionTerrainGrid *)map->cells)->width;
}
static inline s16 GolemReaction_TerrainLevelHeight(u32 level)
{
    return level << 8;
}
#define GolemReaction_CellCenterCoordinate(coordinate_)                                            \
    ({                                                                                             \
        s32 grid_coordinate_ = (coordinate_);                                                      \
        (s16)(((u32)grid_coordinate_ << 8) + 0x80);                                                \
    })

static inline void GolemReaction_CellCenter(s32 cell, struct ContactPosition *position)
{
    s32 x = Mod(cell, GolemReaction_MapWidth(GolemReaction_CurrentMap()));
    s32 z = Div(cell, GolemReaction_MapWidth(GolemReaction_CurrentMap()));
    position->x = GolemReaction_CellCenterCoordinate(x);
    {
        u32 attributes = GolemReaction_CellProperties(cell)->attributes;
        u32 height = 15;
        height &= attributes;
        position->y = GolemReaction_TerrainLevelHeight(height);
    }
    position->z = GolemReaction_CellCenterCoordinate(z);
}

static inline s32 GolemReaction_PlayerSlot(struct GolemReactionTarget *target)
{
    if (target->playerIndex <= 3)
        return target->playerIndex;
    return -1;
}
/* A successful cell-center reroute returns one. Other paths leave the
 * caller to try the separate partner-position route. */
static inline u8 GolemReaction_Arrive(void *obj)
{
    struct ContactPosition target;
    struct SceneActorRegistryResetView *actor = obj;
    struct GolemReactionGroup *group = GolemReaction_GroupOf(obj);
    struct GolemReactionTracking *tracking = (struct GolemReactionTracking *)&actor->fieldDC;
    s32 cell, player;
    const u16 *randomSamples = gRandomTable;
    void **callback;
    GolemReaction_SnapToRoute(group->destination, &group->homeRoute);
    if (GolemReaction_Test(gEngineFlags, 0x4800)) {
        struct GolemReactionTarget *t = tracking->target;
        if (!t)
            goto invalid_target;
        player = GolemReaction_PlayerSlot(t);
        if (player < 0) {
        invalid_target:
            callback = &group->fallback;
            goto invoke;
        }
        cell = ((struct GolemReactionPlayer *)gUnknown_02000580[player])->cell->index;
    } else {
        struct GolemReactionTarget *t = tracking->target;
        cell = GolemReaction_CellIndex(t->position.x >> 8, t->position.z >> 8);
        player = 0;
    }
    if (cell != actor->terrainCell &&
        !GolemReaction_Bits32(&((struct GolemReactionPlayer *)gUnknown_02000580[player])->flags,
                              0x20000))
        goto reroute;
    if (GolemReaction_MotionFlags(actor, 0x400))
        return 0;
    {
        u32 bits = 4;
        group->flags &= ~bits;
    }
    if (GolemReaction_MotionFlags(actor, 0x80)) {
        GolemReaction_SetControl(actor, group->homeControl, 0x1A);
        GolemReaction_SetBehavior(actor, 12);
    } else {
        GolemReaction_SetControl(actor, actor->entryTarget228, 1);
        GolemReaction_SetBehavior(actor, 5);
        GolemReaction_SnapToRoute(group->destination, &group->homeRoute);
        (group->destination)->y =
            GolemReaction_HeightAt(GolemReaction_CellAt(group->destination), group->destination);
        (group->destination)->pad = 0;
    }
    {
        u8 saved = actor->direction;
        GolemReaction_BeginTurn(obj, saved, randomSamples);
    }
    sub_0807E334(obj);
    actor->field2C2 = 0x20;
    return 0;
reroute: {
    GolemReaction_CellCenter(cell, &target);
    if (GolemReaction_TryRoute(obj, group->route, &actor->position, &target))
        goto success;
}
    callback = &group->fallback;
invoke:
    Callback_InvokeActor(actor, *callback);
    return 0;
success:
    return 1;
}

/* Fields are established by 080C9C04 and the ordinary ReturnHome/ChargeOrReturnHome
 * relatives. Names for the response count and flags are tentative. */
static inline u16 GolemReaction_QueryMotionFlags(struct SceneActorRegistryResetView *actor,
                                                 u16 mask)
{
    return actor->flags2D4 & mask;
}
static inline void GolemReaction_ClearMotionFlags(struct SceneActorRegistryResetView *actor,
                                                  u32 bits)
{
    u32 mask = ~bits;
    actor->flags2D4 &= mask;
}
static inline u8 GolemReaction_TrackingFlags(struct GolemReactionTracking *tracking, u32 mask)
{
    if (tracking->flags & mask)
        return 1;
    return 0;
}
static inline void GolemReaction_SetResponseControl(struct SceneActorRegistryResetView *actor,
                                                    void *control, u32 kind)
{
    GolemReaction_InstallControl(actor, control, kind);
    GolemReaction_ResetMotionControl(actor);
}
static inline void GolemReaction_SetResponseState(struct SceneActorRegistryResetView *actor,
                                                  u32 state, u32 phase, u32 behavior)
{
    GolemReaction_SelectState(actor, state, phase, behavior);
    GolemReaction_ResetStateEntry(actor);
}
static inline void GolemReaction_LatchHitReact(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    if (!actor->field5CA && (actor->field2E6 & 8)) {
        actor->field2B8 = 0x2D;
        actor->field5CA = 0x41;
        SceneActorAnimation_SelectDirectionalEntry(actor, 3);
    }
}
static inline s16 GolemReaction_CurrentHeight(struct SceneActorRegistryResetView *actor)
{
    return actor->fieldA8;
}

static inline void GolemReaction_SetChargeHeight(struct SceneActorRegistryResetView *actor,
                                                 s32 height)
{
    actor->fieldAA = height;
}
static inline void GolemReaction_ClearWordFlags(u32 *flags, u32 bits)
{
    u32 mask = ~bits;
    *flags &= mask;
}
static inline void GolemReaction_EnterCharge(struct SceneActorRegistryResetView *actor, s16 height)
{
    GolemReaction_SetChargeHeight(actor, height);
    GolemReaction_SetResponseControl(actor, actor->reactionTarget230, 0x12);
    GolemReaction_SetResponseState(actor, 3, 0, 0);
}
static inline void GolemReaction_HalfwordFlagsSet(u16 *field, u32 bits)
{
    u32 value = *field;
    bits |= value;
    *field = bits;
}
static inline void GolemReaction_GroupSetFlags(struct GolemReactionGroup *owner, u32 bits)
{
    GolemReaction_HalfwordFlagsSet((u16 *)&owner->flags, bits);
}
static inline void GolemReaction_Settle(struct SceneActorRegistryResetView *actor)
{
    GolemReaction_SetResponseControl(actor, actor->entryTarget228, 1);
    GolemReaction_SetResponseState(actor, 0, 0, 7);
    actor->field2C2 = 0x20;
}

/* Dispatch the hit-reaction state; return one when entering a state. */
s32 Golem_UpdateHitReaction(struct SceneActorRegistryResetView *actor)
{
    struct GolemReactionBody *body = actor->context;
    struct GolemReactionGroup *group = (struct GolemReactionGroup *)&actor->group;
    void *object = actor;

    GolemReaction_SetBodyFlags(body, 0x800);
    actor->flags2D0 &= ~0x1000000;
    SceneActorPlacement_PlayVariantEventSound(actor, 1);

    if (GolemReaction_QueryMotionFlags(actor, 0x2000)) {
        actor->direction = actor->field2B6 + 0x80;
        SceneActorAnimation_SelectDirectionalEntry(actor, 0);
        GolemReaction_ClearMotionFlags(actor, 0x2020);
        GolemReaction_SetActorFlags(actor, 0x10);
        group->destination->pad = 0;
        ((struct GolemReactionTracking *)&actor->fieldDC)->target =
            &((struct GolemReactionPlayer *)gUnknown_02000580[0])->target;
        if (!GolemReaction_Arrive(object)) {
            if (!GolemReaction_TryRoute(
                    object, group->route, &actor->position,
                    &((struct GolemReactionTracking *)&actor->fieldDC)->target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return 0;
            }
        }
        GolemReaction_Settle(actor);
        return 1;
    }
    if (GolemReaction_QueryMotionFlags(actor, 0x8000)) {
        GolemReaction_ClearMotionFlags(actor, 0x20);
        return 0;
    }
    if (GolemReaction_QueryMotionFlags(actor, 2)) {
        actor->reactionStart = actor->reactionEnd = actor->position;
        GolemReaction_SetResponseControl(actor, group->contactControl, 7);
        GolemReaction_SetResponseState(actor, 3, 0, 0);
        return 1;
    }
    if (GolemReaction_QueryMotionFlags(actor, 0x100)) {
        if (actor->field2D8 & 0x400) {
            body->shakeStrength = 30;
            body->shakeTimer = 20;
            GolemReaction_EnterCharge(actor, GolemReaction_CurrentHeight(actor));
            actor->field2DC |= 0x80000;
            GolemReaction_ClearWordFlags(&actor->flags2CC, 0x80);
            GolemReaction_ClearWordFlags(&actor->flags2D0, 0x1000003);
            actor->field2C2 = 0x40;
            return 1;
        }
        if (actor->field2DC & 0xC0000) {
            GolemReaction_EnterCharge(actor, GolemReaction_CurrentHeight(actor));
            actor->flags2D0 &= ~0x1000000;
            return 1;
        }
        if (((struct GolemReactionTracking *)&actor->fieldDC)->target) {
            SceneActorRoute_CommitReachableDestination(actor);
            body->routeDelay = 120;
        }
        GolemReaction_ClearMotionFlags(actor, 0x20);
        return 0;
    }
    if (actor->field2DC & 0xC0000) {
        GolemReaction_EnterCharge(actor, GolemReaction_CurrentHeight(actor));
        actor->flags2D0 &= ~0x1000000;
        return 1;
    }

    GolemReaction_ClearMotionFlags(actor, 0x20);
    if (GolemReaction_TrackingFlags((struct GolemReactionTracking *)&actor->fieldDC, 0x10))
        return 0;
    if (GolemReaction_QueryMotionFlags(actor, 0x80)) {
        GolemReaction_LatchHitReact(object);
        if (!((struct GolemReactionTracking *)&actor->fieldDC)->target)
            return 0;
        ((struct GolemReactionTracking *)&actor->fieldDC)->target =
            &((struct GolemReactionPlayer *)gUnknown_02000580[0])->target;
        GolemReaction_SetActorFlags(actor, 0x80);
        GolemReaction_GroupSetFlags(group, 4);
        if (!GolemReaction_Arrive(object)) {
            if (!GolemReaction_TryRoute(
                    object, group->route, &actor->position,
                    &((struct GolemReactionTracking *)&actor->fieldDC)->target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return 0;
            }
        }
        GolemReaction_SetResponseControl(actor, group->homeControl, 0x1A);
        GolemReaction_SetResponseState(actor, 0, 0, 12);
        return 1;
    }
    if (!body->responseCount) {
        GolemReaction_LatchHitReact(object);
        if (!((struct GolemReactionTracking *)&actor->fieldDC)->target)
            return 0;
        ((struct GolemReactionTracking *)&actor->fieldDC)->target =
            &((struct GolemReactionPlayer *)gUnknown_02000580[0])->target;
        actor->direction = actor->fieldE9;
        group->destination->pad = 1;
        if (!GolemReaction_Arrive(object)) {
            if (!GolemReaction_TryRoute(
                    object, group->route, &actor->position,
                    &((struct GolemReactionTracking *)&actor->fieldDC)->target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return 0;
            }
        }
        actor->field2C2 = 0x40;
        if (body->responseCount <= 2)
            body->responseCount++;
        GolemReaction_SetResponseControl(actor, SceneActorReaction_PlayAnimationBUntilComplete, 10);
        GolemReaction_SetResponseState(actor, 0, 1, 1);
        GolemReaction_SetActorFlags(actor, 0x10);
        return 1;
    }

    GolemReaction_LatchHitReact(object);
    if (!((struct GolemReactionTracking *)&actor->fieldDC)->target)
        return 0;
    ((struct GolemReactionTracking *)&actor->fieldDC)->target =
        &((struct GolemReactionPlayer *)gUnknown_02000580[0])->target;
    GolemReaction_SetActorFlags(actor, 0x90);
    group->destination->pad = 0;
    if (!GolemReaction_Arrive(object)) {
        if (!GolemReaction_TryRoute(
                object, group->route, &actor->position,
                &((struct GolemReactionTracking *)&actor->fieldDC)->target->position)) {
            Callback_InvokeActor(actor, group->fallback);
            return 0;
        }
    }
    GolemReaction_Settle(actor);
    return 1;
}

void Golem_UpdateScriptedAction(struct SceneActorRegistryResetView *obj)
{
    if (TakeFlag(((u8 *)obj) + 0x2B1)) {
        void (*fn)(void) = SceneActorReaction_PlayAnimationBUntilComplete;
        u8 code = 10;
        u32 zero;
        u32 *f;
        s32 mask;
        u16 *h;
        u16 value;
        s32 off;
        {
            u8 *taken = &obj->field2B2;
            zero = 0;
            *taken = 1;
        }
        do {
            u32 field = 0x2B0;
            do {
                ((u8 *)obj)[field] = zero;
            } while (0);
            field += 5;
            ((u8 *)obj)[off = field] = code;
            obj->target = (void *)fn;
            field += 0x13;
            *((u16 *)(((u8 *)obj) + field)) = zero;
        } while (0);
        f = (u32 *)(((u8 *)obj) + 0x114);
        mask = -2;
        *f &= mask;
        *((u32 *)(((u8 *)obj) + 0x104)) = zero;
        {
            u32 byte = obj->fieldE9;
            obj->direction = byte;
        }
        SceneActorAttachment_ReplaceResponseSlot((u8 *)obj, 0, 0, obj->category);
        SceneActorBehavior_CallActionScript((u8 *)obj);
        h = (u16 *)(((u8 *)obj) + 0x414);
        {
            u32 flag = 4;
            flag |= *h;
            *h = flag;
        }
        {
            u32 flag = 0x80;
            u16 *flags = (u16 *)(((u8 *)obj) + 0x2D4);
            flag |= *flags;
            *flags = flag;
        }
        off = 0x416;
        value = *((u16 *)(((u8 *)obj) + off));
        off += 2;
        *((u16 *)(((u8 *)obj) - (-off))) = value;
    }
    if (obj->field2B0) {
        Golem_ResetIdleControl((u8 *)obj);
    } else {
        ++(*((u32 *)(((u8 *)obj) + 0x2AC)));
    }
}

#ifdef NONMATCHING
// fuzzy: 80.8%
u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 Mod(s32, s32);
s32 Div(s32, s32);
static inline s32 DelayedRecovery_SceneActorTerrain_Width(void)
{
    return *(s16 *)(gTerrainMap->cells + 4);
}
static inline u8 *DelayedRecovery_SceneActorTerrain_GetCell(s32 cell)
{
    u8 *override = TerrainModifiers_FindCellOverride(cell, 1);
    if (override)
        return override + 4;
    return gTerrainMap->cells + (cell * 4 + 12);
}
static inline void DelayedRecovery_SceneActorTerrain_GetCenter(s32 cell,
                                                               struct ContactPosition *position)
{
    s32 x = Mod(cell, DelayedRecovery_SceneActorTerrain_Width());
    s32 z = Div(cell, DelayedRecovery_SceneActorTerrain_Width());
    position->x = ((u32)x << 8) + 128;
    position->y = (*DelayedRecovery_SceneActorTerrain_GetCell(cell) & 15) << 8;
    position->z = ((u32)z << 8) + 128;
}

// Remaining: recovery stack slots, flag-address reuse and saved-state scheduling.
s32 Callback_InvokeActor(void *, void *);
s32 Nav_IsPositionInMesh(void *, struct ContactPosition *, struct ContactPosition *);
s32 Terrain_ApplyBlocked(void *, struct ContactPosition *, struct ContactPosition *);
void sub_0807E334(struct SceneActorRegistryResetView *);
u8 sub_0812F3CC(void *);
void SceneActorReaction_RestoreAfterDelay(void *);

void Sound_PlaySong(u32);

#define F8(a, o)  (*(u8 *)((u8 *)(a) + (o)))
#define F16(a, o) (*(u16 *)((u8 *)(a) + (o)))
#define S16(a, o) (*(s16 *)((u8 *)(a) + (o)))
#define F32(a, o) (*(u32 *)((u8 *)(a) + (o)))
#define PTR(a, o) (*(void **)((u8 *)(a) + (o)))

struct DelayedRecovery_GolemRoutePoint {
    s16 x, z;
};
struct DelayedRecovery_GolemRouteLink {
    u8 unknown0[8];
    struct DelayedRecovery_GolemRoutePoint *point;
};
struct DelayedRecovery_GolemFlags16 {
    u16 value;
};
#define HOME (*(struct ContactPosition **)(group + 0x200))
struct DelayedRecovery_GolemTurn {
    u8 direction, savedDirection, mode;
};
struct DelayedRecovery_GolemSavedControl {
    void *target;
    u8 state, phase, behavior, kind;
};

static inline u32 DelayedRecovery_Flag16(void *p, u32 maskInput, u32 off)
{
    u32 mask = maskInput;
    return *(u16 *)((u8 *)p + off) & mask;
}
static inline u32 DelayedRecovery_Flag32(void *p, u32 maskInput, u32 off)
{
    u32 mask = maskInput;
    return *(u32 *)((u8 *)p + off) & mask;
}
static inline u8 DelayedRecovery_TestBits16(u16 *p, u32 maskInput)
{
    u32 mask = maskInput;
    if (*p & mask)
        return 1;
    return 0;
}
static inline s32 DelayedRecovery_PlayerSlot(u8 *actor)
{
    if (actor[8] <= 3)
        return actor[8];
    return -1;
}
static inline u8 DelayedRecovery_Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline void DelayedRecovery_SetControl(struct SceneActorRegistryResetView *actor, u32 kind,
                                              void *control)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void DelayedRecovery_SetState(struct SceneActorRegistryResetView *actor, u32 state,
                                            u32 phase, u32 behavior)

{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline u32 DelayedRecovery_MaskFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}

static inline u32 DelayedRecovery_CellIndex(struct ContactPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u32 DelayedRecovery_CellHeight(u32 cell, struct ContactPosition *position)
{
    u32 shape, height;
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        data = map->cells + offset;
    }
    shape = *data >> 4;
    height = (*data & 15) << 8;
    if (shape == 1)
        goto slopeZ;
    if (shape == 2)
        goto slopeX;
    goto done;
slopeZ:
    height -= *((u8 *)position + 4);
    goto done;
slopeX:
    height -= *(u8 *)position;
done:
    return height;
}
static inline u8 DelayedRecovery_TryMove(struct SceneActorRegistryResetView *actor, void *route,
                                         struct ContactPosition *position,
                                         struct ContactPosition *destination)
{
    s32 result;
    if (!Nav_IsPositionInMesh(route, position, destination)) {
        u32 mask = ~0x200;
        actor->flags2D4 = mask & actor->flags2D4;
        return 0;
    } else {
        result = Terrain_ApplyBlocked(route, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->motion, 8);
            return 1;
        } else {
            u32 mask = ~0x200;
            actor->flags2D4 = mask & actor->flags2D4;
            return 0;
        }
    }
}
static inline u32 DelayedRecovery_Random(const u16 *table, u32 *seed, u32 wrap, u32 bits)
{
    u32 index = (*seed + 1) & wrap;
    u32 v;
    *seed = index;
    index *= 2;
    v = *(const u16 *)(index + (u32)table);
    v &= bits;
    return v;
}
static inline u32 DelayedRecovery_RandomDirectionExcept(u32 current)
{
    s32 i = 0;
    u32 d;
    do {
        d = DelayedRecovery_Random(gRandomTable, &gRandomIndex, 0x3FF, 3) << 6;
        if (d != current)
            goto chosen;
    } while (++i <= 3);
    d = current;
chosen:
    return d;
}
static inline void DelayedRecovery_Wander(struct SceneActorRegistryResetView *input, u8 direction)
{
    struct SceneActorRegistryResetView *actor = input;
    u8 *group = (u8 *)&actor->group;
    if (!DelayedRecovery_Flag16(actor, 0x400, 0x2D4) &&
        !DelayedRecovery_TestBits16((u16 *)(group + 0x40), 4)) {
        ((struct DelayedRecovery_GolemTurn *)(group + 0x204))->direction =
            DelayedRecovery_RandomDirectionExcept(actor->direction);
        ((struct DelayedRecovery_GolemSavedControl *)(group + 0x8C))->target = actor->target;
        ((struct DelayedRecovery_GolemSavedControl *)(group + 0x8C))->kind = actor->field2B5;
        ((struct DelayedRecovery_GolemSavedControl *)(group + 0x8C))->state = actor->state2A8;
        ((struct DelayedRecovery_GolemSavedControl *)(group + 0x8C))->phase = actor->phase2A9;
        ((struct DelayedRecovery_GolemSavedControl *)(group + 0x8C))->behavior = actor->behavior2AA;
        group[0x205] = direction;
        {
            void *target = PTR(group, 0x21C);
            DelayedRecovery_SetControl(actor, 0x1B, target);
        }
        DelayedRecovery_SetState(actor, 3, 1, 0);
        group[0x206] = 6;
    }
}
static inline u8 DelayedRecovery_Chase(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor;
    u8 *group;
    u32 cell;
    s32 slot;
    u32 *targetp;
    actor = input;
    group = (u8 *)&actor->group;
    targetp = &actor->fieldDC;
    {
        struct ContactPosition *dest = actor->state5D4;
        struct DelayedRecovery_GolemRouteLink *link = (void *)&actor->route;
        dest->x = link->point->x;
        dest->z = link->point->z;
        dest->y = DelayedRecovery_CellHeight(DelayedRecovery_CellIndex(dest), dest);
    }
    if (DelayedRecovery_Test(gEngineFlags, 0x4800)) {
        u8 *target = (u8 *)*targetp;
        if (!target)
            goto fail;
        slot = DelayedRecovery_PlayerSlot(target);
        if (slot < 0)
            goto fail;
        cell = *(u16 *)((u8 *)PTR(gPlayers[slot], 0x40) + 6);
    } else {
        cell = DelayedRecovery_CellIndex((struct ContactPosition *)(*targetp + 0xC));
        slot = 0;
    }
    if (cell == actor->terrainCell || DelayedRecovery_Flag32(gPlayers[slot], 0x20000, 0x20)) {
        u16 busy = DelayedRecovery_Flag16(actor, 0x400, 0x2D4);
        u16 blocked;
        if (busy)
            return 0;
        {
            struct DelayedRecovery_GolemFlags16 *flags = (void *)(group + 0x40);
            s32 mask = ~4;
            flags->value = mask & flags->value;
        }
        blocked = DelayedRecovery_Flag16(actor, 0x80, 0x2D4);
        if (blocked) {
            void *target = PTR(group, 0x48);
            DelayedRecovery_SetControl(actor, 0x1A, target);
            DelayedRecovery_SetState(actor, 0, 0, 12);
        } else {
            {
                void *target = actor->entryTarget228;
                DelayedRecovery_SetControl(actor, 1, target);
            }
            DelayedRecovery_SetState(actor, 0, 0, 5);
            {
                struct ContactPosition *dest = *(struct ContactPosition **)(group + 0x200);
                struct DelayedRecovery_GolemRouteLink *link =
                    (struct DelayedRecovery_GolemRouteLink *)(group + 0x74);
                dest->x = link->point->x;
                dest->z = link->point->z;
                dest->y = DelayedRecovery_CellHeight(DelayedRecovery_CellIndex(dest), dest);
                HOME->y = DelayedRecovery_CellHeight(DelayedRecovery_CellIndex(HOME), HOME);
                HOME->pad = 0;
            }
        }
        DelayedRecovery_Wander(input, actor->direction);
        sub_0807E334(input);
        actor->field2C2 = 0x20;
        return 0;
    }
    {
        struct ContactPosition center;
        DelayedRecovery_SceneActorTerrain_GetCenter(cell, &center);
        if (!DelayedRecovery_TryMove(input, PTR(group, 0x1FC), &actor->position, &center)) {
        fail:
            Callback_InvokeActor(actor, PTR(group, 0x224));
            return 0;
        }
    }
    return 1;
}

struct DelayedRecovery_DelayedRoute {
    u8 pad0[8];
    u8 state, phase, behavior, kind;
    u32 delay;
    void *resume;
};
static inline void DelayedRecovery_PrepareDelay(u8 *group, void *callback, u32 behavior)
{
    struct DelayedRecovery_DelayedRoute *saved = (void *)(group + 0x8c);
    saved->delay = 30;
    saved->resume = callback;
    saved->state = 0;
    saved->phase = 0;
    saved->behavior = behavior;
}
// confidence=low: waits for a recovery delay, then restores chase or return-home movement.
void Golem_ResumeDelayedRoute(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;
    u32 zero;
    if (actor->field2B1)
        actor->field2B1 = 0;
    if ((u8)Callback_InvokeActor(actor, actor->update270))
        return;
    zero = (u8)sub_0812F3CC(actor);
    if (zero)
        return;
    if ((s32)actor->counter2AC <= 60)
        goto waiting;
    SceneActorAttachment_ReplaceResponseSlot((u8 *)actor, 0, 2, actor->category);
    DelayedRecovery_SetState(actor, 3, 1, zero);
    DelayedRecovery_SetControl(actor, 14, SceneActorReaction_RestoreAfterDelay);
    if (DelayedRecovery_MaskFlags(actor, 0x80)) {
        DelayedRecovery_PrepareDelay(group, *(void **)(group + 0x48), 12);
        DelayedRecovery_Chase(actor);
    } else {
        struct ContactPosition *dest;
        struct DelayedRecovery_GolemRouteLink *link;
        actor->flags2CC |= 0x10;
        DelayedRecovery_PrepareDelay(group, actor->entryTarget228, 7);
        dest = HOME;
        link = (void *)(group + 0x74);
        dest->x = link->point->x;
        dest->z = link->point->z;
        dest->y = DelayedRecovery_CellHeight(DelayedRecovery_CellIndex(dest), dest);
        if (!DelayedRecovery_TryMove(actor, PTR(group, 0x1fc), &actor->position, HOME)) {
            DelayedRecovery_SetControl(actor, 0, SceneActorControl_OnIdle);
            DelayedRecovery_SetState(actor, 3, 0, 0);
        } else
            HOME->pad = 1;
    }
    return;
waiting:
    actor->counter2AC++;
}

#else
INCLUDE_ASM("asm/nonmatching", Golem_ResumeDelayedRoute);
#endif

void Golem_InitActionHandlers(struct SceneActorRegistryResetView *actor)
{
    actor->alternateBehaviorCallbacks = (void **)gUnk_08606134;
}

s32 Golem_TickFacingQuadrant(struct SceneActorRegistryResetView *a)
{
    u8 *base = (u8 *)&a->id;
    struct GolemState *s = a->context;
    if (!Unavailable(a)) {
        if (s->countdown626)
            s->countdown626--;
    }
    {
        u32 quadrant = (base[9] + 0x60) >> 6;
        u32 mask = 3;
        mask &= quadrant;
        a->unknown68C = mask;
    }
    a->savedDirection690 = a->direction68E;
    return 1;
}

s32 Golem_UpdateEffectsAndContacts(struct SceneActorRegistryResetView *actor)
{
    struct GolemState *state = actor->context;
    u32 m = 0x40;
    if (!(actor->flags2D4 & m)) {
        u16 *p = &state->shakeTimer6AE;
        if (*p != 0) {
            Camera_RequestShake(state->shakeStrength6AC);
            *p = *p - 1;
        }
    }
    Skeleton_TryTriggerAction(actor);
    Skeleton_ClearPendingFlag(actor);
    Golem_UpdateAttackProperties(actor);
    return 1;
}
