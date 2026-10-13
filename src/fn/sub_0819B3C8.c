#include "global.h"

static inline s32 AngleNear(s32 angle, s32 ref)
{
    s16 d = (angle - ref) & 0xff;
    s32 x = d;
    if (x > 128)
        d = x - 256;
    return ref + d;
}

void sub_0819B3C8(s16 *cur, s16 *target, s32 count)
{
    while (--count >= 0) {
        *cur = AngleNear(*cur, *target);
        cur++;
        target++;
    }
}
