#include "global.h"
extern s32 gUnk_03004DA0;
s32 Div(s32, s32);
struct S { u8 f[0x46]; s16 a; s16 b; u8 g[4]; s16 c; u8 h[0x2e4 - 0x50]; u32 z; };
void sub_0800D9A0(struct S *s)
{
    if (s->a == 0)
        gUnk_03004DA0 = Div(s->b << 6, s->c);
    s->b++;
    if (s->b > s->c) {
        s->b = s->c;
        s->z = 0;
    }
}
