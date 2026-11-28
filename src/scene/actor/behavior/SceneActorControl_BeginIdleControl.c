#include "scene/motion/scene_actor_terrain_reaction.h"
#include "scene/actor/scene_actor_control_phase.h"
#include "global.h"
#include "scene/actor/scene_actor_reaction.h"

void SceneActorControl_BeginIdleControl(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeStateEntry(actor)) {
        SceneActorTerrain_InstallControl(actor, SceneActorControl_OnIdle, 0x10);
    }
    actor->base.counter2AC++;
}
