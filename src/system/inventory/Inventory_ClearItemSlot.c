#include "global.h"
void Inventory_SetItemId(u32, u32);
void Inventory_SetItemPackedState(u32, u32);
void Inventory_ClearItemSlot(u32 i)
{
    Inventory_SetItemId(i, -1);
    Inventory_SetItemPackedState(i, 0);
}
