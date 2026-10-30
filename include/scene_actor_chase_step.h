#ifndef GUARD_SCENE_ACTOR_CHASE_STEP_H
#define GUARD_SCENE_ACTOR_CHASE_STEP_H

/* Stored selectors and arithmetic selectors feed the same actor state fields. */
#if SCENE_ACTOR_CHASE_WIDE_STATE
static inline void SceneActorChase_SelectState(void *object, u32 state, u32 phase, u32 behavior)
#else
static inline void SceneActorChase_SelectState(void *object, u8 state, u8 phase, u8 behavior)
#endif
{
    struct SceneActorRegistryResetView *actor = object;
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
}

#if SCENE_ACTOR_CHASE_FRACTION_ACCESSOR
static inline u8 SceneActorChase_CoordinateFraction(s16 coordinate)
{
    return coordinate;
}
#define SCENE_ACTOR_CHASE_FRACTION(v_) SceneActorChase_CoordinateFraction(v_)
#else
#define SCENE_ACTOR_CHASE_FRACTION(v_) ((u8)(v_))
#endif

/* The query may own a stored position or receive its already promoted axes. */
#define SCENE_ACTOR_CHASE_CELL_LOOKUP                                                              \
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)                     \
        return 0;                                                                                  \
    return SceneActorChase_MapRowStart(SceneActorChase_CurrentMap(), z) + x;

#if SCENE_ACTOR_CHASE_SCALAR_CELLS
static inline s32 SceneActorChase_CellIndex(s32 x, s32 z)
{
    SCENE_ACTOR_CHASE_CELL_LOOKUP
}
#endif

static inline s32 SceneActorChase_CellAt(struct ContactPosition *position)
{
#if SCENE_ACTOR_CHASE_SCALAR_CELLS
    return SceneActorChase_CellIndex(position->x >> 8, position->z >> 8);
#else
    s32 x = position->x >> 8;
    s32 z = position->z >> 8;
    SCENE_ACTOR_CHASE_CELL_LOOKUP
#endif
}
#undef SCENE_ACTOR_CHASE_CELL_LOOKUP

static inline u32 SceneActorChase_HeightAt(s32 cell, struct ContactPosition *position)
{
    struct SceneActorChaseTerrainCell *data = SceneActorChase_CellProperties(cell);
    u32 kind, height;
    kind = data->attributes >> 4;
    height = (data->attributes & 15) << 8;
    switch (kind) {
        case 1:
            height -= SCENE_ACTOR_CHASE_FRACTION(position->z);
            break;
        case 2:
            height -= SCENE_ACTOR_CHASE_FRACTION(position->x);
            break;
    }
    return height;
}
#undef SCENE_ACTOR_CHASE_FRACTION

#if SCENE_ACTOR_CHASE_JOIN_ROUTE_FAILURES
#define SceneActorChase_RouteStatus(route_, position_, target_)                                    \
    ({                                                                                             \
        void *route_owner_ = (route_);                                                             \
        struct ContactPosition *route_position_ = (position_);                                     \
        struct ContactPosition *route_target_ = (target_);                                         \
        s32 route_status_ = Nav_IsPositionInMesh(route_owner_, route_position_, route_target_);    \
        if (route_status_)                                                                         \
            route_status_ = Terrain_ApplyBlocked(route_owner_, route_position_, route_target_);    \
        route_status_;                                                                             \
    })

#endif

#if SCENE_ACTOR_CHASE_DIRECT_ROUTE_FIELDS
#define SCENE_ACTOR_CHASE_ROUTE_FLAGS(a_)  (&(a_)->flags2D4)
#define SCENE_ACTOR_CHASE_ROUTE_MOTION(a_) (&(a_)->body.motion)
#else
#define SCENE_ACTOR_CHASE_ROUTE_FLAGS(a_)  SceneActorChase_MotionFlagsOf(a_)
#define SCENE_ACTOR_CHASE_ROUTE_MOTION(a_) SceneActorChase_MotionOf(a_)
#endif

static inline u8 SceneActorChase_TryRoute(struct SceneActorRegistryResetView *actor, void *route,
                                          struct ContactPosition *position,
                                          struct ContactPosition *target)
{
#if SCENE_ACTOR_CHASE_JOIN_ROUTE_FAILURES
    s32 result = SceneActorChase_RouteStatus(route, position, target);
#else
    s32 result;
    if (!Nav_IsPositionInMesh(route, position, target)) {
        SceneActorChase_ClearBits(SCENE_ACTOR_CHASE_ROUTE_FLAGS(actor), 0x200);
        return 0;
    } else {
        result = Terrain_ApplyBlocked(route, position, target);
#endif
    if (result) {
        if (result != 2)
            Memory_Clear(SCENE_ACTOR_CHASE_ROUTE_MOTION(actor), 8);
        return 1;
    }
#if !SCENE_ACTOR_CHASE_JOIN_ROUTE_FAILURES
    else {
#endif
        SceneActorChase_ClearBits(SCENE_ACTOR_CHASE_ROUTE_FLAGS(actor), 0x200);
        return 0;
#if !SCENE_ACTOR_CHASE_JOIN_ROUTE_FAILURES
    }
}
#endif
}
#undef SCENE_ACTOR_CHASE_ROUTE_FLAGS
#undef SCENE_ACTOR_CHASE_ROUTE_MOTION
#if SCENE_ACTOR_CHASE_JOIN_ROUTE_FAILURES
#undef SceneActorChase_RouteStatus
#endif

#if SCENE_ACTOR_CHASE_SCALAR_CELLS
#define SCENE_ACTOR_CHASE_TARGET_CELL(t_)                                                          \
    SceneActorChase_CellIndex((t_)->position.x >> 8, (t_)->position.z >> 8)
#else
#define SCENE_ACTOR_CHASE_TARGET_CELL(t_) SceneActorChase_CellAt(&(t_)->position)
#endif

static inline void SceneActorChase_SetState(void *object, u32 state, u32 phase, u32 behavior)
{
    struct SceneActorRegistryResetView *actor = object;
    SceneActorChase_SelectState(actor, state, phase, behavior);
    SceneActorChase_ResetStateEntry(actor);
}

static inline void SceneActorChase_SetPhase(void *object, u32 phase, u32 behavior)
{
    struct SceneActorRegistryResetView *actor = object;
    SceneActorChase_SelectState(actor, 0, phase, behavior);
    SceneActorChase_ResetStateEntry(actor);
}

static inline void SceneActorChase_SetBehavior(void *object, u32 behavior)
{
    struct SceneActorRegistryResetView *actor = object;
    SceneActorChase_SetPhase(actor, 0, behavior);
}

static inline u32 SceneActorChase_Height(struct ContactPosition *position)
{
    return SceneActorChase_HeightAt(SceneActorChase_CellAt(position), position);
}

static inline void SceneActorChase_SnapToRoute(struct ContactPosition *position,
                                               struct SceneActorChaseRouteLink *route)
{
    position->x = route->point->x;
    position->z = route->point->z;
    position->y = SceneActorChase_Height(position);
}

static inline void SceneActorChase_BeginTurn(void *obj, u32 saved, const u16 *randomSamples)
{
    struct SceneActorRegistryResetView *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    if (!SceneActorChase_MotionFlags(actor, 0x400) &&
        !SceneActorChase_TestField((u16 *)&group->flags, 4)) {
        group->turn.direction =
            SceneActorChase_RandomDirectionExcept(actor->body.direction, randomSamples);
        SceneActorChase_SaveControl(&group->saved, actor);
        group->turn.savedDirection = saved;
        SceneActorChase_SetControl(actor, group->turnControl, 0x1B);
        SceneActorChase_SetState(actor, 3, 1, 0);
        group->turn.turnsRemaining = 6;
    }
}

/* A successful cell-center reroute returns one. Other paths leave the
 * caller to try the separate partner-position route. */

static inline u8 SceneActorChase_Arrive(void *obj)
{
    struct ContactPosition target;
    struct SceneActorRegistryResetView *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    struct SceneActorChaseTracking *tracking = (struct SceneActorChaseTracking *)&actor->tracking.target;
    s32 cell, player;
    const u16 *randomSamples = gRandomTable;
    void **callback;
    SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
    if (SceneActorChase_Test(gEngineFlags, 0x4800)) {
        struct SceneActorChaseTarget *t = tracking->target;
        if (!t)
            goto invalid_target;
        player = SceneActorChase_PlayerSlot(t);
        if (player < 0) {
        invalid_target:
            callback = &group->fallback;
            goto invoke;
        }
        cell = ((struct SceneActorChasePlayer *)gUnknown_02000580[player])->cell->index;
    } else {
        struct SceneActorChaseTarget *t = tracking->target;
        cell = SCENE_ACTOR_CHASE_TARGET_CELL(t);
        player = 0;
    }
    if (cell != actor->terrainCell &&
        !SceneActorChase_Bits32(&((struct SceneActorChasePlayer *)gUnknown_02000580[player])->flags,
                                0x20000))
        goto reroute;
    if (SceneActorChase_MotionFlags(actor, 0x400))
        return 0;
    {
        u32 bits = 4;
        group->flags &= ~bits;
    }
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        SceneActorChase_SetControl(actor, group->homeControl, 0x1A);
        SceneActorChase_SetBehavior(actor, 12);
    } else {
        SceneActorChase_SetControl(actor, actor->entryTarget228, 1);
        SceneActorChase_SetBehavior(actor, 5);
        SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
        (group->destination)->y = SceneActorChase_HeightAt(
            SceneActorChase_CellAt(group->destination), group->destination);
        (group->destination)->pad = 0;
    }
    {
        u8 saved = actor->body.direction;
        SceneActorChase_BeginTurn(obj, saved, randomSamples);
    }
    sub_0807E334(obj);
    actor->field2C2 = 0x20;
    return 0;
reroute: {
    SceneActorChase_CellCenter(cell, &target);
    if (SceneActorChase_TryRoute(obj, group->route, &actor->body.position, &target))
        goto success;
}
    callback = &group->fallback;
invoke:
    Callback_InvokeActor(actor, *callback);
    return 0;
success:
    return 1;
}

static inline void SceneActorChase_SetTypedState(struct SceneActorRegistryResetView *actor,
                                                 u32 state, u32 phase, u32 behavior)
{
    SceneActorChase_SelectState(actor, state, phase, behavior);
    SceneActorChase_ResetStateEntry(actor);
}

#undef SCENE_ACTOR_CHASE_TARGET_CELL

#endif
