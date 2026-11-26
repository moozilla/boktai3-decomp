#include "global.h"
extern u8 *gGameState;
s32 Inventory_GetConsumableId(u32);
u32 GameState_HasEquippedConsumable(s32 a)
{
    s32 i;
    for (i = 0; i <= 3; i++) {
        if (Inventory_GetConsumableId(*(s16 *)(gGameState + i * 2 + 0x58)) == a)
            return 1;
    }
    return 0;
}
