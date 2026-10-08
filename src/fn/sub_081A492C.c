#include "global.h"
struct P { u8 f[0xdc]; u32 v; u16 a; u16 b; };
void sub_081A492C(struct P *p, u32 v)
{
    p->v = v;
    p->a = 0;
    p->b = 0;
}
