#include "global.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_chase_shared.h"
// confidence=low: recoil callback selects animation 15 and completes after sixty ticks.
void Ghost_UpdateRecoilAnimation(struct SceneActorRegistryResetView *actor, s32 tick)
{
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 15);
    }
    if (tick <= 29) {
        SceneActorChase_SetReactionFlags(actor, 32);
    }
    if (tick > 59) {
        actor->field2B0 = 1;
    }
}
