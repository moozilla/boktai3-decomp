#include "scalar_approach.h"
#define ScalarApproach_QuarterAngleArray sub_08221E00
void ScalarApproach_QuarterAngleArray(s16 *values, const s16 *targets, s32 count)
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
        if (!delta) {
            result = target;
            goto store;
        }
        if (((u32)delta + 3u) <= 6) {
            s32 minimum = -4;
            if (delta > 0)
                minimum = 4;
            delta = minimum;
        }
        if (delta >= 0)
            step = delta >> 2;
        else
            step = (s32)(0u - (u32)((s32)(0u - (u32)delta) >> 2));
        result = (s32)((u32)near + (u32)step);

    store:
        *cursor = result;
        ++cursor;
        ++targetCursor;
    }
}
