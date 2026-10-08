#include "global.h"

struct S { u8 f[0x1e]; u8 a; };
u32 sub_0803ACC8(struct S *p)
{
    p->a = 1;
    return 0;
}
