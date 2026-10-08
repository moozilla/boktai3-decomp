#include "global.h"
struct A { u32 a, b, c; };
struct B { u32 flags; struct A *args; };
struct P { u8 f0[0x18]; u16 f18; u16 f1a; u32 f1c; u32 f20; u8 f24[4]; u16 f28; u8 f2a; u8 f2b; };
extern u8 *gUnk_02000710;
void sub_0821AD08(u32, struct B *);
void sub_0806F7B0(struct P *p)
{
    s32 v = *(s16 *)(gUnk_02000710 + 0x876);
    u32 r;
    if (p->f2a <= v && p->f2b >= v) {
        r = 1;
        if (p->f28 == 1) return;
    } else {
        r = 0;
        if (p->f28 == 0) return;
    }
    {
        struct A a;
        struct B b;
        a.a = v;
        a.b = p->f1c;
        a.c = p->f20;
        b.flags = (b.flags & 0xFFFF0000) | 3;
        b.args = &a;
        if (r != 0) {
            if (p->f18 != 0) {
                sub_0821AD08(p->f18, &b);
                p->f28 = 1;
            }
        } else {
            if (p->f1a != 0) {
                sub_0821AD08(p->f1a, &b);
                p->f28 = r;
            }
        }
    }
}
