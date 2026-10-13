#include "breakable_barrier.h"

void sub_08012D78(u32 a, u32 b, struct BreakableBarrier *p)
{
    if (p) {
        p->hit = 1;
    }
}
