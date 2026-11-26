// COMPILER: old_agbcc
#include "global.h"
/* LootDepositPool_RemoveStatusFlags: Removes bits from the status flags of a 0x184-byte pooled object */

void LootDepositPool_RemoveStatusFlags(u8 *p, u16 mask)
{
    *(u16 *)(p + 0x156) &= ~mask;
}
