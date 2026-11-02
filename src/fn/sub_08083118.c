#include "scene_actor_control_phase.h"
#include "scene_actor_terrain_reaction.h"
void SceneActorControl_BeginSavedHeadingPhase(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorControl_TakeStateEntry(actor))
        actor->body.direction = actor->tracking.heading;
    SceneActorTerrain_EnterState(actor, 0, 2, 0);
    actor->counter2AC++;
}
