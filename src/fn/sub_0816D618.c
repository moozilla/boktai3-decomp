#include "global.h"
void Inventory_ClearItemSlot(u32);
void PauseMenu_ClearItemSlot(u8 *s, u32 value)
{
    if (s[0x4860] == 0)
        Inventory_ClearItemSlot(value);
}
