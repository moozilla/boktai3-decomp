#include "global.h"

extern const u8 gUnk_0824F488[];
void SparkParticle_Spawn(u32, u32, const u8 *, u32, u32, u32, u32, u32);

void Nidhoggr_EmitSparks(u32 a)
{
    SparkParticle_Spawn(a, 1, gUnk_0824F488, 0x20, 3, 8, 2, 2);
}
