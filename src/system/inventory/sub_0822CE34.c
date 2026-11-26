#include "global.h"
void Inventory_SetKeyItemId(u32, s32);
void Inventory_ClearKeyItemSlot(u32 a)
{
    Inventory_SetKeyItemId(a, -1);
}
