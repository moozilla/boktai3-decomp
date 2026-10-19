#include "global.h"
#include "scene_actor_reset.h"

#define SceneActorReaction_AdvanceTimer sub_080F0108

void SceneActorReaction_AdvanceTimer(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;

    if (actor->field2B1 != 0)
        actor->field2B1 = 0;
    if (*(u16 *)(group + 0x42) <= (s32)actor->counter2AC) {
        actor->state2A8 = 0;
        actor->phase2A9 = 0;
        actor->behavior2AA = 0;
        actor->counter2AC = 0;
        actor->field2B1 = 1;
    } else {
        actor->counter2AC = actor->counter2AC + 1;
    }
}
