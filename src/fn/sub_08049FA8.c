#include "global.h"
extern u8 *gUnk_02000488;
void sub_08049FA8(u32 m)
{
    u8 *g = gUnk_02000488;
    if (g != 0) {
        u8 *q = g + 0x3e4;
        u32 z;
        u8 v = *q & ~m;
        z = 0;
        *q = v;
        *(u16 *)(gUnk_02000488 + 0x3e6) = z;
    }
}
