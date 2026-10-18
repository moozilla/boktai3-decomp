#include "global.h"
void Inventory_SetItemStateValue(u32, u32);
void PauseMenu_ClearItemSlotValues(void)
{
    s32 i;
    for (i = 0; i <= 15; i++)
        Inventory_SetItemStateValue(i, 0);
}
