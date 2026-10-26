// MATCHING_CONCESSION: register-pinning
#include "scene_actor_effects.h"
#include "scene_actor_route.h"
struct SceneActorRouteTargetPosition {
    s16 x, y, z, pad;
};
u32 Nav_IsPositionInMesh(void *, const struct ContactPosition *,
                         struct SceneActorRouteTargetPosition *);
u32 Terrain_ApplyBlocked(void *, const struct ContactPosition *,
                         struct SceneActorRouteTargetPosition *);
static inline s32 GetCell(struct SceneActorRouteTargetPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline s32 GetHeight(s32 cell, struct SceneActorRouteTargetPosition *p)
{
    register u32 attr asm("r1") = *SceneActorTerrain_GetCell(cell);
    u32 kind = attr >> 4;
    s32 h = (attr & 15) << 8;
    switch (kind) {
        case 1:
            h -= ((u8 *)p)[4];
            break;
        case 2:
            h -= ((u8 *)p)[0];
            break;
    }
    return h;
}
void SceneActorMovement_ResolveRoutePosition(struct SceneActorRegistryActor *actor)
{
    /* This local vector has halfword alignment; aggregate copies are not used. */
    struct SceneActorRouteTargetPosition target;
    struct SceneActorRegistryActor *owner = actor;
    struct SceneActorRegistryActor *base = owner;
    u8 *control = (u8 *)owner + 0x3D4;
    void *context;
    {
        struct SceneActorRouteTargetPosition *p = &target;
        struct SceneActorRouteCursor *route = (void *)((u8 *)owner + 0x448);
        s32 cell;
        {
            struct SceneActorRoutePoint *point = route->point;
            p->x = point->x;
            p->z = point->z;
        }
        cell = GetCell(p);
        p->y = GetHeight(cell, p);
    }
    target.y = base->position.y;
    context = *(void **)(control + 0x1FC);
    if (!Nav_IsPositionInMesh(context, &base->position, &target))
        goto failed;
    {
        u32 result = Terrain_ApplyBlocked(context, &base->position, &target);
        if (!result)
            goto failed;
        if (result != 2)
            Memory_Clear(&owner->velocity, 8);
    }
    goto done;
failed: {
    u16 *flags = &owner->flags2D4;
    u32 mask = ~0x200;
    mask &= *flags;
    *flags = mask;
}
done:;
}
