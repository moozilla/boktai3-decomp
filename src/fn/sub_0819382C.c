#include "global.h"
struct P { u8 f[0x7106]; u16 a; u8 g[0xE]; u16 b; };
void sub_0819382C(struct P *p)
{
    p->a |= 0x40;
    p->b = 0x40 | p->b;
}
