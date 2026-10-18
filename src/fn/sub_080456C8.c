#include "pooled_motion.h"
void MotionPool_InitializeSlot(struct MotionPool *, struct MotionPoolSlot *, s32);
void ResourceRegistry_RegisterResource(u32, void *, u32);
s32 MotionPool_Initialize(struct MotionPool *pool)
{
    struct MotionPoolSlot *slot;
    s32 index;
    ResourceRegistry_RegisterResource(0x5B1B, pool, 0);
    pool->activeMask = 0;
    slot = pool->slots;
    index = 0;
    do {
        MotionPool_InitializeSlot(pool, slot, index);
        index++;
        slot++;
    } while (index <= 15);
    return 0;
}
