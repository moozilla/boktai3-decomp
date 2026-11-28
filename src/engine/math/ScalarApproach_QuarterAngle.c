#include "engine/math/scalar_approach.h"
/* ScalarApproach_QuarterAngle: Approaches the target along the wrapped angle difference by one quarter with a minimum unit step */
s32 ScalarApproach_QuarterAngle(s32 current, s32 target)
{
    s32 near = ScalarApproach_Unwrap(current, target);
    s32 delta = target - near;
    s32 step;

    if (!delta)
        return target;
    if ((u32)delta + 3 <= 6) {
        s32 minimum = -4;
        if (delta > 0)
            minimum = 4;
        delta = minimum;
    }
    if (delta >= 0)
        step = delta >> 2;
    else
        step = -(-delta >> 2);
    return near + step;
}
