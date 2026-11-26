#include "global.h"
u32 ForgingBook_IsItemDiscovered(s32);
u32 ForgingBook_HasDiscoveredAllItems(void)
{
    s32 i;
    for (i = 0; i <= 0x2f; i++)
        if (!ForgingBook_IsItemDiscovered(i))
            return 0;
    return 1;
}
