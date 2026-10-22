#include "global.h"
#include "scene_actor_chase.h"
void sub_080E8158(void *);
// confidence=low: action callback resets response and selects behavior two on entry.
void Mimic_BeginActionResponse(struct SceneActorRegistryResetView *actor)
{
    if (Chase_TakeArrived(actor)) {
        sub_080E8158(actor);
        {
            u32 behavior = 2;
            actor->state2A8 = 0;
            actor->phase2A9 = 0;
            actor->behavior2AA = behavior;
            actor->counter2AC = 0;
            actor->field2B1 = 1;
        }
    } else
        actor->counter2AC++;
}
