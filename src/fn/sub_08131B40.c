#include "global.h"

extern u32 gUnk_030053F4, gUnk_030053F0, gUnk_02000560;

static inline u32 Input_Held(u32 mask)
{
    return (gUnk_030053F4 | gUnk_030053F0) & mask;
}

static inline u32 Game_Flag(u32 mask)
{
    return gUnk_02000560 & mask;
}

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

/* Leave the state (2, behaviour 1) on input 0x400 or game flag 4;
   otherwise run the behaviour callback. The masks are helper arguments:
   the ROM loads each before its global. */
void sub_08131B40(u8 *p)
{
    if (Input_Held(0x400) || Game_Flag(4))
        SetState(p, 2, 1);
    else {
        u32 i = p[0x2AA];
        Callback_InvokeActor(p, (*(void ***)(p + 0x284))[i]);
    }
}
