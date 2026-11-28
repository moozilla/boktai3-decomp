#include "global.h"
u32 Inventory_GetConsumableObtainedFlag(u32);
u32 GameState_CountCollectedConsumableTypes(void)
{
    u32 count = 0;
    s32 i;

    for (i = 0; i < 48; i++) {
        if (i != 0x21 && i != 0x1a && i != 0x23 && i != 0x24 &&
            Inventory_GetConsumableObtainedFlag(i))
            count++;
    }
    return count;
}
