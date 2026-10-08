#include "global.h"
struct P { u8 f[0x72AC]; s32 a; u8 g[8]; s32 b; };
void sub_08195140(struct P *p)
{
    s32 *a = &p->a;
    s32 *b = &p->b;
    s32 v = *a + *b;
    *a = v;
    if (*b < 0) {
        if (v < -0x1000) { *a = -0x1000; *b = 0x88; }
    } else {
        if (v > 0x1000) { *a = 0x1000; *b = -0x88; }
    }
}
