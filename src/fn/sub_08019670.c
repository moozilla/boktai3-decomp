#include "global.h"

struct S { u8 f[0x1fc]; u32 a; };
extern u32 gUnk_020000A4;
void sub_08219D38(u32);
u32 sub_08019670(struct S *p)
{
    if (p->a != 0) {
        sub_08219D38(p->a);
        p->a = 0;
    }
    return gUnk_020000A4 = 0;
}
