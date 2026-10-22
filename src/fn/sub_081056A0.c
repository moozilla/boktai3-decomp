#include "global.h"

struct PoolActor {
    u8 pad0[0x144];
    u16 step;
    u8 level;
    u8 target;
    u8 pad148[0x14A - 0x148];
    s16 health;
    s16 maxHealth;
};
u8 Pool184_TestStatusFlags(void *, u32);
void ProximityActor_SetState(void *, u32);

void CallbackPool_RefreshHealthDisplay(struct PoolActor *o)
{
    u8 *level;
    if (o->target > *(level = &o->level)) {
        s32 diff = o->maxHealth - o->health;
        if (!Pool184_TestStatusFlags(o, 1))
            ProximityActor_SetState(o, 2);
        if (diff >= o->step * (*level - 1))
            ProximityActor_SetState(o, 1);
    }
}
