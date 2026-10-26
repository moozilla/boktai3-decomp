#include "global.h"
extern u8 *gGameState;
void Inventory_SetFlag(u32);
void Inventory_SetConsumableEntry(u32 i, u32 v)
{
    Inventory_SetFlag(v);
    *(u16 *)(gGameState + i * 2 + 0x160) = v;
}
