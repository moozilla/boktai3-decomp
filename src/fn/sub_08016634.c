#include "global.h"
struct S { u8 f[0x18]; u8 a, b, c, d; u16 x[4]; u16 y[4]; u8 p[4]; u8 q[4]; };
extern struct S *gUnk_02000070;
u32 sub_08016634(struct S *s)
{
    s32 i;
    gUnk_02000070 = s;
    s->a = 0;
    s->b = 0;
    s->c = 0;
    s->d = 0;
    for (i = 0; i < 4; i++) {
        s->x[i] = 0;
        s->y[i] = 0;
        s->p[i] = 0;
        s->q[i] = 0;
    }
}
