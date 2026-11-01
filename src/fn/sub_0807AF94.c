#include "global.h"
#ifdef NONMATCHING
// fuzzy: 98.9%
#include "scene_actor_chase_direct_arrival.h"
#include "scene_actor_straight_route.h"
#include "scene_actor_cardinal_path.h"

static inline u8 RecoveryRoute_CanReach(s32 cell, s32 goal, s32 y)
{
    u32 kind = *SceneActorTerrain_GetCell(cell) >> 4;
    s32 total = SceneActorPath_Width() * SceneActorPath_Height();
    return SceneActorPath_CanReach(cell, goal, y, kind, total);
}
static inline u32 RecoveryRoute_SampleDirection(void)
{
    struct SceneActorChaseRandomTable table = {gRandomTable};
    return SceneActorChase_NextSample(&table, &gRandomIndex, 1023, 3);
}
static inline u32 RecoveryRoute_NewHeading(u32 current)
{
    s32 tries;
    for (tries = 0; tries < 4; tries++) {
        u32 choice = (RecoveryRoute_SampleDirection() & 3) << 6;
        if (choice != current)
            return choice;
    }
    return current;
}
static inline void RecoveryRoute_BeginTurn(void *obj, u32 saved)
{
    struct SceneActorRegistryResetView *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    if (!SceneActorChase_MotionFlags(actor, 0x400) &&
        !SceneActorChase_TestField((u16 *)&group->common.gate.flags, 4)) {
        group->turn.direction = RecoveryRoute_NewHeading(actor->body.direction);
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
static inline s32 RecoveryRoute_ActorCell(struct SceneActorRegistryResetView *actor)
{
    return actor->terrainCell;
}

static inline s32 RecoveryRoute_SelectDirection(struct SceneActorRegistryResetView *actor)
{
    s32 cell;
    switch (((actor->body.direction + 32) >> 6) & 3) {
        case 0:
        case 2:
            cell = RecoveryRoute_ActorCell(actor) - SceneActorPath_Width();
            if (RecoveryRoute_CanReach(RecoveryRoute_ActorCell(actor), cell,
                                       actor->body.position.y)) {
                return 3;
            }
            cell = RecoveryRoute_ActorCell(actor) + SceneActorPath_Width();
            if (RecoveryRoute_CanReach(RecoveryRoute_ActorCell(actor), cell,
                                       actor->body.position.y)) {
                return 1;
            }
            break;
        case 1:
        case 3:
            cell = actor->terrainCell + 1;
            if (RecoveryRoute_CanReach(actor->terrainCell, cell, actor->body.position.y)) {
                return 0;
            }
            cell = actor->terrainCell - 1;
            if (RecoveryRoute_CanReach(actor->terrainCell, cell, actor->body.position.y)) {
                return 2;
            }
            break;
        default:
            break;
    }

    return -1;
}

u8 sub_0807AF94(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = &actor->group;
    struct SceneActorChaseTracking *tracking = &actor->tracking;
    s16 *actorHeight;
    struct SceneActorChaseTarget *subject;
    s32 direction, target;
    struct ContactPosition *point;
    struct ContactPosition center;
    if (SceneActorChase_TestField((u16 *)&actor->group.common.gate.flags, 2))
        return 0;
    if (!SceneActorChase_TestField((u16 *)&group->common.gate.flags, 1))
        return 0;
    actorHeight = &actor->body.position.y;
    subject = tracking->target;
    if (*actorHeight != subject->position.y) {
        SceneActorChase_ArriveWithDirectFallback(object);
        RecoveryRoute_SetGate((void *)&group->common.gate.flags, 2);
        return 1;
    }
    direction = RecoveryRoute_SelectDirection(actor);
    if (direction < 0)
        return 0;
    target = SceneActorTerrain_LastOpenCell(actor, direction);
    if (!target || target == actor->terrainCell)
        return 0;
    RecoveryRoute_BeginTurn(object, direction);
    point = &center;
    {
        s32 x = Mod(target, SceneActorTerrain_Width());
        s32 z = Div(target, SceneActorTerrain_Width());
        center.x = ((u32)x << 8) + 128;
        center.y = (*SceneActorTerrain_GetCell(target) & 15) << 8;
        center.z = ((u32)z << 8) + 128;
    }
    point->y = actor->body.position.y;
    if (SceneActorChase_TryRoute(object, group->route, &actor->body.position, &center))
        goto routed;
    SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0);
    SceneActorChase_SetState(actor, 3, 0, 0);
    return 0;
routed:
    RecoveryRoute_SetGate((void *)&group->common.gate.flags, 2);
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0807AF94);
#endif
