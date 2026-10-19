#define ActorTimers_GetActiveMask sub_081B654C
#include "global.h"

struct ActorTimers {
    u8 pad0[0x20C];
    s16 timerA;
    u8 pad20E[0x428 - 0x20E];
    s16 timerB;
};
static inline u8 Timer_IsActive(struct ActorTimers *actor, u32 second)
{
    u8 active = 0;
    if ((second ? actor->timerB : actor->timerA) > 0)
        active = 1;
    return active;
}
u32 sub_081B654C(struct ActorTimers *actor)
{
    u32 mask = 0;
    if (Timer_IsActive(actor, 0))
        mask |= 1;
    if (Timer_IsActive(actor, 1))
        mask |= 2;
    return mask;
}
