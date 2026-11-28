#include "engine/math/scalar_approach.h"
/* ScalarApproach_EighthAngle: Approaches the target along the wrapped angle difference by one eighth with a minimum unit step */
s32 ScalarApproach_EighthAngle(s32 current, s32 target)
{
    s32 near = ScalarApproach_Unwrap(current, target);
    s32 delta = target - near;
    s32 step;

    if (!delta)
        return target;
    if ((u32)delta + 7 <= 14) {
        s32 minimum = -8;
        if (delta > 0)
            minimum = 8;
        delta = minimum;
    }
    if (delta >= 0)
        step = delta >> 3;
    else
        step = -(-delta >> 3);
    return near + step;
}
