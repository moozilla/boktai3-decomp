#include "global.h"

void Ivy_UpdateHold();

/* Arm the vine's hold: Ivy_UpdateHold takes over with state 0x25. The
   arguments, the early address and the local constants give the ROM's
   instruction order: agbcc emits code in source evaluation order. */
static inline void Ivy_BeginHold(u8 *p, void (*update)(), u8 state)
{
    u8 *active = &p[0x2B2];
    u32 zero = 0;
    u32 one = 1;
    *active = one;
    p[0x2B0] = zero;
    p[0x2B5] = state;
    *(void (**)())(p + 0x21C) = update;
    *(u16 *)(p + 0x2C8) = zero;
    {
        u32 *flags = (u32 *)(p + 0x114);
        u32 mask = ~1;
        *flags &= mask;
    }
    *(u32 *)(p + 0x104) = zero;
    {
        u8 two = 2;
        p[0x2A8] = 0;
        p[0x2A9] = two;
    }
    p[0x2AA] = one;
    *(u32 *)(p + 0x2AC) = zero;
    p[0x2B1] = one;
}

void sub_080EFEA0(u8 *p)
{
    if (p[0x2B1])
        p[0x2B1] = 0;
    if (p[0x2B0])
        Ivy_BeginHold(p, Ivy_UpdateHold, 0x25);
    else
        (*(u32 *)(p + 0x2AC))++;
}
