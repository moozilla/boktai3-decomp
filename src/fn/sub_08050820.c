#include "global.h"

struct S { u8 f[0x34]; u16 a; u8 b; };
extern struct S *gUnk_0200048C;

void sub_08050820(void)
{
    if (gUnk_0200048C) {
        gUnk_0200048C->b = 1;
        gUnk_0200048C->a = 0;
    }
}
