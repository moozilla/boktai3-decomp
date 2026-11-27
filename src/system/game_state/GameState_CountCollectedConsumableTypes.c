#include "global.h"
#include "debug.h"
u32 Inventory_GetConsumableObtainedFlag(u32);
u32 GameState_CountCollectedConsumableTypes(void)
{
    u32 n = 0;
    s32 i = 0;
    do {
        if (i == 0x21 || i == 0x1a || i == 0x23 || i == 0x24) {
            DEBUG_PRINT();
        } else if (Inventory_GetConsumableObtainedFlag(i))
            n++;
        i++;
    } while (i <= 0x2f);
    return n;
}
