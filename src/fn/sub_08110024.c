#include "global.h"
struct S { u8 f[0x1026]; u16 h; u8 p[6]; u16 h2; };
s32 Mod(s32, s32);
void sub_0822B2F8(s32);
void sub_0810FF8C(struct S *, s32);
void sub_08110024(struct S *s)
{
    if (Mod(s->h, 2) == 0)
        sub_0822B2F8(0x2A5);
    if (s->h2 != 0)
        sub_0810FF8C(s, 0);
}
