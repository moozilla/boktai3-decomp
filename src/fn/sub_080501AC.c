#include "global.h"
extern u8 *gUnk_02000580;
u32 sub_080501AC(u8 *p, s32 b)
{
    u32 v;
    u32 m;
    if (p[0x1C] & 8) v = *(u16 *)(gUnk_02000580 + 0x30);
    else v = *(u16 *)(gUnk_02000580 + 0x34);
    m = 0xFF;
    m &= v;
    if ((s32)m <= b) return 1;
    return 0;
}
