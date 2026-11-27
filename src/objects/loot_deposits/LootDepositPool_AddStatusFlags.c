#include "global.h"

void LootDepositPool_AddStatusFlags(u8 *obj, u16 flags)
{
    *(u16 *)(obj + 0x156) |= flags;
}
