#include "engine/math/scalar_approach.h"
/* ScalarApproach_HalfAngle: Approaches the target along the wrapped angle difference by half and snaps within one unit */
s32 ScalarApproach_HalfAngle(s32 current, s32 target)
{
    s16 angle = (current - target) & 255;
    s32 near, delta, step;
    if (angle > 128)
        angle -= 256;
    near = target + angle;
    delta = target - near;
    if ((u32)delta + 1 <= 2)
        return target;
    if (delta >= 0)
        step = delta >> 1;
    else
        step = -(-delta >> 1);
    return near + step;
}
