#include "global.h"
#include "engine.h"

s32 Inventory_GetItemId(u32);
s32 Inventory_GetItemPackedState(u32);

u8 InventorySlot_PickRandom(s16 *slot, s16 *kind, s16 *param)
{
    u16 start;
    s16 n;
    s16 idx;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    start = Mod(*(u16 *)((u8 *)gUnk_0203B400 + gRandomIndex * 2), 16);
    for (n = 0; n < 16; n++) {
        idx = (s16)start;
        *kind = Inventory_GetItemId(idx);
        if (*kind >= 0 && *kind != 9)
            goto found;
        start = Mod(idx + 1, 16);
    }
    return 0;
found:
    *slot = start;
    if (idx) {
        *param = Inventory_GetItemPackedState(*slot);
        return 1;
    } else {
        *param = Inventory_GetItemPackedState(*slot);
        return 1;
    }
}
