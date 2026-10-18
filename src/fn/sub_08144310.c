#include "global.h"
void sub_0813AFEC(u8 *, u32, u32);
void Player_ResetIdleMotion(u8 *p)
{
    *(u32 *)(p + 0x20) &= ~1;
    if (p[0x5bb] == 0)
        sub_0813AFEC(p, 0, 0);
}
