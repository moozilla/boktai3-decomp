#include "global.h"
s32 ForgingBook_HasDiscoveredAllWeapons(void);
s32 ForgingBook_HasDiscoveredAllItems(void);
u32 ForgingBook_IsComplete(void)
{
    if (ForgingBook_HasDiscoveredAllWeapons()) {
        if (ForgingBook_HasDiscoveredAllItems())
            return 1;
    }
    return 0;
}
