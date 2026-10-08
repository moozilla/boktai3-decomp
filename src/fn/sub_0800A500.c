#include "global.h"

struct E { u8 f[5]; u8 act; u8 g[0x9a]; };
struct S { u8 f[0x2c]; struct E e[16]; };
extern u32 gUnk_0200003C;
void sub_08009F34(void *, void *);

s32 sub_0800A500(struct S *s)
{
    struct E *e = s->e;
    s32 i;
    for (i = 15; i >= 0; i--) {
        if (e->act != 0)
            sub_08009F34(s, e);
        e++;
    }
    {
        u32 z = 0;
        gUnk_0200003C = z;
    }
    return 0;
}
