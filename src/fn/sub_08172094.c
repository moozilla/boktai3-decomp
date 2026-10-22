#include "global.h"
s32 SolarFruit_GetInventorySlot(s32);
s32 GameState_GetUnlockEntry(s32);
s32 sub_08172094(u8 *p)
{
    s32 slot;
    if (p[1] <= 1) {
        slot = p[2] * 2 + p[1];
        if (SolarFruit_GetInventorySlot(slot) == -1)
            return TRUE;
    } else {
        slot = p[2] * 3 - 2;
        slot += p[1];
        if (GameState_GetUnlockEntry(slot) == 0)
            return TRUE;
    }
    return FALSE;
}
