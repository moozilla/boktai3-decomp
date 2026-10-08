#include "global.h"

struct S { u8 f[0x134]; u8 a; };
extern struct S *gUnk_02000484;
u8 *sub_08036A88(void)
{
    struct S *p = gUnk_02000484;
    if (!p) return 0;
    return &p->a;
}
