#include "global.h"
extern u8 *gUnk_02000710;
/* Set bit `index` of the 64-bit game-state flag set at 0x72C. */
void Inventory_MarkEquipmentObtained(s32 index)
{
    if (index > 31)
        *(u32 *)(gUnk_02000710 + 0x730) |= 1 << (index - 32);
    else
        *(u32 *)(gUnk_02000710 + 0x72c) |= 1 << index;
}
