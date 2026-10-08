#include "global.h"

struct Q { u8 f[0x260]; u32 a; u32 b; };

void sub_081BA62C(struct Q *p, u32 v)
{
    p->a = v;
    p->b = 0;
}
