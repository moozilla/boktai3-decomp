#include "breakable_barrier.h"

u32 sub_08012F0C(u32 a, struct BreakableBarrier *p)
{
    Collider_Remove(&p->collider);
    Sprite_Remove(p);
    return 0;
}
