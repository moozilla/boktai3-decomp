// COMPILER: old_agbcc
#include "global.h"
/* Projectile_SetUniformCollisionSize: Sets all three collider dimensions to the same value */
#define Projectile_SetUniformCollisionSize sub_081DACEC

void sub_081DACEC(u8 *p, u32 v)
{
    *(u16 *)(p + 0x28) = v;
    *(u16 *)(p + 0x2a) = v;
    *(u16 *)(p + 0x2c) = v;
}
