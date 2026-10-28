#include "global.h"
#include "engine_globals.h"

extern u8 gUnk_0203B400[];
s32 Mod(s32, s32);
u8 *Path_GetPointData(u8 *);

struct E {
    u8 pad0[2];
    u8 a;
    u8 b;
    u8 *p;
    u8 *q;
};
struct S080FA7D0 {
    u8 filler[0x448];
    struct E e;
};

void Axe_SelectRandomRoutePoint(struct S080FA7D0 *s)
{
    struct E *e = &s->e;
    u8 v;

    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    v = Mod(*(u16 *)(gUnk_0203B400 + (gRandomIndex << 1)), *e->p);
    if (v == e->a)
        v = Mod(v + 1, *e->p);
    e->a = v;
    e->b = 0;
    e->q = Path_GetPointData(e->p) + (e->a << 3);
}
