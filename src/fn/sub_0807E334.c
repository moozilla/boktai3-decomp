#include "scene_actor_straight_route.h"

void SceneActorTerrain_RequestRetreatRoute(void *obj)
{
    struct SceneActorRegistryResetView *actor = obj;
    struct SceneActorChaseGroup *group = &actor->group;
    struct ContactPosition pos;
    s32 dir, dest, attempts;
    dir = ((((actor->body.direction + 32) >> 6) & 3) + 2) & 3;
    attempts = 0;
    do {
        dest = SceneActorTerrain_LastOpenCell(actor, dir);
        dir = (dir + 1) & 3;
        attempts++;
    } while (dest == actor->terrainCell && attempts < 4);
    SceneActorTerrain_GetCenter(dest, &pos);
    SceneActorTerrain_RequestPath(obj, group->route, &actor->body.position, &pos);
}
