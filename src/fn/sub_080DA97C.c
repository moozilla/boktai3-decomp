#define SCENE_ACTOR_CHASE_TYPED_GROUP         1
#define SCENE_ACTOR_CHASE_WIDE_MAP_WIDTH      1
#define SCENE_ACTOR_CHASE_DIRECT_ROUTE_FIELDS 1
#include "scene_actor_chase_reaction.h"

void SceneActorMovement_ResolveRoutePosition(void *);

static inline void Mummy_ChooseRandomRoute(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseRouteSelection *route = actor->currentRoute;
    u8 choice;
    {
        u32 index = (gRandomIndex + 1) & 1023;
        s32 value;
        gRandomIndex = index;
        index *= 2;
        value = *(const u16 *)(RANDOM_TABLE_ADDR + index);
        value >>= 3;
        choice = Mod(value, route->points->count);
    }
    if (choice == route->index)
        choice = Mod(choice + 1, route->points->count);
    route->index = choice;
    route->phase = 0;
    {
        u8 *base = gTerrainMap->routes;
        base += route->points->offset;
        route->current = base + route->index * 8;
    }
}

/* Choose a route, react to contact, or resume pursuit in motion-flag priority order. */
u8 Mummy_SelectMovementResponse(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = (struct SceneActorChaseGroup *)&actor->group;
    if (!actor->reactionSoundVariant)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0xFD);
    {
        u32 *word = &actor->flags2D0;
        s32 mask = ~4;
        *word &= mask;
    }
    if (SceneActorChase_QueryMotionFlags(actor, 0x10)) {
        Mummy_ChooseRandomRoute(actor);
        SceneActorMovement_ResolveRoutePosition(actor);
        SceneActorChase_SetBehavior(actor, 8);
        SceneActorChase_SetControl(actor, group->routeChoiceControl, 0x13);
        SceneActorChase_ClearMotionFlags(actor, 0x20);
        return 1;
    }
    if (SceneActorChase_QueryMotionFlags(actor, 0x2000)) {
        actor->direction = actor->field2B6 + 0x80;
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)actor, 0);
        SceneActorChase_ClearMotionFlags(actor, 0x2020);
        SceneActorChase_SetActorFlags(actor, 0x10);
        group->destination->pad = 0;
        SceneActorChase_TrackPlayer(actor);
        if (!SceneActorChase_Arrive(actor) &&
            !SceneActorChase_TryRoute(
                actor, group->route, &actor->position,
                &((struct SceneActorChaseTracking *)&actor->fieldDC)->target->position)) {
            Callback_InvokeActor(actor, group->fallback);
            return 0;
        }
        SceneActorChase_SetControl(actor, actor->entryTarget228, 1);
        SceneActorChase_SetBehavior(actor, 7);
        actor->field2C2 = Div(64, 3);
        return 1;
    }
    if (SceneActorChase_QueryMotionFlags(actor, 0x8002)) {
        SceneActorChase_ClearMotionFlags(actor, 0x20);
        return 0;
    }
    if (actor->field2DC & 0xC0000) {
        SceneActorChase_SetControl(actor, actor->reactionTarget230, 0x12);
        SceneActorChase_SetState(actor, 3, 0, 0);
        SceneActorChase_ClearWordFlags(&actor->flags2D0, 0x1000000);
        return 1;
    }
    SceneActorChase_ClearMotionFlags(actor, 0x20);
    if (SceneActorChase_QueryMotionFlags(actor, 0x100))
        return 0;
    if (actor->field2B8)
        return 0;
    if (SceneActorChase_QueryMotionFlags(actor, 0x80)) {
        SceneActorChase_BeginContactAnimation(actor);
        if (!actor->fieldDC)
            return 0;
        SceneActorChase_SetActorFlags(actor, 0x80);
        SceneActorChase_GroupSetFlags(group, 4);
        SceneActorChase_TrackPlayer(actor);
        if (!SceneActorChase_Arrive(actor)) {
            if (!SceneActorChase_TryRoute(
                    actor, group->route, &actor->position,
                    &((struct SceneActorChaseTracking *)&actor->fieldDC)->target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return 0;
            }
        }
        SceneActorChase_SetControl(actor, group->homeControl, 0x1A);
        SceneActorChase_SetBehavior(actor, 12);
        return 1;
    } else {
        SceneActorChase_BeginContactAnimation(actor);
        if (!actor->fieldDC)
            return 0;
        SceneActorChase_SetActorFlags(actor, 0x10);
        group->destination->pad = 0;
        SceneActorChase_TrackPlayer(actor);
        if (!SceneActorChase_Arrive(actor) &&
            !SceneActorChase_TryRoute(
                actor, group->route, &actor->position,
                &((struct SceneActorChaseTracking *)&actor->fieldDC)->target->position)) {
            Callback_InvokeActor(actor, group->fallback);
            return 0;
        }
        SceneActorChase_SetControl(actor, actor->entryTarget228, 1);
        SceneActorChase_SetBehavior(actor, 7);
        actor->field2C2 = Div(64, 3);
        return 1;
    }
}
