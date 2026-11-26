#include "scene/actor/scene_actor_control_phase.h"
#include "scene/actor/scene_actor_terrain_reaction.h"
void SceneActorControl_BeginSavedHeadingPhase(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeStateEntry(actor))
        actor->base.body.direction = actor->base.tracking.heading;
    SceneActorTerrain_EnterState(actor, 0, 2, 0);
    actor->base.counter2AC++;
}
