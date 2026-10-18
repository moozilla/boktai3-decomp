#include "global.h"

void Projectile_SetBehaviorMode(u8 *p, u16 a)
{
    *(u16 *)(p + 0x4E) = a;
}
