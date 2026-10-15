#include "scalar_approach.h"
#define ScalarApproach_DivideAngleStep sub_08221A48
s32 ScalarApproach_DivideAngleStep(s32 current, s32 target, s32 divisor)
{
    s32 step, magnitude, near, integerStep;
    near = ScalarApproach_Unwrap(current, target);
    step = Div((s32)((u32)((s32)((u32)target - (u32)near)) << 6), divisor);
    magnitude = step;
    if (magnitude < 0)
        magnitude = (s32)(0u - (u32)magnitude);
    if (magnitude <= 5)
        return target;
    if (((u32)step - 1u) <= 62)
        step = 64;
    else if (step < 0) {
        if (step > -64)
            step = -64;
    }
    if (step >= 0)
        integerStep = step >> 6;
    else
        integerStep = (s32)(0u - (u32)((s32)(0u - (u32)step) >> 6));
    return (s32)((u32)near + (u32)integerStep);
}
