#include "global.h"
struct S { u8 f[4]; u8 b4; u8 b5; u8 b6; u8 b7; u8 b8; u8 g[0x168 - 9]; };
void sub_08020D68(void *, u32);
void sub_081CCAC8(u32 a, struct S *p)
{
    if (p->b8 != 0) {
        p->b8 = 0;
        p->b7 = 0;
        p->b5 = 43;
        sub_08020D68((u8 *)p + 0x168, 1);
    }
}
