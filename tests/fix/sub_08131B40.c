#include "global.h"

extern u32 gUnk_030053F4, gUnk_030053F0, gUnk_02000560;

static inline void SetState(u8 *p, u32 s, u32 b)
{
    u8 *state = &p[0x2A8];
    u32 zero = 0;
    *state = s;
    p[0x2A9] = zero;
    p[0x2AA] = b;
    *(u32 *)(p + 0x2AC) = zero;
    p[0x2B1] = 1;
}

void sub_08131B40(u8 *p)
{
    u32 mask = 0x400;
    if (((gUnk_030053F4 | gUnk_030053F0) & mask) || (gUnk_02000560 & 4))
        SetState(p, 2, 1);
    else
        Callback_InvokeActor(p, (*(void ***)(p + 0x284))[p[0x2AA]]);
}
