#include "global.h"
extern u8 *gGameState;
/* Set bit `index` of the 64-bit game-state flag set at 0x72C. */
void Inventory_MarkEquipmentObtained(s32 index)
{
    if (index > 31)
        *(u32 *)(gGameState + 0x730) |= 1 << (index - 32);
    else
        *(u32 *)(gGameState + 0x72c) |= 1 << index;
}
