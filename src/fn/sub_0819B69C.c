#include "scalar_approach.h"
s32 Div(s32, s32);
void sub_0819B69C(s16 *values, const s16 *targets, s32 count, s32 divisor)
{
    s16 *v = values;
    const s16 *t = targets;
    while (--count >= 0) {
        s32 cur = *v;
        s32 target = *t;
        s32 near = ScalarApproach_Unwrap(cur, target);
        s32 d = Div((target - near) << 6, divisor);
        s32 res;
        if ((d < 0 ? -d : d) <= 5)
            res = target;
        else {
            if ((u32)(d - 1) <= 62)
                d = 64;
            else if (d < 0 && d > -64)
                d = -64;
            if (d >= 0)
                res = d >> 6;
            else
                res = -((-d) >> 6);
            near += res;
            res = near;
        }
        *v = res;
        v++;
        t++;
    }
}
