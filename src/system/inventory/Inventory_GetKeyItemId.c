#include "global.h"
extern u8 *gGameState;
s32 Inventory_GetKeyItemId(s32 i)
{
    return *(s16 *)(gGameState + i * 2 + 0x838);
}
