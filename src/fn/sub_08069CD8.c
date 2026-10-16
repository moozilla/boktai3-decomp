#include "burst_emitter.h"
void BurstEmitter_ReleaseSlot(struct BurstEmitterPool *pool, struct BurstEmitter *emitter)
{
    emitter->state = 0;
    pool->used &= ~(1 << emitter->index);
    emitter->index = 0xFF;
    Memory_Clear(emitter, 0x68);
}
