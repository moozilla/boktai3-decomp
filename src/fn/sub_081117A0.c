#include "global.h"
struct S { u8 f[0x1c]; u32 a; };
u32 sub_0821A520(u32, u16);
void sub_082161B4(u32, u32, u32, u32, u32, u32, void *);
void sub_08215A74(s32, s32);
void sub_0821656C(s32, s32, s32, s32, s32);
void sub_081117A0(struct S *s, u32 a, u32 b)
{
    u32 x;
    u32 r = sub_0821A520(0xC091, a);
    s->a = r;
    x = b;
    sub_082161B4(2, 0, r, 0, 0, 1, &x);
    sub_08215A74(3, 3);
    sub_0821656C(3, 0, 0, 0, 0);
}
