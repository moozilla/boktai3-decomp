#include "scene_actor_chase.h"
void Slime_UpdateRandomWait(void);

struct Q {
    u8 filler[0x408];
    u16 timer;
};

#define Slime_TickWaitTimer sub_0808EACC

void Slime_TickWaitTimer(struct SceneActorRegistryResetView *actor)
{
    struct Q *q = actor->context;
    if (SceneActorReaction_TakePrepared(actor)) {
        u32 index = (gUnk_03005308 + 1) & 1023;
        u32 random;
        gUnk_03005308 = index;
        random = *(u16 *)(index * 2 + (u32)gUnk_0203B400);
        q->timer = (random & 63) + 0x40;
    }
    if (q->timer)
        q->timer--;
    else if (actor->field2B3)
        Chase_SetCtl(actor, Slime_UpdateRandomWait, 0x22);
}
