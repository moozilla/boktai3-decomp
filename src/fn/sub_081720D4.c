#include "global.h"
s32 GameState_HasUnlockedInventoryEntry(s32);
s32 SolarFruit_IsInventorySlotEquipped(s32);
s32 sub_081720D4(u8 *p)
{
    s32 slot;
    if (p[1] <= 1) {
        slot = p[2] * 2 + p[1];
        return SolarFruit_IsInventorySlotEquipped(slot);
    } else {
        slot = p[2] * 3 - 2;
        slot += p[1];
        return GameState_HasUnlockedInventoryEntry(slot);
    }
}
