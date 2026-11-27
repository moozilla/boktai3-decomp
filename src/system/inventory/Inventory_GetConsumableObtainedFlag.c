#include "global.h"
extern u8 *gGameState;
u32 Inventory_GetConsumableObtainedFlag(s32 i)
{
    u32 *p;
    if (i <= 0x1f)
        p = (u32 *)(gGameState + 0x720);
    else {
        p = (u32 *)(gGameState + 0x724);
        i -= 0x20;
    }
    return *p & (1 << i);
}
