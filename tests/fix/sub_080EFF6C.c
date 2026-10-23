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

void sub_080EFF6C(u8 *p)
{
    if (p[0x2B1])
        p[0x2B1] = 0;
    if (*(s32 *)(p + 0x2AC) > 30)
        Ivy_ResetPhase(p);
    (*(s32 *)(p + 0x2AC))++;
}
