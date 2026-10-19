#include "global.h"
#include "scene_actor_reset.h"

#define SceneActorReaction_Tick sub_080EFF6C

void SceneActorReaction_Tick(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1 != 0)
        actor->field2B1 = 0;
    if ((s32)actor->counter2AC > 30) {
        actor->state2A8 = 0;
        actor->phase2A9 = 0;
        actor->behavior2AA = 0;
        actor->counter2AC = 0;
        actor->field2B1 = 1;
    }
    actor->counter2AC++;
}
