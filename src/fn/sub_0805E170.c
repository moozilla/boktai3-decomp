#include "global.h"

struct E { u32 a; u32 b; u32 flags; u8 f[0x54]; };
struct P { u8 f[0x1308]; struct E e[5]; };

void sub_0805E170(struct P *p)
{
    struct E *e = p->e;
    u32 m = 1;
    s32 i = 4;
    do {
        e->flags |= m;
        e++;
    } while (--i >= 0);
}
