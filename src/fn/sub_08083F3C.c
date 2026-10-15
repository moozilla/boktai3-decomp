// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_helpers.h"
#define SceneActorControl_ClearCollisionLatch sub_08083F3C
void SceneActorControl_ClearCollisionLatch(struct SceneActorRegistryResetView *actor)
{
    u16 *flags = &actor->flags2D4;
    register u32 mask asm("r1") = ~0x100;
    u8 *status;
    mask &= *flags;
    *flags = mask;
    status = (u8 *)actor + 0xDC;
    mask = ~8;
    mask &= status[0xF];
    status[0xF] = mask;
}
