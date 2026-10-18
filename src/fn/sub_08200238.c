// COMPILER: old_agbcc
#include "global.h"
/* PlayerClone_IsSlotActive: Tests the requested slot bit in the pool occupancy mask */
#define PlayerClone_IsSlotActive sub_08200238

u32 sub_08200238(u8 *p, s32 bit)
{
    return *(u32 *)(p + 0x18) & (1 << bit);
}
