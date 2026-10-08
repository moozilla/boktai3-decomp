#include "global.h"
s32 sub_0821A454(u32 key, u16 *tbl, s32 x, s32 lo, s32 hi)
{
    s32 mid;
    while (lo < hi) {
        mid = (lo + hi) >> 1;
        if (tbl[mid] < key) lo = mid + 1;
        else hi = mid;
    }
    if (tbl[lo] == key) return lo;
    return -1;
}
