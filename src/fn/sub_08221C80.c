#include "scalar_approach.h"
#define ScalarApproach_UnwrapAngleArray sub_08221C80
void ScalarApproach_UnwrapAngleArray(s16 *values, const s16 *targets, s32 count)
{
    while ((count = (s32)((u32)count - 1u)) >= 0) {
        *values = ScalarApproach_Unwrap(*values, *targets);
        ++values;
        ++targets;
    }
}
