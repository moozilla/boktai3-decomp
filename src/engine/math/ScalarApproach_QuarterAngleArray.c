#include "engine/math/scalar_approach.h"
/* ScalarApproach_QuarterAngleArray: Updates signed halfword angles toward corresponding targets using wrapped quarter steps */
void ScalarApproach_QuarterAngleArray(s16 *values, const s16 *targets, s32 count)
{
    s16 *value = values;
    const s16 *target = targets;

    while (--count >= 0) {
        *value = ScalarApproach_Shift2(ScalarApproach_Unwrap(*value, *target), *target);
        value++;
        target++;
    }
}
