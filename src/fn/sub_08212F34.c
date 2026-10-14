// COMPILER: old_agbcc
#include "global.h"

extern u8 *gUnk_02000710;

void sub_08212F34(s32 bit)
{
    u16 *f = (u16 *)(gUnk_02000710 + 0x596);
    *f = (1 << bit) | *f;
}
