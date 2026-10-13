#include "animated_hazard.h"

void sub_0801204C(void *, struct AnimatedHazard *, u32);

void sub_08012114(void *a, struct AnimatedHazard *p)
{
    p->sprite.flags |= 1;
    sub_0801204C(a, p, 0);
    p->active = 0;
}
