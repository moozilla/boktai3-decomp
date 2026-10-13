#include "breakable_barrier.h"

u32 sub_08012F0C(u32, struct BreakableBarrier *);

u32 sub_08013270(struct BreakableBarrierManager *p)
{
    struct BreakableBarrier *q = p->barriers;
    s32 i = 3;
    do {
        sub_08012F0C((u32)p, q);
        i--;
        q++;
    } while (i >= 0);
    return 0;
}
