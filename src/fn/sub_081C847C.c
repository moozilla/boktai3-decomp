#include "global.h"
struct S { u8 f[4]; u8 g[3]; u8 b7; u8 b8; u8 b9; u8 h[0x16]; u32 a20; u32 a24; };
struct P { u32 a; u32 b; };
void sub_081FE6E0(void);
u32 sub_081FEC9C(void *);
void sub_0822B2F8(u32);
void sub_081C847C(u32 a, struct S *p)
{
    u8 *e = (u8 *)p + 0x20;
    if (p->b9 != 0) {
        struct P *d;
        u16 *h;
        p->b9 = 0;
        p->b7 = 0;
        sub_081FE6E0();
        d = (struct P *)((u8 *)p + 0x54);
        *d = *(struct P *)((u8 *)p + 0x68);
        h = (u16 *)((u8 *)p + 0x56);
        *h = (u16)(0x100 + *h);
        *(u32 *)(e + 4) = sub_081FEC9C(d);
        sub_0822B2F8(0x571);
    }
    p->b7 = 1;
}
