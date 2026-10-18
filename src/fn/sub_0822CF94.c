#include "global.h"
s32 Inventory_IsKeyItemId(void);
s32 Inventory_RemoveValuable(s32);
s32 Inventory_RemoveConsumable(s32);
s32 Inventory_RemoveItem(s32 a)
{
    s32 r;
    if (Inventory_IsKeyItemId())
        r = Inventory_RemoveValuable(a);
    else
        r = Inventory_RemoveConsumable(a);
    return r;
}
