#include "global.h"
struct S { u8 f[0xa]; u16 cd; u8 g[0x18]; u16 a; u16 b; u8 q[0x2c - 0x28 + 0x28 - 0x28]; };
struct O { u16 x; u16 p; u16 y; };
struct P { u16 x; u16 p; u16 y; };
u32 sub_0821E67C(void *, void *, void *, void *, u32, u32, u32);
void sub_0821E618(void *);
u16 sub_0821E7E4(void *);

void sub_08008F00(struct S *s)
{
    u8 *q = (u8 *)s + 0x28;
    if (s->cd != 0) {
        s->cd--;
    } else {
        u16 *r = (u16 *)((u8 *)s + 0x54);
        struct O *op;
        struct { u32 w; struct O o; } m;
        op = &m.o;
        if ((u8)sub_0821E67C(q, r, op, &m.w, s->b, s->b, s->a)) {
            sub_0821E618(q);
            s->cd = sub_0821E7E4(q);
        }
        r[0] += op->x;
        ((u16 *)((u8 *)s + 0x58))[0] += op->y;
    }
}
