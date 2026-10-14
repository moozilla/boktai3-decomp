#define MotionPool_SetCallback sub_08045980
#include "pooled_motion.h"
void MotionPool_SetCallback(struct MotionPoolParams *params,
                            s32 (*callback)(struct MotionPoolSlot *, s32))
{
    params->callback = callback;
}
