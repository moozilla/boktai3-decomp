#ifndef GUARD_SCENE_ACTOR_CHASE_STEP_H
#define GUARD_SCENE_ACTOR_CHASE_STEP_H

/* Stored selectors and arithmetic selectors feed the same actor state fields. */
static inline void SceneActorChase_SelectState(void *object, u8 state, u8 phase, u8 behavior)
{
    struct ChaseActor *actor = object;
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
}

/* The cell index of map coordinates, or 0 off the map. CellAt takes a position in 1/256 cells. */
static inline s32 SceneActorChase_CellIndex(s32 x, s32 z)
{
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return SceneActorChase_MapRowStart(SceneActorChase_CurrentMap(), z) + x;
}

static inline s32 SceneActorChase_CellAt(struct ContactPosition *position)
{
    s32 x = position->x >> 8;
    s32 z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return SceneActorChase_MapRowStart(SceneActorChase_CurrentMap(), z) + x;
}

static inline u32 SceneActorChase_HeightAt(s32 cell, struct ContactPosition *position)
{
    struct SceneActorChaseTerrainCell *data = SceneActorChase_CellProperties(cell);
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

static inline u8 SceneActorChase_TryRoute(struct ChaseActor *actor, void *route,
                                          struct ContactPosition *position,
                                          struct ContactPosition *target)
{
    s32 result;
    if (!Nav_IsPositionInMesh(route, position, target)) {
        SceneActorChase_ClearBits(SceneActorChase_MotionFlagsOf(actor), 0x200);
        return 0;
    } else {
        result = Terrain_ApplyBlocked(route, position, target);
        if (result) {
            if (result != 2)
                Memory_Clear(SceneActorChase_MotionOf(actor), 8);
            return 1;
        } else {
            SceneActorChase_ClearBits(SceneActorChase_MotionFlagsOf(actor), 0x200);
            return 0;
        }
    }
}

static inline void SceneActorChase_SetState(void *object, u32 state, u32 phase, u32 behavior)
{
    struct ChaseActor *actor = object;
    SceneActorChase_SelectState(actor, state, phase, behavior);
    SceneActorChase_ResetStateEntry(actor);
}

static inline void SceneActorChase_SetPhase(void *object, u32 phase, u32 behavior)
{
    struct ChaseActor *actor = object;
    SceneActorChase_SelectState(actor, 0, phase, behavior);
    SceneActorChase_ResetStateEntry(actor);
}

static inline void SceneActorChase_SetBehavior(void *object, u32 behavior)
{
    struct ChaseActor *actor = object;
    SceneActorChase_SetPhase(actor, 0, behavior);
}

static inline u32 SceneActorChase_Height(struct ContactPosition *position)
{
    return SceneActorChase_HeightAt(SceneActorChase_CellAt(position), position);
}

static inline void SceneActorChase_SnapToRoute(struct ContactPosition *position,
                                               struct SceneActorRegistryRoute *route)
{
    position->x = route->point->x;
    position->z = route->point->z;
    position->y = SceneActorChase_Height(position);
}

static inline void SceneActorChase_BeginTurn(void *obj, u32 saved, const u16 *randomSamples)
{
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    if (!SceneActorChase_MotionFlags(actor, 0x400) &&
        !SceneActorChase_TestField((u16 *)&group->common.gate.flags, 4)) {
        group->turn.direction =
            SceneActorChase_RandomDirectionExcept(actor->base.body.direction, randomSamples);
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
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    struct SceneActorChaseTracking *tracking = &actor->base.tracking;
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
        cell = ((struct SceneActorChasePlayer *)gPlayers[player])->cell->index;
    } else {
        struct SceneActorChaseTarget *t = tracking->target;
        cell = SceneActorChase_CellAt(&t->position);
        player = 0;
    }
    if (cell != actor->base.terrainCell &&
        !SceneActorChase_Bits32(&gPlayers[player]->statusFlags,
                                0x20000))
        goto reroute;
    if (SceneActorChase_MotionFlags(actor, 0x400))
        return 0;
    SceneActorChase_GroupClearFlags(group, 4);
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        SceneActorChase_SetBehavior(actor, 12);
    } else {
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        SceneActorChase_SetBehavior(actor, 5);
        SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
        (group->destination)->y = SceneActorChase_HeightAt(
            SceneActorChase_CellAt(group->destination), group->destination);
        (group->destination)->pad = 0;
    }
    {
        u8 saved = actor->base.body.direction;
        SceneActorChase_BeginTurn(obj, saved, randomSamples);
    }
    SceneActorTerrain_RequestRetreatRoute(obj);
    actor->base.field2C2 = 0x20;
    return 0;
reroute: {
    SceneActorChase_CellCenter(cell, &target);
    if (SceneActorChase_TryRoute(obj, group->route, &actor->base.body.position, &target))
        goto success;
}
    callback = &group->fallback;
invoke:
    Callback_InvokeActor(actor, *callback);
    return 0;
success:
    return 1;
}

static inline void SceneActorChase_SetTypedState(struct ChaseActor *actor, u32 state, u32 phase,
                                                 u32 behavior)
{
    SceneActorChase_SelectState(actor, state, phase, behavior);
    SceneActorChase_ResetStateEntry(actor);
}

#endif
