#include "scene_actor_control_helpers.h"
void SceneActorControl_ResetCollisionPhase(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    u32 *state = (void *)SceneActorControl_GetMotionState(actor);
    u32 mask = 0x100000;
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
        u32 mask;
        mask = ~4;
        mask &= collider->flags;
        collider->flags = mask;
    }
}
