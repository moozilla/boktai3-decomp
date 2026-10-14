#include "pooled_motion.h"
void MotionPool_SetOffset(struct MotionPoolParams *params, u16 x, u16 y, u16 z)
{
    params->offset.x = x;
    params->offset.y = y;
    params->offset.z = z;
}
