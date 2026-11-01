#include "scene_actor_control_phase.h"
void SceneActorBehavior_ApplyContactReaction(struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorView *);
static inline u8 Unavailable(struct SceneActorRegistryResetView *actor)
{
    if (actor) {
        u32 mask = 0x40;
        if (!(actor->flags2D4 & mask)) {
            u32 mask = 0x20000;
            if (!(actor->flags2D0 & mask) && actor->health > 0)
                return 0;
        }
        return 1;
    }
    return 0;
}
void sub_08091E58(struct SceneActorBehaviorContactView *source,
                  struct SceneActorBehaviorContactView *destination,
                  struct SceneActorRegistryResetView *actor)
{
    u32 index;
    s32 power;
    u32 sample;
    if (Unavailable(actor))
        return;
    {
        u32 mask = 0x800;
        if (source->flags & mask)
            return;
    }
    SceneActorBehavior_ApplyContactReaction(source, destination, (void *)actor);
    index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    index *= 2;
    sample = *(const u16 *)(index + RANDOM_TABLE_ADDR);
    {
        s32 old = destination->power;
        u32 mask = 15;
        power = old - (sample & mask);
    }
    power = power < 20 ? 20 : power;
    destination->power = power;
    {
        struct SceneActorControlState *control = SceneActorControl_GetState(actor);
        u32 *flags = &control->flags3D4;
        u32 mask = ~4;
        *flags &= mask;
    }
}
