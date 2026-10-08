#include "global.h"
extern u8 *gUnk_02000488;
s32 sub_08049E24(void)
{
    u8 *p = gUnk_02000488;
    s32 r;
    if (p) r = *(s16 *)(p + 0x408);
    else r = -1;
    return r;
}
