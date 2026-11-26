#include "engine/shared/global.h"
#include "engine/shared/scene_actor_chase_direct_arrival.h"
#include "engine/shared/scene_actor_straight_route.h"
#include "engine/shared/scene_actor_cardinal_path.h"

static inline u8 RecoveryRoute_CanReach(s32 cell, s32 goal, s32 y)
{
    u32 kind = *SceneActorTerrain_GetCell(cell) >> 4;
    s32 total = SceneActorPath_Width() * SceneActorPath_Height();
    return SceneActorPath_CanReach(cell, goal, y, kind, total);
}
static inline u16 RecoveryRoute_AdvanceRandomCursor(u32 *cursor, u16 wrap)
{
    u32 index = (*cursor + 1) & wrap;
    *cursor = index;
    return index;
}
static inline u32 RecoveryRoute_NewHeading(u32 current, const u16 *randomSamples)
{
    s32 tries;
    struct SceneActorChaseRandomTable table = {randomSamples};
    for (tries = 0; tries < 4; tries++) {
        u8 direction;
        const struct SceneActorChaseRandomTable *view = &table;
        const u16 *samples = view->samples;
        u32 *cursor = &gRandomIndex;
        u16 index = RecoveryRoute_AdvanceRandomCursor(cursor, 1023);
        u16 sample;
        sample = samples[index];
        sample &= 3;
        direction = sample << 6;
        if (direction != current)
            return direction;
    }
    return current;
}

static inline void RecoveryRoute_BeginTurn(void *obj, u32 saved, const u16 *randomSamples)
{
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    if (!SceneActorChase_MotionFlags(actor, 0x400) &&
        !SceneActorChase_TestField((u16 *)&group->common.gate.flags, 4)) {
        group->turn.direction = RecoveryRoute_NewHeading(actor->base.body.direction, randomSamples);
        SceneActorChase_SaveControl(&group->saved, actor);
        group->turn.savedDirection = saved;
        SceneActorChase_SetControl(actor, group->turnControl, 0x1B);
        SceneActorChase_SetState(actor, 3, 1, 0);
        group->turn.turnsRemaining = 2;
    }
}

static inline void RecoveryRoute_SetGate(struct SceneActorChaseFlags16 *flags, u32 bits)
{
    u32 mask = bits;
    mask |= flags->value;
    flags->value = mask;
}
static inline s32 RecoveryRoute_ActorCell(struct ChaseActor *actor)
{
    return actor->base.terrainCell;
}

static inline s32 RecoveryRoute_SelectDirection(struct ChaseActor *actor)
{
    s32 cell;
    switch (((actor->base.body.direction + 32) >> 6) & 3) {
        case 0:
        case 2:
            cell = RecoveryRoute_ActorCell(actor) - SceneActorPath_Width();
            if (RecoveryRoute_CanReach(RecoveryRoute_ActorCell(actor), cell,
                                       actor->base.body.position.y)) {
                return 3;
            }
            cell = RecoveryRoute_ActorCell(actor) + SceneActorPath_Width();
            if (RecoveryRoute_CanReach(RecoveryRoute_ActorCell(actor), cell,
                                       actor->base.body.position.y)) {
                return 1;
            }
            break;
        case 1:
        case 3:
            cell = actor->base.terrainCell + 1;
            if (RecoveryRoute_CanReach(actor->base.terrainCell, cell,
                                       actor->base.body.position.y)) {
                return 0;
            }
            cell = actor->base.terrainCell - 1;
            if (RecoveryRoute_CanReach(actor->base.terrainCell, cell,
                                       actor->base.body.position.y)) {
                return 2;
            }
            break;
        default:
            break;
    }

    return -1;
}

u8 SceneActorRoute_TryRecoveryDetour(void *object)
{
    struct ChaseActor *actor = object;
    struct SceneActorChaseGroup *group = &actor->group;
    struct SceneActorChaseTracking *tracking = &actor->base.tracking;
    s32 result;
    s16 *actorHeight;
    struct SceneActorChaseTarget *subject;
    s32 direction;
    long routeValue;
    struct ContactPosition *point;
    struct ContactPosition center;
    const u16 *randomSamples;
    if (SceneActorChase_TestField((u16 *)&actor->group.common.gate.flags, 2))
        return 0;
    if (!SceneActorChase_TestField((u16 *)&group->common.gate.flags, 1))
        return 0;
    actorHeight = &actor->base.body.position.y;
    subject = tracking->target;
    if (*actorHeight != subject->position.y) {
        SceneActorChase_ArriveWithDirectFallback(object);
        RecoveryRoute_SetGate((void *)&group->common.gate.flags, 2);
        return 1;
    }
    randomSamples = gRandomTable;
    direction = RecoveryRoute_SelectDirection(actor);
    if (direction < 0)
        return 0;
    routeValue = SceneActorTerrain_LastOpenCell(actor, direction);
    if (!routeValue || routeValue == actor->base.terrainCell)
        return 0;
    RecoveryRoute_BeginTurn(object, direction, randomSamples);
    point = &center;
    {
        s32 x = Mod(routeValue, SceneActorTerrain_Width());
        result = Div(routeValue, SceneActorTerrain_Width());
        center.x = ((u32)x << 8) + 128;
        point->y = (*SceneActorTerrain_GetCell(routeValue) & 15) << 8;
        point->z = ((u32)result << 8) + 128;
    }
    center.y = actor->base.body.position.y;
    result = SceneActorChase_TryRoute(object, group->route, &actor->base.body.position, &center);
    if (result)
        goto routed;
    SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0);
    SceneActorChase_SetState(actor, 3, 0, 0);
    return 0;
routed: {
    u32 mask = 2;
    routeValue = (long)&group->common.gate.flags;
    mask |= *(u16 *)routeValue;
    *(u16 *)routeValue = mask;
    return 1;
}
}
