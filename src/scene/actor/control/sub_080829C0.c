#include "scene/motion/scene_actor_availability.h"
#include "scene/actor/scene_actor_control_motion.h"
void SceneActorBehavior_ApplyContactReaction(struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorContactView *,
                                             struct RoomActor *);

void SceneActorControl_ApplyContactPowerFloor(struct SceneActorBehaviorContactView *source,
                                              struct SceneActorBehaviorContactView *destination,
                                              struct ChaseActor *actor)
{
    s32 power;
    u32 sample;
    if (SceneActor_ActionsBlocked((void *)actor))
        return;
    {
        u32 mask = 0x800;
        if (source->flags & mask)
            return;
    }
    SceneActorBehavior_ApplyContactReaction(source, destination, (void *)actor);
    {
        union RandomTableView table = {(const u16 *)RANDOM_TABLE_ADDR};
        u32 *cursor = &gRandomIndex;
        u32 next = *cursor + 1;
        *cursor = next & 1023;
        sample = table.samples[*cursor];
    }
    {
        s32 old = destination->power;
        power = old - (sample & 15);
    }
    power = power < 20 ? 20 : power;
    destination->power = power;
    {
        struct SceneActorControlMotionState *control = SceneActorControl_GetMotionState(actor);
        u32 *flags = &control->flags3D4;
        {
            u32 mask = ~8;
            *flags &= mask;
        }
    }
}
