#include "global.h"
#include "scene_actor_registry.h"
#include "scene_actor_position_search.h"
#include "scene_actor_terrain.h"
s32 SceneActorTarget_CanReplaceByDistance(struct ChaseActor *actor,
                                          const struct ContactPosition *position, void *identity)
{
    struct ContactPosition delta;
    s32 distance;
    if (((struct SceneActorTargetIdentityView *)((u8 *)actor + 0x3D4))->identity == identity)
        return 1;
    if (actor->group.common.control.field14)
        return 0;
    SceneActorTerrain_Subtract(&delta, position, &actor->base.body.position);
    distance = delta.x * delta.x + delta.z * delta.z;
    if (!actor->base.tracking.target)
        return 1;
    SceneActorTerrain_Subtract(&delta, &actor->base.tracking.target->position,
                               &actor->base.body.position);
    if (delta.x * delta.x + delta.z * delta.z < distance)
        return 0;
    return 1;
}
