#include "global.h"
extern u8 *gGameState;
void Inventory_SetItemId(u32 i, u16 v)
{
    *(u16 *)(gGameState + i * 2 + 0xa0) = v;
}
