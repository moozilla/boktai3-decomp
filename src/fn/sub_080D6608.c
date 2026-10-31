#include "global.h"
#define SCENE_ACTOR_CHASE_DIRECT_SAVED_CONTROL 1
#include "scene_actor_chase_reaction.h"
#include "scene_actor_terrain.h"

struct MummyRouteContext {
    u8 unknown0[0x6D2];
    u16 pursuitDelay;
};
struct MummyRouteTarget {
    struct SceneActorChaseTarget target;
    u8 unknown14[8];
    struct SceneActorChasePlayerCell *cell;
};

u8 SceneActorTerrain_CanTraverseCell(s32, s32, s32, u32);
u8 Mummy_RequestStraightRoute(struct SceneActorRegistryResetView *);
static inline s32 GridWidth(void)
{
    return ((struct SceneActorChaseTerrainGrid *)gTerrainMap->cells)->width;
}
static inline s32 GridHeight(void)
{
    return ((struct SceneActorChaseTerrainGrid *)gTerrainMap->cells)->height;
}

static inline u8 CanReach(s32 cell, s32 goal, s32 y, u32 kind, s32 total)
{
    s32 i, col;
    if (cell < goal) {
        i = cell + 1;
        for (col = Mod(i, GridWidth());
             col < GridWidth() && SceneActorTerrain_CanTraverseCell(cell, i, y, kind); i++, col++) {
            if (i == goal)
                return 1;
        }
        for (i = cell + GridWidth();
             total > i && SceneActorTerrain_CanTraverseCell(cell, i, y, kind); i += GridWidth()) {
            if (i == goal)
                return 1;
        }
    } else {
        i = cell - 1;
        for (col = Mod(i, GridWidth());
             col >= 0 && SceneActorTerrain_CanTraverseCell(cell, i, y, kind); i--, col--) {
            if (i == goal)
                return 1;
        }
        for (i = cell - GridWidth(); i >= 0 && SceneActorTerrain_CanTraverseCell(cell, i, y, kind);
             i -= GridWidth()) {
            if (i == goal)
                return 1;
        }
    }
    return 0;
}

static inline u8 FindPath(struct SceneActorRegistryResetView *actor, s32 goal)
{
    s32 cell = actor->terrainCell;
    s32 y = actor->body.position.y;
    u32 kind = *SceneActorTerrain_GetCell(cell) >> 4;
    s32 total = GridWidth() * GridHeight();
    return CanReach(cell, goal, y, kind, total);
}

static inline u8 Mummy_Arrive(void *obj)
{
    struct ContactPosition target;
    struct SceneActorRegistryResetView *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    struct SceneActorChaseTracking *tracking = &actor->tracking;
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
            Callback_InvokeActor(actor, group->fallback);
            return 0;
        }
        cell = ((struct SceneActorChasePlayer *)gUnknown_02000580[player])->cell->index;
    } else {
        struct SceneActorChaseTarget *t = tracking->target;
        cell = SceneActorChase_CellAt(&t->position);
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
        group->common.gate.flags &= ~bits;
    }
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
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

u8 Mummy_TryDirectPursuit(struct SceneActorRegistryResetView *actor)
{
    struct MummyRouteContext *body = actor->context;
    struct SceneActorChaseGroup *group = &actor->group;
    s32 goal = ((struct MummyRouteTarget *)actor->tracking.target)->cell->index;
    if (group->common.control.counter != gTrackedPlayerId[0])
        return 0;
    if (goal == actor->terrainCell)
        return 1;
    if (FindPath(actor, goal))
        return 1;
    if ((!body->pursuitDelay) && (actor->terrainCell == goal)) {
        Mummy_RequestStraightRoute(actor);
        return 0;
    }
    SceneActorChase_TrackPlayer(actor);
    Mummy_Arrive(actor);
    return 0;
}
