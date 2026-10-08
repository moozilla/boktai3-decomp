#include "global.h"
extern u8 *gUnk_02000710;
s32 sub_0822CCC8(s32 i)
{
    return *(s16 *)(gUnk_02000710 + i * 2 + 0x100);
}
