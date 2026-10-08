#include "global.h"
extern u8 *gUnk_02000498;
u32 sub_08055AF4(u8 *p, u32 b, u32 c, u32 d)
{
    u32 z = 0;
    p[0x18] = d;
    p[0x19] = z;
    gUnk_02000498 = p;
    return 0;
}
