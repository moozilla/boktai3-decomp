#include "global.h"

extern u32 gUnk_0300523C;
struct E { s32 v; u8 f[0x48]; };
struct A { u8 f[0x1c]; struct E e[8]; };
void sub_081B740C(void *, void *, s32);

void sub_081B744C(struct A *p)
{
    s32 i;
    struct E *e = p->e;
    if ((gUnk_0300523C & 7) == 0) {
        for (i = 0; i <= 7; i++) {
            if (e->v > 0)
                sub_081B740C(p, &p->e[i], i);
            e++;
        }
    }
}
