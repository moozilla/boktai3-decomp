#include "global.h"
void sub_0822D080(u32, u32);
void Inventory_SwapValuableSlots(u32, u32);
void PauseMenu_SwapItemSlots(u8 *s, u32 a, u32 b)
{
    if (s[0x4860] == 0)
        sub_0822D080(a, b);
    else
        Inventory_SwapValuableSlots(a, b);
}
