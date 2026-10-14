#include "global.h"

s32 TetherActor_CanStartInteraction(void);

u32 sub_08072BC4(u8 *p)
{
    u32 r;
    if ((p[4] & 2) || *(u16 *)(p + 6) || (p[5] == 1 && TetherActor_CanStartInteraction() == 0))
        r = 0;
    else
        r = 1;
    return r;
}
