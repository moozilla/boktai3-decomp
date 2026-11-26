#include "global.h"
#include "scene/actor/scene_actor_terrain_reaction.h"

void SceneActorControl_AdvancePhase(struct ChaseActor *actor)
{
    if (actor->base.field2B1)
        actor->base.field2B1 = 0;
    actor->base.counter2AC++;
}
