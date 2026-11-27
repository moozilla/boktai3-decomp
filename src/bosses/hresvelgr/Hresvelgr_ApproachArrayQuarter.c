#include "global.h"
/* Hresvelgr_ApproachArrayQuarter: Approaches signed array targets by one quarter of the remaining difference and snaps near targets */

static inline s32 Shift(s32 x)
{
    if (x >= 0)
        return x >> 2;
    return -((-x) >> 2);
}

static inline void SnapArray(s32 snap, s16 *cur, s16 *target, s32 count)
{
    while (--count >= 0) {
        s32 c = *cur;
        s32 t = *target;
        s32 d = Shift(t - c);
        c += d;
        if (d > -snap && d < snap)
            c = t;
        *cur = c;
        cur++;
        target++;
    }
}

void Hresvelgr_ApproachArrayQuarter(s16 *cur, s16 *target, s32 snap, s32 count)
{
    SnapArray(snap, cur, target, count);
}
