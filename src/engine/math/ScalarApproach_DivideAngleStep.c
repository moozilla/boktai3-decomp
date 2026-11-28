#include "engine/math/scalar_approach.h"
/* ScalarApproach_DivideAngleStep: Computes a divisor-controlled wrapped angle step with minimum-step and snap thresholds */
s32 ScalarApproach_DivideAngleStep(s32 current, s32 target, s32 divisor)
{
    s32 near = ScalarApproach_Unwrap(current, target);
    s32 step = Div((target - near) << 6, divisor);
    s32 whole;

    if ((step < 0 ? -step : step) <= 5)
        return target;
    if ((u32)(step - 1) <= 62)
        step = 64;
    else if (step < 0 && step > -64)
        step = -64;
    if (step >= 0)
        whole = step >> 6;
    else
        whole = -(-step >> 6);
    return near + whole;
}
