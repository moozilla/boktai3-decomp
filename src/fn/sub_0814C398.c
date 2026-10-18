#include "global.h"
void Player_OnGrabMotionExit(u8 *p)
{
    u32 *q = (u32 *)(p + 0xec);
    q[2] |= 1;
}
