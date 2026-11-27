#include "npc/manager/scene_actor_animation.h"
#include "scene/actor/scene_actor_status.h"
#include "scene/motion/scene_actor_terrain.h"
#include "scene/actor/scene_actor_reset.h"
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
        u16 *address = &actor->base.heightOffset;
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
    actor->base.paletteTimer = 0;
    actor->base.flashTimer = 0;
    {
        u32 mode = 2;
        if (!actor->base.drawableKind)
            ((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sprite.priority = mode;
        else
            ((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile.attribute1A = mode;
    }
}

void SceneActorRegistry_ResetRoute(struct ChaseActor *argument)
{
    SceneActorRegistry_ResetRouteState((void *)argument);
}
