#include "global.h"

struct S { u8 f[0x1e]; u8 a; };
extern struct S *gUnk_0200047C;
s32 sub_08020D28(void)
{
    struct S *s = gUnk_0200047C;
    if (s == 0)
        return -1;
    if (s->a != 0)
        return -2;
    s->a = 1;
    return 0;
}
