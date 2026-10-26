#include "scene_actor_chase.h"
void Slime_UpdateRandomWait(void);

struct Q {
    u8 filler[0x408];
    u16 timer;
};

void Slime_TickWaitTimer(struct SceneActorRegistryResetView *actor)
{
    struct Q *q = actor->context;
    if (SceneActorReaction_TakePrepared(actor)) {
        u32 index = (gRandomIndex + 1) & 1023;
        u32 random;
        gRandomIndex = index;
        random = *(u16 *)(index * 2 + (u32)gUnk_0203B400);
        q->timer = (random & 63) + 0x40;
    }
    if (q->timer)
        q->timer--;
    else if (actor->field2B3)
        Chase_SetCtl(actor, Slime_UpdateRandomWait, 0x22);
}
