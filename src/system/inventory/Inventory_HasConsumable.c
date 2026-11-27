#include "global.h"
s32 Inventory_GetItemId(s32);
s32 Inventory_HasConsumable(s32 a)
{
    s32 i;
    for (i = 0; i <= 0xf; i++) {
        if (Inventory_GetItemId(i) == a)
            return 1;
    }
    return 0;
}
