#include "global.h"

void Projectile_SetVariant(u8 *p, u16 a)
{
    *(u16 *)(p + 0x3A) = a;
}
