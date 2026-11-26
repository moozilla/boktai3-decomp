#include "effects/particles/burst_emitter.h"
void SparkExplosion_ResetPool(void);
s32 SparkExplosion_InitPool(struct BurstEmitterPool *pool, u32 unused)
{
    SparkExplosion_ResetPool();
    gBurstEmitterPool = pool;
    return 0;
}
