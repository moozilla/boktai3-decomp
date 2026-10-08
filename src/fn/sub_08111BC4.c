#include "global.h"
struct S { u8 f[0x1848]; u16 a; u8 p[6]; s16 h; u16 h2; u32 w; };
extern struct S *gUnk_020001D8;
s32 sub_0821ABA8(s32, s32);
s32 Div(s32, s32);
void sub_08111BC4(void)
{
    struct S *g = gUnk_020001D8;
    if (g != 0) {
        struct S *s = g;
        s->h = sub_0821ABA8(0x66, 0);
        s->h2 = sub_0821ABA8(0x70, 0) << 7;
        if (s->h == 0)
            s->w = s->h2;
        else
            s->w = Div(s->h2 - s->a, s->h);
        *((u8 *)s + 0x184D) = 1;
    }
}
