#define MotionPool_SetTracking sub_08045948
#include "pooled_motion.h"
void MotionPool_SetTracking(struct MotionPoolParams *params, u16 frames, u32 targetId, u8 turnSpeed)
{
    params->trackingFrames = frames;
    params->targetId = targetId;
    params->turnSpeed = turnSpeed;
}
