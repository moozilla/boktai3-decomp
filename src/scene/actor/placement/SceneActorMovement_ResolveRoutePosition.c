#include "scene/actor/scene_actor_effects.h"
#include "scene/motion/scene_actor_route.h"
#include "scene/terrain/terrain_sampling.h"
u32 Nav_IsPositionInMesh(void *, const struct ContactPosition *,
                         struct ContactPosition *);
u32 Terrain_ApplyBlocked(void *, const struct ContactPosition *,
                         struct ContactPosition *);
void SceneActorMovement_ResolveRoutePosition(struct ChaseActor *actor)
{
    struct ContactPosition target;
    struct ChaseActor *owner = actor;
    struct ChaseActor *base = owner;
    struct SceneActorChaseGroup *group = &owner->group;
    void *context;
    {
        struct ContactPosition *p = &target;
        struct SceneActorRegistryRoute *route = &owner->group.homeRoute;
        s32 cell;
        {
            struct SceneActorChaseRoutePoint *point = route->point;
            p->x = point->x;
            p->z = point->z;
        }
        cell = Terrain_GetCellIndex(p);
        p->y = SceneActorTerrain_GetHeight(cell, p);
    }
    target.y = base->base.body.position.y;
    context = group->route;
    if (!Nav_IsPositionInMesh(context, &base->base.body.position, &target))
        goto failed;
    {
        u32 result = Terrain_ApplyBlocked(context, &base->base.body.position, &target);
        if (!result)
            goto failed;
        if (result != 2)
            Memory_Clear(&owner->base.body.motion, 8);
    }
    goto done;
failed: {
    u16 *flags = &owner->base.motionFlags;
    u32 mask = ~0x200;
    mask &= *flags;
    *flags = mask;
}
done:;
}
