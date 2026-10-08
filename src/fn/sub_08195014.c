#include "global.h"
struct P { u8 f0[0x5404]; s16 x; s16 pad; s16 y; u8 f1[0x72A8 - 0x540A]; s32 a, b, c, d, e, f; };
void sub_08195014(struct P *p, s32 x, s32 y)
{
    p->a = (p->x - x) << 8;
    p->b = 0;
    p->c = (p->y - y) << 8;
    p->d = 0;
    p->f = -0x400;
}
