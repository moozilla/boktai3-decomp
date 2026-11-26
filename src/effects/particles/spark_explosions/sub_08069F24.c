#include "effects/particles/burst_emitter.h"
void SparkExplosion_DestroyPool(struct BurstEmitterPool *unused)
{
    gBurstEmitterPool = NULL;
}
