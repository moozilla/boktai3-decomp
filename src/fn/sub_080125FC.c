#include "animated_hazard.h"

extern const u32 gUnk_08604B6C[];

void sub_080125FC(void *a, struct AnimatedHazard *p)
{
    sub_08249240(a, p, (void *)gUnk_08604B6C[p->state]);
    p->timer += 1;
}
