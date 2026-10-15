#include "burst_emitter.h"
#define BurstEmitter_ReleaseSlot sub_08069CD8
void BurstEmitter_ReleaseSlot(struct BurstEmitterPool *pool, struct BurstEmitter *emitter)
{
    emitter->state = 0;
    pool->used &= ~(1 << emitter->index);
    emitter->index = 0xFF;
    Memory_Clear(emitter, 0x68);
}
