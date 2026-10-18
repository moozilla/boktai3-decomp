#include "global.h"

void Projectile_SetCollisionMask(u8 *p, u16 a)
{
    *(u16 *)(p + 0x52) = a;
}
