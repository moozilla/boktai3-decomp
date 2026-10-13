#include "animated_hazard.h"

void sub_0801204C(void *unused, struct AnimatedHazard *p, u32 v)
{
    u32 z = 0;
    p->state = v;
    p->timer = z;
    p->entered = 1;
}
