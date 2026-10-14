#include "pooled_motion.h"
void MotionPool_HideSlot(struct MotionPool *, struct MotionPoolSlot *, s32);
void sub_0821A2AC(u32);
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
    sub_0821A2AC(0x5B1B);
    return 0;
}
