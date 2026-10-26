#include "global.h"
#include "area_effect.h"

struct S {
    u8 filler[0x24];
    u16 a;
    u8 b;
    u8 c;
    u8 d;
};

s32 AreaEffect_InitializeLink(struct S *p)
{
    p->b = 0;
    p->a = 0x1C7;
    p->d = 0;
    gAreaEffectManager = p;
    return 0;
}
