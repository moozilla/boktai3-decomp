#include "burst_emitter.h"
void BurstEmitter_ResetPool(void);
s32 BurstEmitter_InitPool(struct BurstEmitterPool *pool, u32 unused)
{
    BurstEmitter_ResetPool();
    gBurstEmitterPool = pool;
    return 0;
}
