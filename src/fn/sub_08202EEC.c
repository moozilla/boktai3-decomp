// COMPILER: old_agbcc
#include "global.h"
/* ProjectileConfig_SetHoming: Sets homing duration target identifier and maximum turning step */
#define ProjectileConfig_SetHoming sub_08202EEC

void sub_08202EEC(u8 *p, u16 a, u32 b, u8 c)
{
    *(u16 *)(p + 4) = a;
    *(u32 *)(p + 0xc) = b;
    p[7] = c;
}
