#include "breakable_barrier.h"

u32 sub_08012D20(struct BreakableBarrier *p)
{
    u8 *q = &p->entered;
    u32 r;
    if (*q == 0) {
        r = 0;
    } else {
        *q = 0;
        r = 1;
    }
    return r;
}
