#include "global.h"
#include "area_effect.h"

struct S {
    u8 filler[0x1c];
    u16 v;
    u8 f1e[6];
    u16 a;
    u8 b;
    u8 c;
    u8 d;
};
u16 AreaEffect_GetInitialParticleMode(struct S *);

s32 AreaEffect_Initialize(struct S *p)
{
    u8 z;
    p->v = AreaEffect_GetInitialParticleMode(p);
    z = 0;
    p->b = z;
    p->a = 0x1C7;
    p->d = z;
    gAreaEffectManager = p;
    return 0;
}
