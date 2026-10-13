#include "breakable_barrier.h"

u32 sub_08013220(void *, struct BreakableBarrier *);

u32 sub_0801324C(struct BreakableBarrierManager *p)
{
    struct BreakableBarrier *q = p->barriers;
    s32 i = 3;
    do {
        sub_08013220(p, q);
        i--;
        q++;
    } while (i >= 0);
    return 0;
}
