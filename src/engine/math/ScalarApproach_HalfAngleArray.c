#include "engine/math/scalar_approach.h"
/* ScalarApproach_HalfAngleArray: Updates signed halfword angles toward corresponding targets using wrapped half steps */
void ScalarApproach_HalfAngleArray(s16 *values, const s16 *targets, s32 count)
{
    s16 *value = values;
    const s16 *target = targets;

    while (--count >= 0) {
        *value = ScalarApproach_Half(ScalarApproach_Unwrap(*value, *target), *target);
        value++;
        target++;
    }
}
