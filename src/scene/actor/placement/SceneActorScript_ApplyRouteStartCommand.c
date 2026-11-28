#include "scene/actor/scene_actor_reset.h"
#include "scene/motion/scene_actor_route.h"

void SceneActorScript_ApplyRouteStartCommand(struct ChaseActor *actor, const u16 *command)
{
    if (actor->base.health > 0) {
        u16 a = command[1];
        u16 b = command[2];
        u16 c = command[3];
        Route_Start((void *)&actor->group.homeRoute, a, b, c);
    }
}
