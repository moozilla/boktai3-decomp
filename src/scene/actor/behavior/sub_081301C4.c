#include "scene/collision/scene_actor_delayed_response.h"
#include "scene/motion/scene_actor_terrain_reaction.h"
#include "scene/actor/scene_actor_behavior.h"
#include "global.h"
#include "scene/motion/scene_actor_chase.h"
#include "scene/motion/scene_actor_straight_route.h"
#include "scene/actor/scene_actor_attachment.h"

// confidence=low: timed movement recovery restores a route and saves a delayed control.
u8 sub_0812F3CC(struct ChaseActor *);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void SceneActorReaction_UpdateRouteRecovery(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    u8 result;
    u32 *savedFlags;
    if (actor->base.field2B1)
        actor->base.field2B1 = 0;
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    result = sub_0812F3CC(actor);
    if (result)
        return;
    if ((s32)actor->base.counter2AC > 60) {
        struct ContactPosition target;
        {
            u32 *flags = &actor->base.behaviorFlags;
            u32 mask = ~0x100;
            *flags &= mask;
        }
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->base.category);
        SceneActorTerrain_EnterState(actor, 3, 1, 0);
        SceneActorTerrain_InstallControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
        SceneActorBehavior_SetWordFlags(&actor->base.behaviorFlags, 0x10);
        {
            struct ContactPosition *p = &target;
            struct SceneActorChaseRoutePoint *route = group->homeRoute.point;
            p->x = route->x;
            p->z = route->z;
            {
                s32 x = p->x >> 8;
                s32 z = p->z >> 8;

                savedFlags = &actor->base.behaviorFlags;
                p->y = SceneActorTerrain_GetHeight(Chase_CellIndex(x, z), p);
            }
        }
        target.y = actor->base.body.position.y;
        SceneActorTerrain_RequestPath(actor, group->route, &actor->base.body.position, &target);
        group->destination->pad = 1;
        if (!(u8)SceneActorAttachment_TestPositionWindow(actor, &target, 32, 32)) {
            {
                u32 *flags = savedFlags;
                u32 mask = ~0xc0;
                *flags &= mask;
            }
            SceneActorDelay_Prepare((void *)&group->saved, actor->base.onAlternateEntry, 7, 30);
        } else {
            {
                u32 *flags = savedFlags;
                u32 mask = ~0xd0;
                *flags &= mask;
            }
            SceneActorDelay_Prepare((void *)&group->saved, actor->base.onStanding, 0, 30);
        }
        actor->base.field2C2 = 64;
    }
    actor->base.counter2AC++;
}