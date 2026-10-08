#include "global.h"
struct S { u8 f[0x1844]; u16 a; u16 b; };
extern struct S *gUnk_020001D8;
void sub_081119F8(s32 a, s32 b)
{
    struct S *s = gUnk_020001D8;
    if (s != 0) {
        s->a = a;
        s->b = b;
    }
}
