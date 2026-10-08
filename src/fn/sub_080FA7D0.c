#include "global.h"

extern s32 gUnk_03005308;
extern u8 gUnk_0203B400[];
s32 Mod(s32, s32);
u8 *sub_0821E76C(u8 *);

struct E { u8 pad0[2]; u8 a; u8 b; u8 *p; u8 *q; };
struct S080FA7D0 { u8 filler[0x448]; struct E e; };

void sub_080FA7D0(struct S080FA7D0 *s)
{
    struct E *e = &s->e;
    u8 v;

    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
    v = Mod(*(u16 *)(gUnk_0203B400 + (gUnk_03005308 << 1)), *e->p);
    if (v == e->a)
        v = Mod(v + 1, *e->p);
    e->a = v;
    e->b = 0;
    e->q = sub_0821E76C(e->p) + (e->a << 3);
}
