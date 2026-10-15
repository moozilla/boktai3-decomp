// MATCHING_CONCESSION: register-pinning
#include "burst_emitter.h"
#define BurstEmitter_AllocateSlot sub_08069D0C
static inline u8 BurstEmitter_IsOccupied(struct BurstEmitterPool *pool, s32 index, u32 one)
{
    if (pool->used & (one << index))
        return TRUE;
    return FALSE;
}
struct BurstEmitter *BurstEmitter_AllocateSlot(struct BurstEmitterPool *input)
{
    register struct BurstEmitterPool *pool asm("r3") = input;
    s32 index = 0;
    u32 one;
    asm("" : "+r"(pool) : "r"(index) : "r0", "r1", "r2");
    one = 1;
    for (; index <= 4; index++) {
        if (!BurstEmitter_IsOccupied(pool, index, one)) {
            struct BurstEmitter *emitter = &pool->slots[index];
            emitter->index = index;
            asm("" : "+r"(one));
            pool->used |= one << index;
            return emitter;
        }
    }
    return NULL;
}
