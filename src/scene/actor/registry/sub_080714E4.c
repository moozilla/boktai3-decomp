#include "npc/manager/scene_actor_animation.h"
#include "scene/actor/scene_actor_status.h"
#include "scene/motion/scene_actor_terrain.h"
#include "scene/actor/scene_actor_reset.h"
static inline void SceneActorRegistry_ResetRouteState(struct ChaseActor *actor)
{
    struct SceneActorRegistryControlGroup *group = &actor->group.common;
    struct SceneActorRegistryRoute *route = &actor->group.homeRoute;
    struct ContactCollider *collider = &actor->base.colliderA;
    struct ContactPosition *cachedPosition;
    struct ContactPosition *position;
    struct SceneActorRegistryControl *control;
    u32 *flags, *renderFlags;
    s32 x, z;
    s32 cell;
    u32 category;
    s32 computedCell;
    route->currentPoint = route->pointId;
    category = actor->base.category;
    if (category - 18 > 1 && category != 9 && category != 22) {
        struct ContactPosition *earlyPosition = &actor->base.body.position;
        Path_GetPointPositionById((u16 *)earlyPosition, route->routeId, route->pointId);
        Route_Start(route, route->routeId, route->parameterB, route->pointId);
        ProximityVolume_TranslateBounds(actor->group.route, earlyPosition);
        actor->base.body.currentRoute = route;
    }
    {
        struct ContactPosition *lowPosition = &actor->base.body.position;
        x = lowPosition->x >> 8;
        z = lowPosition->z >> 8;
        cachedPosition = lowPosition;
    }
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        computedCell = 0;
    else {
        u32 row = gTerrainMap->rows[z];
        computedCell = row + x;
    }
    {
        u16 *address;
        address = &actor->base.terrainCell;
        *address = computedCell;
        cell = *address;
    }
    position = cachedPosition;
    actor->base.body.position.y = SceneActorTerrain_GetHeight(cell, position);
    if (!actor->base.drawableKind)
        ((struct SceneActorRegistrySequenceDrawable *)actor->base.drawable)->position =
            actor->base.body.position;
    else
        ((struct SceneActorRegistryTileDrawable *)actor->base.drawable)->position =
            actor->base.body.position;
    {
        u16 *address;
        u32 zero;
        {
            u32 direction = actor->base.saved.direction;
            u8 *destination = &actor->base.body.direction;
            zero = 0;
            *destination = direction;
        }
        address = &actor->base.heightOffset;
        *address = zero;
        {
            Memory_Clear(&actor->base.secondaryMotion, 8);
        }
        Memory_Clear(&actor->base.body.motion, 8);
        control = &group->control;
        {
            u32 mask = 0x200000;
            u32 *address;
            u32 selected;
            address = &actor->base.behaviorFlags;
            selected = *address & mask;
            flags = address;
            renderFlags = &actor->base.statusFlags;
            if (selected)
                ActorStatus_ClearInteraction(actor, control);
        }
    }
    {
        const u16 *base;
        const u16 *offset;
        u32 bank;
        base = &actor->base.basePaletteBank;
        offset = &actor->base.paletteOffset;
        bank = *base;
        bank += *offset;
        if (!actor->base.drawableKind)
            AnimData_SelectPaletteBank(
                ((struct SceneActorRegistrySequenceDrawable *)actor->base.drawable)->animation,
                bank);
        else
            ((struct SceneActorRegistryTileDrawable *)actor->base.drawable)->palette =
                gPaletteBankData + (bank << 5);
    }
    {
        u32 mask = 0xFDFF7FFF;
        *flags &= mask;
        mask = 0xFFFFBE3F;
        *renderFlags &= mask;
    }
    Collider_SetPosition(&actor->base.colliderB, cachedPosition, 0);
    Collider_SetPosition(collider, cachedPosition, 0);
    {
        struct ContactCollider *lowCollider = collider;
        u32 savedFlags = lowCollider->flags;
        u32 value;
        value = 0xFFFB;
        value &= savedFlags;
        collider->flags = value;
        actor->base.paletteTimer = 0;
        actor->base.flashTimer = 0;
    }
    {
        u32 mode = 2;
        if (!actor->base.drawableKind)
            ((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sprite.priority = mode;
        else
            ((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile.attribute1A = mode;
    }
    collider->responseValue = 0;
}

void SceneActorRegistry_ResetRouteAndColliders(struct ChaseActor *argument)
{
    SceneActorRegistry_ResetRouteState((void *)argument);
}
