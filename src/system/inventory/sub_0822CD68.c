#include "global.h"
extern u8 *gGameState;
void Inventory_ClearItemOverrideFlag(u32 i)
{
    *(u16 *)(gGameState + i * 2 + 0x100) &= 0x7FFF;
}
