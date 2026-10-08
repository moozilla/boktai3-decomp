#include "global.h"

struct S { u8 pad[0x1f]; u8 b1f; };
extern struct S *gUnk_02000298;

s32 sub_081D372C(void)
{
    struct S *p = gUnk_02000298;
    s32 r;
    if (p) {
        p->b1f = 1;
        r = 0;
    } else {
        r = -1;
    }
    return r;
}
