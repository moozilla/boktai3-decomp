#include "global.h"

struct E { u8 act; u8 f[0x147]; };
struct S { u8 f[0x28]; struct E e[6]; };
extern struct S *gUnk_02000040;
void sub_0800A80C(void *, void *);

s32 sub_0800A830(void)
{
    struct S *s = gUnk_02000040;
    struct E *e;
    s32 i;
    if (s == 0)
        return -1;
    e = s->e;
    for (i = 5; i >= 0; i--) {
        if (e->act != 0)
            sub_0800A80C(s, e);
        e++;
    }
    return 0;
}
