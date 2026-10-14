#define MotionPool_SetBounds sub_08045968
#include "pooled_motion.h"
void MotionPool_SetBounds(struct MotionPoolParams *params, u16 x, u16 y, u16 z)
{
    params->bounds.x = x;
    params->bounds.y = y;
    params->bounds.z = z;
}
