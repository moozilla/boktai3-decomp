// COMPILER: old_agbcc
#include "global.h"

extern u8 *gUnk_02000710;

void sub_08210A2C(s32 bit)
{
    *(u32 *)(gUnk_02000710 + 0x728) |= 1 << bit;
}
