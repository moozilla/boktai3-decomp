// COMPILER: old_agbcc
#include "global.h"
/* CollectionBook_SetEntryCollected: Sets the indexed bit in the persistent ten-entry collection mask */
#define CollectionBook_SetEntryCollected sub_08212F34

extern u8 *gUnk_02000710;

void sub_08212F34(s32 bit)
{
    u16 *f = (u16 *)(gUnk_02000710 + 0x596);
    *f = (1 << bit) | *f;
}
