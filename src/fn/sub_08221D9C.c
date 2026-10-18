#include "scalar_approach.h"
/* ScalarApproach_StepAngleHalfArray: Updates signed halfword angles toward corresponding targets using wrapped half steps */
#define ScalarApproach_StepAngleHalfArray sub_08221D9C
#define ScalarApproach_HalfAngleArray sub_08221D9C
void ScalarApproach_HalfAngleArray(s16 *values, const s16 *targets, s32 count)
{
    s32 remaining = count;
    s16 *cursor;
    const s16 *targetCursor;
    cursor = values;
    targetCursor = targets;
    while ((remaining = (s32)((u32)remaining - 1u)) >= 0) {
        s32 current;
        s32 target;
        s32 result;

        u32 masked;
        s16 angle;
        s32 converted;
        s32 near;
        s32 delta;
        s32 step;
        current = *cursor;
        target = *targetCursor;
        masked = ((u32)current - (u32)target) & 255;
        angle = masked;
        if (angle > 128)
            angle = angle - 256;
        converted = angle;
        near = (s32)((u32)target + (u32)converted);
        delta = (s32)((u32)target - (u32)near);
        if (((u32)delta + 1u) <= 2) {
            result = target;
            goto store;
        }
        if (delta >= 0)
            step = delta >> 1;
        else
            step = (s32)(0u - (u32)((s32)(0u - (u32)delta) >> 1));
        result = (s32)((u32)near + (u32)step);

    store:
        *cursor = result;
        ++cursor;
        ++targetCursor;
    }
}
