#include "global.h"
void sub_08219D38(u32);
extern u32 gUnk_020004B8;
struct S { u8 f[0x24]; u32 a; u8 g[8]; u32 b; };
u32 sub_08110E38(struct S *p)
{
    sub_08219D38(p->a);
    sub_08219D38(p->b);
    return gUnk_020004B8 = 0;
}
