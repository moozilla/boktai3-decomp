#include "scene/actor/scene_actor_control_helpers.h"
void SceneActorControl_ResetCollisionPhase(struct ChaseActor *input)
{
    struct ChaseActor *actor = input;
    u32 *state = (void *)SceneActorControl_GetMotionState(actor);
    u32 mask = 0x100000;
    u32 *flags = &actor->base.statusFlags;
    *flags |= mask;
    {
        u32 mask;
        state = (void *)((u8 *)state + 0x3D4);
        mask = ~7;
        *state &= mask;
    }
    {
        struct ContactCollider *collider = &actor->base.colliderB;
        u32 mask;
        mask = ~4;
        mask &= collider->flags;
        collider->flags = mask;
    }
}
