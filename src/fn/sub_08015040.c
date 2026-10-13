#include "global.h"
struct E {
    u8 on;
    u8 f[0x177];
};
struct S {
    u8 f[0x18];
    u32 n;
    u8 g[4];
    struct E e[4];
};
void FragmentGroup_Update(void *, void *);
u32 sub_08015040(struct S *s)
{
    struct E *e = s->e;
    s32 i = 3;
    do {
        if (e->on != 0)
            FragmentGroup_Update(s, e);
        e++;
        i--;
    } while (i >= 0);
    s->n++;
    return 0;
}
