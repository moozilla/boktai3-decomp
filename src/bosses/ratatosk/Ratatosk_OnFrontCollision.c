#include "global.h"

void Ratatosk_OnFrontCollision(u32 a, u32 b, u8 *q)
{
    *(u32 *)(q + 0xCC0) = 1;
}
