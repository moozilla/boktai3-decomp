#include "engine/math/scalar_approach.h"
/* ScalarApproach_QuarterAngle: Approaches the target along the wrapped angle difference by one quarter with a minimum unit step */
s32 ScalarApproach_QuarterAngle(s32 current, s32 target)
{
    return ScalarApproach_Shift2(ScalarApproach_Unwrap(current, target), target);
}
