#include "global.h"

void Ratatosk_SetAnimatedParticleUpdateCallback(u8 *p, u32 a)
{
    *(u32 *)(p + 0x14) = a;
}
