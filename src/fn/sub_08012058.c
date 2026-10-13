#include "animated_hazard.h"

s32 sub_08012058(void *unused, struct AnimatedHazard *p)
{
    s32 r;
    if (p->entered == 0) {
        r = 0;
    } else {
        p->entered = 0;
        r = 1;
    }
    return r;
}
