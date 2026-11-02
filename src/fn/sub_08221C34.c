#include "scalar_approach.h"
/* ScalarApproach_StepSixteenthArray: Updates signed halfword values toward corresponding targets using sixteenth steps */
#define ScalarApproach_StepSixteenthArray sub_08221C34
#define ScalarApproach_SixteenthArray     sub_08221C34
void ScalarApproach_SixteenthArray(s16 *values, const s16 *targets, s32 count)
{
    while ((count = (s32)((u32)count - 1u)) >= 0) {
        ScalarApproach_AdvanceValue(values, targets, 4);
        ++values;
        ++targets;
    }
}
