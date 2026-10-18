#include "global.h"

void EnemyParticle_SetGraphicsParameters(u8 *p, u32 a, u32 b, u32 c)
{
    *(u32 *)(p + 0x14) = a;
    *(u32 *)(p + 0x18) = b;
    *(u32 *)(p + 0x1C) = c;
}
