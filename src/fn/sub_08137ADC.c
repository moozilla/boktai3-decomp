#include "global.h"
#include "scene_actor_terrain_reaction.h"

// confidence=low: counts down the control's field14 timer and clears its counter once it has expired.
void sub_08137ADC(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (group->common.control.field14)
        group->common.control.field14--;
    else
        group->common.control.counter = 0;
}
