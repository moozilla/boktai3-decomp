#include "scalar_approach.h"
#define ScalarApproach_QuarterAngle sub_0822194C
s32 ScalarApproach_QuarterAngle(s32 current, s32 target)
{
    s16 angle = ((u32)current - (u32)target) & 255;
    s32 near, delta, step;
    if (angle > 128)
        angle = angle - 256;
    near = (s32)((u32)target + angle);
    delta = (s32)((u32)target - (u32)near);
    if (!delta)
        return target;
    if (((u32)delta + 3u) <= 6) {
        s32 minimum = -4;
        if (delta > 0)
            minimum = 4;
        delta = minimum;
    }
    if (delta >= 0)
        step = delta >> 2;
    else
        step = (s32)(0u - (u32)((s32)(0u - (u32)delta) >> 2));
    return (s32)((u32)near + (u32)step);
}
