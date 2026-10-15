#include "scalar_approach.h"
#define ScalarApproach_EighthAngle sub_082219A0
s32 ScalarApproach_EighthAngle(s32 current, s32 target)
{
    s16 angle = ((u32)current - (u32)target) & 255;
    s32 near, delta, step;
    if (angle > 128)
        angle = angle - 256;
    near = (s32)((u32)target + angle);
    delta = (s32)((u32)target - (u32)near);
    if (!delta)
        return target;
    if (((u32)delta + 7u) <= 14) {
        s32 minimum = -8;
        if (delta > 0)
            minimum = 8;
        delta = minimum;
    }
    if (delta >= 0)
        step = delta >> 3;
    else
        step = (s32)(0u - (u32)((s32)(0u - (u32)delta) >> 3));
    return (s32)((u32)near + (u32)step);
}
