#define ActorTimers_CountActive sub_081B658C
#include "global.h"

struct ActorTimers {
    u8 pad0[0x20C];
    s16 timerA;
    u8 pad20E[0x428 - 0x20E];
    s16 timerB;
};
u32 sub_081B658C(struct ActorTimers *actor)
{
    u32 count = 0;
    u32 active = 0;
    if (actor->timerA > 0)
        active = 1;
    if ((u8)active)
        count = 1;
    active = 0;
    if (actor->timerB > 0)
        active = 1;
    if ((u8)active)
        count++;
    return count;
}
