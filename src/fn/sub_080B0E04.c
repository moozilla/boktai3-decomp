#include "global.h"

s32 Inventory_GetItemPackedState(s32 i);
s32 Inventory_GetItemId(u32);

s32 SceneInventory_FindSlotByValue(s16 *p0, s16 *p1, s16 *p2, s32 p3)
{
    s16 i;
    for (i = 0; i <= 15; i++) {
        s32 v = Inventory_GetItemId(i);
        *p1 = v;
        if ((s16)v < 0)
            continue;
        if (*p1 != p3)
            continue;
        *p0 = i;
        {
            s32 w = Inventory_GetItemPackedState(*p0);
            *p2 = w;
        }
        return 1;
    }
    return 0;
}
