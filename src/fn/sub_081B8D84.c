#define gSineTable gUnk_086149C4
#include "orbit_motion.h"

static inline s32 absolute(s32 n)
{
    if (n < 0)
        n = -n;
    return n;
}

s32 OrbitMotion_UpdateFoldedOrbit(struct OrbitMotion *p)
{
    s32 angle = (256 - absolute(256 - (p->angle + p->speed * p->timer))) & 255;
    polar(&p->offset, angle, p->radius);
    add(p->position, &p->offset, p->position);
    if ((u32)(++p->timer * p->speed) > 512) {
        p->state = 2;
        p->timer = 0;
        return 1;
    }
    return 0;
}
