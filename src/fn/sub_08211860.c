#include "global.h"
extern u8 *gUnk_02000710;

void sub_08211860(s32 bit)
{
    if (bit <= 31)
        *(u32 *)(gUnk_02000710 + 0x720) |= 1 << bit;
    else
        *(u32 *)(gUnk_02000710 + 0x724) |= 1 << (bit - 32);
}
