#include "global.h"

void EnemyParticle_EnableShadow(u8 *p)
{
    *(u16 *)(p + 0x2E) = 1;
}
