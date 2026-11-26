#include "global.h"
extern u8 *gGameState;
u32 Inventory_GetItemOverrideFlag(u32 i)
{
    return *(s16 *)(gGameState + i * 2 + 0x100) & 0x8000;
}
