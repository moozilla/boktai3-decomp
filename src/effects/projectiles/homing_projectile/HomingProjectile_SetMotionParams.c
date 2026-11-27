#include "global.h"

void HomingProjectile_SetMotionParams(u8 *p, u16 a, u32 b, u8 c, u8 d)
{
    *(u16 *)(p + 2) = a;
    *(u32 *)(p + 0x8) = b;
    *(u8 *)(p + 6) = c;
    *(u8 *)(p + 0x7) = d;
}
