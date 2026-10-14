#include "pooled_motion.h"
void MotionPool_SetLifetimeAndVelocity(struct MotionPoolParams *params, u16 firstFrame,
                                       u16 endFrame, u8 direction, s32 speed)
{
    params->firstFrame = firstFrame;
    params->endFrame = endFrame;
    params->direction = direction;
    params->speed = speed;
}
