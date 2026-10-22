#include "global.h"
extern u8 *gUnk_02000710;

u32 sub_0822D650(s32 bit)
{
    u32 *flags;
    if (bit <= 31) {
        flags = (u32 *)(gUnk_02000710 + 0x72c);
    } else {
        flags = (u32 *)(gUnk_02000710 + 0x730);
        bit -= 32;
    }
    return *flags & (1 << bit);
}
