#include "breakable_barrier.h"

extern const u32 gUnk_08604B84[];

u32 sub_08013220(void *a, struct BreakableBarrier *p)
{
    sub_08249240(a, p, (void *)gUnk_08604B84[p->state]);
    p->timer += 1;
    return 0;
}
