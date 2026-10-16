#include "burst_emitter.h"
static inline u8 BurstEmitter_IsOccupied(struct BurstEmitterPool *pool, s32 index)
{
    if (pool->used & (1 << index))
        return TRUE;
    return FALSE;
}
s32 BurstEmitter_UpdatePool(struct BurstEmitterPool *pool)
{
    s32 i;
    if (pool->used) {
        for (i = 0; i <= 4; i++) {
            if (BurstEmitter_IsOccupied(pool, i)) {
                struct BurstEmitter *emitter = &pool->slots[i];
                ((void (*)(struct BurstEmitterPool *,
                           struct BurstEmitter *))gUnk_08605B78[emitter->state])(pool, emitter);
            }
        }
    }
    return 0;
}
