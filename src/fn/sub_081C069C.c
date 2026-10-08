#include "global.h"
struct S { u8 f[0x20]; s16 a20; s16 a22; u8 g[4]; s16 a28; s16 a2a; };
void sub_08216520(u32, s32, s32);
void sub_081C069C(struct S *p)
{
    sub_08216520(1, p->a28, p->a2a);
    sub_08216520(3, p->a20, p->a22);
}
