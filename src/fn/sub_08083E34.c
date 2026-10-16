// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_helpers.h"
void SceneActorControl_ResetCollisionPhase(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    register u32 *state asm("r1") = (void *)SceneActorControl_GetMotionState(actor);
    register u32 mask asm("r3") = 0x100000;
    u32 *flags = &actor->flags2D0;
    *flags |= mask;
    {
        u32 offset = 0x3D4;
        u32 mask;
        state = (void *)((u8 *)state + offset);
        mask = ~7;
        *state &= mask;
    }
    {
        struct ContactCollider *collider = &actor->colliderB;
        register u32 mask asm("r0");
        mask = ~4;
        mask &= collider->flags;
        collider->flags = mask;
    }
}
