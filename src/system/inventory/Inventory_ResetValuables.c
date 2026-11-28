#include "global.h"
void Inventory_SetKeyItemId(u32, u32);
void Inventory_ResetValuables(void)
{
    s32 i;

    for (i = 0; i < 16; i++)
        Inventory_SetKeyItemId(i, -1);
}
