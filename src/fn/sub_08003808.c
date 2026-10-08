#include "global.h"

struct B { u32 v; u32 a; u32 b; u32 c; u32 d; u32 e; };
struct S { u8 f[0x18]; struct B x; struct B y; };
extern u32 gUnk_03004DA0;
extern u16 gUnk_030051C4;
extern u16 gUnk_03005214;
extern u32 gUnk_030051D4;
extern u16 gUnk_03005210;
extern u16 gUnk_030051DC;

void sub_08003808(struct S *s)
{
    struct B *p = &s->x;
    if (s->x.v != 0) {
        gUnk_03004DA0 = p->c;
        gUnk_030051C4 = p->b;
        gUnk_03005214 = p->a;
    }
    p = &s->y;
    if (s->y.v != 0) {
        gUnk_030051D4 = p->c;
        gUnk_03005210 = p->d;
        gUnk_030051DC = p->a;
    }
}
