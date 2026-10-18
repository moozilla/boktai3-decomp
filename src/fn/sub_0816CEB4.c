#include "global.h"
u32 Inventory_GetKeyItemId(u32);
u32 Inventory_GetItemId(u32);
u32 Inventory_GetQuantityByStorage(u32 condition, u32 value)
{
    if (condition == 0)
        return Inventory_GetItemId(value);
    return Inventory_GetKeyItemId(value);
}
