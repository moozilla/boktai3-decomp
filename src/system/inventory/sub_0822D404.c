#include "global.h"
void Inventory_SetItemId(u32, u32);
void Inventory_SetItemPackedState(u32, u32);
void Inventory_ClearItemOverrideFlag(u32);
void Inventory_ResetConsumables(void)
{
    s32 i = 0;
    do {
        Inventory_SetItemId(i, -1);
        Inventory_SetItemPackedState(i, 0);
        Inventory_ClearItemOverrideFlag(i);
        i++;
    } while (i <= 0x2f);
}
