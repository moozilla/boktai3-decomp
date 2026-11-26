#include "effects/particles/burst_emitter.h"
static inline u8 SparkExplosion_IsOccupied(struct BurstEmitterPool *pool, s32 index, u32 one)
{
    if (pool->used & (one << index))
        return TRUE;
    return FALSE;
}
struct BurstEmitter *SparkExplosion_AllocateSlot(struct BurstEmitterPool *input)
{
    struct BurstEmitterPool *pool = input;
    s32 index = 0;
    for (; index <= 4; index++) {
        if (!SparkExplosion_IsOccupied(pool, index, 1)) {
            struct BurstEmitter *emitter = &pool->slots[index];
            emitter->index = index;
            pool->used |= 1U << index;
            return emitter;
        }
    }
    return NULL;
}
