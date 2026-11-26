#include "global.h"

struct S {
    u8 f[6];
    s16 fl;
};
struct GrowthHazard {
    u8 f0[0x48];
    struct S s;
    u8 g[0xFC - 0x50];
    s16 cooldown;
    u8 g2[2];
    u8 pending;
};
void AnimatedMarker_BeginStateThree(void *);

void GrowthHazard_UpdateCooldown(struct GrowthHazard *p)
{
    struct S *s = &p->s;
    s32 m = ~4;
    s->fl &= m;
    if (p->pending == 0 && p->cooldown <= 0)
        AnimatedMarker_BeginStateThree(p);
}
