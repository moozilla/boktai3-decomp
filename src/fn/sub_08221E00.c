// MATCHING_CONCESSION: register-pinning
#include "scalar_approach.h"
#define ScalarApproach_QuarterAngleArray sub_08221E00
void ScalarApproach_QuarterAngleArray(s16 *values, const s16 *targets, s32 count)
{
    s32 remaining = count;
    register s16 *cursor asm("r5");
    register const s16 *targetCursor asm("r6");
    cursor = values;
    targetCursor = targets;
    while ((remaining = (s32)((u32)remaining - 1u)) >= 0) {
        register s32 current asm("r0");
        s32 target;
        register s32 result asm("r0");

        register u32 masked asm("r3");
        s16 angle;
        register s32 converted asm("r0");
        s32 near;
        register s32 delta asm("r3");
        register s32 step asm("r0");
        current = *cursor;
        target = *targetCursor;
        masked = ((u32)current - (u32)target) & 255;
        asm("" : : "r"(masked));
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
