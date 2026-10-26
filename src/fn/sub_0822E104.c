#include "global.h"
extern u8 *gGameState;
s32 Inventory_GetConsumableId(u32 i)
{
    return *(s16 *)(gGameState + i * 2 + 0x160);
}
