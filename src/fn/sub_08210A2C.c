// COMPILER: old_agbcc
#include "global.h"
/* Collection_SetEntryUnlocked: Sets the indexed bit in the persistent collection unlock mask */
#define Collection_SetEntryUnlocked sub_08210A2C

extern u8 *gGameState;

void sub_08210A2C(s32 bit)
{
    *(u32 *)(gGameState + 0x728) |= 1 << bit;
}
