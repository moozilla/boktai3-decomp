#include "global.h"
extern u8 *gUnk_02000114;
void sub_08051158(u32 a, u32 b, u32 c, u32 d, u32 e)
{
    u8 *p = gUnk_02000114;
    if (p) {
        u32 z = 0;
        *(u16 *)(p + 0x1A) = 2;
        *(u16 *)(p + 0x2A) = a;
        *(u16 *)(p + 0x1C) = b;
        *(u16 *)(p + 0x1E) = c;
        *(u16 *)(p + 0x20) = d;
        *(u16 *)(p + 0x22) = e;
        *(u16 *)(p + 0x2C) = z;
    }
}
