#include "global.h"
#include "scene_actor_registry.h"
#include "scene_actor_position_search.h"
static inline void Subtract(struct ContactPosition *out, const struct ContactPosition *a,
                            const struct ContactPosition *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}
s32 SceneActorTarget_CanReplaceByDistance(struct SceneActorRegistryActor *actor,
                                          const struct ContactPosition *position, void *identity)
{
    struct ContactPosition delta;
    s32 distance;
    if (((struct SceneActorTargetIdentityView *)((u8 *)actor + 0x3D4))->identity == identity)
        return 1;
    if (actor->distanceTargetLock)
        return 0;
    Subtract(&delta, position, &actor->position);
    distance = delta.x * delta.x + delta.z * delta.z;
    if (!actor->distanceTarget)
        return 1;
    Subtract(&delta, &actor->distanceTarget->position, &actor->position);
    if (delta.x * delta.x + delta.z * delta.z < distance)
        return 0;
    return 1;
}
