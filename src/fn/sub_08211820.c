#include "global.h"
extern u8 *gUnk_02000710;
void sub_08211820(s32 bit)
{
    if (bit <= 31)
        *(u32 *)(gUnk_02000710 + 0x72c) |= 1 << bit;
    else
        *(u32 *)(gUnk_02000710 + 0x730) |= 1 << (bit - 32);
}