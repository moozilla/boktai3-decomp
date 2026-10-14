#include "scene_pool.h"
#define ScenePool_StateDepartForward sub_0803ED84
void sub_0811D4AC(struct ScenePoolActor *, s32, s32, s32, s32, s32, s32);
static inline void Or(u32 mask, u32 *flags)
{
    *flags |= mask;
}
s32 ScenePool_StateDepartForward(void *manager, struct ScenePoolActor *actor,
                                 struct ScenePoolEntry *entry)
{
    if (entry->stateEntered) {
        u32 zero = 0;
        entry->stateEntered = zero;
        entry->turnStep = 0x200;
        actor->savedPosition = actor->position;
        sub_0811D4AC(actor, 0x3000, 0x2000, -0x2000, 0x5000, zero, 0x6000);
    }
    {
        u8 *flags = &actor->flags5ee;
        s32 mask;
        u32 zero;
        mask = -2;
        mask &= *flags;
        zero = 0;
        *flags = mask;
        actor->unknown57a = zero;
        actor->unknown68 = zero;
        Or(0x2c, &actor->stateFlags);
        if ((actor->position.x - actor->savedPosition.x) >> 12 > 127) {
            actor->controlFlags = zero;
            actor->forwardSpeed = zero;
            actor->forwardBias = zero;
        } else
            actor->controlFlags |= 0x110;
    }
    entry->stateFrame++;
    return 0;
}
