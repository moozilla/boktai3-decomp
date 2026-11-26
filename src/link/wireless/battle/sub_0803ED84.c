#include "link/wireless/scene_pool.h"
void ActorStats_ConfigureScaledParameters(struct ScenePoolActor *, s32, s32, s32, s32, s32, s32);
s32 ScenePool_StateDepartForward(void *manager, struct ScenePoolActor *actor,
                                 struct ScenePoolEntry *entry)
{
    if (entry->stateEntered) {
        entry->stateEntered = 0;
        entry->turnStep = 0x200;
        actor->savedPosition = actor->position;
        ActorStats_ConfigureScaledParameters(actor, 0x3000, 0x2000, -0x2000, 0x5000, 0, 0x6000);
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
        actor->unknown68 = 0;
        ScenePoolActor_SetStateFlags(actor, 0x2c);
        if ((actor->position.x - actor->savedPosition.x) >> 12 > 127) {
            actor->controlFlags = 0;
            actor->forwardSpeed = 0;
            actor->forwardBias = 0;
        } else
            actor->controlFlags |= 0x110;
    }
    entry->stateFrame++;
    return 0;
}
