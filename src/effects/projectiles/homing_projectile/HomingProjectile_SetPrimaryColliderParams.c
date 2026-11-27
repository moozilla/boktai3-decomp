#include "global.h"

void HomingProjectile_SetPrimaryColliderParams(u8 *p, u16 a, u16 b, u16 c)
{
    *(u16 *)(p + 0x10) = a;
    *(u16 *)(p + 0x12) = b;
    *(u16 *)(p + 0x14) = c;
}
