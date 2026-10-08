#include "global.h"

struct E { u8 f[0x1e]; u8 act; u8 g[0x1d5-0x1f+0x0]; };
struct S { u8 f[0x18]; u16 n; u8 g[0x56]; struct E *e; };
extern struct S *gUnk_02000024;
void sub_080047AC(struct E *);

s32 sub_080047EC(void)
{
    struct S *s = gUnk_02000024;
    struct E *e;
    s32 i;
    if (s == 0)
        return -1;
    e = s->e;
    for (i = 0; i < s->n; i++) {
        if (e->act != 0)
            sub_080047AC(e);
        e = (struct E *)((u8 *)e + 0x1a8);
    }
    return 0;
}
