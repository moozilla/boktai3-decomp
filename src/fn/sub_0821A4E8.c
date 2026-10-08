#include "global.h"
struct E { u32 k; u32 v; };
u32 sub_0821A4E8(u32 key, struct E *tbl, s32 lo, s32 hi)
{
    s32 mid;
    struct E *e;
    while (lo < hi) {
        mid = (lo + hi) >> 1;
        if (tbl[mid].k < key) lo = mid + 1;
        else hi = mid;
    }
    e = (struct E *)((lo << 3) + (u32)tbl);
    if (e->k == key) return e->v;
    return 0;
}
