// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_control_phase.h"
// confidence=low: recoil callback selects animation 15 and completes after sixty ticks.
void Ghost_UpdateRecoilAnimation(struct SceneActorRegistryResetView *actor, s32 tick)
{
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 15);
    }
    if (tick <= 29) {
        u32 mask = 0x20;
        u16 *flags = &actor->flags2D6;
        DEBUG_NOP();
        *flags = mask | *flags;
    }
    if (tick > 59) {
        actor->field2B0 = 1;
    }
}
