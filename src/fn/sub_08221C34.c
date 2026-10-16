// MATCHING_CONCESSION: register-pinning
#include "scalar_approach.h"
#define ScalarApproach_SixteenthArray sub_08221C34
void ScalarApproach_SixteenthArray(s16 *values, const s16 *targets, s32 count)
{
    while ((count = (s32)((u32)count - 1u)) >= 0) {
        s32 target = *targets;
        asm("" : "+r"(target));
        {
            s32 current = *values;
            *values = ScalarApproach_Shift4(current, target);
        }
        ++values;
        ++targets;
    }
}
