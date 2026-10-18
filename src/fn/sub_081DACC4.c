// COMPILER: old_agbcc
#include "global.h"
/* Projectile_SetVerticalMotion: Sets the secondary duration and vertical motion fields; precise motion roles tentative */
#define Projectile_SetVerticalMotion sub_081DACC4

void sub_081DACC4(u8 *p, u16 a, u32 b, u8 c)
{
    *(u16 *)(p + 4) = a;
    *(u32 *)(p + 0xc) = b;
    p[7] = c;
}
