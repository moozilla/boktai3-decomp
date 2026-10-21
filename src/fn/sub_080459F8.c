#include "pooled_motion.h"
void MotionPool_AdjustHeading(struct MotionPoolSlot *);
s32 MotionPool_UpdateUntilDeadline(struct MotionPoolSlot *slot, s32 index)
{
    struct MotionPoolParams *params = &slot->params;
    MotionPool_AdjustHeading(slot);
    {
        u32 flag = 2;
        slot->flags |= flag;
    }
    if (slot->frame >= params->endFrame)
        return 1;
    return 0;
}
