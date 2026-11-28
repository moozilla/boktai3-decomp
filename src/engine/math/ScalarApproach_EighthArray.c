#include "engine/math/scalar_approach.h"
/* ScalarApproach_EighthArray: Updates signed halfword values toward corresponding targets using eighth steps */
void ScalarApproach_EighthArray(s16 *values, const s16 *targets, s32 count)
{
    while (--count >= 0) {
        ScalarApproach_AdvanceValue(values, targets, 3);
        values++;
        targets++;
    }
}
