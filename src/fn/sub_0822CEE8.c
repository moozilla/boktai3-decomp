#include "global.h"
u32 Inventory_IsKeyItemId(u32);
u32 Inventory_AddKeyItem(u32);
u32 Inventory_AddItemWithState(u32, u32);
u32 Inventory_AddItem(u32 a, u32 b)
{
    u32 r;
    if (Inventory_IsKeyItemId(a) != 0)
        r = Inventory_AddKeyItem(a);
    else
        r = Inventory_AddItemWithState(a, b);
    return r;
}
