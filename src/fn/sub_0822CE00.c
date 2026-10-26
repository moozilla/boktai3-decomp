#include "global.h"
extern u8 *gGameState;
void Inventory_SetKeyItemId(s32 i, u16 v)
{
    *(u16 *)(gGameState + i * 2 + 0x838) = v;
}
