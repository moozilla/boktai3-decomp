#include "scalar_approach.h"
/* ScalarApproach_StepQuarterArray: Updates signed halfword values toward corresponding targets using quarter steps */
#define ScalarApproach_StepQuarterArray sub_08221B9C
#define ScalarApproach_QuarterArray     sub_08221B9C
void ScalarApproach_QuarterArray(s16 *values, const s16 *targets, s32 count)
{
    while ((count = (s32)((u32)count - 1u)) >= 0) {
        ScalarApproach_AdvanceValue(values, targets, 2);
        ++values;
        ++targets;
    }
}
