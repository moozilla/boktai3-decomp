#include "engine/math/scalar_approach.h"
/* ScalarApproach_StepHalfArray: Updates signed halfword values toward corresponding targets using half steps */
void ScalarApproach_StepHalfArray(s16 *values, const s16 *targets, s32 count)
{
    s16 *value = values;
    const s16 *target = targets;

    while (--count >= 0) {
        *value = ScalarApproach_Half(*value, *target);
        value++;
        target++;
    }
}
