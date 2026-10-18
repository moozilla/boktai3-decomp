#include "pooled_motion.h"
void MotionPool_HideSlot(struct MotionPool *, struct MotionPoolSlot *, s32);
void ResourceRegistry_UnregisterResource(u32);
s32 MotionPool_Destroy(struct MotionPool *pool)
{
    struct MotionPoolSlot *slot = pool->slots;
    s32 index = 0;
    do {
        if (pool->activeMask & (1 << index))
            MotionPool_HideSlot(pool, slot, index);
        index++;
        slot++;
    } while (index <= 15);
    ResourceRegistry_UnregisterResource(0x5B1B);
    return 0;
}
