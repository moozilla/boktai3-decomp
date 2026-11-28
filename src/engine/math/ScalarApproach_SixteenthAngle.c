#include "engine/math/scalar_approach.h"
/* ScalarApproach_SixteenthAngle: Approaches the target along the wrapped angle difference by one sixteenth with a minimum unit step */
s32 ScalarApproach_SixteenthAngle(s32 current, s32 target)
{
    return ScalarApproach_Shift4(ScalarApproach_Unwrap(current, target), target);
}
