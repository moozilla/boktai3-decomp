// COMPILER: old_agbcc
#include "global.h"
/* Projectile_SetTurnParameters: Sets a secondary timing value and direction adjustment parameters; interpretation tentative */
#define Projectile_SetTurnParameters sub_081FD46C

void sub_081FD46C(u8 *p, u16 a, u32 b, u8 c)
{
    *(u16 *)(p + 4) = a;
    *(u32 *)(p + 0xc) = b;
    p[7] = c;
}
