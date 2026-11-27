#include "ui/town/shop_actions.h"

s32 Shop_FindEmptyItemSlot(const void *unusedOwner)
{
    s32 i;
    for (i = 0; i < 16; i++) {
        if (Inventory_GetItemId(i) < 0)
            return i;
    }
    return -1;
}
