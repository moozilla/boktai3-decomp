#include "engine/math/scalar_approach.h"
s32 Div(s32, s32);
void sub_08221F54(s16 *values, const s16 *targets, s32 count, s32 divisor)
{
    s16 *value = values;
    const s16 *target = targets;

    while (--count >= 0) {
        s32 current = *value;
        s32 to = *target;
        s32 near = ScalarApproach_Unwrap(current, to);
        s32 step = Div((to - near) << 6, divisor);
        s32 result;
        s32 whole;

        if ((step < 0 ? -step : step) <= 5) {
            result = to;
        } else {
            if ((u32)(step - 1) <= 62)
                step = 64;
            else if (step < 0 && step > -64)
                step = -64;
            if (step >= 0)
                whole = step >> 6;
            else
                whole = -(-step >> 6);
            result = near + whole;
        }
        *value = result;
        value++;
        target++;
    }
}
