#include "global.h"
extern u8 *gGameState;
u32 Inventory_GetItemStateValue(u32 i)
{
    return *(u16 *)(gGameState + i * 2 + 0x100) & 0x7FFF;
}
