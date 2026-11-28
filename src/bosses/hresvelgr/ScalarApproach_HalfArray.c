#include "engine/math/scalar_approach.h"
/* ScalarApproach_HalfArray: Updates signed halfword values toward corresponding targets using half steps */
void ScalarApproach_HalfArray(s16 *values, const s16 *targets, s32 count)
{
    s16 *value = values;
    const s16 *target = targets;
    while (--count >= 0) {
        *value = ScalarApproach_Half(*value, *target);
        value++;
        target++;
    }
}
