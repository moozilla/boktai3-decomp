#include "global.h"
struct P { u8 f[0xde]; u16 h; u8 b0; u8 b1; };
void sub_0819AB64(struct P *p, s32 v)
{
    p->b0 = v;
    p->b1 = 0;
    p->h = 0;
}
