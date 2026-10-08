#include "global.h"

struct S { u8 f[0x19c]; u32 a; };
extern struct S *gUnk_020000DC;
u32 sub_08031E34(void)
{
    struct S *p = gUnk_020000DC;
    if (!p) return 0;
    return p->a;
}
