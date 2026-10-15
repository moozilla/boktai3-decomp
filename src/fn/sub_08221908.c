#include "scalar_approach.h"
#define ScalarApproach_HalfAngle sub_08221908
s32 ScalarApproach_HalfAngle(s32 current, s32 target)
{
    s16 angle = ((u32)current - (u32)target) & 255;
    s32 near, delta, step;
    if (angle > 128)
        angle = angle - 256;
    near = (s32)((u32)target + angle);
    delta = (s32)((u32)target - (u32)near);
    if (((u32)delta + 1u) <= 2)
        return target;
    if (delta >= 0)
        step = delta >> 1;
    else
        step = (s32)(0u - (u32)((s32)(0u - (u32)delta) >> 1));
    return (s32)((u32)near + (u32)step);
}
