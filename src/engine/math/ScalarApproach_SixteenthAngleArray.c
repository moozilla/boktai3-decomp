#include "engine/math/scalar_approach.h"
/* ScalarApproach_SixteenthAngleArray: Updates signed halfword angles toward corresponding targets using wrapped sixteenth steps */
void ScalarApproach_SixteenthAngleArray(s16 *values, const s16 *targets, s32 count)
{
    s16 *value = values;
    const s16 *target = targets;

    while (--count >= 0) {
        *value = ScalarApproach_Shift4(ScalarApproach_Unwrap(*value, *target), *target);
        value++;
        target++;
    }
}
