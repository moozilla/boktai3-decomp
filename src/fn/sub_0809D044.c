#include "global.h"
#include "scene_actor_control_helpers.h"


void SceneActorControl_ResetCollisionFlags(struct SceneActorRegistryResetView *input)
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
        mask = ~1;
        *state &= mask;
    }
    {
        struct ContactCollider *colliderB = &actor->colliderB;
        u32 mask;
        mask = ~4;
        colliderB->flags = mask & colliderB->flags;
        {
            struct ContactCollider *colliderA = &actor->colliderA;
            mask &= colliderA->flags;
            colliderA->flags = mask;
        }
    }
    {
        u16 *p = &actor->flags2C0;
        u32 mask;
        mask = 0xFFFFF7FF;
        *p &= mask;
    }
}
