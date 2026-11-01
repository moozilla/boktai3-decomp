#include "global.h"
#define SCENE_ACTOR_CHASE_DIRECT_SAVED_CONTROL 1
#include "scene_actor_chase_direct_arrival.h"
#include "scene_actor_cardinal_path.h"

struct MummyRouteContext {
    u8 unknown0[0x6D2];
    u16 pursuitDelay;
};
struct MummyRouteTarget {
    struct SceneActorChaseTarget target;
    u8 unknown14[8];
    struct SceneActorChasePlayerCell *cell;
};

u8 Mummy_RequestStraightRoute(struct SceneActorRegistryResetView *);
static inline u8 FindPath(struct SceneActorRegistryResetView *actor, s32 goal)
{
    s32 cell = actor->terrainCell;
    s32 y = actor->body.position.y;
    u32 kind = *SceneActorTerrain_GetCell(cell) >> 4;
    s32 total = SceneActorPath_Width() * SceneActorPath_Height();
    return SceneActorPath_CanReach(cell, goal, y, kind, total);
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
    SceneActorChase_ArriveWithDirectFallback(actor);
    return 0;
}
