#include "ui/town/shop_actions.h"

s32 Shop_FindEmptyEquipmentSlot(const void *unusedOwner)
{
    s32 i;
    for (i = 0; i < 16; i++) {
        if (Inventory_GetEquipmentId(i) == 0xff)
            return i;
    }
    return -1;
}
