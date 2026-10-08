#include "global.h"
struct S { u8 f[0xFE4]; void *fp; u32 a; u32 b[2]; u32 c; u32 d[2]; u8 st; u8 pad[7]; u32 h; u8 q[0x18]; u8 c24; u8 pad2; u16 t; };
extern u16 gUnk_03005260[];
void sub_08223270(void);
void sub_0803386C(u32, u32);
void sub_08033924(u32, u32);
void sub_0822B2F8(s32);
void sub_08224FA4(void);
void sub_08110410(struct S *s)
{
    if (s->c24 != 0) {
        sub_08223270();
        sub_0803386C(s->h, s->c);
        sub_08033924(s->h, 3);
        sub_0822B2F8(0x192);
        s->t = 0;
        s->c24 = 0;
    }
    if (s->t <= 0x3b) {
        s->t++;
    } else if (gUnk_03005260[1] & 0x30F) {
        sub_08224FA4();
    }
}
