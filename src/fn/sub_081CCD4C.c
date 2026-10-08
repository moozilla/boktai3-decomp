#include "global.h"
struct S { u8 f[5]; u8 b5; u8 f6; u8 b7; u8 b8; u8 g[0x168 - 9]; u8 d[1]; };
void sub_08020D68(void *, s32);
void sub_081CCD4C(u32 a, struct S *p)
{
    if (p->b8 != 0) {
        p->b8 = 0;
        p->b7 = 0;
        p->b5 = 0x3b;
    }
    if (p->b7 != 0)
        sub_08020D68(&p->d, 1);
}
