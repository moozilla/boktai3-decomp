#include "global.h"
extern u8 *gGameState;
void Inventory_SetItemPackedState(s32 i, u16 v)
{
    *(u16 *)(gGameState + i * 2 + 0x100) = v;
}
