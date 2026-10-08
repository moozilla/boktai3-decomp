#include "global.h"

struct S { u8 f[0x2c]; u32 a; };
u32 sub_08046870(struct S *p)
{
    if (!p)
        return 0;
    return p->a;
}
