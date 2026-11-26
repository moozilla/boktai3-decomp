#include "global.h"
u32 Inventory_GetKeyItemId(u32);
void Inventory_SetKeyItemId(u32, u32);
void Inventory_SwapValuableSlots(u32 a, u32 b)
{
    u32 x = Inventory_GetKeyItemId(a);
    Inventory_SetKeyItemId(a, Inventory_GetKeyItemId(b));
    Inventory_SetKeyItemId(b, x);
}
