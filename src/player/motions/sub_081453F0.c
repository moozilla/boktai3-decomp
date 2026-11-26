#include "global.h"
void Player_OnFadeMotionExit(u8 *p)
{
    u32 *q = (u32 *)(p + 0x8c);
    q[2] &= ~2;
}
