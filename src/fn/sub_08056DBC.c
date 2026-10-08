#include "global.h"
struct P { u8 f[0x9a]; u16 f9a; u16 f9c; u8 f9e[0xa8 - 0x9e]; u32 fa8; u32 fac; };
void sub_0824923C(struct P *, u32);
void sub_0821AD08(u32, u32);
void sub_0821A0C0(struct P *);
s32 sub_08056DBC(struct P *p)
{
    u16 *c;
    u32 v;
    sub_0824923C(p, p->fac);
    c = &p->f9a;
    v = *c + 1;
    *c = v;
    if ((u16)v >= p->f9c) {
        if (p->fa8 != 0)
            sub_0821AD08(p->fa8, 0);
        sub_0821A0C0(p);
    }
}
