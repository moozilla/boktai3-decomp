#include "engine/math/scalar_approach.h"
/* ScalarApproach_HalfArray: Updates signed halfword values toward corresponding targets using half steps */
void ScalarApproach_HalfArray(s16 *values, const s16 *targets, s32 count)
{
    s32 remaining = count;
    s16 *cursor = values;
    const s16 *targetCursor;
    targetCursor = targets;
    while ((remaining = (s32)((u32)remaining - 1)) >= 0) {
        s32 target = *targetCursor;
        s32 current;
        s32 result;
        current = *cursor;
        result = ScalarApproach_Half(current, target);
        *cursor = result;
        ++cursor;
        ++targetCursor;
    }
}
