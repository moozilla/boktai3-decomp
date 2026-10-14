#include "pooled_motion.h"
#define MotionPool_Initialize     sub_080456C8
#define MotionPool_InitializeSlot sub_080454C8
void MotionPool_InitializeSlot(struct MotionPool *, struct MotionPoolSlot *, s32);
void sub_0821A284(u32, void *, u32);
s32 MotionPool_Initialize(struct MotionPool *pool)
{
    struct MotionPoolSlot *slot;
    s32 index;
    sub_0821A284(0x5B1B, pool, 0);
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
