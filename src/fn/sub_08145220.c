#include "global.h"
void Player_AddControlFlags(u8 *, u32);
void Player_UpdateFallPose(u8 *p)
{
    u32 m;
    if (p[0x4f2] != 0)
        Player_AddControlFlags(p, 0x1c);
    else
        Player_AddControlFlags(p, 0x1d);
    m = 0x1100;
    *(u32 *)(p + 0x20) = *(u32 *)(p + 0x20) | m;
}
