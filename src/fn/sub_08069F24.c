#include "burst_emitter.h"
void BurstEmitter_DestroyPool(struct BurstEmitterPool *unused)
{
    gBurstEmitterPool = NULL;
}
