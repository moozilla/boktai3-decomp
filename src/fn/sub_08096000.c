// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_control_phase.h"
// confidence=low: recoil callback selects animation 15 and completes after sixty ticks.
void Ghost_UpdateRecoilAnimation(struct SceneActorRegistryResetView *actor, s32 tick)
{
    if (SceneActorControl_TakeFlag(&actor->field2B2))
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 15);
    if (tick <= 29) {
        register u32 mask asm("r0") = 0x20;
        register u16 *flags asm("r2") = &actor->flags2D6;
        *flags = mask | *flags;
    }
    if (tick > 59)
        actor->field2B0 = 1;
}
