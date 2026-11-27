#include "global.h"
s32 Inventory_GetItemId(s32);
void Inventory_ClearItemSlot(s32);
s32 Inventory_RemoveConsumable(s32 a)
{
    s32 i;
    for (i = 0; i <= 0xf; i++) {
        if (Inventory_GetItemId(i) == a) {
            Inventory_ClearItemSlot(i);
            return 1;
        }
    }
    return 0;
}
