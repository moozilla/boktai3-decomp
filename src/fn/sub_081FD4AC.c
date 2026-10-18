#include "global.h"

void Projectile_SetBehaviorCallback(u8 *p, u32 a)
{
    *(u32 *)(p + 0x10) = a;
}
