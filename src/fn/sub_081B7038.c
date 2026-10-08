#include "global.h"

struct E { u8 f[0x234]; };
struct A { u8 f[0x1c]; u16 h; u8 g[6]; struct E e[8]; };
void sub_081B6888(void *, void *, s32);

s32 sub_081B7038(struct A *p, u16 v)
{
    s32 i;
    struct E *e;
    p->h = v;
    e = p->e;
    for (i = 0; i <= 7; i++, e++) {
        sub_081B6888(p, e, i);
    }
    return 0;
}
