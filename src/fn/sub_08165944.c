#include "global.h"
struct S { u8 f[0x428]; u16 a; u16 b; };
struct P { u8 f[0xA40]; struct S *s; };
void sub_0816446C(struct P *, s32, s32, s32);
void sub_0816471C(struct P *, s32, s32, s32, s32, s32);
void sub_08165594(s32, s32, s32, s32, s32);
void sub_08165944(struct P *p, s32 b, s32 c)
{
    sub_0816446C(p, 1, b, c);
    c += 2;
    sub_0816471C(p, 1, b, c, p->s->a, p->s->b);
    sub_08165594(8, 4, 8, 3, 0);
}
