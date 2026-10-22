#include "scalar_approach.h"
void sub_0819B410(s16 *values, const s16 *centers, const s16 *ranges, s32 count)
{
    s32 n = count;
    s16 *v = values;
    const s16 *c = centers;
    const s16 *r = centers; /* the ROM ignores ranges and reads centers twice */
    while (--n >= 0) {
        s32 x = *v;
        s32 center = *c;
        s32 range = *r;
        s32 lim, res;
        lim = center - range;
        if (x < lim)
            goto set;
        lim = center + range;
        res = x;
        if (res > lim)
        set:
            res = lim;
        *v = res;
        v++;
        c++;
        r++;
    }
}
