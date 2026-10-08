#include "global.h"
extern u8 *gUnk_02000488;
void sub_08049F78(u32 a, u32 b)
{
    u8 *p = gUnk_02000488;
    if (p) {
        p[0x3E4] |= a;
        *(u16 *)(gUnk_02000488 + 0x3E6) = b;
    }
}
