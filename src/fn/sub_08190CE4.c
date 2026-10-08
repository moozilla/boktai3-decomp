#include "global.h"
struct P { u8 f[0x71C2]; u8 a[0x1a]; u8 b[0x1a]; u8 c[0x1a]; u8 d[0x1a]; };
void sub_08190CE4(struct P *p, s32 i, s32 v)
{
    p->a[i] = v;
    p->b[i] = 0;
    p->c[i] = 0;
    p->d[i] = 0;
}
