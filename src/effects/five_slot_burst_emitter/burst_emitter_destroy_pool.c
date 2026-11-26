#include "effects/five_slot_burst_emitter/burst_emitter.h"
void BurstEmitter_DestroyPool(struct BurstEmitterPool *unused)
{
    gBurstEmitterPool = NULL;
}
