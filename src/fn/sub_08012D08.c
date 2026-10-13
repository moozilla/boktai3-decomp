#include "breakable_barrier.h"

void sub_08012D08(struct BreakableBarrier *s, u8 v)
{
    s->state = v;
    s->timer = 0;
    s->entered = 1;
}
