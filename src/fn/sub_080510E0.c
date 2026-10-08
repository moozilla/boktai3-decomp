#include "global.h"
extern u8 *gUnk_02000114;
void sub_080510E0(u32 a, u32 b, u32 c, u32 d)
{
    u8 *p = gUnk_02000114;
    if (p) {
        u32 z = 0;
        *(u16 *)(p + 0x1A) = 1;
        *(u16 *)(p + 0x1C) = a;
        *(u16 *)(p + 0x1E) = b;
        *(u16 *)(p + 0x20) = c;
        *(u16 *)(p + 0x22) = d;
        *(u16 *)(p + 0x2C) = z;
    }
}
