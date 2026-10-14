// COMPILER: old_agbcc
#include "global.h"

u32 sub_08200238(u8 *p, s32 bit)
{
    return *(u32 *)(p + 0x18) & (1 << bit);
}
