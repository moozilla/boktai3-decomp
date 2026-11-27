#include "global.h"
u32 ForgingBook_IsWeaponDiscovered(s32);
u32 ForgingBook_HasDiscoveredAllWeapons(void)
{
    s32 i;
    for (i = 0; i <= 0x30; i++)
        if (!ForgingBook_IsWeaponDiscovered(i))
            return 0;
    return 1;
}
