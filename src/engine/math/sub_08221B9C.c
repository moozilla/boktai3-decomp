#include "engine/math/scalar_approach.h"
/* ScalarApproach_QuarterArray: Updates signed halfword values toward corresponding targets using quarter steps */
void ScalarApproach_QuarterArray(s16 *values, const s16 *targets, s32 count)
{
    while ((count = (s32)((u32)count - 1)) >= 0) {
        ScalarApproach_AdvanceValue(values, targets, 2);
        ++values;
        ++targets;
    }
}
