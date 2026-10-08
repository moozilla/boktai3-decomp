#include "global.h"

struct E { u32 a; u32 b; };
struct A { u8 f[0x18]; s32 n; };

void sub_081B73F8(struct A *p, struct E *e)
{
    e->a = 0;
    e->b |= 1;
    p->n--;
}
