// MATCHING_CONCESSION: register-pinning
#include "scalar_approach.h"
#define ScalarApproach_HalfArray sub_08221B5C
void ScalarApproach_HalfArray(s16 *values, const s16 *targets, s32 count)
{
    s32 remaining = count;
    s16 *cursor = values;
    register const s16 *targetCursor asm("r6");
    asm("" : "+r"(remaining), "+r"(cursor));
    targetCursor = targets;
    while ((remaining = (s32)((u32)remaining - 1u)) >= 0) {
        register s32 target asm("r1") = *targetCursor;
        s32 current;
        register s32 result asm("r0");
        asm("" : : "r"(target));
        current = *cursor;
        result = ScalarApproach_Half(current, target);
        *cursor = result;
        ++cursor;
        ++targetCursor;
    }
}
