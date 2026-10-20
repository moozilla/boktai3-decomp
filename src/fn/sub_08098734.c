#include "global.h"
inline static u8 Actor_IsInactive(u8 *c)
{
    u32 mask;
    u32 *p;
    u32 off;
    if (!c)
        goto zero;
    mask = 0x40;
    off = 0xB5;
    off <<= 2;
    p = (u32 *)(c + off);
    if ((*((u16 *)p)) & mask)
        goto one;
    mask = 0x80;
    mask <<= 10;
    off -= 4;
    p = (u32 *)(c + off);
    if ((*p) & mask)
        goto one;
    if ((*((s16 *)(c + 0x9C))) > 0)
        goto zero;
one:
    return 1;
zero:
    return 0;
}

#define ActorTick_DecrementCounterAA sub_08098734
void ActorTick_DecrementCounterAA(u8 *actor)
{
    if (!Actor_IsInactive(actor)) {
        s16 *p = (s16 *)(actor + 0xAA);
        s32 v = *p;
        if (v > 0)
            *p = v - 1;
    }
}
