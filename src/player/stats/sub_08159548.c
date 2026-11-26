#include "global.h"

void Player_SetResistance(u8 *p, s32 i, u8 v)
{
    u8 *q = p + 0x450;

    q[i] = v;
}
