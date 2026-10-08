#include "global.h"
extern u8 *gUnk_02000710;
s32 sub_0811D580(u32 i)
{
    s32 r;
    if (i <= 11) r = *(s32 *)(i * 4 + gUnk_02000710 + 0x7c8);
    else r = -1;
    return r;
}
