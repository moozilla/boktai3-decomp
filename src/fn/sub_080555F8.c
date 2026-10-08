#include "global.h"
extern u8 *gUnk_02000494;
u32 sub_080555F8(u8 *p, u32 b, u32 c, u32 d)
{
    u32 z = 0;
    p[0x18] = d;
    p[0x19] = z;
    gUnk_02000494 = p;
    return 0;
}
