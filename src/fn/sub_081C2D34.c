#include "global.h"
struct E { u8 pad[0x38]; };
struct S { u8 f[0x18]; struct E e[32]; u8 g[0x734 - 0x18 - 0x700]; };
extern u32 gUnk_02000264;
void sub_08214514(void *);
void sub_08020D08(void *);
void sub_081C2D34(struct S *p)
{
    s32 i;
    struct E *e = p->e;
    u32 z;
    for (i = 31; i >= 0; i--) { sub_08214514(e); e++; }
    z = 0;
    sub_08020D08((u8 *)p + 0x734);
    gUnk_02000264 = z;
}
