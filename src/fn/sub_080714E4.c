#include "scene_actor_reset.h"
static inline void SceneActorRegistry_ResetRouteState(struct ChaseActor *actor)
{
    struct SceneActorRegistryControlGroup *group = &actor->group.common;
    struct SceneActorRegistryRoute *route = &actor->group.homeRoute;
    struct ContactCollider *collider = &actor->base.colliderA;
    struct ContactPosition *cachedPosition;
    struct ContactPosition *position;
    struct SceneActorRegistryControl *control;
    u32 *flags, *renderFlags;
    s32 x, z, height, slope;
    s32 cell;
    u32 category;
    s32 computedCell;
    u8 *data;
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
        u32 offset = 0x2F4;
        u16 *address;
        address = (u16 *)((u8 *)actor + offset);
        *address = computedCell;
        cell = *address;
    }
    position = cachedPosition;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        data = gTerrainMap->cells + 12 + cell * 4;
    }
    slope = *data >> 4;
    height = (*data & 15) << 8;
    switch (slope) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    actor->base.body.position.y = height;
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
            u32 offset = 0x2EC;
            Memory_Clear((u8 *)actor + offset, 8);
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
            if (selected) {
                u32 clearMask = ~0x1000000;
                u32 value;
                u16 *shortFlags;
                u32 offset;
                u32 mask0;
                value = *renderFlags;
                value &= clearMask;
                *renderFlags = value;
                offset = 0x2D4;
                shortFlags = &actor->base.motionFlags;
                mask0 = 0xFFFFFDFF;
                mask0 &= *shortFlags;
                *shortFlags = mask0;
                control->mode = 0;
                control->counter = zero;
                control->timer = zero;
            }
        }
    }
    {
        u32 fieldOffset = 0x1E0;
        const u16 *base;
        const u16 *offset;
        u32 bank;
        base = (const u16 *)((u8 *)actor + fieldOffset);
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
    {
        u32 offset = 0x188;
        Collider_SetPosition(&actor->base.colliderB, cachedPosition, 0);
    }
    Collider_SetPosition(collider, cachedPosition, 0);
    {
        u32 offset;
        u16 *address;
        struct ContactCollider *lowCollider = collider;
        u32 savedFlags = lowCollider->flags;
        u32 value;
        value = 0xFFFB;
        value &= savedFlags;
        collider->flags = value;
        offset = 0x1EA;
        address = &actor->base.paletteTimer;
        *address = 0;
        offset -= 2;
        address = &actor->base.flashTimer;
        *address = 0;
    }
    {
        u32 mode = 2;
        if (!actor->base.drawableKind)
            ((u8 *)actor->base.drawable)[7] = mode;
        else
            ((u8 *)actor->base.drawable)[0x3A] = mode;
    }
    collider->responseValue = 0;
}

void SceneActorRegistry_ResetRouteAndColliders(struct ChaseActor *argument)
{
    SceneActorRegistry_ResetRouteState((void *)argument);
}
