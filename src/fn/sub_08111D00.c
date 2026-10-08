#include "global.h"
struct S { u8 f[0x11C0]; u32 a; u8 p[8]; u32 b; };
extern u8 *gUnk_02000710;
void sub_0803386C(u32, u32);
void sub_08033924(u32, u32);
void sub_08033A38(u32, u32, void *);
void sub_080337FC(u32);
void sub_08111D00(struct S *s)
{
    u8 *q = gUnk_02000710 + 0x790;
    u32 *p = &s->b;
    sub_0803386C(*p, s->a);
    sub_08033924(*p, 0);
    sub_08033A38(*p, 1, q);
    sub_080337FC(*p);
}
