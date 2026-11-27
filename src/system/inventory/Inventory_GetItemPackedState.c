#include "global.h"
extern u8 *gGameState;
s32 Inventory_GetItemPackedState(s32 i)
{
    return *(s16 *)(gGameState + i * 2 + 0x100);
}
