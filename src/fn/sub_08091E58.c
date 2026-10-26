// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_phase.h"
void SceneActorBehavior_ApplyContactReaction(struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorView *);
static inline u8 Unavailable(struct SceneActorRegistryResetView *actor)
{
    register u32 mask asm("r1");
    u32 offset;
    if (actor) {
        mask = 0x40;
        offset = 0x2D4;
        asm("" : "+r"(mask), "+r"(offset));
        if (*(u16 *)((u8 *)actor + offset) & mask)
            return 1;
        mask = 0x20000;
        offset -= 4;
        {
            u32 value = *(u32 *)((u8 *)actor + offset);
            if (value & mask)
                return 1;
        }
        if (actor->health <= 0)
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
    {
        register const u16 *table asm("r2") = gUnk_0203B400;
        {
            u16 *entry = (void *)(index + (u32)table);
            sample = *entry;
        }
    }
    {
        s32 old = destination->power;
        u32 mask = 15;
        power = old - (sample & mask);
    }
    power = power < 20 ? 20 : power;
    destination->power = power;
    {
        struct SceneActorControlState *control = SceneActorControl_GetState(actor);
        register u32 offset asm("r2");
        u32 *flags;
        offset = 0x3D4;
        flags = (void *)((u8 *)control + offset);
        asm("" : "+r"(flags) : "r"(offset));
        {
            u32 mask = ~4;
            *flags &= mask;
        }
    }
}
