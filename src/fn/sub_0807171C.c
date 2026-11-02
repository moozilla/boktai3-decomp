#include "scene_actor_terrain.h"
#include "scene_actor_reset.h"
static inline void SceneActorRegistry_ResetRouteState(struct SceneActorRegistryResetView *actor)
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
        position = &actor->body.position;
        ProximityVolume_TranslateBounds(object, position);
    }
    actor->body.currentRoute = route;
    actor->terrainCell = SceneActorTerrain_GetCellIndex(position);
    actor->body.position.y = SceneActorTerrain_GetHeight(actor->terrainCell, &actor->body.position);
    if (!actor->drawableKind)
        ((struct SceneActorRegistrySequenceDrawable *)actor->drawable)->position =
            actor->body.position;
    else
        ((struct SceneActorRegistryTileDrawable *)actor->drawable)->position = actor->body.position;
    {
        u16 *address = &actor->field2EA;
        u32 zero;
        zero = 0;
        *address = zero;
        Memory_Clear(&actor->secondaryMotion, sizeof(actor->secondaryMotion));
        Memory_Clear(&actor->body.motion, 8);
        control = &group->control;
        {
            u32 mask = 0x200000;
            u32 *address;
            u32 selected;
            address = &actor->flags2CC;
            selected = *address & mask;
            flags = address;
            renderFlags = &actor->flags2D0;
            if (selected) {
                u32 clearMask = ~0x1000000;
                u32 value;
                u16 *shortFlags;
                u32 mask0;
                value = *renderFlags;
                value &= clearMask;
                *renderFlags = value;
                shortFlags = &actor->flags2D4;
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
        base = &actor->basePaletteBank;
        offset = &actor->paletteOffset;
        bank = *base;
        bank += *offset;
        if (!actor->drawableKind)
            AnimData_SelectPaletteBank(
                ((struct SceneActorRegistrySequenceDrawable *)actor->drawable)->animation, bank);
        else
            ((struct SceneActorRegistryTileDrawable *)actor->drawable)->palette =
                gPaletteBankData + (bank << 5);
    }
    {
        u32 mask = 0xFDFF7FFF;
        *flags &= mask;
        mask = 0xFFFFBE3F;
        *renderFlags &= mask;
    }
    actor->field1EA = 0;
    actor->field1E8 = 0;
    {
        u32 mode = 2;
        if (!actor->drawableKind)
            ((u8 *)actor->drawable)[7] = mode;
        else
            ((u8 *)actor->drawable)[0x3A] = mode;
    }
}

void SceneActorRegistry_ResetRoute(struct SceneActorRegistryActor *argument)
{
    SceneActorRegistry_ResetRouteState((void *)argument);
}
