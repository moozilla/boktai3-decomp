#include "global.h"

struct S { u8 f[0x384]; u8 a; };
struct S *sub_0802140C(u32);
void sub_08028DDC(void)
{
    struct S *p = sub_0802140C(0);
    if (p && p->a != 0)
        p->a--;
}
