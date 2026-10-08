#include "global.h"
struct P { u8 f[0x67e]; u16 f67e; u8 f680[0x694 - 0x680]; u16 f694; };
void sub_080597E0(struct P *);
void sub_080595AC(struct P *, u32);
void sub_0805A3D4(struct P *p)
{
    u16 *c = &p->f67e;
    u32 v = *c + 1;
    *c = v;
    if ((u16)v >= p->f694) {
        sub_080597E0(p);
        sub_080595AC(p, 3);
    }
}
