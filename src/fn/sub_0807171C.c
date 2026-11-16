#include "scene_actor_terrain.h"
#include "scene_actor_reset.h"
static inline void SceneActorRegistry_ResetRouteState(struct ChaseActor *actor)
{
    struct SceneActorRegistryControlGroup *group = &actor->group.common;
    struct SceneActorRegistryRoute *route = &actor->group.homeRoute;
    struct ContactPosition *position;
    struct SceneActorRegistryControl *control;
    u32 *flags, *renderFlags;
    route->currentPoint = route->pointId;
    Route_Start(route, route->routeId, route->parameterB, route->pointId);
    {
        void *object = actor->group.route;
        position = &actor->base.body.position;
        ProximityVolume_TranslateBounds(object, position);
    }
    actor->base.body.currentRoute = route;
    actor->base.terrainCell = SceneActorTerrain_GetCellIndex(position);
    actor->base.body.position.y =
        SceneActorTerrain_GetHeight(actor->base.terrainCell, &actor->base.body.position);
    if (!actor->base.drawableKind)
        ((struct SceneActorRegistrySequenceDrawable *)actor->base.drawable)->position =
            actor->base.body.position;
    else
        ((struct SceneActorRegistryTileDrawable *)actor->base.drawable)->position =
            actor->base.body.position;
    {
        u16 *address = &actor->base.field2EA;
        u32 zero;
        zero = 0;
        *address = zero;
        Memory_Clear(&actor->base.secondaryMotion, sizeof(actor->base.secondaryMotion));
        Memory_Clear(&actor->base.body.motion, 8);
        control = &group->control;
        {
            u32 mask = 0x200000;
            u32 *address;
            u32 selected;
            address = &actor->base.flags2CC;
            selected = *address & mask;
            flags = address;
            renderFlags = &actor->base.flags2D0;
            if (selected) {
                u32 clearMask = ~0x1000000;
                u32 value;
                u16 *shortFlags;
                u32 mask0;
                value = *renderFlags;
                value &= clearMask;
                *renderFlags = value;
                shortFlags = &actor->base.flags2D4;
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
    actor->base.field1EA = 0;
    actor->base.field1E8 = 0;
    {
        u32 mode = 2;
        if (!actor->base.drawableKind)
            ((u8 *)actor->base.drawable)[7] = mode;
        else
            ((u8 *)actor->base.drawable)[0x3A] = mode;
    }
}

void SceneActorRegistry_ResetRoute(struct SceneActorRegistryActor *argument)
{
    SceneActorRegistry_ResetRouteState((void *)argument);
}
