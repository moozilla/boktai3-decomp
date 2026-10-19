#define gSineTable gUnk_086149C4
#include "orbit_motion.h"
static inline s32 absolute(s32 n)
{
    if (n < 0)
        n = -n;
    return n;
}
void OrbitMotion_UpdateAbsoluteSines(struct OrbitMotion *p)
{
    p->offset.x = absolute(scale12(p->radius * gSineTable[(p->angle + p->speed * p->timer) & 255]));
    p->offset.y =
        absolute(scale12(p->radius2 * gSineTable[(p->angle + p->speed * p->timer) & 255]));
    p->offset.z =
        absolute(scale12(p->radius3 * gSineTable[(p->angle + p->speed * p->timer) & 255]));
}
