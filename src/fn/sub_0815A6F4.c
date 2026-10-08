#include "global.h"
struct S { u8 f[0x42c]; u16 x; };
extern struct S *gUnk_02000580;
void sub_0815A6F4(void)
{
    struct S *s = gUnk_02000580;
    if (s)
        s->x = 1000;
}
