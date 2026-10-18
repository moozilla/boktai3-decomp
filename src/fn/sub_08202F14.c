#include "global.h"

void ProjectileConfig_SetCollisionOffset(u8 *p, u16 a, u16 b, u16 c)
{
    *(u16 *)(p + 0x2C) = a;
    *(u16 *)(p + 0x2E) = b;
    *(u16 *)(p + 0x30) = c;
}
