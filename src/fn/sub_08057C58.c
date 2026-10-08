#include "global.h"

struct S { u32 f; u8 g[0x64]; u16 v; };

s32 sub_08057C58(struct S *p)
{
    if (p->v != 0 && !(p->f & 1))
        return 1;
    return 0;
}
