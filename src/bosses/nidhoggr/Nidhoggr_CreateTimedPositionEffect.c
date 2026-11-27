#include "global.h"

void ExplosionEffect_Create(u32, u32, u32, u32, u32);

void Nidhoggr_CreateTimedPositionEffect(u32 a, u8 *p)
{
    ExplosionEffect_Create(a, 0x10, *(u16 *)(p + 0x726E), 0xa, 0);
}
