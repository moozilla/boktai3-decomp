#include "pooled_motion.h"
void MotionPool_SetCallback(struct MotionPoolParams *params,
                            s32 (*callback)(struct MotionPoolSlot *, s32))
{
    params->callback = callback;
}
