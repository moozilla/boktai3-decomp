#include "global.h"
/* Math_BlendWithTableFactor: Blends endpoints using a capped table index and returns the target for a zero table entry */
#define Math_BlendWithTableFactor sub_0819B010

extern const s16 gUnk_08611D24[];

static inline s32 Shift8(s32 x)
{
    if (x >= 0)
        return x >> 8;
    return -((-x) >> 8);
}

s32 sub_0819B010(s32 from, s32 to, s32 t)
{
    if (t > 15)
        t = 15;
    t = gUnk_08611D24[t];
    if (t == 0)
        return to;
    return (to - from) * Shift8(t) + from;
}
