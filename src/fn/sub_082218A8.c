#include "global.h"
s32 sub_082218A8(s32 current, s32 target, s32 step)
{
    s32 n, r;
    if (current < target) {
        n = current + step;
        if (n < target)
            goto set;
        r = target;
    } else {
        n = current - step;
        r = target;
        if (n > r)
        set:
            r = n;
    }
    return r;
}
