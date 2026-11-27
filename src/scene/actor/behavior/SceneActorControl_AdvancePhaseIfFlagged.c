#include "global.h"
#include "scene/motion/scene_actor_terrain_reaction.h"
#include "scene/actor/scene_actor_behavior.h"

void SceneActorControl_AdvancePhaseIfFlagged(struct ChaseActor *actor)
{
    if (actor->base.field2B1)
        actor->base.field2B1 = 0;
    if (SceneActorBehavior_MotionBits(2, actor))
        actor->base.counter2AC++;
}
