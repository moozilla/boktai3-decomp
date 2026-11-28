#include "engine/math/scalar_approach.h"
/* ScalarApproach_SixteenthAngle: Approaches the target along the wrapped angle difference by one sixteenth with a minimum unit step */
s32 ScalarApproach_SixteenthAngle(s32 current, s32 target)
{
    s32 near = ScalarApproach_Unwrap(current, target);
    s32 delta = target - near;
    s32 step;

    if (!delta)
        return target;
    if ((u32)delta + 15 <= 30) {
        s32 minimum = -16;
        if (delta > 0)
            minimum = 16;
        delta = minimum;
    }
    if (delta >= 0)
        step = delta >> 4;
    else
        step = -(-delta >> 4);
    return near + step;
}
