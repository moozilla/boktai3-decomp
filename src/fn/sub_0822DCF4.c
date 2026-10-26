#include "global.h"

struct Inv0822DCF4 {
    u8 pad0[0x50];
    s16 cursor;
    u8 pad52[0x68 - 0x52];
    u16 slots[8];
};
extern struct Inv0822DCF4 *gGameState;
u32 Inventory_IsEquipmentAssigned(s32);
s32 ItemRecord_MeetsLevelRequirement(u32);

s32 Inventory_AddAssignedSlot(s32 slot)
{
    s32 list[8];
    s32 n;
    s32 i;
    s32 found;
    s32 *out;
    if (!ItemRecord_MeetsLevelRequirement(slot))
        return 0;
    n = 0;
    found = -1;
    i = 0;
    out = list;
    for (; i <= 15; i++) {
        if (i == slot) {
            if (n > 7)
                return 0;
            *out++ = i;
            found = n;
            n++;
        } else if (Inventory_IsEquipmentAssigned(i)) {
            if (n > 7)
                return 0;
            *out++ = i;
            n++;
        }
    }
    if (found < 0)
        return 0;
    gGameState->cursor = found;
    for (; n <= 7; n++)
        list[n] = -1;
    for (i = 0; i <= 7; i++)
        *(u16 *)((u8 *)gGameState + i * 2 + 0x68) = list[i];
    return 1;
}
