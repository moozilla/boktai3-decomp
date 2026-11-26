#include "global.h"
#include "scene/actor/scene_actor_terrain_reaction.h"

// confidence=low: counts down control.field14, then clears control.counter once it has expired.
void sub_08137ADC(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (group->common.control.field14)
        group->common.control.field14--;
    else
        group->common.control.counter = 0;
}
