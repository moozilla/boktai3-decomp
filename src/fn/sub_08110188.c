#include "global.h"
struct S { u32 f0[7]; u32 g; u8 f[0xFE4-0x20]; void *fp; u32 a; u32 b[2]; u32 c; u32 d[2]; u8 st; u8 pad[7]; u32 h; u8 q[0x18]; u8 c24; u8 pad2; u16 t; };
struct G { u8 f[0x1c]; u8 b; };
extern struct G *gUnk_020004B4;
void sub_0810FD90(struct S *);
void sub_082156B8(s32);
void sub_082164AC(s32, u32, s32);
void sub_0821656C(s32, s32, s32, s32, s32);
void sub_0803381C(u32);
void sub_080339B8(u32, s32, s32, s32, s32);
void sub_0803386C(u32, u32);
void sub_08033924(u32, u32);
void sub_080337FC(u32);
void sub_08109358(void);
void sub_0811024C(void);
static inline void set(struct S *s, void *fp, u8 st) { s->fp = fp; s->c24 = 1; s->st = st; }
void sub_08110188(struct S *s)
{
    struct G *g;
    s32 v;
    if (s->c24 != 0) {
        u8 z;
        sub_0810FD90(s);
        sub_082156B8(0);
        sub_082156B8(3);
        sub_082164AC(0, s->g, 3);
        sub_0821656C(0, 0, 0, 0, 0);
        sub_0803381C(s->h);
        sub_080339B8(s->h, 1, 6, 0x1c, 6);
        sub_0803386C(s->h, s->a);
        sub_08033924(s->h, 8);
        sub_080337FC(s->h);
        sub_08109358();
        s->c24 = 0;
    }
    g = gUnk_020004B4;
    v = -1;
    if (g != 0)
        v = g->b;
    if (v == 0x19)
        set(s, sub_0811024C, 4);
}
