// MATCHING_CONCESSION: register-pinning
#include "scalar_approach.h"
#define ScalarApproach_Snap2Array sub_0822204C
void ScalarApproach_Snap2Array(s16 *values, const s16 *targets, s32 threshold, s32 count)
{
    s32 initialLimit = threshold;
    s32 limit;
    register s16 *cursor asm("r2");
    register const s16 *targetCursor asm("r5");
    limit = initialLimit;
    cursor = values;
    targetCursor = targets;
    while ((count = (s32)((u32)count - 1u)) >= 0) {
        s32 current = *cursor, target = *targetCursor;
        s32 delta = (s32)((u32)target - (u32)current), step;
        if (delta >= 0)
            step = delta >> 1;
        else
            step = (s32)(0u - (u32)((s32)(0u - (u32)delta) >> 1));
        current = (s32)((u32)current + (u32)step);
        if (step > (s32)(0u - (u32)limit) && step < limit)
            current = target;
        *cursor = current;
        ++cursor;
        ++targetCursor;
    }
}
