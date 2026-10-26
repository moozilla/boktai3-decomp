#include "global.h"
extern u8 *gGameState;
void *Inventory_GetEquipmentEntry(s32);
void PauseMenu_ClearEquippedItemState(void)
{
    s16 i = *(s16 *)(gGameState + 0x50);
    s16 v = *(s16 *)(gGameState + 0x68 + i * 2);
    s16 *q = Inventory_GetEquipmentEntry(v);
    q[1] = 0;
}
