#include "global.h"
void Inventory_SetKeyItemId(u32, u32);
void Inventory_ResetValuables(void)
{
    s32 i = 0;
    do {
        Inventory_SetKeyItemId(i, -1);
        i++;
    } while (i <= 0xf);
}
