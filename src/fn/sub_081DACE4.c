#include "global.h"

void Projectile_SetCollisionSize(u8 *p, u16 a, u16 b, u16 c)
{
    *(u16 *)(p + 0x28) = a;
    *(u16 *)(p + 0x2A) = b;
    *(u16 *)(p + 0x2C) = c;
}
