#include "global.h"
s32 Inventory_GetKeyItemId(s32);
void Inventory_SetKeyItemId(s32, s32);
/* Add `id` to the first free of 16 slots unless it is already present. */
s32 Inventory_AddKeyItem(s32 id)
{
    s32 i;
    for (i = 0; i <= 15; i++)
        if (Inventory_GetKeyItemId(i) == id)
            return 0;
    for (i = 0; i <= 15; i++) {
        if (Inventory_GetKeyItemId(i) < 0) {
            Inventory_SetKeyItemId(i, id);
            return 1;
        }
    }
    return 0;
}
