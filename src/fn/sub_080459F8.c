// MATCHING_CONCESSION: register-pinning
#include "pooled_motion.h"
#define MotionPool_UpdateUntilDeadline sub_080459F8
#define MotionPool_AdjustHeading       sub_08045984
void MotionPool_AdjustHeading(struct MotionPoolSlot *);
s32 MotionPool_UpdateUntilDeadline(struct MotionPoolSlot *slot, s32 index)
{
    register s32 result asm("r0");
    struct MotionPoolParams *params = &slot->params;
    MotionPool_AdjustHeading(slot);
    {
        u32 flag = 2;
        slot->flags |= flag;
    }
    if (slot->frame < params->endFrame)
        result = 0;
    else
        result = 1;
    return result;
}
