#include "engine/math/scalar_approach.h"
/* ScalarApproach_SixteenthArray: Updates signed halfword values toward corresponding targets using sixteenth steps */
void ScalarApproach_SixteenthArray(s16 *values, const s16 *targets, s32 count)
{
    while ((count = (s32)((u32)count - 1)) >= 0) {
        ScalarApproach_AdvanceValue(values, targets, 4);
        ++values;
        ++targets;
    }
}
