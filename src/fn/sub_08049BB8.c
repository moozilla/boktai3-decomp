#include "global.h"
extern u8 *gUnk_02000488;
u32 sub_08049BB8(u32 a)
{
    u8 *p = gUnk_02000488;
    if (!p) return 0;
    if (p[0x255] == a) return 1;
    return 0;
}
