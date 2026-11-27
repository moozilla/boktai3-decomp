#include "global.h"

void Ratatosk_SetParticleUpdateCallback(u8 *p, u32 a)
{
    *(u32 *)(p + 0x10) = a;
}
