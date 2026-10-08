#include "global.h"
struct S { u8 f[4]; u8 b4; u8 b5; u8 b6; u8 b7; u8 b8; u8 g[0xb]; u32 a14; u8 h[0x168 - 0x18]; };
void sub_08020D68(void *, u32);
void sub_081D16EC(u32 a, struct S *p)
{
    if (p->b8 != 0) {
        p->b8 = 0;
        p->b7 = 0;
        p->b5 = 14;
    }
    if (p->b7 != 0) {
        u32 o = 1;
        u32 z = 0;
        p->b4 = o;
        p->a14 = z;
        p->b8 = o;
        sub_08020D68((u8 *)p + 0x168, 1);
    }
}
