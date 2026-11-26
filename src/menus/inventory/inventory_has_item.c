#include "engine/shared/global.h"
s32 Inventory_IsKeyItemId(s32);
s32 Inventory_HasValuable(s32);
s32 Inventory_HasConsumable(s32);
s32 Inventory_HasItem(s32 a)
{
    s32 r;
    if (Inventory_IsKeyItemId(a))
        r = Inventory_HasValuable(a);
    else
        r = Inventory_HasConsumable(a);
    return r;
}
