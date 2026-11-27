#include "global.h"

void HomingProjectile_SetSecondaryColliderParams(u8 *p, u16 a, u16 b, u16 c)
{
    *(u16 *)(p + 0x18) = a;
    *(u16 *)(p + 0x1A) = b;
    *(u16 *)(p + 0x1C) = c;
}
