#include "global.h"

struct E { u8 f[0x4c]; };
struct A { u8 f[0x18]; s32 n; struct E e[8]; };
void sub_081B7394(void *, void *, s32);

s32 sub_081B74A8(struct A *p)
{
    s32 i;
    struct E *e;
    p->n = 0;
    for (i = 0, e = p->e; i <= 7; i++) {
        sub_081B7394(p, e, i);
        e++;
    }
    return 0;
}
