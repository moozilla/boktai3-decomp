#include "global.h"

struct Q { u8 f[0x1a4]; u32 a; };

void sub_081BAF8C(struct Q *p)
{
    u32 m = 1;
    p->a |= m;
}
