#include "engine/math/scalar_approach.h"
/* ScalarApproach_EighthAngleArray: Updates signed halfword angles toward corresponding targets using wrapped eighth steps */
void ScalarApproach_EighthAngleArray(s16 *values, const s16 *targets, s32 count)
{
    s16 *value = values;
    const s16 *target = targets;

    while (--count >= 0) {
        *value = ScalarApproach_Shift3(ScalarApproach_Unwrap(*value, *target), *target);
        value++;
        target++;
    }
}
