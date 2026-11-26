#include "scene/actor/scene_actor_effects.h"
#include "scene/motion/scene_actor_route.h"
u32 Nav_IsPositionInMesh(void *, const struct ContactPosition *,
                         struct ContactPosition *);
u32 Terrain_ApplyBlocked(void *, const struct ContactPosition *,
                         struct ContactPosition *);
static inline s32 GetCell(struct ContactPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
void SceneActorMovement_ResolveRoutePosition(struct ChaseActor *actor)
{
    struct ContactPosition target;
    struct ChaseActor *owner = actor;
    struct ChaseActor *base = owner;
    u8 *control = (u8 *)owner + 0x3D4;
    void *context;
    {
        struct ContactPosition *p = &target;
        struct SceneActorRouteCursor *route = (void *)((u8 *)owner + 0x448);
        s32 cell;
        {
            struct SceneActorRoutePoint *point = route->point;
            p->x = point->x;
            p->z = point->z;
        }
        cell = GetCell(p);
        p->y = SceneActorTerrain_GetHeight(cell, p);
    }
    target.y = base->base.body.position.y;
    context = *(void **)(control + 0x1FC);
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
