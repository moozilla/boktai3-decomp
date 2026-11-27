#include "scene/actor/scene_actor_drawable_flags.h"
#include "scene/actor/scene_actor_control_helpers.h"
void SceneActorControl_TickVisibility(struct ChaseActor *);
void SceneActorControl_TickGroundBob(struct ChaseActor *);
s32 SceneActorControl_TickHiddenResponse(struct ChaseActor *actor)
{
    u32 mask = 0x4000;
    u16 *field;
    field = &actor->base.motionFlags;
    if (*field & mask) {
        u32 mask = 0x10;
        u32 *input;
        u32 *flags;
        u32 enabled;
        input = &((struct SceneActorControlMotionState *)actor)->flags3D4;
        enabled = *input & mask;
        flags = input;
        if (SceneActorControl_TestFlags(enabled, ~0)) {
            ActorStatus_SetDrawableFlags(actor, 1);
            {
                u32 mask = ~0x60;
                *flags &= mask;
            }
        } else
            SceneActorControl_TickVisibility(actor);
    }
    SceneActorControl_TickGroundBob(actor);
    return 1;
}
