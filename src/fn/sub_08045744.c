#define MotionPool_FindFreeSlot sub_08045744
#include "pooled_motion.h"
struct MotionPoolSlot *MotionPool_FindFreeSlot(struct MotionPool *pool, s32 *out)
{
    struct MotionPoolSlot *slot = pool->slots;
    s32 index = 0;
    u32 one = 1;
    u32 active = pool->activeMask;
    do {
        if ((one << index & active) != 0) {
            index++;
            slot++;
            continue;
        }
        *out = index;
        return slot;
    } while (index <= 15);
    return 0;
}
