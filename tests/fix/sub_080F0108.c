#include "global.h"

static inline void Ivy_ResetPhase(u8 *p)
{
    u32 zero = 0;
    p[0x2A8] = zero;
    p[0x2A9] = zero;
    p[0x2AA] = zero;
    *(u32 *)(p + 0x2AC) = zero;
    p[0x2B1] = 1;
}

void sub_080F0108(u8 *p)
{
    u8 *sub = p + 0x3D4;
    if (p[0x2B1])
        p[0x2B1] = 0;
    if (*(u16 *)(sub + 0x42) <= *(s32 *)(p + 0x2AC))
        Ivy_ResetPhase(p);
    else
        (*(s32 *)(p + 0x2AC))++;
}
