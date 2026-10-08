#include "global.h"

struct E { u8 f[0xdc]; u32 v; u8 g[8]; };
struct A { u8 f[0x64]; struct E e[16]; };
void sub_0824923C(void *, u32);

void sub_081A4D8C(struct A *p)
{
    s32 i;
    struct E *e = p->e;
    for (i = 15; i >= 0; i--) {
        if (e->v != 0)
            sub_0824923C(e, e->v);
        e++;
    }
}
