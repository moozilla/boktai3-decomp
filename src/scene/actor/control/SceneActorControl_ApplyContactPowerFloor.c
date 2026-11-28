#include "scene/motion/scene_actor_availability.h"
#include "scene/actor/scene_actor_control_motion.h"
void SceneActorBehavior_ApplyContactReaction(struct ContactCollider *, struct ContactCollider *,
                                             struct RoomActor *);

void SceneActorControl_ApplyContactPowerFloor(struct ContactCollider *source,
                                              struct ContactCollider *destination,
                                              struct ChaseActor *actor)
{
    s32 power;
    u32 sample;
    if (SceneActor_ActionsBlocked((struct RoomActor *)actor))
        return;
    {
        u32 mask = 0x800;
        if (source->flags & mask)
            return;
    }
    SceneActorBehavior_ApplyContactReaction(source, destination, (struct RoomActor *)actor);
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    sample = ((u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
    power = destination->responseParameter - (sample & 15);
    destination->responseParameter = power < 20 ? 20 : power;
    {
        struct SceneActorControlMotionState *control = SceneActorControl_GetMotionState(actor);
        u32 *flags = &control->flags3D4;
        {
            u32 mask = ~8;
            *flags &= mask;
        }
    }
}
