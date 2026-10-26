#include "global.h"
extern u8 *gGameState;
s32 Inventory_IsKeyItemId(s32);
s32 Inventory_GetItemId(s32);
s32 Inventory_GetKeyItemId(s32);
s32 Inventory_GetEquipmentId(s32);
s32 SolarFruit_GetInventorySlot(s32);
s32 GameState_GetUnlockEntry(s32);
s32 sub_0822D490(u32 category, s32 item)
{
    s32 i;
    switch (category) {
        case 0:
            if (!Inventory_IsKeyItemId(item)) {
                for (i = 0; i <= 15; i++)
                    if (Inventory_GetItemId(i) < 0)
                        return 1;
            } else {
                for (i = 0; i <= 15; i++)
                    if (Inventory_GetKeyItemId(i) < 0)
                        return 1;
            }
            break;
        case 1:
            if (*(s16 *)(gGameState + 0x5A8) == 6)
                break;
            for (i = 0; i <= 15; i++)
                if (Inventory_GetEquipmentId(i) == 0xFF)
                    return 1;
            break;
        case 2:
            if (*(s16 *)(gGameState + 0x5A8) == 6)
                break;
            for (i = 0; i <= 15; i++)
                if (((s16 *)(gGameState + 0x160))[i] < 0)
                    return 1;
            break;
        case 4:
            if (*(s16 *)(gGameState + 0x5A8) == 6)
                break;
            for (i = 0; i <= 7; i++)
                if (SolarFruit_GetInventorySlot(i) == -1)
                    return 1;
            break;
        case 5:
            if (*(s16 *)(gGameState + 0x5A8) == 6)
                break;
            for (i = 0; i <= 11; i++)
                if (GameState_GetUnlockEntry(i) == 0)
                    return 1;
            break;
        case 3:
        case 6:
            if (*(s16 *)(gGameState + 0x5A8) == 6)
                break;
            return 1;
    }
    return 0;
}
