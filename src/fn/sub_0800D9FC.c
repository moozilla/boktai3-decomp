#include "global.h"
extern s32 gUnk_03004DA0;
s32 Div(s32, s32);
void sub_0821AD08(void *, u32);
void sub_0821A0C0(void *);
struct S { u8 f[0x40]; void *t; u8 g[2]; s16 a; s16 b; u8 gg[4]; s16 c; u8 h[0x2e4 - 0x50]; u32 z; };
void sub_0800D9FC(struct S *s)
{
    u32 k;
    if (s->a == 0)
        gUnk_03004DA0 = Div(s->b << 6, s->c);
    s->b--;
    k = 0;
    if (s->b < 0) {
        s->b = k;
        s->z = k;
        if (s->t != 0)
            sub_0821AD08(s->t, 0);
        sub_0821A0C0(s);
    }
}
