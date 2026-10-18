#include "scalar_approach.h"
#define ScalarApproach_Snap8Array sub_082220DC
void ScalarApproach_Snap8Array(s16 *values, const s16 *targets, s32 threshold, s32 count)
{
    s32 initialLimit = threshold;
    s32 limit;
    s16 *cursor;
    const s16 *targetCursor;
    limit = initialLimit;
    cursor = values;
    targetCursor = targets;
    while ((count = (s32)((u32)count - 1u)) >= 0) {
        s32 current = *cursor, target = *targetCursor;
        s32 delta = (s32)((u32)target - (u32)current), step;
        if (delta >= 0)
            step = delta >> 3;
        else
            step = (s32)(0u - (u32)((s32)(0u - (u32)delta) >> 3));
        current = (s32)((u32)current + (u32)step);
        if (step > (s32)(0u - (u32)limit) && step < limit)
            current = target;
        *cursor = current;
        ++cursor;
        ++targetCursor;
    }
}
