#include "engine/word_flags.h"
#include "scene/actor/scene_actor_control_helpers.h"
void SceneActorControl_ResetCollisionPhase(struct ChaseActor *actor)
{
    struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);
    WordFlags_Set(0x100000, &actor->base.statusFlags);
    {
        u32 mask;
        u32 *stateFlags = &state->flags3D4;
        mask = ~7;
        *stateFlags &= mask;
    }
    {
        struct ContactCollider *collider = &actor->base.colliderB;
        u32 mask;
        mask = ~4;
        mask &= collider->flags;
        collider->flags = mask;
    }
}
